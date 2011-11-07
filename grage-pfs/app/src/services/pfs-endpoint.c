/*
 * pfs-endpoint.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

#include <linux-commons-errors.h>

#include "pfs-endpoint.h"
#include "nipc-messaging.h"

	void pfs_endpoint_callPutSector( DiskSector diskSector){

		uint32_t offset = diskSector.sectorNumber * SECTOR_SIZE;
		char * start = ppd_state_getDiskStartAddress();

		char * validator = memcpy(start + offset , diskSector.sectorContent , SECTOR_SIZE );
        if (validator == NULL){
        	perror("Error en memcpy");
        }

        msync(start , SECTOR_SIZE , MS_SYNC );
	}


	DiskSector pfs_endpoint_callGetSector(uint32_t sectorNumber){
		DiskSector diskSector;

		uint32_t offset = sectorNumber * SECTOR_SIZE;
		char * start = ppd_state_getDiskStartAddress();

		char * validator = memcpy(diskSector.sectorContent , start + offset , SECTOR_SIZE );
        if (validator == NULL){
        	perror("Error en memcpy");
        }

        diskSector.sectorNumber = sectorNumber;
		return diskSector;
	}


	char * ppd_persistance_mapDisk(char * diskId){
		struct stat filestat;

		int32_t fd =  open(diskId , O_RDWR);
		if (fd == -1){
			perror("Error en open del disco");
			return NULL;
		}

		if ( fstat(fd , &filestat) ){
			perror("fstat - error al obtener atributos del disco");
			return NULL;
		}

		char * map = mmap((caddr_t)0 , filestat.st_size , PROT_WRITE , MAP_SHARED , fd , 0);
		if( map == MAP_FAILED ){
			perror("Error al mapear el disco");
			return NULL;
		}

		if( posix_madvise(map , filestat.st_size , POSIX_MADV_SEQUENTIAL) ){
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

	char * diskStartAddress;

	char * ppd_state_getDiskStartAddress(){
		return diskStartAddress;
	}

	void ppd_state_setDiskStartAddress(char * anAddress){
		diskStartAddress = anAddress;
	}

	void ppd_initializeDisk(){
		ppd_state_setDiskStartAddress( ppd_persistance_mapDisk("/vfs/fat32.disk.0") );
	}


	/*
	DiskSector pfs_endpoint_callGetSector(uint32_t sectorNumber){
		RuntimeErrorValidator * v = commons_errors_buildSuccessValidator();

		if(commons_errors_hasError(v)){
			puts(v->errorDescription);
		}
		DiskSector d;
		return d;
	}
	*/
