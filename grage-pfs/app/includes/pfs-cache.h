/*
 * pfs-cache.h
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */

#ifndef PFS_CACHE_H_
#define PFS_CACHE_H_
#include "grage-commons.h"

		typedef struct {
			Cluster * cluster;
			uint32_t estado;
		}CacheRecord;

		void pfs_cache_clusters_initialize(void);
		Cluster * pfs_cache_get_clusters(int clusterID);
		void pfs_cache_put_clusters(Cluster * cluster);
		void pfs_cache_clusters_registrar_acceso();
		int pfs_cache_tiene_cluster(int clusterID);
		void pfs_cache_clusters_dump();

#endif /* PFS_CACHE_H_ */
