/*
 * praid-ppd-thread.c
 *
 *  Created on: 06/10/2011
 *      Author: utn_so
 */
#include <unistd.h>
#include <pthread.h>
#include "linux-commons.h"
#include "nipc-messaging.h"
#include "linux-commons-errors.h"
#include "praid-state.h"
#include "praid-queue.h"

	void praid_ppd_thread_run(PPDConnectionStorage * );



	pthread_t ppdSlaveThread;



	void praid_ppd_thread_launchNewSlaveThread(PPDConnectionStorage * aStorage){
		pthread_create(&ppdSlaveThread , NULL , (void * (*)(void *)) praid_ppd_thread_run , aStorage);
	}


	void praid_ppd_thread_run(PPDConnectionStorage * aStorage){

		//1) realizar la sincronizacion

		while(TRUE){

			while(commons_queue_isEmpty(aStorage->pendingJobs))
				sleep(5);

			aStorage->availability.inUse = TRUE;
			aStorage->availability.accessCount++;

			NipcMessage message = praid_storage_queue_get(aStorage->pendingJobs);

			nipc_messaging_send(aStorage->connection , message);

			if(message.header.operationId == NIPC_OPERATION_ID_GET_SECTORS){
				NipcMessage response = nipc_messaging_receive(aStorage->connection);

				//enviamos al pfs por el endpoint la respuesta
			}

			aStorage->availability.inUse = FALSE;
		}
	}
