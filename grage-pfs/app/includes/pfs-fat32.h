/*
 * pfs-fat32.h
 *
 *  Created on: 04/11/2011
 *      Author: gonzalo
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <time.h>

#include "linux-commons.h"
#include "linux-commons-strings.h"
#include "linux-commons-list.h"
#include "grage-commons.h"
#include "nipc-messaging.h"
#include "pfs-endpoint.h"
#include "pfs-state.h"
#include "pfs-utils.h"

#ifndef PFS_FAT32_H_
#define PFS_FAT32_H_


	#define FAT_32_ROOT_FORWARD_SLASH					"/"
	#define FAT_32_ENDOFDIR 							0x00
	#define FAT_32_DIR_ENTRY_SIZE						32
	#define FAT_32_BLOCK_ENTRY_SIZE						64
	#define FAT_32_SECTOR_SIZE 							512
	#define FAT_32_FREEENT  0xE5 /* The directory entry is free             */
	#define FAT_32_ENDOFDIR 0x00 /* This and the following entries are free */

	#define FAT_32_ATTR_READ_ONLY 								0x1
	#define FAT_32_ATTR_HIDDEN 									0x2
	#define FAT_32_ATTR_SYSTEM 									0x4
	#define FAT_32_ATTR_VOLUME_ID 								0x8
	#define FAT_32_ATTR_DIRECTORY 								0x10
	#define FAT_32_ATTR_ARCHIVE 								0x20
	#define FAT_32_ATTR_LONG_NAME ( FAT_32_ATTR_READ_ONLY | FAT_32_ATTR_HIDDEN | FAT_32_ATTR_SYSTEM | FAT_32_ATTR_VOLUME_ID )
	#define FAT_32_FAT_FREE_ENTRY								0x00000000

	#define FAT_32_ISEOC(FatEntryValue)  (((FatEntryValue) & 0x0FFFFFFF) >= 0x0FFFFFF8)
	#define FAT_32_ISFREE(FatEntryValue) (((FatEntryValue) & 0x0FFFFFFF) == 0x00000000)
	#define FAT_32_DIRENT_ISFREE(D) (((D) == FAT_32_FREEENT) || ((D) == FAT_32_ENDOFDIR))
	#define FAT_32_DIRENT_ISLAST(D) (D == ENDOFDIR)
	#define FAT_32_LDIR_ISLAST(Ord)	((Ord & 0x40) == 0x40)
	#define FAT_32_LFN_ISNULL(character)	(character == 0x00)


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
		uint32_t sourceOffset; 		// Offset dentro del cluster
		uint32_t content;			// Cluster donde comienza el contenido
		uint32_t nextCluster;		// LO + HI del DirEntry
		DiskSector currentSector;	// Cluster actual para el recorrido en el readdir
		DirEntry shortEntry;		// LDirEntry del archivo/directorio
		LongDirEntry longEntry;		// DirEntry del archivo/directorio
		uint32_t dirEntryOffset;	// cluster offset para los direntries del archivo
		uint8_t	dirType;			// Define si es root o subdir: 0 = root ; 1 = subdir

		//pfs_fat32_read
		uint32_t fileClusterNumber;
		uint32_t fileAbsoluteClusterNumber;
		uint32_t fileSectorNumberOfCluster;
		uint16_t sectorByteOffset;

	} FatFile;

//FAT32
	FatFile * pfs_fat32_utils_openRootDirectory(Volume * v);
	FatFile * pfs_fat32_utils_openNonRootDirectory(const char * path , Volume * v );
	FatFile * pfs_fat32_open(const char * path);
	int8_t pfs_fat32_readDirectory( struct dirent * direntry , FatFile * file , Volume * volume);
	void pfs_fat32_unlink_dirEntry(Volume * v , FatFile * fd , DiskSector * diskSector);
	int8_t pfs_fat32_unlink_FatEntryChain(Volume * v , FatFile * fd);
	void pfs_fat32_unlink(Volume * v , FatFile * fd);
	void pfs_fat32_rmdir(Volume * v , FatFile * fd);
	void fat_stat(Volume * v , FatFile * fatFile , struct stat * st);
	uint16_t pfs_fat32_read(Volume * , FatFile * , char * , size_t);

//UTILS
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
	void pfs_fat32_utils_toDirent(struct dirent * de , DirEntry direntry , LongDirEntry ldirentry , Volume * v);
	uint8_t pfs_fat32_isDirectoryEmpty(Volume * v, FatFile * fd);

	time_t pfs_fat32_utils_processTime(int s, int m, int h, int d, int mo, int y);
	time_t pfs_fat32_utils_getTime(DirEntry *D);
	uint8_t pfs_fat32_utils_fillTime(uint16_t * , uint16_t * , time_t);

	int8_t pfs_fat32_utils_seek(Volume * , FatFile * , off_t , uint32_t);
	DiskSector pfs_fat32_utils_getSectorFromNthCluster(FatFile *);

#endif /* PFS_FAT32_H_ */
