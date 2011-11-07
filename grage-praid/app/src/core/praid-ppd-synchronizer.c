/*
 * praid_ppd_synchronizer.c
 *
 *  Created on: 03/11/2011
 *      Author: gonzalo
 */

#include <linux-commons-list.h>


#include "praid-sync.h"
#include "praid-queue.h"
#include "praid-state.h"
#include "praid-endpoint.h"



	Boolean replicationStatusActive;

	Boolean praid_sync_isReplicationActive(){
		return replicationStatusActive;
	}
	void praid_sync_setReplicationStatus(Boolean status){
		replicationStatusActive = status;
	}


	SyncProcessState syncState;

	void praid_sync_setSyncProcessState(SyncProcessState s){
		syncState = s;
	}
	SyncProcessState praid_sync_getSyncProcessState(){
		return syncState;
	}


	void praid_sync_incrementSyncSectorId(){
		syncState.sectorId += 1;
	}





	SyncProcessState praid_sync_buildSyncProcessState
	(uint32_t sid , PPDConnectionStorage * src , PPDConnectionStorage * dest){
		SyncProcessState syncProcess;

		syncProcess.sectorId = 0;
		syncProcess.source = src;
		syncProcess.destiny = dest;

		return syncProcess;
	}



	Boolean praid_ppd_sync_isValidReplication(){
		return (praid_state_getPpdStorages()->size >= 2);
	}


	PPDConnectionStorage * praid_ppd_sync_selectMasterStorage(){
		Iterator * ite = commons_iterator_buildIterator(praid_state_getPpdStorages());
		PPDConnectionStorage * storage = commons_iterator_next(ite);
		commons_misc_doFreeNull((void**)ite);
		return storage;
	}


	void praid_ppd_sync_fireSynchronization(PPDConnectionStorage * source ,
			PPDConnectionStorage * dest){

		puts("Sincronizando datos");

		SyncProcessState syncProcess = praid_sync_buildSyncProcessState(0 , source , dest);
		praid_sync_setSyncProcessState(syncProcess);

		praid_endpoint_ppd_callSyncGetSector(source->connection , 0);
	}


	void praid_ppd_sync_synchronize(PPDConnectionStorage * destiny){

		praid_sync_setReplicationStatus(TRUE);

		PPDConnectionStorage * master = praid_ppd_sync_selectMasterStorage();
		praid_ppd_sync_fireSynchronization(master , destiny);
	}

