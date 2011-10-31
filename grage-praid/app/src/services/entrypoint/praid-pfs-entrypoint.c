/*
 * pfs-entrypoint.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "linux-commons.h"
#include "linux-commons-list.h"

#include "nipc-messaging.h"

#include "praid-state.h"
#include "praid-queue.h"

	void praid_pfs_entrypoint_receiveInvocation(ListenSocket * ls);
	void praid_pfs_entrypoint_executePutSector(NipcMessage message);
	void praid_pfs_entrypoint_executeGetSector(NipcMessage message);


	pthread_t pfsSlaveThread;



	void praid_pfs_launchNewSlaveThread(ListenSocket ls){


		ListenSocket * theSocket = malloc(sizeof(ListenSocket));
		*theSocket = ls;

		puts("Lanzando hilo entrypoint PFS");
		pthread_create(&pfsSlaveThread , NULL , (void * (*)(void *))praid_pfs_entrypoint_receiveInvocation , theSocket);
	}

	void praid_pfs_entrypoint_receiveInvocation(ListenSocket * ls){

		puts("Ejecutando hilo entrypoint PFS");

		NipcMessage message = nipc_messaging_receive(*ls);

		if(message.header.operationId == NIPC_OPERATION_ID_PUT_SECTORS){
			praid_pfs_entrypoint_executePutSector(message);
		}else if (message.header.operationId == NIPC_OPERATION_ID_GET_SECTORS){
			praid_pfs_entrypoint_executeGetSector(message);
		}

		close(*ls);
	}


	void praid_pfs_entrypoint_executePutSector(NipcMessage message){

		puts("Ejecutando put sector");

		Iterator * storages = commons_iterator_buildIterator(praid_state_getPpdStorages());

		while( commons_iterator_hasMoreElements( storages ) ){
			PPDConnectionStorage * storage = commons_iterator_next(storages);
			praid_storage_queue_put(storage->pendingJobs , message);
		}

		free(storages);
	}


	void praid_pfs_entrypoint_executeGetSector(NipcMessage message){

		puts("Ejecutando get sector");

		PPDConnectionStorage * storage = praid_balancer_selectStorage();
		praid_storage_queue_put(storage->pendingJobs , message);
	}
