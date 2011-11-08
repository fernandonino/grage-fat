/*
 * praid-ppd-thread.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "linux-commons.h"
#include "linux-commons-errors.h"

#include "nipc-messaging.h"

#include "praid-state.h"
#include "praid-queue.h"
#include "praid-endpoint.h"
#include "praid-configuration.h"
#include "praid-sync.h"



	void praid_ppd_thread_listener(PPDConnectionStorage * );
	void praid_ppd_thread_sender(PPDConnectionStorage * );



	void praid_ppd_thread_launchNewSlaveThread(PPDConnectionStorage * aStorage){

		puts("Lanzando hilo entrypoint PPD");

		pthread_create(&aStorage->storageThreadListener , NULL , (void * (*)(void *)) praid_ppd_thread_listener , aStorage);
		pthread_create(&aStorage->storageThreadSender , NULL , (void * (*)(void *)) praid_ppd_thread_sender , aStorage);
	}




	void praid_ppd_thread_listener(PPDConnectionStorage * storage){

		puts("Ejecutando Hilo Listener de proceso PPD");

		while(praid_state_storage_isConnected(storage)){

			NipcMessage message = nipc_messaging_receive(storage->connection);

			if( nipc_mbuilder_isBlanckMessage(message) ){

				praid_state_storage_setDisconnected(storage);

				break;
			}else if(message.header.messageType == NIPC_MESSAGE_TYPE_SYNC_PROCESS){

				PPDConnectionStorage * dest = praid_sync_getSyncProcessState().destiny;

				if(message.header.operationId == NIPC_OPERATION_ID_SYNC_PUT_SECTOR){

					praid_endpoint_ppd_callSyncPutSector(dest->connection , message.payload.diskSector , message.header.payloadLength);

					praid_sync_incrementBytesSynchronized(message.header.payloadLength);

					char buffer[20];
					bzero(buffer , 20);
					memcpy(buffer , message.payload.diskSector.sectorContent , 20);

					printf("[Sincronizados %i bytes de un total de %i, contenido: %s ]\n" ,
							praid_sync_getSyncProcessState().bytesSynchronized,
							praid_sync_getSyncProcessState().source->volumeSize , buffer);

				}else if (message.header.operationId == NIPC_OPERATION_ID_SYNC_END){

					praid_sync_setReplicationStatus(FALSE);
					praid_endpoint_ppd_callFinishReplication(dest->connection);
					puts("Replicacion finalizada");
				}

			}else{
				praid_state_storage_decrementPendingResponses(storage);

				praid_endpoint_pfs_responseAndClose(message.payload.pfsSocket , message);
			}
		}

		printf("Eliminando PPD con Id %i tras desconexion\n" , storage->id);
		praid_state_removePddStorage(storage);
	}



	void praid_ppd_thread_sender(PPDConnectionStorage * aStorage){

		puts("Ejecutando Hilo Sender de process PPD");

		while(praid_state_storage_isConnected(aStorage)){

			while( commons_queue_isEmpty(aStorage->pendingJobs));

			/*
			 * Si se esta realizando la replicacion entonces no se envian trabajos
			 */
			if(praid_sync_isReplicationActive()){
				sleep(10);
				continue;
			}

			NipcMessage message = praid_storage_queue_get(aStorage->pendingJobs);

			praid_endpoint_ppd_sendMessage(aStorage->connection , message);

			if(message.header.operationId == NIPC_OPERATION_ID_GET_SECTORS){
				praid_state_storage_incrementPendingResponses(aStorage);
			}
		}
	}










