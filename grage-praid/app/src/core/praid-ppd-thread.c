/*
 * praid-ppd-thread.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include <linux-commons-strings.h>
#include <linux-commons-logging.h>
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


	void praid_ppd_redistributeJobs(Queue jobs);
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

				Boolean eq(Job * j){
					return j->sectorId == message.payload.diskSector.sectorNumber;
				}

				Job * job = commons_list_getNodeByCriteria(storage->sendedJobs , eq);
				if(job != NULL)
					commons_list_removeNode(storage->sendedJobs , job , free);
			}
		}

		if(commons_console_logging_isDefault()){
			printf("[ Se ha desconectado el PPD %i ]\n" , storage->id);
		}
		log_info_t( commons_string_concat("Se ha desconectado el PPD " , commons_misc_intToString(storage->id)));

		//TODO: probar esto, tiene bugs
		praid_ppd_checkIfContinueDenegatingRequests(storage->id);

		Queue sendedJobs = storage->sendedJobs;

		printf("[ Cantidad de trabajos pendientes que tenia el PPD %i ]\n" , sendedJobs->size);

		uint8_t ppdId = storage->id;
		praid_state_removePddStorage(storage);

		printf("[ Redistribuyendo peticiones tras la caida del PPD-%i ]\n" , ppdId);

		praid_ppd_redistributeJobs(sendedJobs);

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

			praid_storage_queue_put(aStorage->sendedJobs , message);

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







	void praid_ppd_redistributeJobs(Queue jobs){

		Iterator * ite = commons_iterator_buildIterator(jobs);

		while(commons_iterator_hasMoreElements(ite)){

			Job * job = commons_iterator_next(ite);

			if(job->operationId == NIPC_OPERATION_ID_GET_SECTORS){

				PPDConnectionStorage * bestCandidate = praid_balancer_selectStorage();

				if(bestCandidate == NULL){
					puts("[ No se encuentran PPDs conectados ]");
					break;
				}

				printf("[ Cantidad de ppds conectados: %i ]\n" , praid_state_getPpdStorages()->size);
				printf("[ Encolando pedido de sector %i en PPD-%i ]\n" , job->sectorId , bestCandidate->id);

				commons_queue_put(bestCandidate->pendingJobs , job);

			}else if(job->operationId == NIPC_OPERATION_ID_PUT_SECTORS){

				Iterator * storages = commons_iterator_buildIterator(praid_state_getPpdStorages());

				while(commons_iterator_hasMoreElements(storages)){

					PPDConnectionStorage * storage = commons_iterator_next(storages);

					printf("[ Encolando pedido de sector %i en PPD-%i ]\n" , job->sectorId , storage->id);

					commons_queue_put(storage->pendingJobs , job);
				}

				free(storages);
			}
		}

		free(ite);

	}


