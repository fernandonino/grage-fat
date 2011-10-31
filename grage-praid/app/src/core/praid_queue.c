/*
 * praid_queue.c
 *
 *  Created on: 10/10/2011
 *      Author: gonzalo
 */
#include <stdlib.h>

#include "linux-commons.h"
#include "linux-commons-queue.h"

#include "praid-queue.h"


	void praid_storage_queue_put(Queue aQueue , NipcMessage aMessage){
		Job * theJob = praid_jobs_buildJob(aMessage);
		commons_queue_put(aQueue , theJob);
	}

	NipcMessage praid_storage_queue_get(Queue aQueue){
		Job * theJob = commons_queue_get(aQueue);
		NipcMessage theMessage = praid_jobs_buildNipcMessageFromJob(theJob);
		free(theJob);
		return theMessage;
	}

	/*
	 * Ante una invocacion con alguno o ambos de los Jobs
	 * en NULL el resultado de la invocacion sera FALSE
	 */
	Boolean praid_jobs_eq(Job * j1 , Job * j2){
		if( j1 == NULL || j2 == NULL)
			return FALSE;
		return (j1->jobId == j2->jobId);
	}



	Job * praid_jobs_buildJob(NipcMessage mes){
		Job * theJob = (Job *) malloc(sizeof(Job));

		theJob->pfsSocket = mes.payload.pfsSocket;
		theJob->sectorId = mes.payload.diskSector.sectorNumber;
		theJob->payloadLength = mes.header.payloadLength;
		theJob->messageType = mes.header.messageType;
		theJob->operationId = mes.header.operationId;
		memcpy(theJob->sectorContent ,
				mes.payload.diskSector.sectorContent ,
				sizeof(mes.payload.diskSector.sectorContent));

		return theJob;
	}


	NipcMessage praid_jobs_buildNipcMessageFromJob(Job * theJob){
		NipcMessage mes;
		mes.header.payloadLength = theJob->payloadLength;
		mes.payload.pfsSocket = theJob->pfsSocket;
		mes.payload.diskSector.sectorNumber = theJob->sectorId;
		memcpy(mes.payload.diskSector.sectorContent ,
				theJob->sectorContent , sizeof(mes.payload.diskSector.sectorContent));
		mes.header.messageType = theJob->messageType;
		mes.header.operationId = theJob->operationId;
		return mes;
	}
