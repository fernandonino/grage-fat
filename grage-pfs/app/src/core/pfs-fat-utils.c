/*
 * pfs-fat-utils.c
 *
 *  Created on: 04/11/2011
 *      Author: gonzalo-joaco
 */

#include <stdlib.h>
#include <time.h>

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
		return (sectorId - os) * 512 + offset - FAT_32_DIR_ENTRY_SIZE;
	}

	void pfs_fat32_utils_toDirent(struct dirent * de , DirEntry  * direntry , LongDirEntry * ldirentry , Volume * v){

		if ( ldirentry != NULL ) {
			uint8_t length = pfs_fat32_utils_getNameLength(ldirentry);

			uint16_t utf16name[13];
			pfs_fat32_utils_extractName(ldirentry,utf16name,length);

			char * utf8name = (char *)calloc(length,sizeof(char));

			size_t utf8length = 0;
			unicode_utf16_to_utf8_inbuffer(utf16name , length - 1 , utf8name , &utf8length);

			strcpy(de->d_name,utf8name);
			free(utf8name);
		} else {

			memcpy(de->d_name , direntry->DIR_Name , sizeof(uint8_t));

			if( direntry->DIR_Name[1] == '.' ){
				memcpy(de->d_name + 1 , direntry->DIR_Name + 1 , sizeof(uint8_t));
				de->d_name[2] = '\0';
			} else {
				de->d_name[1] = '\0';
			}

		}

		de->d_ino = pfs_fat_getFirstClusterFromDirEntry(direntry);

		if ( direntry->DIR_Attr == FAT_32_ATTR_DIRECTORY ){
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

			if(offset == v->bps){
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

	time_t pfs_fat32_utils_processTime(int s, int m, int h, int d, int mo, int y) {
		struct tm t;
		memset((char *) &t, 0, sizeof(struct tm));

		t.tm_sec=s;         /* seconds */
	    t.tm_min=m;         /* minutes */
	    t.tm_hour=h;        /* hours */
	    t.tm_mday=d;        /* day of the month */
	    t.tm_mon=mo;         /* month */
	    t.tm_year=y;        /* year */

	 	return mktime(&t);
	}

	time_t pfs_fat32_utils_getTime(DirEntry *D) {
		int s=((((uint8_t *) &(D->DIR_WrtTime))[0] & 0x1f) * 2);
		int m=((((((uint8_t *) &(D->DIR_WrtTime))[1]&0x7) << 3) + (((uint8_t *) &(D->DIR_WrtTime))[0] >> 5)));
		int h=(((uint8_t *) &(D->DIR_WrtTime))[1] >> 3);
		int d=(((uint8_t *) &(D->DIR_WrtDate))[0] & 0x1f);
		int mo=((((((uint8_t *) &(D->DIR_WrtDate))[1]&0x1) << 3) + (((uint8_t *) &(D->DIR_WrtDate))[0] >> 5)));
		int y=(( ((uint8_t *) &(D->DIR_WrtDate))[1] >> 1) + 80);
	 	return pfs_fat32_utils_processTime(s,m,h,d,mo,y);
	}

	/*
	 *  pfs_fat32_utils_fillTime(uint16_t *Date, uint16_t *Time, time_t t);
	 *
	 * 	Uso:
	 *
	 * 	tim = time(NULL);
	 *  fat_fill_time(&(sDirEntry.DIR_CrtDate), &(sDirEntry.DIR_CrtTime), tim);
	 *  fat_fill_time(&(sDirEntry.DIR_WrtDate), &(sDirEntry.DIR_WrtTime), tim);
	 */
	uint8_t pfs_fat32_utils_fillTime(uint16_t * d , uint16_t * t , time_t actualTime) {
		struct tm time;
		uint16_t date=0;
		uint16_t tim=0;
		uint16_t bmask3=0x07FF;
		uint16_t bmask2=0x01FF;
		uint16_t bmask1=0x001F;

		localtime_r(&actualTime, &time);

		date = (uint16_t) time.tm_mday;
		date &= bmask1; // to set 0 first 11 bits;
		date |= ((uint16_t) time.tm_mon + 1) << 5;
		date &= bmask2; // to set 0 first 6 bits;
		date |= (((uint16_t) ((time.tm_year + 1900) -1980)) << 9);

		tim = (uint16_t) (time.tm_sec / 2);
		tim &= bmask1;
		tim |= (((uint16_t) (time.tm_min)) << 5);
		tim &= bmask3;
		tim |= (((uint16_t) (time.tm_hour)) << 11);

		*d = date;
		*t = tim;
		return 0;
	}

	int8_t pfs_fat32_utils_seek(Volume * v , FatFile * f , off_t offset , uint32_t fsize){

		if ( offset > fsize || offset < 0 || f->shortEntry.DIR_Attr == FAT_32_ATTR_DIRECTORY ){
			return EXIT_FAILURE;
		}

		//Revisar en el caso que de 0
		f->fileClusterNumber = offset / v->bpc + 1;
		f->fileSectorNumberOfCluster = offset % v->bpc / v->bps + 1;
		f->sectorByteOffset = offset % v->bpc;

		return EXIT_SUCCESS;
	}

	DiskSector pfs_fat32_utils_getSectorFromNthCluster(FatFile * f){
		Volume * v = pfs_state_getVolume();
		uint16_t clusterCount = 0;

		uint32_t c = pfs_fat_getFirstClusterFromDirEntry( &(f->shortEntry) );
		clusterCount++;
		while( clusterCount < f->fileClusterNumber ){
			c = pfs_fat32_utils_getNextClusterInChain(v,c);
			clusterCount++;
		}

		f->fileAbsoluteClusterNumber = c;

		uint32_t s = pfs_fat_utils_getFirstSectorOfCluster(v,c);

		if ( f->fileSectorNumberOfCluster != 1)
			s = s + f->fileSectorNumberOfCluster - 1;

		return pfs_endpoint_callGetSector(s);
	}


	void pfs_fat32_utils_fileStat(Volume * v , FatFile * fatFile , struct stat * st) {
		memset((char *) st, 0, sizeof(struct stat));

		st->st_dev = (int)(v);
		st->st_nlink = 1;
		st->st_rdev = 0;
		st->st_blksize = v->bpc;

		if(fatFile->dirType == 0) {
			st->st_ino = v->root;
			st->st_mode = S_IFDIR | S_IRWXU;
			st->st_size = 0;
			st->st_blocks=0;
			//Hay que ver por que y como solucionar esto
			st->st_ctim.tv_sec = st->st_atim.tv_sec = st->st_mtim.tv_sec = 0;
		} else {
			st->st_ino = pfs_fat_getFirstClusterFromDirEntry(&(fatFile->shortEntry));

			if ( fatFile->shortEntry.DIR_Attr == FAT_32_ATTR_DIRECTORY ) {
				st->st_mode = S_IFDIR | S_IRWXU;
			} else {
				st->st_mode = S_IFREG | S_IRWXU;
			}
			st->st_size = fatFile->shortEntry.DIR_FileSize;
			st->st_blocks = (st->st_size / v->bpc) + 1;
			st->st_ctim.tv_sec = st->st_atim.tv_sec = st->st_mtim.tv_sec = pfs_fat32_utils_getTime(&(fatFile->shortEntry));
		}
	}



