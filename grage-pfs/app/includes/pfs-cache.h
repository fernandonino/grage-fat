/*
 * pfs-cache.h
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */

#ifndef PFS_CACHE_H_
#define PFS_CACHE_H_
#include "grage-commons.h"
#include "linux-commons-list.h"
		typedef struct {
			Cluster * cluster;
			uint32_t estado;
		}CacheRecord;
		typedef struct {
			uint8_t estado;
			DiskSector sector;
		}CacheSectorRecord;
		List pfs_cache_sectors_initialize(uint32 cacheSize);
		CacheSectorRecord * pfs_cache_get_sector(uint32 sectorID);
		void pfs_cache_put_sectors(DiskSector * sectorNuevo);
		void pfs_cache_sectors_registrar_acceso();
		void pfs_cache_sectores_dump();

#endif /* PFS_CACHE_H_ */
