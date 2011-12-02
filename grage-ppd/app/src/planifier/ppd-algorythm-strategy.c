


#include "linux-commons-queue.h"
#include "ppd-configuration.h"
#include "ppd-queues.h"
#include "ppd-console-entreypoint.h"
#include "ppd-utils.h"
#include <math.h>
	uint32 currentPossition;
//	void ppd_alg_setCurrentPossition(uint32 posicion){
//		currentPossition = posicion;
//	}
	Boolean ppd_alg_planif_strategy_look(Object obj1, Object obj2){
		Job * oldJob = (Job *) obj1;
		Job * newJob = (Job *) obj2;
//		int sectorMax = ppd_utils_get_sector_from_sectorofcilinder(
//				atoi(getPpdDiskSector()),
//				atoi(getPpdDiskCilinder()));
		currentPossition = ppd_utils_get_sector_from_sectorofcilinder(
				ppd_console_entrypoint_getPosicionCabezal().sectorNumber,
				ppd_console_entrypoint_getPosicionCabezal().pista) ;

		if ( abs(currentPossition - oldJob->sectorId) >
			abs(currentPossition - newJob->sectorId) && newJob->sectorId >= currentPossition){
			return TRUE;
		}
		return FALSE;
	}

	Boolean ppd_alg_planif_strategy_sstf(Object obj1, Object obj2){
		Job * oldJob = (Job *) obj1;
		Job * newJob = (Job *) obj2;
		currentPossition = ppd_utils_get_sector_from_sectorofcilinder(
				ppd_console_entrypoint_getPosicionCabezal().sectorNumber,
				ppd_console_entrypoint_getPosicionCabezal().pista) ;
		if ( abs(currentPossition - oldJob->sectorId) > abs(currentPossition - newJob->sectorId)){
			return TRUE;
		}
		return FALSE;
	}
