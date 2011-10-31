/*
 * praid-ppd-thread.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */

#include <unistd.h>
#include <pthread.h>

#include "linux-commons.h"
#include "linux-commons-errors.h"

#include "nipc-messaging.h"

#include "praid-state.h"
#include "praid-queue.h"
#include "praid-endpoint.h"


	void praid_ppd_thread_run(PPDConnectionStorage * );




	void praid_ppd_thread_launchNewSlaveThread(PPDConnectionStorage * aStorage){


		if (! commons_socket_setSocketTimeOut(aStorage->connection , 2 , 0) ){
			puts("No se pudo setear el timeout para el socket");
			exit(1);
		}

		puts("Lanzando hilo entrypoint PPD");
		pthread_create(&aStorage->storageThread , NULL , (void * (*)(void *)) praid_ppd_thread_run , aStorage);
	}


	void praid_ppd_thread_run(PPDConnectionStorage * aStorage){

		puts("Ejecutando Hilo de proceso PPD");

		while(TRUE){

			if(commons_queue_isEmpty(aStorage->pendingJobs)){
				NipcMessage message = nipc_messaging_receive(aStorage->connection);

				if(message.header.responseCode == NIPC_RESPONSE_CODE_NO_CODE)
					continue;

				praid_endpoint_pfs_responseGetSectors(message.payload.pfsSocket , message);

			}else{

				aStorage->availability.inUse = TRUE;
				aStorage->availability.accessCount++;

				NipcMessage message = praid_storage_queue_get(aStorage->pendingJobs);

				praid_endpoint_ppd_sendMessage(aStorage->connection , message);

				if(message.header.operationId == NIPC_OPERATION_ID_GET_SECTORS){
					NipcMessage response = nipc_messaging_receive(aStorage->connection);

					praid_endpoint_pfs_responseGetSectors(response.payload.pfsSocket , response);
				}

				aStorage->availability.inUse = FALSE;
			}
		}
	}
