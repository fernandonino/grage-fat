/*
 * ppd-planifier-worker.c
 *
 *  Created on: 09/10/2011
 *      Author: gonzalo
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "linux-commons.h"
#include "grage-commons.h"
#include "nipc-messaging.h"
#include "ppd-queues.h"
#include "ppd-persistance.h"
#include "ppd-state.h"
#include "ppd-entrypoint.h"
#include "ppd-configuration.h"

	void ppd_planifier_worker_doJob(void * arg);


	pthread_t jobsThread;


	void ppd_planifier_worker_doJobs(){

		printf("lanzando workers\n");
		pthread_create(&jobsThread , NULL , (void * (*)(void *)) ppd_planifier_worker_doJob , NULL);
	}


	void ppd_planifier_worker_applyDelayForRead(){
		uint8_t delay = atoi( getPpdReadDelay() );
		if (delay != 0)
			sleep(delay);
	}

	void ppd_planifier_worker_applyDelayForWrite(){
		uint8_t delay = atoi( getPpdWriteDelay() );
		if (delay != 0)
			sleep(delay);
	}


	void ppd_planifier_worker_doJob(void * arg){

		while(TRUE){

			DiskSector currentSector;
			NipcMessage m = ppd_queues_pickFromQueue();

			currentSector.sectorNumber = m.payload.diskSector.sectorNumber;

			if ( m.header.operationId == NIPC_OPERATION_ID_PUT_SECTORS ) {

				ppd_planifier_worker_applyDelayForWrite();

				memcpy(currentSector.sectorContent , m.payload.diskSector.sectorContent , sizeof(currentSector.sectorContent));
				ppd_persistence_writeSector( &currentSector , ppd_state_getDiskStartAddress() );

			} else if ( m.header.operationId == NIPC_OPERATION_ID_GET_SECTORS ) {

				ppd_planifier_worker_applyDelayForRead();

				ppd_persistence_readSector(&currentSector , ppd_state_getDiskStartAddress());

				m = nipc_mbuilder_addResponseCode(m , NIPC_RESPONSE_CODE_SUCCESS);

				ppd_endpoint_responseGetSector(m);
			}
		}
	}



