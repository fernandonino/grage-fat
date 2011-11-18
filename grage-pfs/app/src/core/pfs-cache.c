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
#include "linux-commons-list.h"
#include "grage-commons.h"
#include "linux-commons-file.h"
#include "linux-commons-strings.h"
#include "pfs-configuration.h"
#include "pfs-cache.h"


	//CacheRecord Cache[8];

	List listaCacheSectors;
    uint32 cacheSectorsMaxCount;

    void pfs_cache_setCacheSectorsMaxCount(uint32 count){
    	cacheSectorsMaxCount = count;
    }
    uint32 pfs_cache_getCacheSectorsMaxCount(){
        return cacheSectorsMaxCount;
    }

	void pfs_cache_sectors_initialize()
	{
		pfs_cache_setCacheSectorsMaxCount(pfs_configuration_getCacheSize()*2);


		Boolean eq(void * s1 , void * s2){
			CacheSectorRecord * p1 = (CacheSectorRecord *) s1;
			CacheSectorRecord * p2 = (CacheSectorRecord *) s2;
			return p1->sector.sectorNumber== p2->sector.sectorNumber;
        }
		Boolean ordenar(void * s1 , void * s2){
			CacheSectorRecord * p1 = (CacheSectorRecord *) s1;
			CacheSectorRecord * p2 = (CacheSectorRecord *) s2;
			return p1->estado < p2->estado;
		}
		listaCacheSectors = commons_list_buildList(NULL,eq,commons_list_ORDER_ALWAYS_FIRST);
	}

	void pfs_cache_sectors_registrar_acceso()
	{
		Iterator * fuckingIterator = commons_iterator_buildIterator(listaCacheSectors);

		while (commons_iterator_hasMoreElements(fuckingIterator)){
			((CacheSectorRecord *)commons_iterator_next(fuckingIterator))->estado++;
		}
		free(fuckingIterator);
	}

	CacheSectorRecord * pfs_cache_get_sector(uint32 sectorID){
		Boolean get(CacheSectorRecord * a){
			return (a->sector.sectorNumber == sectorID);
		}
		pfs_cache_sectors_registrar_acceso();
		return commons_list_getNodeByCriteria(listaCacheSectors , get);

	}
	void pfs_cache_put_sectors(DiskSector * sectorNuevo)
	{
		if (commons_list_getSize(listaCacheSectors) < pfs_cache_getCacheSectorsMaxCount()){
			CacheSectorRecord * nodo = (CacheSectorRecord *) malloc (sizeof (CacheSectorRecord));
			nodo->estado=0;
			memcpy(nodo->sector.sectorContent,sectorNuevo->sectorContent,sizeof sectorNuevo->sectorContent);
			nodo->sector.sectorNumber=sectorNuevo->sectorNumber;
			commons_list_addNode(listaCacheSectors,nodo);
			pfs_cache_sectors_registrar_acceso();
		}
	}


	void pfs_cache_sectores_dump()
	{
		char linea[512];
		int i;Iterator * fuckingIterator = commons_iterator_buildIterator(listaCacheSectors);
		time_t log_time;
		struct tm *log_tm;
		char str_time[128];
    	FILE * cache_dump = commons_file_openOrCreateFile("cache_dump.txt");
		time (&log_time);
		log_tm = localtime(&log_time);
		strftime(str_time, 128, "%Y.%m.%d  %H:%M:%S", log_tm);

		sprintf(linea,"%s",str_time);
		commons_file_insertLine(linea,cache_dump);
		sprintf(linea,"Tamaño de Bloque de Cache: %d b \n",pfs_configuration_getCacheSize());
		commons_file_insertLine(linea,cache_dump);
		i=0;
		//if (pfs_configuration_getCacheSize()!=NULL)

		printf("tamanio de cache: %d\n",pfs_configuration_getCacheSize());

		sprintf(linea,"Cantidad de bloques de Cache: %d",commons_list_getSize(listaCacheSectors));
		commons_file_insertLine(linea,cache_dump);
		i = 0;
		while (commons_iterator_hasMoreElements(fuckingIterator))
		{
			sprintf(linea,"Contenido de Bloque de Cache %d:",i);
			commons_file_insertLine(linea,cache_dump);
			commons_file_insertLine(((CacheSectorRecord *)commons_iterator_next(fuckingIterator))->sector.sectorContent,cache_dump);
			i++;
		}

		commons_file_closeFile(cache_dump);
	}

