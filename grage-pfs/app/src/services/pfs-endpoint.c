/*
 * pfs-endpoint.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

#include "linux-commons-strings.h"
#include "linux-commons-logging.h"
#include <linux-commons-errors.h>

#include <grage-commons.h>
#include "nipc-messaging.h"

#include "pfs-endpoint.h"
#include "pfs-state.h"
#include "pfs-cache.h"
#include "pfs-fat32.h"

#include "pfs-configuration.h"
#include "pfs-connection-pool.h"




	ListenSocket pfs_endpoint_doHandshake(){

		char * host = pfs_configuration_getDeviceAddress();
		char * port = pfs_configuration_getDevicePort();

		ListenSocket dataSocket = commons_socket_openClientConnection(host , port);

		nipc_sendHandshake(dataSocket , NIPC_PROCESS_ID_PFS);

		NipcMessage message =  nipc_receiveHandshake(dataSocket);

		if(message.header.responseCode == NIPC_RESPONSE_CODE_ERROR){
			puts("Fallo el handshake");
			exit(1);
		}

		return dataSocket;
	}


	void pfs_endpoint_callPutSector(DiskSector diskSector){
		if(pfs_pool_isPooledConnectionsEnabled()){
			return pfs_endpoint_callPooledPutSector(diskSector);
		}else{

			return pfs_endpoint_callNonPooledPutSector(diskSector);
		}
	}


	DiskSector pfs_endpoint_callGetSector(uint32_t sectorNumber){
		if(pfs_pool_isPooledConnectionsEnabled()){
			return pfs_endpoint_callPooledGetSector(sectorNumber);

		}else{
			return pfs_endpoint_callNonPooledGetSector(sectorNumber);
		}
	}


	DiskSector pfs_endpoint_buildDiskSectorFromCacheCluster(CacheSectorRecord * a){
		DiskSector d;
		d.sectorNumber = 0;

		if(a == NULL)
			return d;

		memcpy(d.sectorContent , a->sector.sectorContent , sizeof(a->sector.sectorContent));
		d.sectorNumber = a->sector.sectorNumber;
		return d;
	}

	Block pfs_endpoint_buildBlockFromCacheCluster(CacheBlockRecord * a){
		Block d;
		d.id = 0;

		if(a == NULL)
			return d;

		memcpy(d.content , a->block->content , sizeof(a->block->content));
		d.id = a->block->id;
		return d;
	}

	DiskSector pfs_endpoint_utils_getFromFatCache(uint32_t sectorNumber){
		CacheSectorRecord * s = pfs_cache_get_sector(
			sectorNumber,pfs_cache_getListaCacheFat()
			, pfs_cache_getCacheSectorsFatMaxCount());

		return pfs_endpoint_buildDiskSectorFromCacheCluster(s);
	}

	Block pfs_endpoint_utils_getFromFileCache(uint32_t clusterId , FatFile * fatFile){

		if( pfs_cache_habilitada() ){
			CacheBlockRecord * s = pfs_cache_getBlock(clusterId, fatFile->cache ,
					pfs_cache_getCacheSectorsMaxCount());

			return pfs_endpoint_buildBlockFromCacheCluster(s);
		} else {
			Block b;
			b.id = 0;
			return b;
		}
	}

	DiskSector pfs_endpoint_utils_getFromCache(uint32_t sectorNumber){

		DiskSector defaultSector;
		defaultSector.sectorNumber = 0;

		if (pfs_cache_isFatSectorReserved(sectorNumber))
			return pfs_endpoint_utils_getFromFatCache(sectorNumber);

		return defaultSector;
	}

	void pfs_endpoint_utils_putInCache(DiskSector d , List cache){
		uint8_t present = FALSE;

		Iterator * ite = commons_iterator_buildIterator(pfs_cache_getListaCacheFat());

		while( commons_iterator_hasMoreElements(ite) ){
			CacheSectorRecord * nodo = (CacheSectorRecord *)commons_iterator_next(ite);
			if(nodo->sector.sectorNumber == d.sectorNumber){
				memcpy(nodo->sector.sectorContent , d.sectorContent , sizeof(d.sectorContent));
				nodo->modificado = TRUE;
				present = TRUE;
				break;
			}
		}

		if(present == FALSE){
			DiskSector * disk = malloc(sizeof (DiskSector));
			memcpy(disk->sectorContent , d.sectorContent , sizeof d.sectorContent);
			disk->sectorNumber = d.sectorNumber;
			pfs_cache_put_sectors(disk , cache, pfs_cache_getCacheSectorsFatMaxCount());
		}
	}

	void pfs_endpoint_callCachedPutSector(DiskSector sector){

		if(pfs_cache_isFatSectorReserved(sector.sectorNumber)){
			pfs_endpoint_utils_putInCache(sector , pfs_cache_getListaCacheFat());
			return;
		}

		pfs_endpoint_callPutSector(sector);
	}


	DiskSector pfs_endpoint_callCachedGetSector(uint32_t sectorNumber){

		DiskSector returningSector = pfs_endpoint_utils_getFromCache(sectorNumber);

		if(returningSector.sectorNumber != 0){
			//printf("Se toma de la cache el sector %i\n" , returningSector.sectorNumber);
			return returningSector;
		}

		returningSector = pfs_endpoint_callGetSector(sectorNumber);

		if(pfs_cache_isFatSectorReserved(sectorNumber)){
			//printf("Se pone en cache fat el sector %i \n" , sectorNumber);
			pfs_endpoint_utils_putInCache(returningSector , pfs_cache_getListaCacheFat());
		}

		return returningSector;
	}


	DiskSector pfs_endpoint_callPooledGetSector(uint32_t sectorNumber){
		PooledConnection * conn = pfs_pool_getConection();

		DiskSector diskSector = pfs_endpoint_buildAndSendGet(conn->listenSocket , sectorNumber);

		pfs_pool_releaseConnection(conn);

		return diskSector;
	}


	void pfs_endpoint_callPooledPutSector( DiskSector diskSector){
		PooledConnection * conn = pfs_pool_getConection();

		pfs_endpoint_bulidAndSendPut(conn->listenSocket , diskSector);

		pfs_pool_releaseConnection(conn);
	}

	DiskSector pfs_endpoint_callNonPooledGetSector(uint32_t sectorNumber){
		ListenSocket ds = pfs_endpoint_doHandshake();

		DiskSector diskSector = pfs_endpoint_buildAndSendGet(ds , sectorNumber);

		close(ds);

		return diskSector;
	}

	void pfs_endpoint_callNonPooledPutSector( DiskSector diskSector){
		ListenSocket ds = pfs_endpoint_doHandshake();

		pfs_endpoint_bulidAndSendPut(ds , diskSector);

		close(ds);
	}



	DiskSector pfs_endpoint_buildAndSendGet(ListenSocket ds, uint32_t sectorNumber){
		NipcMessage message = nipc_mbuilder_buildNipcMessage();
		message = nipc_mbuilder_addOperationId(message , NIPC_OPERATION_ID_GET_SECTORS);
		message = nipc_mbuilder_addDiskSectorId(message , sectorNumber);
		nipc_messaging_send(ds , message);
		message = nipc_messaging_receive(ds);

		return message.payload.diskSector;
	}

	void pfs_endpoint_bulidAndSendPut(ListenSocket ds , DiskSector diskSector){
		NipcMessage message = nipc_mbuilder_buildNipcMessage();
		message = nipc_mbuilder_addOperationId(message , NIPC_OPERATION_ID_PUT_SECTORS);
		message = nipc_mbuilder_addDiskSector(message , diskSector);
		nipc_messaging_send(ds , message );
	}

    void pfs_endpoint_blocks_callExecuteGetSector(ListenSocket ds , uint32_t sectorId){

        NipcMessage message = nipc_mbuilder_buildNipcMessage();
        message = nipc_mbuilder_addOperationId(message , NIPC_OPERATION_ID_GET_SECTORS);
        message = nipc_mbuilder_addDiskSectorId(message , sectorId);
        nipc_messaging_send(ds , message);

    }

    DiskSector pfs_endpoint_blocks_callReturnGetSector(ListenSocket ds){
        return nipc_messaging_receive(ds).payload.diskSector;
    }

