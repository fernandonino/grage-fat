/*
 * ppd-sync.c
 *
 *  Created on: 08/11/2011
 *      Author: gonzalo
 */
#include <stdlib.h>
#include <linux-commons-file.h>
#include <grage-commons.h>

#include "ppd-configuration.h"
#include "ppd-sync.h"
#include "ppd-state.h"
#include "ppd-endpoint.h"
#include "ppd-planifier.h"
#include "ppd-persistance.h"

	uint32_t allBytesWritten = 0;
	uint32_t sectorsWrittenCount = 0;

	uint32_t allBytesReaded = 0;
	uint32_t sectorsReadedCount = 0;





	void ppd_sync_replicateGet(){
		File * fatFile = commons_file_openFile(ppd_conf_getDiskPath());

		if( fatFile == NULL){
			printf("No existe el archivo %s\n." , ppd_conf_getDiskPath());
			exit(1);
		}

		ppd_state_setReplicationDiskVolume(fatFile);

		ppd_sync_processFileSectors(fatFile);

		ppd_sync_finishReplication();

		ppd_endpoint_sendFinishReplication();
	}





	void ppd_sync_replicatePut(uint16_t bytesCount , DiskSector sector){

		if(ppd_state_getReplicationDiskVolume() == NULL){

			File * fatFile = fopen(ppd_conf_getDiskPath() , "w");
					//commons_file_openOrCreateFile(ppd_conf_getDiskPath());

			if( fatFile == NULL){
				printf("No existe el archivo %s\n." , ppd_conf_getDiskPath());
				exit(1);
			}
			ppd_state_setReplicationDiskVolume(fatFile);
		}

		if(bytesCount > 0){

			if( bytesCount < sizeof(sector.sectorContent))
				puts("Escribiendo el ultimo tramo");

			size_t bytesWritten = fwrite(sector.sectorContent ,	sizeof(char) ,
					bytesCount , ppd_state_getReplicationDiskVolume());

			allBytesWritten += bytesWritten;
			sectorsWrittenCount ++;

			if(bytesWritten < 0){
				puts("Error en la escritura del archivo");
				puts("No se pudo finalizar la replicacion");
				exit(1);
			}

			fflush(ppd_state_getReplicationDiskVolume());
		}
	}




	void ppd_sync_finishReplication(){

		/*
		 * Si esta activo el proceso de replicacion se finaliza
		 * cerrando los recursos ocupados por el mismo
		 */
		if(ppd_state_isReplicationProcessActive()){

			File * file = ppd_state_getReplicationDiskVolume();
			fclose(file);
			ppd_state_setReplicationDiskVolume(NULL);

			ppd_state_setReplicationProcessActive(FALSE);
		}

		/*
		 * Si aun no esta corriendo el worker del PPD
		 * se lo levanta
		 */
		if(!ppd_state_isWorkerRunning()){

			char * startAddress = ppd_persistance_mapDisk(ppd_conf_getDiskPath());

			ppd_state_setDiskStartAddress(startAddress);

			ppd_planifier_worker_doJobs();

			ppd_state_setWorkerRunning(TRUE);
		}
	}




	void ppd_sync_processFileSectors(File * fatFile){

		DiskSector disk = commons_buildDiskSector();

		size_t bytesReaded = fread(disk.sectorContent , sizeof(char) , sizeof(disk.sectorContent) , fatFile);

		allBytesReaded += bytesReaded;
		sectorsReadedCount++;

		while( ! feof(fatFile) ){

			ppd_endpoint_sendSyncSector(bytesReaded , disk);

			disk = commons_buildDiskSector();

			bytesReaded = fread(disk.sectorContent , sizeof(char) , sizeof(disk.sectorContent) , fatFile);
			allBytesReaded += bytesReaded;
			sectorsReadedCount++;

		}

		if(bytesReaded > 0){
			ppd_endpoint_sendSyncSector(bytesReaded , disk);
		}
	}





