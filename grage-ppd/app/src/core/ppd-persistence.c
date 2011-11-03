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
	void ppd_persistence_writeSector(DiskSector * aSector , char * dest){

        /*
		uint32_t offset = aSector->sectorNumber * SECTOR_SIZE;

		char * validator = memcpy(dest + offset , aSector->sectorContent , SECTOR_SIZE );
        if (validator == NULL){
        	perror("Error en memcpy");
        }

        msync(dest , SECTOR_SIZE , MS_SYNC );

        */

		puts("Escribiendo sector");
		printf("SectorId: %i , Contenido: %s\n" , aSector->sectorNumber , aSector->sectorContent);
	}

	void ppd_persistence_readSector(DiskSector * aSector , char * source){

        /*
		uint32_t offset = aSector->sectorNumber * SECTOR_SIZE;
		char * validator = memcpy(aSector->sectorContent , source + offset , SECTOR_SIZE );
        if (validator == NULL){
        	perror("Error en memcpy");
        }

        msync(source , SECTOR_SIZE , MS_SYNC );

		 */
		char * message = commons_string_concat("hola mundo - Get Sectores response from PPD: " , ppd_conf_getPpdIdDisk());
		strcpy(aSector->sectorContent , message);

		puts("Leyendo sector");
		printf("SectorId: %i\n" , aSector->sectorNumber );
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

	// TODO: Revisar si el return value es sensato
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


