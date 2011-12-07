/*
 * pfs-cache.h
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */
#include "grage-commons.h"
#include "linux-commons-list.h"
#include "pfs-fat32.h"

#ifndef PFS_CACHE_H_
#define PFS_CACHE_H_

#define FAT_CACHE		0
#define FILE_CACHE		1

		typedef struct {
			Cluster * cluster;
			uint32_t estado;
		}CacheRecord;

		typedef struct {
			uint8_t estado;
			DiskSector sector;
		}CacheSectorRecord;

		Boolean pfs_cache_habilitada();
		void pfs_cache_initialize();
		List pfs_cache_getListaCacheFat();
		void pfs_cache_setListaCacheFat(List lista);
		List pfs_cache_sectors_initialize();
		Boolean pfs_cache_isFatSectorReserved(uint32 sectorNumber);
		CacheSectorRecord * pfs_cache_get_sector(uint32 sectorID,List listaCacheSectors,uint32 sectorsMaxCount);
		void pfs_cache_put_sectors(DiskSector * sectorNuevo,List listaCacheSectors,uint32 sectorsMaxCount);
		void pfs_cache_sectors_registrar_acceso(List listaCacheSectors);
		void pfs_cache_sectores_dumpBIS(List listaCacheSectors,uint32 sectorsMaxCount);
		void pfs_cache_sectores_dump();



		void pfs_cache_setCacheSectorsMaxCount(uint32 count) ;
		uint32 pfs_cache_getCacheSectorsMaxCount() ;


		void pfs_cache_setCacheSectorsFatMaxCount(uint32 count) ;
		uint32 pfs_cache_getCacheSectorsFatMaxCount() ;

	DiskSector pfs_endpoint_callCachedGetSector(uint32_t sectorNumber , FatFile  * fatFile)  ;

#endif /* PFS_CACHE_H_ */
