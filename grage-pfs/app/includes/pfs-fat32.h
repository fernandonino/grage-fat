/*
 * pfs-fat32.h
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */

#ifndef PFS_FAT32_H_
#define PFS_FAT32_H_

#include <grage-commons.h>

#define SSIZE 512 // Tamaño de sector.


typedef struct {
	uint8_t BS_jmpBoot[3]; /* 0  Jump to boot code.                BS_jmpBoot */
	uint8_t BS_OEMName[8]; /* 3  OEM name & version.               BS_OEMName */
	uint16_t BPB_BytsPerSec; /* 11 Bytes per sector hopefully 512.   BPB_BytsPerSec */
	uint8_t BPB_SecPerClus; /* 13 Cluster size in sectors.          BPB_SecPerClus */
	uint16_t BPB_ResvdSecCnt; /* 14 Number of reserved (boot) sectors. BPB_RsvdSecCount */
	uint8_t BPB_NumFATs; /* 16 Number of FAT tables hopefully 2. BPB_NumFATs */
	uint16_t BPB_RootEntCnt; /* 17 Number of directory slots.        BPB_RootEntCnt */
	uint16_t BPB_TotSec16; /* 19 Total sectors on disk.            BPB_TotSec16 */
	uint8_t BPB_Media; /* 21 Media descriptor=first byte of FAT. BPB_Media */
	uint16_t BPB_FATSz16; /* 22 Sectors in FAT.                   BPB_FATSz16 */
	uint16_t BPB_SecPerTrk; /* 24 Sectors/track.                    BPB_SecPerTrk */
	uint16_t BPB_NumHeads; /* 26 Heads.                            BPB_NumHeads */
	uint32_t BPB_HiddSec; /* 28 number of hidden sectors.         BPB_HiddSec */
	uint32_t BPB_TotSec32; /* 32 big total sectors.                BPB_TotSec32 */
	uint32_t BPB_FATSz32; /* 36 32bit count of sectors occupied by each FAT. BPB_FATSz16 must be 0    */
	uint16_t BPB_ExtFlags; /* 40 extension flags. Usually 0.*/
	uint16_t BPB_FSVer; /* 42 Version number of the FAT32 volume. For future extension. Must be 0:0 */
	uint32_t BPB_RootClus; /* 44 start cluster of root dir. Usually 2 */
	uint16_t BPB_FSInfo; /* 48 changeable global info */
	uint16_t BPB_BkBootSec; /* 50 back up boot sector. Recomended 6 */
	uint8_t BPB_Reserved[12]; /* 52 reserved for future expansion */
	uint8_t BS_DrvNum; /* 64 physical drive ?		*/
	uint8_t BS_Reserved1; /* 65 reserved			*/
	uint8_t BS_BootSig; /* 66 dos > 4.0 diskette. signature.	*/
	uint32_t BS_VolID; /* 67 serial number 		*/
	uint8_t BS_VolLab[11]; /* 71 disk label 		*/
	uint8_t BS_FilSysType[8]; /* 82 FAT type 			*/
}__attribute__ ((packed)) Bpb_t;

typedef struct {
	uint32_t FSI_LeadSig; /* FSI_LeadSig.   0x41615252        				*/
	uint8_t FSI_Reserved1[480]; /* FSI_Reserved1 (size 480 bytes should be 0)  			*/
	uint32_t FSI_StrucSig; /* FSI_StructSig. 0x61417272    				*/
	uint32_t FSI_Free_Count; /* FSI_Free_Count.                      			*/
	uint32_t FSI_Nxt_Free; /* FSI_Nxt_Free. Just an hint. 0xFFFFFFFF =  no hint available	*/
	uint8_t FSI_Reserved2[12]; /* FSI_Reserved2                        			*/
	uint32_t FSI_TrailSig; /* FSI_TrailSig.  0xAA550000					*/
}__attribute__ ((packed)) FSInfo_t;

typedef struct {
	uint8_t DIR_Name[11]; /*  0 file name (8+3) */
	uint8_t DIR_Attr; /* 11 attribute byte */
	uint8_t DIR_NTRes; /* 12 case of short filename *//* MS says "reserved for NT */
	uint8_t DIR_CrtTimeTenth; /* 13 creation time, milliseconds (?) */
	uint16_t DIR_CrtTime; /* 14 creation time */
	uint16_t DIR_CrtDate; /* 16 creation date */
	uint16_t DIR_LstAccDate; /* 18 last access date */
	uint16_t DIR_FstClusHI; /* 20 start cluster, Hi */
	uint16_t DIR_WrtTime; /* 22 time stamp */
	uint16_t DIR_WrtDate; /* 24 date stamp */
	uint16_t DIR_FstClusLO; /* 26 starting cluster number */
	uint32_t DIR_FileSize; /* 28 size of the file */
} __attribute__ ((packed)) DirEntry_t;

typedef struct {
	uint8_t LDIR_Ord; //The order of this entry in the sequence of long dir entries associated with the short dir entry at the end of the long dir set.
                     // If masked with 0x40 (LAST_LONG_ENTRY), this indicates the entry is the last long dir entry in a set of long dir entries. All valid sets of long dir entries must begin with an entry having this mask.
	uint16_t LDIR_Name1[5]; // Characters 1-5 of the long-name sub-component in this dir entry.
	uint8_t LDIR_Attr;// Attributes - must be ATTR_LONG_NAME
	uint8_t LDIR_Type;// If zero, indicates a directory entry that is a sub-component of a
	uint8_t LDIR_Chksum;// Checksum of name in the short dir entry at the end of the long dirset.
	uint16_t LDIR_Name2[6];//Characters 6-11 of the long-name sub-component in this dir entry.
	uint16_t LDIR_FstClusLO;//Must be ZERO. This is an artifact of the FAT "first cluster" and
	uint16_t LDIR_Name3[2];//Characters 12-13 of the long-name sub-component in this dir entry.
} __attribute__ ((packed)) LDirEntry_t;

/* FAT32 valores y macros */
#define	 FAT32_EOC_VALUE	0x0FFFFFF8	//it was 0x0FFFFFFF but linux driver set
#define  FAT32_BAD_VALUE 0x0FFFFFF7

#define  FAT32_ISEOC(EntryValue)  (((EntryValue) & 0x0FFFFFFF) >= 0x0FFFFFF8)
#define  FAT32_ISFREE(EntryValue) (((EntryValue) & 0x0FFFFFFF) == 0x00000000)
#define  FAT32_ISBAD(EntryValue)  (EntryValue == 0x0FFFFFF7)
#define  FAT32_LEGALCLUS(EntryValue)  (!( (FAT32_ISEOC(EntryValue)) || (FAT32_ISFREE(EntryValue)) || (FAT32_ISBAD(EntryValue))))

// Directory valores y macros
#define FREEENT  0xE5 /* The directory entry is free             */
#define ENDOFDIR 0x00 /* This and the following entries are free */

#define DIRENT_ISFREE(D) \
	(((D) == FREEENT) || ((D) == ENDOFDIR))

#define DIRENT_ISLAST(D) \
	(D == ENDOFDIR)
//TODO ver si hacen falta pero creo qu no.
#define FILE_YEAR(dir) (( ((BYTE *) &((dir)->DIR_WrtDate))[1] >> 1) + 1980)
#define FILE_MONTH(dir) ((((((BYTE *) &((dir)->DIR_WrtDate))[1]&0x1) << 3) + (((BYTE *) &((dir)->DIR_WrtDate))[0] >> 5)))
#define FILE_DAY(dir) (((BYTE *) &((dir)->DIR_WrtDate))[0] & 0x1f)

#define FILE_HOUR(dir) (((BYTE *) &((dir)->DIR_WrtTime))[1] >> 3)
#define FILE_MINUTE(dir) ((((((BYTE *) &((dir)->DIR_WrtTime))[1]&0x7) << 3) + (((BYTE *) &((dir)->DIR_WrtTime))[0] >> 5)))
#define FILE_SEC(dir) ((((BYTE *) &((dir)->DIR_WrtTime))[0] & 0x1f) * 2)

typedef struct {
	Bpb_t boot;
	FSInfo_t infoSec;
	uint32_t cc_total; /*cantidad de total clusters*/
	uint32_t cs_reserved; /* Cantidad de sectores reservados reserved + (fatsize * cantfats)*/
	//lista sectores libres
	//TODO integrar funcion de listas para los sectores libres
	//arbol de directorios
	//TODO implementar arbol binario n-ario
} FatData;

/* determina si es el ultimo cluster en FAT */
Boolean fat_isClusterEnd(uint32_t value);

/* determina si es un cluster vacio */
Boolean fat_isClusterFree(uint32_t value);

/* determina si es un cluster invalido */
Boolean fat_isClusterBad(uint32_t value);

//TODO revisar esta funcion pq el EOCluster puede contener datos.
Boolean fat_isLegalCluster(uint32_t value);

/* en base a un numero de cluster devuelve el sector en que esta su fat */
uint32_t fat_cluster_getInfoSector(Bpb_t * bpbPtr, uint32_t clusterNum);

/* en base a un numero de cluster devuelve el offset dentro del sector de la fat */
uint32_t fat_cluster_getInfoOffset(Bpb_t * bpbPtr, uint32_t clusterNum);

/* devuelve la cantidad de sectores reservados en base a la info de fat */
uint32_t fat_getReservedSectorCount(Bpb_t *bootSector);

/* en base a un numero de cluster devuelve el primer sector del mismo */
uint32_t cluster_getFirstSector(Bpb_t *bootSector, uint32_t clusterNumber);

#endif /* PFS_FAT32_H_ */
