/*
 * pfs-fat32.h
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */

#ifndef PFS_FAT32_H_
#define PFS_FAT32_H_

#include <grage-commons.h>

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
}__attribute__ ((packed)) BPB;

typedef struct {
	uint32_t FSI_LeadSig;
	uint8_t FSI_Reserved1[480];
	uint32_t FSI_StrucSig;
	uint32_t FSI_Free_Count;
	uint32_t FSI_Nxt_Free;
	uint8_t FSI_Reserved2[12];
	uint32_t FSI_TrailSig;
}__attribute__ ((packed)) FSInfo;

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
} __attribute__ ((packed)) DirEntry;

typedef struct {
	uint8_t LDIR_Ord;
	uint16_t LDIR_Name1[5];
	uint8_t LDIR_Attr;
	uint8_t LDIR_Type;
	uint8_t LDIR_Chksum;
	uint16_t LDIR_Name2[6];
	uint16_t LDIR_FstClusLO;
	uint16_t LDIR_Name3[2];
} __attribute__ ((packed)) LDirEntry;

typedef struct {
	BPB boot;
	FSInfo infoSec;
	uint32_t cc_total; /*cantidad de total clusters*/
	uint32_t cs_reserved; /* Cantidad de sectores reservados reserved + (fatsize * cantfats)*/
	//lista sectores libres
	//TODO integrar funcion de listas para los sectores libres
	//arbol de directorios
	//TODO implementar arbol binario n-ario
} FatData;

/* File attributes */
#define ATTR_READ_ONLY 		0x1
#define ATTR_HIDDEN 		0x2
#define ATTR_SYSTEM 		0x4
#define ATTR_VOLUME_ID 		0x8
#define ATTR_DIRECTORY 		0x10
#define ATTR_ARCHIVE 		0x20
#define ATTR_LONG_NAME 		( ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID )


/* FAT32 valores y macros */
#define	 FAT32_EOC_VALUE	0x0FFFFFF8
#define  FAT32_BAD_VALUE	0x0FFFFFF7

#define  FAT32_ISEOC(EntryValue)	(((EntryValue) & 0x0FFFFFFF) >= 0x0FFFFFF8)
#define  FAT32_ISFREE(EntryValue)	(((EntryValue) & 0x0FFFFFFF) == 0x00000000)
#define  FAT32_ISBAD(EntryValue)  	(EntryValue == 0x0FFFFFF7)
#define  FAT32_LEGALCLUS(EntryValue)	(!( (FAT32_ISEOC(EntryValue)) || (FAT32_ISFREE(EntryValue)) || (FAT32_ISBAD(EntryValue))))


// Directory valores y macros
#define FREEENT  0xE5
#define ENDOFDIR 0x00
#define LASTLONG 0x40

#define LDIR_ISFREE(D) 			(((D) == FREEENT) || ((D) == ENDOFDIR))
#define LDIR_ISLAST(D) 			(D == ENDOFDIR)
#define LDIR_ISLASTLONG(Ord)		(Ord == LASTLONG)


//TODO Averiguar si hacen falta
#define FILE_YEAR(dir) (( ((BYTE *) &((dir)->DIR_WrtDate))[1] >> 1) + 1980)
#define FILE_MONTH(dir) ((((((BYTE *) &((dir)->DIR_WrtDate))[1]&0x1) << 3) + (((BYTE *) &((dir)->DIR_WrtDate))[0] >> 5)))
#define FILE_DAY(dir) (((BYTE *) &((dir)->DIR_WrtDate))[0] & 0x1f)

#define FILE_HOUR(dir) (((BYTE *) &((dir)->DIR_WrtTime))[1] >> 3)
#define FILE_MINUTE(dir) ((((((BYTE *) &((dir)->DIR_WrtTime))[1]&0x7) << 3) + (((BYTE *) &((dir)->DIR_WrtTime))[0] >> 5)))
#define FILE_SEC(dir) ((((BYTE *) &((dir)->DIR_WrtTime))[0] & 0x1f) * 2)

	/* determina si el volumen es FAT32 */
	Boolean pfs_fat_isValidVolume(BPB *);

	/* determina si es el ultimo cluster en FAT */
	Boolean pfs_fat_isClusterEnd(uint32_t value);

	/* determina si es un cluster vacio */
	Boolean pfs_fat_isClusterFree(uint32_t value);

	/* determina si es un cluster invalido */
	Boolean pfs_fat_isClusterBad(uint32_t value);

	//TODO revisar esta funcion pq el EOCluster puede contener datos.
	Boolean pfs_fat_isLegalCluster(uint32_t value);


	///////////// File Allocation Table functions /////////////

	/* En base a un numero de cluster devuelve el sector en que esta su fat */
	uint32_t pfs_fat_getFatEntrySector(BPB * , uint32_t);

	/* En base a un numero de cluster devuelve el offset dentro del sector de la fat */
	uint32_t pfs_fat_getFatEntrySectorOffset(BPB * , uint32_t);

	uint32_t pfs_fat_getFatEntry(uint32_t , uint32_t);

	///////////////////////////////////////////////////////////


	/* devuelve el sector donde comienan los datos en base a la info de fat */
	/* Generlamente, es el primer sector del cluster 2 */
	uint32_t pfs_fat_getFirstDataSector(BPB *);


	///////////// Cluster functions /////////////

	/* Dado un clustner n, devuelve el primer sector de ese cluster */
	uint64_t pfs_fat_getFirstSectorOfCluster(BPB * , uint32_t);

	/* Ubica el siguente cluster en funcion del DirEntry*/
	uint32_t pfs_fat_getFirstClusterFromDirEntry(DirEntry *);

	/* Setea los campos HI y LO de DirEntry para un cluster dado */
	uint32_t pfs_fat_setFirstClusterForDirEntry(DirEntry * , uint32_t);

	/////////////////////////////////////////////

#endif /* PFS_FAT32_H_ */
