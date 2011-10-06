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

#include "grage-commons.h"
#include "ppd-configuration.h"
#include "ppd-persistance.h"

	// TODO: REEMPLAZAR perror POR FUNCIONES DE commons-logging

	// Funciones (read+write)sector candidatas para RuntimeValidator
	void ppd_persistence_writeSector(DiskSector * aSector , void * dest){

		//int delay = atoi( getPpdWriteDelay() );
		//sleep(delay);

		char * validator = memcpy(dest , aSector->sectorContent , SECTOR_SIZE );
        if (validator == NULL){
                perror("Error en memcpy");
        }

	}

	void ppd_persistence_readSector(DiskSector * aSector , void * source){

		//int delay = atoi( getPpdReadDelay() );
		//sleep(delay);

		char * validator = memcpy(aSector->sectorContent , source , SECTOR_SIZE );
        if (validator == NULL){
                perror("Error en memcpy");
        }

	}


	char * ppd_persistance_mapDisk(char * diskId){
		struct stat filestat;

		int fd =  open(diskId , O_RDWR);
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

	// TODO: Revisar si el return value es sensato
	char * ppd_persistance_unmapDisk(char * diskId , char * mapping){
		struct stat filestat;

		int fd =  open(diskId , O_RDONLY);
		if (fd == -1){
			perror("Error en open del disco");
		}

		if ( fstat(fd , &filestat) ){
			perror("fstat - error al obtener atributos del disco");
		}

		if( munmap(mapping , filestat.st_size) ){
			perror("Error en unmapping.");
		}

		close(fd);

		return NULL;
	}


