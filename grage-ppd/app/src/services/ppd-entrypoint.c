/*
 * ppd-entrypoint.c
 *
 *  Created on: 08/10/2011
 *      Author: gonzalo
 */

#include <unistd.h>
#include <pthread.h>

#include "grage-commons.h"
#include "linux-commons.h"
#include "linux-commons-socket.h"

#include "nipc-messaging.h"
#include "ppd-entrypoint.h"
#include "ppd-persistance.h"
#include "ppd-queues.h"
#include "ppd-state.h"

	void ppd_entrypoint_executePutSector(NipcMessage message);
	void ppd_entrypoint_executeGetSector(NipcMessage message);
	void ppd_entrypoint_doLunch();



	pthread_t entrypointThread;



	void ppd_entrypoint_launch(){
		pthread_create(&entrypointThread , NULL , (void * (*) (void* )) ppd_entrypoint_doLunch , NULL);
	}

	void ppd_entrypoint_executePutSector(NipcMessage message){
		ppd_queues_putInQueue(message);
	}

	void ppd_entrypoint_executeGetSector(NipcMessage message){
		ppd_queues_putInQueue(message);
	}

	void ppd_entrypoint_executeSyncPutSector(NipcMessage message){

		ppd_persistence_writeSector(message.payload.diskSector , ppd_state_getDiskStartAddress());
	}

	NipcMessage ppd_entrypoint_executeSyncGetSector(uint32_t sectorId){

		NipcMessage message = nipc_mbuilder_buildNipcMessage();
		DiskSector disk = ppd_persistence_readSector(sectorId , ppd_state_getDiskStartAddress());
		message = nipc_mbuilder_addDiskSector( message , disk );
		message = nipc_mbuilder_addMessageType( message , NIPC_MESSAGE_TYPE_SYNC_PROCESS);
		return message;
	}





	void ppd_entrypoint_doLunch(){

		while(TRUE){

			printf("esperando peticiones de PRAID/PFS\n");

			NipcMessage m = nipc_messaging_receive(ppd_state_getActiveSocket());

			/*
			 * Si se desconecta el PRAID
			 */
			if(m.header.operationId == NIPC_FIELD_BLANK)
				break;

			if(m.header.operationId == NIPC_OPERATION_ID_GET_SECTORS){
				ppd_entrypoint_executeGetSector(m);
			}else if (m.header.operationId == NIPC_OPERATION_ID_PUT_SECTORS){
				ppd_entrypoint_executePutSector(m);
			}else if(m.header.operationId == NIPC_OPERATION_ID_SYNC_PUT_SECTOR){
				ppd_entrypoint_executeSyncPutSector(m);
			}else if(m.header.operationId == NIPC_OPERATION_ID_SYNC_GET_SECTOR){
				NipcMessage m = ppd_entrypoint_executeSyncGetSector(m.payload.diskSector.sectorNumber);
				ppd_endpoint_responseGetSector(m);
			}
		}

		puts("finalizando la aplicacion por desconexion del PRAID");
		close(ppd_state_getActiveSocket());
		exit(1);

	}
