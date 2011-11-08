/*
 * ppd-entrypoint.c
 *
 *  Created on: 08/10/2011
 *      Author: gonzalo
 */

#include <unistd.h>
#include <pthread.h>

#include <grage-commons.h>
#include <linux-commons.h>
#include <linux-commons-socket.h>
#include <linux-commons-file.h>

#include "nipc-messaging.h"

#include "ppd-entrypoint.h"
#include "ppd-persistance.h"
#include "ppd-queues.h"
#include "ppd-state.h"
#include "ppd-configuration.h"


	void ppd_entrypoint_executePutSector(NipcMessage message);
	void ppd_entrypoint_executeGetSector(NipcMessage message);
	void ppd_entrypoint_doLunch();



	pthread_t entrypointThread;


	Boolean inReplicationProcess = FALSE;

	void ppd_entrypoint_launch(){
		pthread_create(&entrypointThread , NULL , (void * (*) (void* )) ppd_entrypoint_doLunch , NULL);
	}

	void ppd_entrypoint_executePutSector(NipcMessage message){
		ppd_queues_putInQueue(message);
	}

	void ppd_entrypoint_executeGetSector(NipcMessage message){
		ppd_queues_putInQueue(message);
	}

	void ppd_entrypoint_executeSyncPutSector(NipcMessage message){

		//ppd_persistence_writeSector(message.payload.diskSector , ppd_state_getDiskStartAddress());

		if(ppd_state_getReplicationDiskVolume() == NULL){

			File * fatFile = commons_file_openOrCreateFile(ppd_conf_getDiskPath());

			if( fatFile == NULL){
				printf("No existe el archivo %s\n." , ppd_conf_getDiskPath());
				exit(1);
			}

			ppd_state_setReplicationDiskVolume(fatFile);

		}

		if(message.header.payloadLength > 0){

			if( message.header.payloadLength < sizeof(message.payload.diskSector.sectorContent))
				puts("Escribiendo el ultimo tramo");

			size_t bytesWritten = fwrite(message.payload.diskSector.sectorContent ,	sizeof(char) ,
					message.header.payloadLength , ppd_state_getReplicationDiskVolume());

			if(bytesWritten < 0){
				puts("Error en la escritura del archivo");
				puts("No se pudo finalizar la replicacion");
				exit(1);
			}

			fflush(ppd_state_getReplicationDiskVolume());
		}
	}


	void ppd_entrypoint_endReplicationProcess(){

		if(inReplicationProcess){

			File * file = ppd_state_getReplicationDiskVolume();
			fclose(file);
			ppd_state_setReplicationDiskVolume(NULL);
			inReplicationProcess = FALSE;
		}

		ppd_planifier_worker_doJobs();
	}



	void ppd_entrypoint_processFileRead(File * fatFile){


		DiskSector disk = commons_buildDiskSector();

		uint32_t allReaded = 0;
		size_t bytesReaded = fread(disk.sectorContent , sizeof(char) , sizeof(disk.sectorContent) , fatFile);

		allReaded += bytesReaded;

		while( ! feof(fatFile) ){

			ppd_endpoint_buildAndSendSyncMessage(bytesReaded , disk);

			disk = commons_buildDiskSector();

			bytesReaded = fread(disk.sectorContent , sizeof(char) , sizeof(disk.sectorContent) , fatFile);
			allReaded += bytesReaded;

		}

		if(bytesReaded > 0){
			ppd_endpoint_buildAndSendSyncMessage(bytesReaded , disk);
		}

	}



	void ppd_entrypoint_startReplicationInChunk(){

		File * fatFile = commons_file_openFile(ppd_conf_getDiskPath());

		if( fatFile == NULL){
			printf("No existe el archivo %s\n." , ppd_conf_getDiskPath());
			exit(1);
		}

		ppd_state_setReplicationDiskVolume(fatFile);

		ppd_entrypoint_processFileRead(fatFile);

		ppd_entrypoint_endReplicationProcess();

		ppd_endpoint_sendFinishReplication();
	}




	void ppd_entrypoint_doLunch(){

		while(TRUE){

			NipcMessage m = nipc_messaging_receive(ppd_state_getActiveSocket());

			/*
			 * Si se desconecta el PRAID
			 */
			if(m.header.operationId == NIPC_FIELD_BLANK)
				break;

			if(m.header.operationId == NIPC_OPERATION_ID_GET_SECTORS){

				if(!inReplicationProcess)
					ppd_entrypoint_executeGetSector(m);

			}else if (m.header.operationId == NIPC_OPERATION_ID_PUT_SECTORS){

				if(!inReplicationProcess)
					ppd_entrypoint_executePutSector(m);

			}else if(m.header.messageType == NIPC_MESSAGE_TYPE_SYNC_PROCESS){

				if(m.header.operationId == NIPC_OPERATION_ID_SYNC_PUT_SECTOR){
					if(!inReplicationProcess)
						inReplicationProcess = TRUE;

					ppd_entrypoint_executeSyncPutSector(m);
				}else if(m.header.operationId == NIPC_OPERATION_ID_SYNC_GET_SECTOR){

					if(!inReplicationProcess)
						inReplicationProcess = TRUE;

					ppd_entrypoint_startReplicationInChunk();
				}else if(m.header.operationId == NIPC_OPERATION_ID_SYNC_END){

					ppd_entrypoint_endReplicationProcess();

				}
			}
		}

		puts("finalizando la aplicacion por desconexion del PRAID");
		close(ppd_state_getActiveSocket());
		exit(1);

	}


	void ppd_entrypoint_runPfsSlaveThread(ListenSocket * ls){

	}

	void ppd_entrypoint_servePfsRequests(ListenSocket listenSocket){

	}

