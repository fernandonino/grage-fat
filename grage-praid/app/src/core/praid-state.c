/*
 * praid-state.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */

#include <unistd.h>

#include "linux-commons-socket.h"
#include "linux-commons-queue.h"

#include "praid-state.h"
#include "praid-queue.h"

	List ppdStorages;

	uint8_t availablePpdId = 0;


	List praid_state_getPpdStorages(){
		return ppdStorages;
	}

	Boolean praid_state_storage_eq(PPDConnectionStorage * s1 , PPDConnectionStorage * s2){
		if(s1 == NULL || s2 == NULL)
			return FALSE;
		return (s1->connection == s2->connection);
	}

	void praid_state_initializeStorages(){
		ppdStorages = commons_list_buildList(NULL , (Boolean (*)(void *, void *)) praid_state_storage_eq , commons_list_ORDER_ALWAYS_LAST);
	}

	void praid_state_addPpdStorage(PPDConnectionStorage * aState){
		commons_list_addNode(ppdStorages , aState);
	}

	PPDConnectionStorage * praid_state_buildPPDConnectionStorage(ListenSocket aSocket){

		PPDConnectionStorage * storage = (PPDConnectionStorage * ) malloc(sizeof(PPDConnectionStorage));

		//esto se podria sincronizar para asegurar q nunca habra bajo ninguna
		//circunstancia dos storages con el mismo id, pero igual es improbable y no tengo
		//ganas de hacerlo, asi q si se detecta un caso lo hago jeje.
		storage->ppdId = availablePpdId++;

		storage->pendingJobs = commons_queue_buildQueue((Boolean (*)(void *, void *)) praid_jobs_eq);
		storage->connection = aSocket;

		return storage;
	}


	void praid_state_removePddStorage(PPDConnectionStorage * storage){

		void removingStorage(PPDConnectionStorage * s){
			//remover la cola del storage
			free(s);
			close(s->connection);
		}

		commons_list_removeNode(ppdStorages , storage , (void (*)(Object)) removingStorage);
	}




