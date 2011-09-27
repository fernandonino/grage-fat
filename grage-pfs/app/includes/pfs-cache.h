/*
 * pfs-cache.h
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */
#include "grage-commons.h"
#ifndef PFS_CACHE_H_
#define PFS_CACHE_H_
	typedef struct {
		Cluster * cluster;
		uint32 estado;
	}CacheRecord;
	void pfs_cache_initialize(void);
	Cluster * pfs_cache_get(int clusterID);
	void pfs_cache_put(Cluster * cluster);
	void pfs_cache_registrar_acceso();
	int pfs_cache_tiene_cluster(int clusterID);

#endif /* PFS_CACHE_H_ */
