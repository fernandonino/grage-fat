/*
 * pfs-entrypoint.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux-commons.h>
#include <linux-commons-list.h>
#include <linux-commons-console-logging.h>

#include "nipc-messaging.h"

#include "praid-state.h"
#include "praid-queue.h"
#include "praid-utils.h"
#include "praid-configuration.h"




	Boolean praid_pfs_entrypoint_processRequest(ListenSocket * pfsSocket);
	void praid_pfs_entrypoint_receiveInvocation(ListenSocket * ls);
	void praid_pfs_entrypoint_executePutSector(NipcMessage message);
	void praid_pfs_entrypoint_executeGetSector(NipcMessage message);




	pthread_t pfsSlaveThread;



	void praid_pfs_launchNewSlaveThread(ListenSocket ls){

		ListenSocket * theSocket = malloc(sizeof(ListenSocket));
		*theSocket = ls;

		pthread_create(&pfsSlaveThread , NULL , (void * (*)(void *))praid_pfs_entrypoint_receiveInvocation , theSocket);
	}

	void praid_pfs_entrypoint_receiveInvocation(ListenSocket * ls){

		if( praid_conf_isPooledConnections()){

			while(praid_pfs_entrypoint_processRequest(ls));

		}else{
			praid_pfs_entrypoint_processRequest(ls);
		}

		puts("Se murio un hilo");

	}



	Boolean praid_pfs_entrypoint_processRequest(ListenSocket * pfsSocket){

		NipcMessage message = nipc_messaging_receive(*pfsSocket);
		message.payload.pfsSocket = *pfsSocket;

		if(message.header.operationId == NIPC_OPERATION_ID_GET_SECTORS){

			puts("[ Recibiendo peticion GET Sectores ]");
			printf("[ Solicitando sectorId: %i ]\n" , message.payload.diskSector.sectorNumber);

			message.payload.pfsSocket = *pfsSocket;

			praid_pfs_entrypoint_executeGetSector(message);

		}else if(message.header.operationId == NIPC_OPERATION_ID_PUT_SECTORS){

			puts("[ Recibiendo peticion PUT Sectores ]");
			printf("[ Solicitando sectorId: %i ]\n" , message.payload.diskSector.sectorNumber);

			message.payload.pfsSocket = *pfsSocket;

			praid_pfs_entrypoint_executePutSector(message);

		}else if(nipc_mbuilder_isBlanckMessage(message)){

			return FALSE;
		}

		return TRUE;
	}





	void praid_pfs_entrypoint_executePutSector(NipcMessage message){


		praid_utils_printLines();

		if(commons_console_logging_isDefault()){
			puts("[ Ejecutando PUT ]");
			printf("[ Se escribira sobre los PPD's con los siguientes ID: ");
		}

		Iterator * storages = commons_iterator_buildIterator(praid_state_getPpdStorages());

		while( commons_iterator_hasMoreElements( storages ) ){

			PPDConnectionStorage * storage = commons_iterator_next(storages);

			praid_storage_queue_put(storage->pendingJobs , message);

			if(commons_console_logging_isDefault()){
				printf("%i " , storage->id);

				if(commons_iterator_hasMoreElements(storages))
					printf(" , ");
			}
		}

		if(commons_console_logging_isDefault())
			printf("\n");


		free(storages);
	}


	void praid_pfs_entrypoint_executeGetSector(NipcMessage message){

		praid_utils_printLines();

		PPDConnectionStorage * storage = praid_balancer_selectStorage();

		if(commons_console_logging_isDefault())
			printf("[ Se realizará un GET (PPD: %i , SectorId: %i) ]\n"
					, storage->id , message.payload.diskSector.sectorNumber);

		praid_storage_queue_put(storage->pendingJobs , message);


	}
