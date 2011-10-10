


#include "linux-commons-queue.h"
#include "ppd-queues.h"
#include <math.h>
	uint32 currentPossition;

	Boolean ppd_alg_planif_strategy_scan(Object obj1, Object obj2){
		Job * oldJob = (Job *) obj1;
		Job * newJob = (Job *) obj2;

	}

	Boolean ppd_alg_planif_strategy_sstf(Object obj1, Object obj2){
		Job * oldJob = (Job *) obj1;
		Job * newJob = (Job *) obj2;

		if (abs(currentPossition-oldJob->sectorId)>abs(currentPossition-newJob->sectorId)){
			return TRUE;
		}
		return FALSE;
	}
