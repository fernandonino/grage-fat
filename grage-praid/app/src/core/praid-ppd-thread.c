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



	void praid_ppd_thread_run(PPDConnectionStorage * );
	void praid_ppd_thread_listener(PPDConnectionStorage * );
	void praid_ppd_thread_sender(PPDConnectionStorage * );




	void praid_ppd_thread_launchNewSlaveThread(PPDConnectionStorage * aStorage){

		puts("Lanzando hilo entrypoint PPD");
		//pthread_create(&aStorage->storageThread , NULL , (void * (*)(void *)) praid_ppd_thread_run , aStorage);

		aStorage->connected = TRUE;
		pthread_create(&aStorage->storageThreadListener , NULL , (void * (*)(void *)) praid_ppd_thread_listener , aStorage);
		pthread_create(&aStorage->storageThreadSender , NULL , (void * (*)(void *)) praid_ppd_thread_sender , aStorage);
	}



	void praid_ppd_thread_listener(PPDConnectionStorage * storage){

		puts("Ejecutando Hilo Listener de proceso PPD");

		while(storage->connected){
			NipcMessage message = nipc_messaging_receive(storage->connection);


			if( nipc_mbuilder_isBlanckMessage(message) ){
				storage->connected = FALSE;
				break;
			}else{
				praid_endpoint_pfs_responseAndClose(message.payload.pfsSocket , message);
			}

		}

		printf("Eliminando PPD con Id %i tras desconexion\n" , storage->ppdId);
		praid_state_removePddStorage(storage);
	}

	void praid_ppd_thread_sender(PPDConnectionStorage * aStorage){
		puts("Ejecutando Hilo Sender de proceso PPD");

		while(aStorage->connected){

			while( commons_queue_isEmpty(aStorage->pendingJobs))
				sleep(2);

			aStorage->availability.inUse = TRUE;
			aStorage->availability.accessCount++;

			NipcMessage message = praid_storage_queue_get(aStorage->pendingJobs);

			praid_endpoint_ppd_sendMessage(aStorage->connection , message);

			aStorage->availability.inUse = FALSE;
		}
	}















	void praid_ppd_thread_run(PPDConnectionStorage * aStorage){

		puts("Ejecutando Hilo de proceso PPD");

		while(TRUE){

			if(commons_queue_isEmpty(aStorage->pendingJobs)){
				NipcMessage message = nipc_messaging_receiveCheckingTimeout(aStorage->connection);

				/*
				 * Si termina el time-out...
				 */
				if(message.header.operationId == NIPC_FIELD_BLANK)
					continue;
				/*
				 * Si se desconecto el PPD...
				 */
				else if(message.header.operationId == NIPC_OPERATION_ID_DISCONNECT)
					break;
				/*
				 * Si el PPD respondio...
				 */
				else
					praid_endpoint_pfs_responseAndClose(message.payload.pfsSocket , message);

			}else{

				//esto hay q sincronizarlo
				aStorage->availability.inUse = TRUE;
				aStorage->availability.accessCount++;

				NipcMessage message = praid_storage_queue_get(aStorage->pendingJobs);

				praid_endpoint_ppd_sendMessage(aStorage->connection , message);

				aStorage->availability.inUse = FALSE;
			}
		}

		puts("Eliminando PPD tras desconexion");
		praid_state_removePddStorage(aStorage);
	}
