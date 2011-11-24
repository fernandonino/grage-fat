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

#include <linux-commons-errors.h>

#include "nipc-messaging.h"

#include "pfs-endpoint.h"
#include "pfs-state.h"

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




	void pfs_endpoint_callPutSector( DiskSector diskSector){
		if(pfs_pool_isPooledConnectionsEnabled()){
			return pfs_endpoint_callPooledPutSector(diskSector);
		}else{

			log_info_t(commons_string_concat("Sector: " , commons_misc_intToString(diskSector.sectorNumber)));

			return pfs_endpoint_callNonPooledPutSector(diskSector);
		}
	}


	DiskSector pfs_endpoint_callGetSector(uint32_t sectorNumber){
		if(pfs_pool_isPooledConnectionsEnabled()){
			return pfs_endpoint_callPooledGetSector(sectorNumber);
		}else{
			log_info_t(commons_string_concat("Sector: " , commons_misc_intToString(sectorNumber)));

			return pfs_endpoint_callNonPooledGetSector(sectorNumber);
		}
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



