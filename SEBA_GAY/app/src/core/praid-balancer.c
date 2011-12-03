/*
 * praid-queue.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */
#include <stdlib.h>

#include "praid-state.h"
#include "linux-commons-list.h"



#define PRAID_BALANCER_WEIGHT_PENDING_RESPONSE			1
#define PRAID_BALANCER_WEIGHT_PENDING_REQUEST			2


	ThreadMutex loadBalancingMutex = PTHREAD_MUTEX_INITIALIZER;



	uint32_t praid_balancer_storageRanking(PPDConnectionStorage * storage){
		if(storage->pendingJobs == NULL){
			puts("[ Alerta roja, la cola de un storage esta en null, cagamos! ]");
			exit(1);
		}

		return storage->pendingJobs->size * PRAID_BALANCER_WEIGHT_PENDING_REQUEST
				+ storage->pendingResponses * PRAID_BALANCER_WEIGHT_PENDING_RESPONSE;
	}


	PPDConnectionStorage * praid_balancer_selectStorage(){

		commons_misc_lockThreadMutex(&loadBalancingMutex);

		Iterator * storages = commons_iterator_buildIterator(praid_state_getPpdStorages());

		uint32_t minimumRanking = 0;
		PPDConnectionStorage * selected = NULL;

		while( commons_iterator_hasMoreElements( storages ) ){

			PPDConnectionStorage * storage = commons_iterator_next(storages);

			if( selected == NULL || (praid_balancer_storageRanking(storage) < minimumRanking) ){

				minimumRanking = praid_balancer_storageRanking(storage);
				selected = storage;

				if(praid_balancer_storageRanking(storage) == 0)
					break;
			}
		}

		free(storages);
		commons_misc_unlockThreadMutex(&loadBalancingMutex);

		return selected;
	}
