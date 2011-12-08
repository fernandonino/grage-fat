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
#include <signal.h>

#include "linux-commons.h"
#include "grage-commons.h"
#include "nipc-messaging.h"

#include "ppd-queues.h"
#include "ppd-persistance.h"
#include "ppd-state.h"
#include "ppd-entrypoint.h"
#include "ppd-configuration.h"
#include "ppd-endpoint.h"
#include "ppd-console-entreypoint.h"
#include "ppd-utils.h"

	void ppd_planifier_worker_doJob(void * arg);


	void ppd_planifier_worker_doJobs(){
		pthread_t jobsThread;
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

			NipcMessage m = ppd_queues_pickFromQueue();

			if ( m.header.operationId == NIPC_OPERATION_ID_PUT_SECTORS ) {

				ppd_planifier_worker_applyDelayForWrite();

				ppd_persistence_writeSector( m.payload.diskSector );

			} else if ( m.header.operationId == NIPC_OPERATION_ID_GET_SECTORS ) {

				ppd_planifier_worker_applyDelayForRead();

				DiskSector disk = ppd_persistence_readSector(m.payload.diskSector.sectorNumber);

				m = nipc_mbuilder_addDiskSector( m , disk);
				m = nipc_mbuilder_addResponseCode(m , NIPC_RESPONSE_CODE_SUCCESS);

				ppd_endpoint_responseGetSector(m);
			}
			if (m.header.operationId == 69){

				MessageConsolePPD mensaje;
				mensaje.pistaSector.pista = ppd_utils_get_cilinder_from_sector(m.payload.diskSector.sectorNumber);
				mensaje.pistaSector.sectorNumber = ppd_utils_get_sectorofcilinder_from_sector(m.payload.diskSector.sectorNumber);
				if ((mensaje.timeInMiliseconds = ppd_console_entrypoint_TiempoConsumido(
								mensaje.pistaSector.pista,
								mensaje.pistaSector.sectorNumber))==-1){
					mensaje.messageID = MESSAGE_ID_ERROR;
				}else{
					mensaje.messageID = MESSAGE_ID_TIEMPO_CONSUMIDO;

				}
				commons_socket_sendBytes(ppd_state_getPpdConsoleSocket(), &mensaje,
									sizeof mensaje);
				pthread_kill( (pthread_t)m.payload.pfsSocket , SIGQUIT);

			}
			ppd_console_entrypoint_setearPosicionCabezal(
					ppd_utils_get_cilinder_from_sector(m.payload.diskSector.sectorNumber),
					ppd_utils_get_sectorofcilinder_from_sector(m.payload.diskSector.sectorNumber));
			//ppd_alg_setCurrentPossition(m.payload.diskSector.sectorNumber);
		}
	}



