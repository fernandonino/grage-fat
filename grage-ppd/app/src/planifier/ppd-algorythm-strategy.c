


#include "linux-commons-queue.h"
#include "ppd-queues.h"
#include <math.h>
	uint32 currentPossition;
	void ppd_alg_setCurrentPossition(uint32 posicion){
		currentPossition = posicion;
	}
	Boolean ppd_alg_planif_strategy_look(Object obj1, Object obj2){
		Job * oldJob = (Job *) obj1;
		Job * newJob = (Job *) obj2;

		if ( abs(currentPossition - oldJob->sectorId) >
			abs(currentPossition - newJob->sectorId) && newJob->sectorId >= currentPossition){
			return TRUE;
		}
		return FALSE;
	}

	Boolean ppd_alg_planif_strategy_sstf(Object obj1, Object obj2){
		Job * oldJob = (Job *) obj1;
		Job * newJob = (Job *) obj2;

		if ( abs(currentPossition - oldJob->sectorId) > abs(currentPossition - newJob->sectorId)){
			return TRUE;
		}
		return FALSE;
	}
