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
#include "grage-commons.h"

	NipcMessage ppd_queues_buildNipcMessageFromJob(Job * theJob);


	Queue readingQueue;
	Queue writingQueue;

	uint32_t readingJobId;
	uint32_t writingJobId;


	void ppd_queues_initialize(){

		readingQueue = commons_queue_buildQueue((Boolean (*)(void *, void *))ppd_queues_isTheSameJob);
		writingQueue = commons_queue_buildQueue((Boolean (*)(void *, void *))ppd_queues_isTheSameJob);

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

		DiskSector currentSector;
		currentSector.sectorNumber = theJob->sectorId;
		memcpy( currentSector.sectorContent , theJob->sectorContent , sizeof(currentSector.sectorContent) );

		NipcMessage mes = ppd_queues_buildNipcMessageFromJob(theJob);

		if ( theJob->operationId ==  NIPC_OPERATION_ID_PUT_SECTORS ){
			ppd_persistence_readSector(currentSector , ppd_state_getDiskStartAddress() );
		} else if ( theJob->operationId ==  NIPC_OPERATION_ID_GET_SECTORS ) {
			ppd_persistence_writeSector( currentSector , ppd_state_getDiskStartAddress() );
		} else {
			//No es ni lectura ni escritura
			nipc_mbuilder_addResponseCode(mes, 1 );
		}

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
