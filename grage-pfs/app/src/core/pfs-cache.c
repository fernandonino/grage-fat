/*
 * pfs-cache.c
 *
 *  Created on: 15/09/2011
 *      Author: sebas
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
#include "pfs-state.h"



	//CacheRecord Cache[8];
	List listaCacheFat;

	uint32 cacheSectorsMaxCount;
	uint32 cacheSectorsFatMaxCount;

	List pfs_cache_getListaCacheFat(){
		return listaCacheFat;
	}

	void pfs_cache_setListaCacheFat(List lista){
		listaCacheFat = lista;
	}

	void pfs_cache_setCacheSectorsMaxCount(uint32 count){
    	cacheSectorsMaxCount = count;
    }

    uint32 pfs_cache_getCacheSectorsMaxCount(){
        return cacheSectorsMaxCount;
    }

    void pfs_cache_setCacheSectorsFatMaxCount(uint32 count){
    	cacheSectorsFatMaxCount = count;
    }

    uint32 pfs_cache_getCacheSectorsFatMaxCount(){
        return cacheSectorsFatMaxCount;
    }

    Boolean pfs_cache_isFatSectorReserved(uint32 sectorNumber){
    	Volume * v = pfs_state_getVolume();
    	if (sectorNumber <= (v->rsv)+1024) return TRUE;
    	return FALSE;
    }

	List pfs_cache_sectors_initialize(){

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

		return (commons_list_buildList(NULL,eq,commons_list_ORDER_ALWAYS_FIRST));
	}


	void pfs_cache_initialize(){
		Volume * v = pfs_state_getVolume();
		pfs_state_initializeOpenFiles();
		pfs_cache_setCacheSectorsFatMaxCount(v->fatSize*20/100);
		pfs_cache_setListaCacheFat(pfs_cache_sectors_initialize());
		pfs_cache_setCacheSectorsMaxCount(pfs_configuration_getCacheSize()*2);
	}


	void pfs_cache_sectors_registrar_acceso(List listaCacheSectors){
		Iterator * fuckingIterator = commons_iterator_buildIterator(listaCacheSectors);

		while (commons_iterator_hasMoreElements(fuckingIterator)){
			((CacheSectorRecord *)commons_iterator_next(fuckingIterator))->estado++;
		}
		free(fuckingIterator);
	}

	CacheSectorRecord * pfs_cache_get_sector(uint32 sectorID,List listaCacheSectors,uint32 sectorsMaxCount){
		Boolean get(CacheSectorRecord * a){
			return (a->sector.sectorNumber == sectorID);
		}
		pfs_cache_sectors_registrar_acceso(listaCacheSectors);
		return commons_list_getNodeByCriteria(listaCacheSectors , get);
	}

	void pfs_cache_put_sectors(DiskSector * sectorNuevo,List listaCacheSectors,uint32 sectorsMaxCount){
		CacheSectorRecord * nodo = (CacheSectorRecord *) malloc (sizeof (CacheSectorRecord));
		nodo->estado=0;
		if (commons_list_getSize(listaCacheSectors) >= sectorsMaxCount){
			Iterator * fuckingIterator = commons_iterator_buildIterator(listaCacheSectors);
			while (commons_iterator_hasMoreElements(fuckingIterator)){
				if (nodo->estado <= ((CacheSectorRecord *)commons_iterator_next(fuckingIterator))->estado){
					nodo = (CacheSectorRecord *)commons_iterator_next(fuckingIterator);
				}
			}
			commons_list_removeNode(listaCacheSectors, nodo,free);
		}
		//AGREGO EL NODO NUEVO
		nodo->estado=0;
		memcpy(nodo->sector.sectorContent,sectorNuevo->sectorContent,sizeof sectorNuevo->sectorContent);
		nodo->sector.sectorNumber=sectorNuevo->sectorNumber;
		commons_list_addNode(listaCacheSectors,nodo);
		pfs_cache_sectors_registrar_acceso(listaCacheSectors);
	}


	void pfs_cache_sectores_dump(List listaCacheSectors,uint32 sectorsMaxCount){
		char linea[512];
		bzero(linea,512);
		int i;Iterator * fuckingIterator = commons_iterator_buildIterator(listaCacheSectors);
		time_t log_time;
		struct tm *log_tm;
		char str_time[128];
    	FILE * cache_dump = commons_file_openOrCreateFile("cache_dump.txt");
		time (&log_time);
		log_tm = localtime(&log_time);
		strftime(str_time, 128, "%Y.%m.%d  %H:%M:%S", log_tm);
		sprintf(linea,"%s",str_time);
		//fwrite(linea , sizeof(char) , sizeof(linea) , cache_dump);
		commons_file_insertLine(linea,cache_dump);
		sprintf(linea,"Tamanio de Bloque de Cache: %d kb ",sectorsMaxCount);
		//fwrite(linea , sizeof(char) , sizeof(linea) , cache_dump);
		commons_file_insertLine(linea,cache_dump);
		i=0;
		//if (pfs_configuration_getCacheSize()!=NULL)



		sprintf(linea,"Cantidad de bloques de Cache: %d ",commons_list_getSize(listaCacheSectors));
		//fwrite(linea , sizeof(char) , sizeof(linea) , cache_dump);
		commons_file_insertLine(linea,cache_dump);
		i = 0;
		while (commons_iterator_hasMoreElements(fuckingIterator))
		{
			sprintf(linea,"Contenido de Bloque de Cache %d:",i);
			//fwrite(linea , sizeof(char) , sizeof(linea) , cache_dump);
			commons_file_insertLine(linea,cache_dump);
			commons_file_insertLine(((CacheSectorRecord *)commons_iterator_next(fuckingIterator))->sector.sectorContent,cache_dump);
			//fwrite(((CacheSectorRecord *)commons_iterator_next(fuckingIterator))->sector.sectorContent ,
			//			sizeof(char) ,
			//			sizeof(((CacheSectorRecord *)commons_iterator_next(fuckingIterator))->sector.sectorContent) ,
			//			cache_dump);
			i++;
		}

		commons_file_closeFile(cache_dump);
	}



	void pfs_cache_sectores_dumpBIS(List listaCacheSectors,uint32 sectorsMaxCount){
		char linea[512];CacheSectorRecord * nodo;
		bzero(linea,512);
		int i;Iterator * fuckingIterator = commons_iterator_buildIterator(listaCacheSectors);
		time_t log_time;
		struct tm *log_tm;
		char str_time[128];
		FILE * cache_dump = commons_file_openOrCreateFile("cache_dump.txt");
		time (&log_time);
		log_tm = localtime(&log_time);
		strftime(str_time, 128, "%Y.%m.%d  %H:%M:%S", log_tm);
		sprintf(linea,"%s",str_time);

		fwrite(linea , sizeof(char) , strlen(linea) , cache_dump);
		i=strlen(linea);
		while(i < sizeof(linea)){
			if (linea[i]=='\0'){
				fputc(' ',cache_dump);
			}else fputc(linea[i],cache_dump);
			i++;
		}
		fputc('\n',cache_dump);
		//commons_file_insertLine(linea,cache_dump);
		sprintf(linea,"Tamanio de Bloque de Cache: %d kb ",sectorsMaxCount);
		fwrite(linea , sizeof(char) , strlen(linea) , cache_dump);
		i=strlen(linea);
		while(i < sizeof(linea)){
			if (linea[i]=='\0'){
				fputc(' ',cache_dump);
			}else fputc(linea[i],cache_dump);
			i++;
		}
		fputc('\n',cache_dump);

		//commons_file_insertLine(linea,cache_dump);
		i=0;
		//if (pfs_configuration_getCacheSize()!=NULL)



		sprintf(linea,"Cantidad de bloques de Cache: %d ",commons_list_getSize(listaCacheSectors));
		fwrite(linea , sizeof(char) , strlen(linea) , cache_dump);
		i=strlen(linea);
		while(i < sizeof(linea)){
			if (linea[i]=='\0'){
				fputc(' ',cache_dump);
			}else fputc(linea[i],cache_dump);
			i++;
		}
		fputc('\n',cache_dump);
		//commons_file_insertLine(linea,cache_dump);
		i = 0;

		while (commons_iterator_hasMoreElements(fuckingIterator))
		{
			nodo = ((CacheSectorRecord *)commons_iterator_next(fuckingIterator));
			sprintf(linea,"Contenido de Bloque de Cache %d:",i);
			fwrite(linea , sizeof(char) , strlen(linea) , cache_dump);
			i=strlen(linea);
			while(i < sizeof(linea)){
				if (linea[i]=='\0'){
					fputc(' ',cache_dump);
				}else fputc(linea[i],cache_dump);
				i++;
			}
			fputc('\n',cache_dump);
			//commons_file_insertLine(linea,cache_dump);
			//commons_file_insertLine(((CacheSectorRecord *)commons_iterator_next(fuckingIterator))->sector.sectorContent,cache_dump);
			fwrite(nodo->sector.sectorContent ,
						sizeof(char) ,
						strlen(nodo->sector.sectorContent) ,
						cache_dump);
			i=strlen(linea);
			while(i < sizeof(linea)){
				if (linea[i]=='\0'){
					fputc(' ',cache_dump);
				}else fputc(linea[i],cache_dump);
				i++;
			}
			fputc('\n',cache_dump);
			i++;
		}

		commons_file_closeFile(cache_dump);
	}
