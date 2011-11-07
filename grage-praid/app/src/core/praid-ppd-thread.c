/*
 * praid-ppd-thread.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "linux-commons.h"
#include "linux-commons-errors.h"

#include "nipc-messaging.h"

#include "praid-state.h"
#include "praid-queue.h"
#include "praid-endpoint.h"
#include "praid-configuration.h"
#include "praid-sync.h"



	void praid_ppd_thread_listener(PPDConnectionStorage * );
	void praid_ppd_thread_sender(PPDConnectionStorage * );



	void praid_ppd_thread_launchNewSlaveThread(PPDConnectionStorage * aStorage){

		puts("Lanzando hilo entrypoint PPD");

		if(praid_ppd_sync_isValidReplication())
			praid_ppd_sync_synchronize(aStorage);

		pthread_create(&aStorage->storageThreadListener , NULL , (void * (*)(void *)) praid_ppd_thread_listener , aStorage);
		pthread_create(&aStorage->storageThreadSender , NULL , (void * (*)(void *)) praid_ppd_thread_sender , aStorage);
	}




	void praid_ppd_thread_printStatus(uint32_t sectorId , uint32_t totalSectorsCount , uint8_t masterId , uint8_t slaveId ){
		if(sectorId % 1024 == 0)
			printf("Copiando sector %i/%i desde PPD %i hacia PPD %i \n", sectorId , totalSectorsCount , masterId ,slaveId);

	}


	void praid_ppd_thread_syncDiskSectorFromMasterToSlave
	(DiskSector diskSector , PPDConnectionStorage * master , PPDConnectionStorage * slave){

		praid_ppd_thread_printStatus(praid_sync_getSyncProcessState().sectorId , master->sectorsCount , master->id , slave->id);

		praid_endpoint_ppd_callSyncPutSector(slave->connection , diskSector);

		praid_sync_incrementSyncSectorId();

		if(praid_sync_getSyncProcessState().sectorId == master->sectorsCount){
			praid_sync_setReplicationStatus(FALSE);
			puts("Replicacion finalizada");
		}else{
			praid_endpoint_ppd_callSyncGetSector(master->connection , praid_sync_getSyncProcessState().sectorId);
		}
	}




	void praid_ppd_thread_listener(PPDConnectionStorage * storage){

		puts("Ejecutando Hilo Listener de proceso PPD");

		while(praid_state_storage_isConnected(storage)){

			NipcMessage message = nipc_messaging_receive(storage->connection);

			if( nipc_mbuilder_isBlanckMessage(message) ){

				praid_state_storage_setDisconnected(storage);

				break;
			}else if(message.header.messageType == NIPC_MESSAGE_TYPE_SYNC_PROCESS){

				PPDConnectionStorage * dest = praid_sync_getSyncProcessState().destiny;

				praid_ppd_thread_syncDiskSectorFromMasterToSlave(message.payload.diskSector , storage , dest);

			}else{
				praid_state_storage_decrementPendingResponses(storage);

				praid_endpoint_pfs_responseAndClose(message.payload.pfsSocket , message);
			}
		}

		printf("Eliminando PPD con Id %i tras desconexion\n" , storage->id);
		praid_state_removePddStorage(storage);
	}



	void praid_ppd_thread_sender(PPDConnectionStorage * aStorage){

		puts("Ejecutando Hilo Sender de process PPD");

		while(praid_state_storage_isConnected(aStorage)){

			while( commons_queue_isEmpty(aStorage->pendingJobs));

			/*
			 * Si se esta realizando la replicacion entonces no se envian trabajos
			 */
			if(praid_sync_isReplicationActive()){
				sleep(10);
				continue;
			}

			NipcMessage message = praid_storage_queue_get(aStorage->pendingJobs);

			praid_endpoint_ppd_sendMessage(aStorage->connection , message);

			if(message.header.operationId == NIPC_OPERATION_ID_GET_SECTORS){
				praid_state_storage_incrementPendingResponses(aStorage);
			}
		}
	}










