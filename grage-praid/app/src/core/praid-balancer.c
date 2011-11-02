/*
 * praid-queue.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */
#include <stdlib.h>

#include "praid-state.h"
#include "linux-commons-list.h"


	ThreadMutex loadBalancingMutex = PTHREAD_MUTEX_INITIALIZER;



	PPDConnectionStorage * praid_balancer_selectStorage(){

		commons_misc_lockThreadMutex(&loadBalancingMutex);

		Iterator * storages = commons_iterator_buildIterator(praid_state_getPpdStorages());

		uint32_t minimumPendingResponsesCount = 0;
		PPDConnectionStorage * selected = NULL;

		while( commons_iterator_hasMoreElements( storages ) ){

			PPDConnectionStorage * storage = commons_iterator_next(storages);

			if( selected == NULL || (storage->pendingResponses < minimumPendingResponsesCount) ){

				minimumPendingResponsesCount = storage->pendingResponses;
				selected = storage;

				if(selected->pendingResponses == 0)
					break;
			}
		}

		free(storages);
		commons_misc_unlockThreadMutex(&loadBalancingMutex);

		return selected;
	}
