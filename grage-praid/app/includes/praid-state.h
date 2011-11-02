/*
 * praid-state.h
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */




#include "linux-commons-queue.h"
#include "linux-commons-socket.h"

#ifndef PRAID_STATE_H_
#define PRAID_STATE_H_


	typedef struct {

		Boolean inUse;
		uint32 accessCount;

	} PPDAvailability;


	typedef struct {

		uint8_t ppdId;

		PPDAvailability availability;
		Queue pendingJobs;
		ListenSocket connection;

		Boolean connected;

		pthread_t storageThreadListener;
		pthread_t storageThreadSender;

	} PPDConnectionStorage;


	List praid_state_getPpdStorages();

	Boolean praid_state_storage_eq(PPDConnectionStorage * s1 , PPDConnectionStorage * s2);

	void praid_state_initializeStorages();

	void praid_state_addPpdStorage(PPDConnectionStorage * aState);

	PPDConnectionStorage * praid_state_buildPPDConnectionStorage(ListenSocket aSocket);


	PPDConnectionStorage * praid_balancer_selectStorage();


	void praid_state_removePddStorage(PPDConnectionStorage * storage);

#endif /* PRAID_STATE_H_ */
