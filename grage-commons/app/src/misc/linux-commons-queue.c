/*
 * linux-commons-queue.c
 *
 *  Created on: 26/09/2011
 *      Author: gonzalo
 */

#include "linux-commons-queue.h"



	Object commons_queue_get(Queue aQueue){
		if(aQueue == NULL || aQueue->elements == NULL)
			return NULL;

		Node * theFirst = aQueue->elements;
		aQueue->elements = theFirst->next;
		aQueue->size = --aQueue->size;

		Object theData = theFirst->data;
		commons_misc_doFreeNull(theFirst);

		return theData;
	}


	void commons_queue_put( Queue aQueue , Object anElement){
		commons_list_addNode(aQueue , anElement);
	}



	void commons_queue_destroy(Queue aQueue){
		printf("fabi se la come!");
	}


	/*
	 * Crea una cola loca :D...
	 */
	Queue commons_queue_buildQueue(
			Boolean (*aEqualityCriteria)(Object , Object)){

		ThreadMutex m = PTHREAD_MUTEX_INITIALIZER;

		Queue theQueue = malloc(sizeof(L));
		theQueue->size = 0;
		theQueue->elements = NULL;
		theQueue->mutex = m;
		theQueue->equalityCriteria = aEqualityCriteria;
		theQueue->sortingCriteria = commons_list_ORDER_ALWAYS_LAST;
		return theQueue;
	}
