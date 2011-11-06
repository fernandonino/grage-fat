/*
 * pfs-fat-utils.c
 *
 *  Created on: 04/11/2011
 *      Author: gonzalo-joaco
 */

#include <stdlib.h>
#include "pfs-fat32.h"


	Volume * pfs_fat_utils_loadVolume( BPB * b ){

		Volume * v = (Volume*) malloc(sizeof(Volume));

		v->bps = b->BPB_BytsPerSec;
		v->spc = b->BPB_SecPerClus;
		v->bpc = v->bps * v->spc;
		v->rsv = b->BPB_ResvdSecCnt;

		v->fatQty = b->BPB_NumFATs;
		v->fatSize = b->BPB_FATSz32;
		v->fatStartSector = v->rsv;

		v->root = b->BPB_RootClus;
		v->sectors = b->BPB_TotSec32;
		v->dataSectors = v->sectors - ( v->rsv + (v->fatQty * v->fatSize) );
		v->clusters = v->dataSectors / v->spc;
		v->fds = v->rsv + (v->fatQty * v->fatSize);

		printf("Clusters totales: %u.\n" , v->clusters);

		if ( v->clusters < 65525 ) {
			return NULL;
		} else {
			return v;
		}
	}


	uint32_t pfs_fat32_utils_fetchChar(LongDirEntry *D, int8_t n) {
	    uint8_t i = (n % 13);

	    if ( i <= 4  ) return D->LDIR_Name1[i];
	    if ( i <= 10 ) return D->LDIR_Name2[i-5];
	    return D->LDIR_Name3[i-11];
	}

	int8_t pfs_fat32_utils_getNameLength(LongDirEntry * ldirentry){
		uint8_t i;
		uint16_t character;

	    for ( i=0 ; i<13 ; i++) {
	    	character = pfs_fat32_utils_fetchChar(ldirentry , i);
			if ( FAT_32_LFN_ISNULL(character) )
				return (i + 1);
	    }

	    return (i + 1);
	}

	void pfs_fat32_utils_extractName( LongDirEntry * d, uint16_t * dest, int8_t length) {
		int8_t i;

		for (i=0; i < (length - 1); i++) {
	    	dest[i] = pfs_fat32_utils_fetchChar(d , i);
	    }

	    dest[length - 1] = 0x00;

	    return;
	}

	char *  pfs_fat_utils_getFileName(LongDirEntry * l){
		uint8_t nameLength = pfs_fat32_utils_getNameLength(l);

		uint16_t utf16name[13];
		pfs_fat32_utils_extractName(l , utf16name , nameLength);

		char * utf8name = (char *)calloc(nameLength,sizeof(char));
		size_t utf8length = 0;

		unicode_utf16_to_utf8_inbuffer(utf16name , nameLength - 1 , utf8name , &utf8length);

		return utf8name;
	}

	uint32_t pfs_fat_utils_getFatEntrySector(Volume * v , uint32_t cluster){
		uint32_t sectorNumberInFat = v->rsv + (cluster * 4 / v->bps);
		return sectorNumberInFat;
	}

	uint32_t pfs_fat_utils_getFatEntryOffset(Volume * v , uint32_t cluster){
		uint32_t fatEntryOffset = (cluster * 4) % v->bps;
		return fatEntryOffset;
	}



	uint32_t pfs_fat_utils_getFirstSectorOfCluster(Volume * v , uint32_t cluster){
		uint32_t firstSector = ((cluster - 2) * v->spc) + v->fds;
		return firstSector;
	}

	uint32_t pfs_fat32_utils_getNextClusterInChain(Volume * v , uint32_t clusterId){
		uint32_t nextCluster;

		uint32_t sector = pfs_fat_utils_getFatEntrySector(v, clusterId);
		uint32_t offset = pfs_fat_utils_getFatEntryOffset(v, clusterId);

		DiskSector diskSector = pfs_endpoint_callGetSector(sector);

		memcpy(&nextCluster, diskSector.sectorContent + offset, sizeof(uint32_t));

		return nextCluster;
	}


	uint32_t pfs_fat32_utils_getFirstSectorFromNextClusterInChain(Volume * v , uint32_t clusterId){

		uint32_t nextCluster = pfs_fat32_utils_getNextClusterInChain(v , clusterId);
		return pfs_fat_utils_getFirstSectorOfCluster(v , nextCluster);
	}

	Boolean pfs_fat32_utils_isLastSectorFromCluster(Volume * v , uint32_t sectorId){
		return ((v->bps * (1 + sectorId) % v->bpc) == 0);
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

	uint32_t pfs_fat32_utils_getDirEntryOffset(uint32_t sectorId , uint32_t os , uint32_t offset){
		return (sectorId - os) * 512 + offset - FAT_32_BLOCK_ENTRY_SIZE;
	}

	void pfs_fat32_utils_toDirent(struct dirent * de , DirEntry direntry , LongDirEntry ldirentry , Volume * v){
		uint8_t length = pfs_fat32_utils_getNameLength(&ldirentry);

		uint16_t utf16name[13];
		pfs_fat32_utils_extractName(&ldirentry,utf16name,length);

		char * utf8name = (char *)calloc(length,sizeof(char));

		size_t utf8length = 0;
		unicode_utf16_to_utf8_inbuffer(utf16name , length - 1 , utf8name , &utf8length);

		strcpy(de->d_name,utf8name);
		de->d_ino = pfs_fat_getFirstClusterFromDirEntry(&direntry);
		free(utf8name);

		if ( direntry.DIR_Attr == FAT_32_ATTR_DIRECTORY ){
			de->d_type = DT_DIR;
		}
		else
			de->d_type = DT_REG;
	}

	uint8_t pfs_fat32_isDirectoryEmpty(Volume * v, FatFile * fd){
		uint32_t sector = pfs_fat_utils_getFirstSectorOfCluster(v , fd->nextCluster);
		uint32_t offset = FAT_32_BLOCK_ENTRY_SIZE;
		uint32_t next = fd->nextCluster;
		DiskSector diskSector = pfs_endpoint_callGetSector(sector);
		LongDirEntry lDirEntry;
		memcpy(&lDirEntry, diskSector.sectorContent + offset, sizeof(LongDirEntry));

		while(lDirEntry.LDIR_Ord == FAT_32_FREEENT){
			offset += FAT_32_BLOCK_ENTRY_SIZE;
			memcpy(&lDirEntry, diskSector.sectorContent + offset, sizeof(LongDirEntry));

			if(offset == FAT_32_SECTOR_SIZE){
				if(pfs_fat32_utils_isLastSectorFromCluster(v , sector)){
					if(FAT_32_ISEOC(next)) return 1;
					sector = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v , next);
					diskSector = pfs_endpoint_callGetSector(sector);
					next = pfs_fat32_utils_getNextClusterInChain(v , next);
				}
				else{
					diskSector = pfs_endpoint_callGetSector(++sector);
				}
				offset = 0;
			}
		}

		if(lDirEntry.LDIR_Ord == FAT_32_ENDOFDIR)
			return 1;
		else
			return 0;

	}
