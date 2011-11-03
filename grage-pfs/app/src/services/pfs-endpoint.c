/*
 * pfs-endpoint.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "grage-commons.h"
#include "pfs-endpoint.h"
#include "nipc-messaging.h"

	void pfs_endpoint_callPutSectors(){

	}

	DiskSector pfs_endpoint_callGetSectors(uint32_t sectorNumber){
		RuntimeErrorValidator * v = commons_errors_buildSuccessValidator();
		ListenSocket sock;

		nipc_sendHandshake(sock,NIPC_PROCESS_ID_PFS,v);
		NipcMessage mensaje = nipc_receiveHandshake(sock,v);

		mensaje = nipc_mbuilder_buildNipcMessage();
		mensaje = nipc_mbuilder_addOperationId(mensaje,NIPC_OPERATION_ID_GET_SECTORS);
		mensaje = nipc_mbuilder_addMessageType(mensaje,NIPC_MESSAGE_TYPE_SECTOR_ID);
		mensaje = nipc_mbuilder_addProcessId(mensaje,NIPC_PROCESS_ID_PFS);
		mensaje = nipc_mbuilder_addDiskSectorId(mensaje,sectorNumber);
		nipc_messaging_send(sock,mensaje);
		mensaje = nipc_messaging_receive(sock);

		return mensaje.payload.diskSector;
	}

	Cluster pfs_endpoint_getCluster(uint32_t s){
		uint8_t i;
		Cluster cluster;

		for( i = 0 ; i < 8 ; i++){
			cluster.sectors[i] = pfs_endpoint_callGetSectors(s + i);
		}

		return cluster;
	}



