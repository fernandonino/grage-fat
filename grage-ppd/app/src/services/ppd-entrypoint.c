/*
 * ppd-entrypoint.c
 *
 *  Created on: 08/10/2011
 *      Author: gonzalo
 */

#include <unistd.h>
#include <pthread.h>

#include <grage-commons.h>
#include <linux-commons.h>
#include <linux-commons-socket.h>
#include <linux-commons-file.h>

#include "nipc-messaging.h"

#include "ppd-entrypoint.h"
#include "ppd-endpoint.h"
#include "ppd-persistance.h"
#include "ppd-queues.h"
#include "ppd-state.h"
#include "ppd-configuration.h"
#include "ppd-sync.h"







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
		ppd_sync_replicatePut(message.header.payloadLength , message.payload.diskSector);
	}

	void ppd_entrypoint_executeSyncGetSector(){
		ppd_sync_replicateGet();
	}
	void ppd_entrypoint_endReplicationProcess(){
		ppd_sync_finishReplication();
	}




	void ppd_entrypoint_doLunch(){

		while(TRUE){

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

			}else if(m.header.messageType == NIPC_MESSAGE_TYPE_SYNC_PROCESS){

				if(m.header.operationId == NIPC_OPERATION_ID_SYNC_PUT_SECTOR){
					if(!ppd_state_isReplicationProcessActive())
						ppd_state_setReplicationProcessActive(TRUE);

					ppd_entrypoint_executeSyncPutSector(m);
				}else if(m.header.operationId == NIPC_OPERATION_ID_SYNC_GET_SECTOR){

					if(!ppd_state_isReplicationProcessActive())
						ppd_state_setReplicationProcessActive(TRUE);

					ppd_sync_replicateGet();
				}else if(m.header.operationId == NIPC_OPERATION_ID_SYNC_END){

					ppd_entrypoint_endReplicationProcess();

				}
			}else{
				printf(" Entro por aca: %s\n" , m.payload.diskSector.sectorContent);
			}
		}

		puts("finalizando la aplicacion por desconexion del PRAID");
		close(ppd_state_getActiveSocket());
		exit(1);

	}


	void ppd_entrypoint_runPfsSlaveThread(ListenSocket * ls){

	}

	void ppd_entrypoint_servePfsRequests(ListenSocket listenSocket){

	}

