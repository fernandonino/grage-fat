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


	void ppd_planifier_worker_doWritingJobs(void * arg);
	void ppd_planifier_worker_doReadingJobs(void * arg);




	pthread_t readingJobThread;
	pthread_t writingJobThread;


	void ppd_planifier_worker_doJobs(){

		printf("lanzando workers\n");

		pthread_create(&writingJobThread , NULL , (void * (*)(void *)) ppd_planifier_worker_doWritingJobs , NULL);
		pthread_create(&readingJobThread , NULL , (void * (*)(void *)) ppd_planifier_worker_doReadingJobs , NULL);
	}

	void ppd_planifier_worker_doWritingJobs(void * arg){

		while(TRUE){
			//NipcMessage m = ppd_queues_pickForWrite();

			printf("buscando mensajes de la cola de escritura\n");
			sleep(5);
		}
	}

	void ppd_planifier_worker_doReadingJobs(void * arg){

		while(TRUE){
			DiskSector currentSector;
			NipcMessage m = ppd_queues_pickForRead();

			currentSector.sectorNumber = m.payload.diskSector.sectorNumber;

			if ( m.header.operationId == NIPC_OPERATION_ID_PUT_SECTORS ) {
				memcpy(currentSector.sectorContent , m.payload.diskSector.sectorContent , sizeof(currentSector.sectorContent));
				ppd_persistence_writeSector( &currentSector , ppd_state_getDiskStartAddress() );
			} else if ( m.header.operationId == NIPC_OPERATION_ID_PUT_SECTORS ) {
				ppd_persistence_readSector(&currentSector , ppd_state_getDiskStartAddress());
			}

			printf("buscando mensajes de la cola de lectura\n");
		}
	}



