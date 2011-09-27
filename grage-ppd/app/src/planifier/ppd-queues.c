/*
 * ppd-queues.c
 *
 *  Created on: 26/09/2011
 *      Author: gonzalo
 */
#include <stdlib.h>
#include "linux-commons.h"
#include "linux-commons-queue.h"
#include "ppd-queues.h"

	Queue readingQueue;
	Queue writingQueue;


	void ppd_queues_initialize(){

		readingQueue = commons_queue_buildQueue((Boolean (*)(void *, void *))ppd_queues_isTheSameJob);
		writingQueue = commons_queue_buildQueue((Boolean (*)(void *, void *))ppd_queues_isTheSameJob);

	}



	Job * ppd_queues_buildJob(Object aData){
		Job * theJob = (Job *) malloc(sizeof(Job));
		theJob->jobData = aData;

		//TODO: generar el jobId
		return theJob;
	}




	/*
	 * Ante una invocacion con alguno o ambos de los Jobs
	 * en NULL el resultado de la invocacion sera FALSE
	 */
	Boolean ppd_queues_isTheSameJob(Job * j1 , Job * j2){
		if( j1 == NULL || j2 == NULL)
			return FALSE;
		return (j1->jobId == j2->jobId);
	}


	Object ppd_queues_pickForRead(){
		Job * theJob = commons_queue_get(readingQueue);
		Object theData = theJob->jobData;
		commons_misc_doFreeNull((void**)theJob);
		return theData;
	}

	Object ppd_queues_pickForWrite(){
		Job * theJob = commons_queue_get(writingQueue);
		Object theData = theJob->jobData;
		commons_misc_doFreeNull((void**)theJob);
		return theData;
	}


	void ppd_queues_putForRead(Object aData){
		Job * theJob = ppd_queues_buildJob(aData);
		commons_queue_put(readingQueue , theJob);
	}

	void ppd_queues_putForWrite(Object aData){
		Job * theJob = ppd_queues_buildJob(aData);
		commons_queue_put(writingQueue , theJob);
	}


