/*
 * pfs-fat32.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */


#include <stdlib.h>
#include <stdio.h>

#include "pfs-fat32.h"


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
uint32_t fat_cluster_getInfoSector(Bpb_t * bpbPtr, uint32_t clusterNum){
	uint32_t count = bpbPtr->BPB_ResvdSecCnt + ((clusterNum * 4) / bpbPtr->BPB_BytsPerSec);
	return count;
}

/* en base a un numero de cluster devuelve el offset dentro del sector de la fat */
uint32_t fat_cluster_getInfoOffset(Bpb_t * bpbPtr, uint32_t clusterNum) {
	uint32_t count = (clusterNum * 4) % bpbPtr->BPB_BytsPerSec;
	return count;
}

/* devuelve la cantidad de sectores reservados en base a la info de fat */
uint32_t fat_getReservedSectorCount(Bpb_t *bootSector){
	return bootSector->BPB_ResvdSecCnt + (bootSector->BPB_NumFATs * bootSector->BPB_FATSz32);
}

uint32_t cluster_getFirstSector(Bpb_t *bootSector, uint32_t clusterNumber){
	return (fat_getReservedSectorCount(bootSector) + (clusterNumber - 2) * bootSector->BPB_SecPerClus);
}
