/*
 * pfs-fat32.h
 *
 *  Created on: 04/11/2011
 *      Author: gonzalo
 */

#include <linux-commons.h>
#include "linux-commons-list.h"
#include "grage-commons.h"
#include "nipc-messaging.h"
#include "pfs-endpoint.h"

#ifndef PFS_FAT32_H_
#define PFS_FAT32_H_


	#define FAT_32_ROOT_FORWARD_SLASH					"/"
	#define FAT_32_ENDOFDIR 							0x00
	#define FAT_32_DIR_ENTRY_SIZE						32
	#define FAT_32_BLOCK_ENTRY_SIZE						64
	#define FAT_32_SECTOR_SIZE 							512
	#define FAT_32_FREEENT  0xE5 /* The directory entry is free             */
	#define FAT_32_ENDOFDIR 0x00 /* This and the following entries are free */


	#define FAT_32_ISEOC(EntryValue)  (((EntryValue) & 0x0FFFFFFF) >= 0x0FFFFFF8)
	#define FAT_32_ATTR_LONG_NAME ( ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID )
	#define FAT_32_ISFREE(EntryValue) (((EntryValue) & 0x0FFFFFFF) == 0x00000000)
	#define FAT_32_DIRENT_ISFREE(D) (((D) == FAT_32_FREEENT) || ((D) == FAT_32_ENDOFDIR))
	#define FAT_32_DIRENT_ISLAST(D) (D == ENDOFDIR)
	#define FAT_32_LDIR_ISLAST(Ord)	((Ord & 0x40) == 0x40)
	#define FAT_32_LFN_ISNULL(character)	(character == 0x00)


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
	} BPB;



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
	} Volume;

	typedef struct {
		uint8_t DIR_Name[11];
		uint8_t DIR_Attr;
		uint8_t DIR_NTRes;
		uint8_t DIR_CrtTimeTenth;
		uint16_t DIR_CrtTime;
		uint16_t DIR_CrtDate;
		uint16_t DIR_LstAccDate;
		uint16_t DIR_FstClusHI;
		uint16_t DIR_WrtTime;
		uint16_t DIR_WrtDate;
		uint16_t DIR_FstClusLO;
		uint32_t DIR_FileSize;
	} DirEntry;



	typedef struct {
		uint8_t LDIR_Ord;
		uint16_t LDIR_Name1[5];
		uint8_t LDIR_Attr;
		uint8_t LDIR_Type;
		uint8_t LDIR_Chksum;
		uint16_t LDIR_Name2[6];
		uint16_t LDIR_FstClusLO;
		uint16_t LDIR_Name3[2];
	}  LongDirEntry;



	typedef struct {
		uint32_t source; 			// Cluster donde se encuentran los (L)DirEntries del archivo/directorio
		uint32_t sourceOffset; 		// Sector del cluster donde se encuentran los (L)DirEntries (del 1 al 8)
		uint32_t content;			// Cluster donde comienza el contenido
		uint32_t nextCluster;		// LO + HI del DirEntry
		DiskSector currentSector;	// Cluster actual para el recorrido en el readdir
		DirEntry shortEntry;		// LDirEntry del archivo/directorio
		LongDirEntry longEntry;		// DirEntry del archivo/directorio
		uint32_t dirEntryOffset;	// cluster offset para los direntries del archivo
		uint8_t	dirType;			// Define si es root o subdir: 0 = root ; 1 = subdir

	} FatFile;







	Volume * pfs_fat_utils_loadVolume( BPB * b );
	uint32_t pfs_fat32_utils_fetchChar(LongDirEntry *D, int8_t n);
	int8_t pfs_fat32_utils_getNameLength(LongDirEntry * ldirentry);
	void pfs_fat32_utils_extractName( LongDirEntry * d, uint16_t * dest, int8_t length);
	int pfs_fat32_unicode_utf16_to_utf8_inbuffer(const uint16_t *src_utf16, const size_t src_utf16size, char* dest_utf8, size_t *dest_utf8size);
	char *  pfs_fat_utils_getFileName(LongDirEntry * l);
	uint32_t pfs_fat_utils_getFatEntrySector(Volume * v , uint32_t cluster);
	uint32_t pfs_fat_utils_getFatEntryOffset(Volume * v , uint32_t cluster);
	uint32_t pfs_fat_utils_getFirstSectorOfCluster(Volume * v , uint32_t cluster);
	uint32_t pfs_fat32_utils_getNextClusterInChain(Volume * v , uint32_t clusterId);
	uint32_t pfs_fat32_utils_getFirstSectorFromNextClusterInChain(Volume * v , uint32_t clusterId);
	Boolean pfs_fat32_utils_isLastSectorFromCluster(Volume * v , uint32_t sectorId);
	uint32_t pfs_fat_getFirstClusterFromDirEntry(DirEntry * D);
	uint32_t pfs_fat32_utils_getDirEntryOffset(uint32_t sectorId , uint32_t os , uint32_t offset);

#endif /* PFS_FAT32_H_ */
