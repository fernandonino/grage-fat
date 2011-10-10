/*
 * ppd-planifier-worker.c
 *
 *  Created on: 09/10/2011
 *      Author: gonzalo
 */


#include <stdio.h>
#include <stdlib.h>

#include "linux-commons.h"


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
			//NipcMessage m = ppd_queues_pickForRead();

			printf("buscando mensajes de la cola de lectura\n");
			sleep(5);
		}
	}
