/*
 * pfs-fat32.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */


#include <stdlib.h>
#include <stdio.h>

#include "pfs-fat32.h"

Boolean isValidVolume( BPB * unaBpb ){

	uint32_t RootDirSectors , FATSz , TotSec , DataSec , CountofClusters;
	RootDirSectors = 0;

	if ( unaBpb->BPB_FATSz16 != 0 )
		FATSz = unaBpb->BPB_FATSz16;
	else
		FATSz = unaBpb->BPB_FATSz32;

	if ( unaBpb->BPB_TotSec16 != 0 )
		TotSec = unaBpb->BPB_TotSec16;
	else
		TotSec = unaBpb->BPB_TotSec32;

	DataSec = TotSec - (unaBpb->BPB_ResvdSecCnt + (unaBpb->BPB_NumFATs * FATSz) + RootDirSectors);
	CountofClusters = DataSec / unaBpb->BPB_SecPerClus;

	//TODO Convertir los printf a entradas de log
	if ( CountofClusters < 65525 ) {

		//printf("El volumen esta formateado en FAT12 o FAT16.\n");
		//printf("Esta implementacion soporta unicamente FAT32.\n");
		//printf("Saliendo...\n");
		return FALSE;
	} else {
		//printf("Volumen FAT32 valido.\n");
		return TRUE;
	}
}


Boolean fat_isClusterEnd(uint32_t value){
	return (value & 0x0FFFFFFF) >= 0x0FFFFFF8;
}

Boolean fat_isClusterFree(uint32_t value){
	return (value & 0x0FFFFFFF) == 0x00000000;
}

Boolean fat_isClusterBad(uint32_t value){
	return value == 0x0FFFFFF7;
}

//TODO revisar esta funcion pq el EOCluster puede contener datos.
Boolean fat_isLegalCluster(uint32_t value){
	return !(fat_isClusterBad(value) || fat_isClusterEnd(value) || fat_isClusterFree(value));
}

/* en base a un numero de cluster devuelve el sector en que esta su fat */
uint32_t getFatEntrySector(BPB * unaBpb , uint32_t cluster){
	uint32_t sectorNumberInFat = unaBpb->BPB_ResvdSecCnt + (cluster * 4 / unaBpb->BPB_BytsPerSec);
	return sectorNumberInFat;
}

/* en base a un numero de cluster devuelve el offset dentro del sector de la fat */
uint32_t getFatEntrySectorOffset(BPB * unaBpb , uint32_t cluster){
	uint32_t fatEntryOffset = (cluster * 4) % unaBpb->BPB_BytsPerSec;
	return fatEntryOffset;
}

uint32_t getFatEntry(uint32_t sector , uint32_t offset){
	uint32_t fatEntry = sector * 512 + offset;
	return fatEntry;
}

/* devuelve la cantidad de sectores reservados en base a la info de fat */
uint32_t getFirstDataSector( BPB * unaBpb){
	int sector = 0;
	if ( unaBpb->BPB_FATSz16 == 0)
		sector = unaBpb->BPB_ResvdSecCnt + (unaBpb->BPB_NumFATs * unaBpb->BPB_FATSz32);
	return sector;
}

/* Dado un clustner n, devuelve el primer sector de ese cluster */
uint64_t getFirstSectorOfCluster(BPB * unaBpb , uint32_t cluster){
	uint64_t firstSectorOfCluster = ((cluster - 2) * unaBpb->BPB_SecPerClus) + getFirstDataSector(unaBpb);
	return firstSectorOfCluster;
}

uint32_t getFirstClusterFromDirEntry(DirEntry * D) {
	uint32_t val = 0;

	char *dst = (char *) &val;
	char *src = (char *) &(D->DIR_FstClusLO);

	dst[0] = src[0];
	dst[1] = src[1];

	src = (char *) &(D->DIR_FstClusHI);
	dst[2] = src[0];
	dst[3] = src[1];

	return val;
}

