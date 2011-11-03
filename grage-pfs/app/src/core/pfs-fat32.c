/*
 * pfs-fat32.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */


#include <stdlib.h>
#include <stdio.h>

#include "pfs-fat32.h"
#include "pfs-fat-utils.h"


BPB * bpb;

Boolean pfs_fat_isValidVolume( BPB * unaBpb ){

	uint32_t RootDirSectors , FATSz , TotSec , CountofClusters;
	RootDirSectors = 0;

	if ( unaBpb->BPB_FATSz16 != 0 )
		FATSz = unaBpb->BPB_FATSz16;
	else
		FATSz = unaBpb->BPB_FATSz32;

	if ( unaBpb->BPB_TotSec16 != 0 )
		TotSec = unaBpb->BPB_TotSec16;
	else
		TotSec = unaBpb->BPB_TotSec32;

	CountofClusters = pfs_fat_getTotalClusters(unaBpb);

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


Boolean pfs_fat_isClusterEnd(uint32_t value){
	return (value & 0x0FFFFFFF) >= 0x0FFFFFF8;
}

Boolean pfs_fat_isClusterFree(uint32_t value){
	return (value & 0x0FFFFFFF) == 0x00000000;
}

Boolean pfs_fat_isClusterBad(uint32_t value){
	return value == 0x0FFFFFF7;
}

//TODO revisar esta funcion pq el EOCluster puede contener datos.
Boolean pfs_fat_isLegalCluster(uint32_t value){
	return !(pfs_fat_isClusterBad(value) || pfs_fat_isClusterEnd(value) || pfs_fat_isClusterFree(value));
}

/* en base a un numero de cluster devuelve el sector en que esta su fat */
uint32_t pfs_fat_getFatEntrySector(BPB * unaBpb , uint32_t cluster){
	uint32_t sectorNumberInFat = unaBpb->BPB_ResvdSecCnt + (cluster * 4 / unaBpb->BPB_BytsPerSec);
	return sectorNumberInFat;
}

/* en base a un numero de cluster devuelve el offset dentro del sector de la fat */
uint32_t pfs_fat_getFatEntrySectorOffset(BPB * unaBpb , uint32_t cluster){
	uint32_t fatEntryOffset = (cluster * 4) % unaBpb->BPB_BytsPerSec;
	return fatEntryOffset;
}

/* en base al contenido del sector y un offset, lee y devuelve la entrada en la FAT */
uint32_t pfs_fat_getFatEntry(char * sectorContent , uint32_t offset){
	uint32_t fatEntry;
	memcpy(&fatEntry , sectorContent , sizeof(uint32_t));
	return fatEntry;
}

/* devuelve la cantidad de sectores reservados en base a la info de fat */
uint32_t pfs_fat_getFirstDataSector( BPB * unaBpb){
	int sector = 0;
	if ( unaBpb->BPB_FATSz16 == 0)
		sector = unaBpb->BPB_ResvdSecCnt + (unaBpb->BPB_NumFATs * unaBpb->BPB_FATSz32);
	return sector;
}

/* Dado un clustner n, devuelve el primer sector de ese cluster */
uint32_t pfs_fat_getFirstSectorOfCluster(BPB * unaBpb , uint32_t cluster){
	uint64_t firstSectorOfCluster = ((cluster - 2) * unaBpb->BPB_SecPerClus) + pfs_fat_getFirstDataSector(unaBpb);
	return firstSectorOfCluster;
}

uint32_t pfs_fat_getFirstClusterFromDirEntry(DirEntry * D) {
	if ( D == NULL )
		return EXIT_FAILURE;

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

uint32_t pfs_fat_setFirstClusterForDirEntry(DirEntry * D , uint32_t cluster) {
	if ( cluster == 0 || cluster == 1 )
		return EXIT_FAILURE;

	uint32_t val = 0;
	char *src = (char *) &cluster;
	char *dst = (char *) &(D->DIR_FstClusLO);	

	dst[0] = src[0];
	dst[1] = src[1];

	dst = (char *) &(D->DIR_FstClusHI);
	dst[0] = src[2];
	dst[1] = src[3];	

	return EXIT_SUCCESS;
}

BPB pfs_fat_readBPB(char * sectorContent){
	BPB bpb;
	memcpy(&bpb , sectorContent , sizeof(BPB));
	return bpb;
}

DirEntry pfs_fat_readDirEntry(char * sectorContent){
	DirEntry shortDirEntry;
	memcpy(&shortDirEntry , sectorContent , sizeof(DirEntry));
	return shortDirEntry;
}

LDirEntry pfs_fat_readLDirEntry(char * sectorContent){
	LDirEntry longDirEntry;
	memcpy(&longDirEntry , sectorContent , sizeof(DirEntry));
	return longDirEntry;
}

/* REVISAR ESTA FUNCION URGENTEMENTE */
uint32_t pfs_fat_getFreeCluster(BPB * bpb , FSInfo * fs){
	uint32_t currentCluster = fs->FSI_Nxt_Free;
	uint32_t freeCluster = fs->FSI_Nxt_Free;
	uint32_t totalClusters = pfs_fat_getTotalClusters(bpb);

	for( ; currentCluster < totalClusters ; currentCluster++){

		uint32_t sector = pfs_fat_getFatEntrySector(bpb , currentCluster);
		uint32_t offset = pfs_fat_getFatEntrySectorOffset(bpb , currentCluster);

		//pedirle al PPD el sector "uint32_t sector"
		char * sectorContent;

		uint32_t fatEntry = pfs_fat_getFatEntry(sectorContent , offset);

		if( pfs_fat_isClusterFree(fatEntry) ){
			fs->FSI_Nxt_Free = currentCluster;
			break;
		}
	}
	return freeCluster;
}

uint32_t pfs_fat_getTotalClusters(BPB * bpb){
	uint32_t DataSec = bpb->BPB_TotSec32 - (bpb->BPB_ResvdSecCnt + (bpb->BPB_NumFATs * bpb->BPB_FATSz32));
	uint32_t clusterCount = DataSec / bpb->BPB_SecPerClus;
	return clusterCount;
}


int8_t pfs_fat_open(){

	return EXIT_SUCCESS;
}

int8_t pfs_fat_findDirectory(path){

	return EXIT_SUCCESS;
}

int8_t pfs_fat_readdir(char * path , struct dirent * direntry , FatFile * file){
	LDirEntry lfnentry;
	DirEntry  sfnentry;
	uint32_t cluster = pfs_fat_getFirstClusterFromDirEntry(&file->firstEntry);
	uint32_t sector = pfs_fat_getFirstSectorOfCluster(bpb,cluster);
	char buffer[512];

	//chequear si esta el sector en cache, y usarlo si esta
	// si no, traerlo:
	//char * buffer = pfs_endpoint_callGetSectors(sector);

	//trabajar con el LDirEntry
	memcpy(&lfnentry,buffer,sizeof(LDirEntry));
	if( ! LDIR_ISLASTLONG(lfnentry.LDIR_Ord) )
		return EXIT_FAILURE;

	//convertir el nombre
	char * name = pfs_get_fileName(&lfnentry);
	strcpy(direntry->d_name , name );
	free(name);

	memcpy(&sfnentry, buffer + 32, sizeof(sfnentry));
	file->dirEntryOffset += 64; //Se leyeron un LDirEntry y un DirEntry

	return EXIT_SUCCESS;
}
