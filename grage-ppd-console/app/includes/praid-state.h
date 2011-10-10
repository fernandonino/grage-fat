/*
 * praid-state.h
 *
 *  Created on: 06/10/2011
 *      Author: utn_so
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

		PPDAvailability availability;

		Queue pendingJobs;

		ListenSocket connection;

	} PPDConnectionStorage;


#endif /* PRAID_STATE_H_ */
