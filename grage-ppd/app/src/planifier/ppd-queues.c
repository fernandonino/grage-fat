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
#include "ppd-planifier.h"
#include "grage-commons.h"
#include "ppd-persistance.h"
#include "ppd-state.h"


	NipcMessage ppd_queues_buildNipcMessageFromJob(Job * theJob);


	Queue readingQueue;
	Queue writingQueue;

	uint32_t readingJobId;
	uint32_t writingJobId;


	void ppd_queues_initialize(){

		if(commons_string_equals(getPpdAlgoritmo() , "scan")){
			readingQueue = commons_queue_buildQueueWithSortingCriteria(
					(Boolean (*)(void *, void *))ppd_queues_isTheSameJob ,
					(Boolean (*)(void *, void *))ppd_alg_planif_strategy_scan);

		}else if(commons_string_equals(getPpdAlgoritmo() , "sstf")){
			readingQueue = commons_queue_buildQueueWithSortingCriteria(
					(Boolean (*)(void *, void *))ppd_queues_isTheSameJob ,
					(Boolean (*)(void *, void *))ppd_alg_planif_strategy_sstf);
		}
	}



	Job * ppd_queues_buildJob(NipcMessage mes){
		Job * theJob = (Job *) malloc(sizeof(Job));

		theJob->pfsSocket = mes.payload.pfsSocket;
		theJob->sectorId = mes.payload.diskSector.sectorNumber;
		theJob->payloadLength = mes.header.payloadLength;
		theJob->messageType = mes.header.messageType;
		theJob->operationId = mes.header.operationId;
		memcpy(theJob->sectorContent ,
				mes.payload.diskSector.sectorContent ,
				mes.header.payloadLength);

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



	NipcMessage  ppd_queues_pickForRead(){
			Job * theJob = commons_queue_get(readingQueue);
			NipcMessage mes = ppd_queues_buildNipcMessageFromJob(theJob);

			commons_misc_doFreeNull((void**)theJob);
			return mes;
	}


	NipcMessage ppd_queues_pickForWrite(){
		Job * theJob = commons_queue_get(writingQueue);

		NipcMessage mes = ppd_queues_buildNipcMessageFromJob(theJob);

		commons_misc_doFreeNull((void**)theJob);
		return mes;
	}


	void ppd_queues_putForRead(NipcMessage mes){
		Job * theJob = ppd_queues_buildJob(mes);
		theJob->jobId = readingJobId++;
		commons_queue_put(readingQueue , theJob);
	}


	void ppd_queues_putForWrite(NipcMessage mes){
		Job * theJob = ppd_queues_buildJob(mes);
		theJob->jobId = writingJobId++;
		commons_queue_put(writingQueue , theJob);
	}




	NipcMessage ppd_queues_buildNipcMessageFromJob(Job * theJob){
		NipcMessage mes;
		mes.header.payloadLength = theJob->payloadLength;
		mes.payload.pfsSocket = theJob->pfsSocket;
		mes.payload.diskSector.sectorNumber = theJob->sectorId;
		memcpy(mes.payload.diskSector.sectorContent ,
				theJob->sectorContent , theJob->payloadLength);
		mes.header.messageType = theJob->messageType;
		mes.header.operationId = theJob->operationId;

		return mes;
	}
