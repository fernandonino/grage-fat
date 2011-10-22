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

		uint32_t minimumAccessCount = 0;
		PPDConnectionStorage * selected = NULL;

		while( commons_iterator_hasMoreElements( storages ) ){

			PPDConnectionStorage * storage = commons_iterator_next(storages);

			if(! storage->availability.inUse ){

				free(storages);
				commons_misc_unlockThreadMutex(&loadBalancingMutex);

				return storage;
			}else{
				if(minimumAccessCount == 0 && selected == NULL){
					minimumAccessCount = storage->availability.accessCount;
					selected = storage;
				}else if(storage->availability.accessCount < minimumAccessCount){
					minimumAccessCount = storage->availability.accessCount;
					selected = storage;
				}
			}
		}

		free(storages);
		commons_misc_unlockThreadMutex(&loadBalancingMutex);

		return selected;
	}
