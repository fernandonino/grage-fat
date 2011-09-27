/*
 * pfs-cache.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */


#include <stdlib.h>
#include <stdio.h>
#include "grage-commons.h"
#include "pfs-configuration.h"
#include "pfs-cache.h"
	CacheRecord Cache[8];
	void pfs_cache_initialize()
	{
		int i;
		for (i=0;i<8;i++)
		{
			Cache[i].cluster = (Cluster *) malloc (32768);
			Cache[i].cluster->clusterNumber=-1;
			free(Cache[i].cluster);
		}
	}
	Cluster * pfs_cache_get(int clusterID){
		int posicionCluster_Cache;
		Cluster * cluster = NULL;
		posicionCluster_Cache=pfs_cache_tiene_cluster(clusterID);
		if (posicionCluster_Cache!=-1)
		{
			cluster = Cache[posicionCluster_Cache].cluster;
			return cluster;
		}
		return cluster;
	}

	void pfs_cache_put(Cluster * cluster)
	{
		int aux,i,variable_LRU;
		variable_LRU = -1;
		for(i=0;i<8;i++)
		{
			if (Cache[i].cluster->clusterNumber==-1)
			{
				aux=i;
				break;
			}
			else
			{
				if (variable_LRU<Cache[i].estado)
				{
					aux=i;
					variable_LRU=Cache[i].estado;
				}
			}
		}
		Cache[aux].cluster=cluster;
	}

	void pfs_cache_registrar_acceso()
	{
		int i;
		for(i=0;i<8;i++)
		{
			if (Cache[i].cluster->clusterNumber!=-1) Cache[i].estado++;
		}
	}
	int pfs_cache_tiene_cluster(int clusterID)
	{
		int i;
		for(i=0;i<8;i++)
		{
			if (Cache[i].cluster->clusterNumber==clusterID) return i;
		}
		return -1;
	}
