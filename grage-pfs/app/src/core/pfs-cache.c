/*
 * pfs-cache.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */

#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include <stdio.h>
#include "grage-commons.h"
#include "linux-commons-file.h"
#include "linux-commons-strings.h"
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


	void pfs_cache_dump()
	{
		char linea[512];
		int i;
		time_t log_time;
		struct tm *log_tm;
		char str_time[128];
		FILE * cache_dump = commons_file_openOrCreateFile("cache_dump.txt");
		time (&log_time);
		log_tm = localtime(&log_time);
		strftime(str_time, 128, "%Y.%m.%d  %H:%M:%S", log_tm);

		sprintf(linea,"%s",str_time);
		commons_file_insertLine(linea,cache_dump);
		sprintf(linea,"Tamaño de Bloque de Cache: %s b",pfs_configuration_getCacheSize());
		commons_file_insertLine(linea,cache_dump);
		i=0;
		/*if (pfs_configuration_getCacheSize()!=NULL)*/	i=atoi( pfs_configuration_getCacheSize() );
		printf("tamanio de cache: %d\n",i);

		sprintf(linea,"Cantidad de bloques de Cache: %d",i);
		commons_file_insertLine(linea,cache_dump);
		i = 0;
		while (Cache[i].cluster->clusterNumber!=-1 && i != 8)
		{
			sprintf(linea,"Contenido de Bloque de Cache %d:",i);
			commons_file_insertLine(linea,cache_dump);
			commons_file_insertLine((commons_string_concatAll(8,Cache[i].cluster->sectors[0].sectorContent,Cache[i].cluster->sectors[1].sectorContent,
					Cache[i].cluster->sectors[2].sectorContent , Cache[i].cluster->sectors[3].sectorContent,
					Cache[i].cluster->sectors[4].sectorContent , Cache[i].cluster->sectors[5].sectorContent,
					Cache[i].cluster->sectors[6].sectorContent , Cache[i].cluster->sectors[7].sectorContent)),cache_dump);
			i++;
		}

		commons_file_closeFile(cache_dump);
	}
