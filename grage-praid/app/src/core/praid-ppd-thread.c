/*
 * praid-ppd-thread.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include <linux-commons.h>
#include <linux-commons-errors.h>
#include <linux-commons-console-logging.h>

#include "nipc-messaging.h"

#include "praid-state.h"
#include "praid-queue.h"
#include "praid-endpoint.h"
#include "praid-configuration.h"
#include "praid-sync.h"
#include "praid-utils.h"


	void praid_ppd_thread_listener(PPDConnectionStorage * );
	void praid_ppd_thread_sender(PPDConnectionStorage * );
	void praid_ppd_checkIfContinueDenegatingRequests(uint8_t ppdid);


	void praid_ppd_thread_launchNewSlaveThread(PPDConnectionStorage * aStorage){

		pthread_create(&aStorage->storageThreadListener , NULL , (void * (*)(void *)) praid_ppd_thread_listener , aStorage);
		pthread_create(&aStorage->storageThreadSender , NULL , (void * (*)(void *)) praid_ppd_thread_sender , aStorage);
	}




	void praid_ppd_thread_listener(PPDConnectionStorage * storage){

		while(praid_state_storage_isConnected(storage)){

			NipcMessage message = nipc_messaging_receive(storage->connection);

			if( nipc_mbuilder_isBlanckMessage(message) ){

				praid_state_storage_setDisconnected(storage);

				break;

			}else if(message.header.messageType == NIPC_MESSAGE_TYPE_SYNC_PROCESS){

				PPDConnectionStorage * dest = praid_sync_getSyncProcessState().destiny;

				SyncProcessState syncState = praid_sync_getSyncProcessState();

				if(message.header.operationId == NIPC_OPERATION_ID_SYNC_PUT_SECTOR){

					praid_sync_incrementSyncSectorsCounter();

					praid_endpoint_ppd_callSyncPutSector(dest->connection , message.payload.diskSector , message.header.payloadLength);

					praid_sync_incrementBytesSynchronized(message.header.payloadLength);

					praid_utils_printSynchingInformation(syncState);

				}else if (message.header.operationId == NIPC_OPERATION_ID_SYNC_END){

					praid_endpoint_ppd_callProcessJobs(dest->connection);

					praid_utils_printEndSynchingInformation(syncState);

					praid_utils_printClusterInformation();

					praid_sync_setReplicationStatusActive(FALSE);

					praid_utils_printLines();
				}

			}else{
				praid_state_storage_decrementPendingResponses(storage);

				praid_endpoint_pfs_responseAndClose(message.payload.pfsSocket , message);
			}
		}

		if(commons_console_logging_isDefault()){
			printf("[ Se ha desconectado el PPD %i ]\n" , storage->id);
			printf("[ Eliminando PPD con Id %i tras desconexion ]\n" , storage->id);
		}

		//TODO: probar esto, tiene bugs
		praid_ppd_checkIfContinueDenegatingRequests(storage->id);

		praid_state_removePddStorage(storage);

		praid_utils_printClusterInformation();

	}



	void praid_ppd_thread_sender(PPDConnectionStorage * aStorage){

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



	void praid_ppd_checkIfContinueDenegatingRequests(uint8_t ppdid){

		SyncProcessState state = praid_sync_getSyncProcessState();

		if( (state.source != NULL && state.source->id == ppdid)
				|| (state.destiny != NULL && state.destiny->id == ppdid)){
			if( praid_sync_isReplicationActive())
				praid_sync_setReplicationStatusActive(FALSE);
		}
	}






