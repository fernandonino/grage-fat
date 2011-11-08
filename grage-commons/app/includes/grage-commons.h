/*
 * grage-commons.h
 *
 *  Created on: 19/09/2011
 *      Author: utn_so
 */


#ifndef GRAGE_COMMONS_H_
#define GRAGE_COMMONS_H_

#include "linux-commons.h"

#define SECTOR_SIZE	512


	typedef struct {
		uint32 pista;
		uint32 sectorNumber;
	}__attribute__((packed)) PistaSector;
	typedef struct {
		uint32 menssageID;
		PistaSector pistaSector;
		uint32 timeInMiliseconds;
	}__attribute__((packed)) MessageConsolePPD;

	typedef struct {

		uint32 sectorNumber;
		char sectorContent[SECTOR_SIZE];

	} __attribute__((packed)) DiskSector;


	typedef struct {
		uint32 clusterNumber;
		DiskSector sectors[8];
	}Cluster;



	typedef struct {
		uint8_t BS_jmpBoot[3];
		uint8_t BS_OEMName[8];
		uint16_t BPB_BytsPerSec;
		uint8_t BPB_SecPerClus;
		uint16_t BPB_ResvdSecCnt;
		uint8_t BPB_NumFATs;
		uint16_t BPB_RootEntCnt;
		uint16_t BPB_TotSec16;
		uint8_t BPB_Media;
		uint16_t BPB_FATSz16;
		uint16_t BPB_SecPerTrk;
		uint16_t BPB_NumHeads;
		uint32_t BPB_HiddSec;
		uint32_t BPB_TotSec32;
		uint32_t BPB_FATSz32;
		uint16_t BPB_ExtFlags;
		uint16_t BPB_FSVer;
		uint32_t BPB_RootClus;
		uint16_t BPB_FSInfo;
		uint16_t BPB_BkBootSec;
		uint8_t BPB_Reserved[12];
		uint8_t BS_DrvNum;
		uint8_t BS_Reserved1;
		uint8_t BS_BootSig;
		uint32_t BS_VolID;
		uint8_t BS_VolLab[11];
		uint8_t BS_FilSysType[8];
	} __attribute__((packed)) BPB;



	typedef struct {
		uint16_t bps; 			// bytes per sector
		uint16_t spc; 			// sectors per cluster
		uint16_t bpc; 			// bytes per cluster
		uint16_t fds; 			// first data sector
		uint16_t rsv;			// reserved sector count

		uint8_t	fatQty; 		// quantity of FAT tables
		uint16_t fatSize; 		// size of FAT in sectors
		uint16_t fatStartSector; // sector donde comienza la fat

		uint32_t clusters;		// cluster quantity
		uint32_t sectors;		// sector quantity
		uint32_t dataSectors;	// data sector quantity
		uint32_t root;			// cluster number of root directory

		int32_t disk;			// disk file descriptor - solo ese usa en los tests
	} __attribute__((packed)) Volume;



	DiskSector commons_buildDiskSector();

#endif /* GRAGE_COMMONS_H_ */
