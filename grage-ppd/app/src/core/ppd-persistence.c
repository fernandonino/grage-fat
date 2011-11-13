/*
 * ppd-persistence.c
 *
 *  Created on: 26/09/2011
 *      Author: gonzalo
 */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

#include <linux-commons-strings.h>

#include <grage-commons.h>

#include "ppd-configuration.h"
#include "ppd-persistance.h"
#include "ppd-state.h"


	void ppd_persistence_writeSector(DiskSector aSector){

		uint32_t offset = aSector.sectorNumber * SECTOR_SIZE;

		char * validator = memcpy(ppd_state_getDiskStartAddress() + offset , aSector.sectorContent , SECTOR_SIZE );
        if (validator == NULL){
        	perror("Error en memcpy");
        }

        msync(ppd_state_getDiskStartAddress() , SECTOR_SIZE , MS_SYNC );

	}


	DiskSector ppd_persistence_readSector(uint32_t sectorId ){

		DiskSector disk = commons_buildDiskSector();
		uint32_t offset = sectorId * SECTOR_SIZE;
		char * validator = memcpy(&disk.sectorContent , ppd_state_getDiskStartAddress() + offset , SECTOR_SIZE );
        if (validator == NULL){
        	perror("Error en memcpy");
        }

		disk.sectorNumber = sectorId;

		return disk;
	}


	char * ppd_persistance_mapDisk(char * diskId){

		int32_t fd =  open(diskId , O_RDWR);
		if (fd == -1){
			perror("Error en open del disco");
			return NULL;
		}

		char * map = mmap((caddr_t)0 , ppd_state_getVolumeSize() , PROT_WRITE , MAP_SHARED , fd , 0);
		if( map == MAP_FAILED ){
			perror("Error al mapear el disco");
			return NULL;
		}

		if( posix_madvise(map , ppd_state_getVolumeSize() , POSIX_MADV_SEQUENTIAL) ){
			perror("Error en posix_madvise");
			return NULL;
		}

		close(fd);

		return map;
	}



	char * ppd_persistance_unmapDisk(char * diskId , char * mapping){
		struct stat filestat;

		int32_t fd =  open(diskId , O_RDONLY);
		if (fd == -1){
			perror("Error en open del disco");
		}

		if ( fstat(fd , &filestat) ){
			perror("fstat - error al obtener atributos del disco");
		}

		msync(mapping , filestat.st_size , MS_SYNC );

		if( munmap(mapping , filestat.st_size) ){
			perror("Error en unmapping.");
		}

		close(fd);

		return NULL;
	}


	void ppd_persistence_mapDevice(){
		char * startAddress = ppd_persistance_mapDisk(ppd_conf_getDiskPath());
		ppd_state_setDiskStartAddress(startAddress);
	}

	void ppd_persistence_unmapDevice(){
		ppd_persistance_unmapDisk( ppd_conf_getDiskPath() , ppd_state_getDiskStartAddress() );
	}

