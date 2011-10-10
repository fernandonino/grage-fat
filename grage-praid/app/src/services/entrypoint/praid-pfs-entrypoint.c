/*
 * pfs-entrypoint.c
 *
 *  Created on: 06/10/2011
 *      Author: utn_so
 */

#include <pthread.h>

#include "nipc-messaging.h"
#include "linux-commons.h"
#include "linux-commons-list.h"
#include "praid-state.h"

	void praid_pfs_entrypoint_receiveInvocation(ListenSocket * ls);
	void praid_pfs_entrypoint_executePutSector(NipcMessage message);
	void praid_pfs_entrypoint_executeGetSector(NipcMessage message);


	pthread_t pfsSlaveThread;



	void praid_pfs_launchNewSlaveThread(ListenSocket ls){
		pthread_create(&pfsSlaveThread , NULL , (void * (*)(void *))praid_pfs_entrypoint_receiveInvocation , &ls);
	}

	void praid_pfs_entrypoint_receiveInvocation(ListenSocket * ls){
		NipcMessage message = nipc_messaging_receive(*ls);

		if(message.header.operationId == NIPC_OPERATION_ID_PUT_SECTORS){
			praid_pfs_entrypoint_executePutSector(message);
		}else if (message.header.operationId == NIPC_OPERATION_ID_PUT_SECTORS){
			praid_pfs_entrypoint_executeGetSector(message);
		}
	}


	void praid_pfs_entrypoint_executePutSector(NipcMessage message){

		Iterator * storages = commons_iterator_buildIterator(praid_state_getPpdStorages());

		while( commons_iterator_hasMoreElements( storages ) ){
			PPDConnectionStorage * storage = commons_iterator_next(storages);
			praid_storage_queue_put(storage->pendingJobs , message);
		}

		//liberar la memoria del iterador
	}


	void praid_pfs_entrypoint_executeGetSector(NipcMessage message){

		PPDConnectionStorage * storage = praid_balancer_selectStorage();
		praid_storage_queue_put(storage->pendingJobs , message);
	}
