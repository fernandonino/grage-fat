/*
 * praid_ppd_synchronizer.c
 *
 *  Created on: 03/11/2011
 *      Author: gonzalo
 */

#include <linux-commons-list.h>

#include "praid-queue.h"
#include "praid-state.h"




	Boolean praid_ppd_sync_isValidReplication(){
		return (praid_state_getPpdStorages()->size >= 2);
	}

	PPDConnectionStorage * praid_ppd_sync_selectMasterStorage(){
		Iterator * ite = commons_iterator_buildIterator(praid_state_getPpdStorages());
		PPDConnectionStorage * storage = commons_iterator_next(ite);
		commons_misc_doFreeNull(ite);
		return storage;
	}


	void praid_ppd_sync_synchronize(PPDConnectionStorage * storage){
		PPDConnectionStorage * master = praid_ppd_sync_selectMasterStorage();
		praid_ppd_sync_synchronizeStorage(master , storage);
	}


	void praid_ppd_sync_synchronizeStorage(PPDConnectionStorage * source ,
			PPDConnectionStorage * dest){

	}
