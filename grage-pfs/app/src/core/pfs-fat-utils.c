/*
 * pfs-fat-utils.c
 *
 *  Created on: 24/10/2011
 *      Author: joaquin
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "pfs-fat-utils.h"
#include "utils.h"

CacheRecord Cache[255]; // 319 registros ya que hay 1 byte de estado por cada uno, de esta forma tiene 1MB de contenido.
void pfs_fat_utils_cache_initialize()
{
	int i;
	for (i=0;i<256;i++)
	{
		Cache[i].contenido=-1;
	}
}
void pfs_fat_utils_cache_registrar_acceso()
{
	int i;
	for(i=0;i<256;i++)
	{
		if (Cache[i].contenido!=-1) Cache[i].estado++;
	}
}
uint32_t pfs_fat_utils_cache_tiene_contenido(uint32_t contenidoBuscado)
{
	int i;
	for(i=0;i<256;i++)
	{
		if (Cache[i].contenido==contenidoBuscado) return i;
	}
	return -1;
}
uint32_t pfs_fat_utils_cache_get(uint32_t contenidoBuscado){
	uint32_t posicion;

	posicion=pfs_fat_utils_cache_tiene_contenido(contenidoBuscado);
	if (posicion!=-1)
	{
		pfs_fat_utils_cache_registrar_acceso();
		Cache[posicion].estado=0;

		return Cache[posicion].contenido;
	}
	pfs_fat_utils_cache_registrar_acceso();
	return -1;
}

void pfs_fat_utils_cache_put(uint32_t contenido)
{
	int aux,i,variable_LRU;
	variable_LRU = -1;
	if (pfs_fat_utils_cache_tiene_contenido(contenido)==-1){
		for(i=0;i<256;i++)
		{
			if (Cache[i].contenido==-1)
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

		pfs_fat_utils_cache_registrar_acceso();
		Cache[aux].contenido=contenido;
	}
}
/************************ Función pfs_fat_utils_CreateList() ***************************/
/*
   Propósito..: Inicializa la lista
   Invocacion: 	pfs_fat_utils_CreateList(&firstNode);

*/
/********************************************************************/
void pfs_fat_utils_CreateList(rsvCluster ** node){
	*node = NULL;
}

/************************ Función pfs_fat_utils_insertNodo() ***************************/
/*
   Propósito..: Inserta un nodo al final de la lista sin ningun tipo de criterio de orden
   Invocacion: 	insertarNodo(&FirstCluster, nroCluster);
*/
/********************************************************************/
void pfs_fat_utils_insertNodo(rsvCluster ** FirstCluster, uint32_t Cluster){
	//printf("\tInsertando nodo %d\n", Cluster);

   	rsvCluster * newNode = (rsvCluster *) malloc(sizeof(rsvCluster));
   	newNode->Cluster = Cluster;

	if(pfs_fat_utils_IsListEmpty(*FirstCluster)){
		*FirstCluster = (rsvCluster *)malloc(sizeof (rsvCluster));
		(*FirstCluster)->Cluster = Cluster;
		(*FirstCluster)->Next = NULL;
		//printf("\tLa cabeza tiene ahora: %d\n", newNode->Cluster);
	}
	else
	{
		rsvCluster *previousNode, *nextNode;
		previousNode = *FirstCluster;
		nextNode = previousNode->Next;
		while (nextNode!=NULL)
		{
			previousNode = nextNode;
			nextNode = nextNode->Next;
		}
		previousNode->Next = newNode;
		newNode->Next = nextNode;
		//printf("\tEl valor agregado fue: %d\n", newNode->Cluster);
	}
}

/************************ Función pfs_fat_utils_IsListEmpty() ***************************/
/*
   Propósito..: Verifica si la lista esta vacia (primerNodo = NULL)
   Invocacion: 	fs_fat_utils_IsListEmpty(*FirstCluster);
   Devuelve: 1 si esta vacia 0 si no lo esta
*/
/********************************************************************/
uint32_t pfs_fat_utils_IsListEmpty(rsvCluster * FirstCluster){
	if(FirstCluster == NULL){
		return 1;
	}
	else{
		return 0;
	}
}

/************************ Función pfs_fat_utils_ShowList() ***************************/
/*
   Propósito..: Muestra la lista por pantalla
   Invocacion: 	pfs_fat_utils_ShowList(firtsNode);
*/
/********************************************************************/

void pfs_fat_utils_ShowList(rsvCluster * FirstCluster){
	uint32_t n=0;
	//puts("\tListando...");
	rsvCluster * nodoAux = (rsvCluster *)malloc(sizeof (rsvCluster));
	nodoAux = FirstCluster;
	while(nodoAux != NULL){
		printf("\t%d-. El valor es: %d\n", n, nodoAux->Cluster);
		nodoAux = nodoAux->Next;
		n++;
	}

}

/************************ Función pfs_fat_utils_ShowList() ***************************/
/*
   Propósito..: Obtiene el siguiente sector que sera escrito y lo borra de la lista
   Invocacion: 	pfs_fat_utils_GetNextCluster(&firstNode);
   Devuelve: El numero de cluster a escribir
*/
/********************************************************************/
uint32_t pfs_fat_utils_GetNextCluster(rsvCluster ** FirstCluster){
	uint32_t rsv_cluster = (*FirstCluster)->Cluster;
	rsvCluster * aux = (*FirstCluster);
	*FirstCluster = (*FirstCluster)->Next;
	free(aux);
	return rsv_cluster;
}

/************************ Función pfs_fat_utils_FreeList() ***************************/
/*
   Propósito..: Libera la lista completa
   Invocacion: 	pfs_fat_utils_FreeList(&firstNode);
*/
/********************************************************************/

void pfs_fat_utils_FreeList(rsvCluster **FirstCluster)
{
   rsvCluster *currentNode,*nextNode;

   currentNode = *FirstCluster;
   while (currentNode!=NULL)
   {
	   nextNode = currentNode->Next;
      free(currentNode);
      currentNode = nextNode;
   }
   *FirstCluster = NULL;
}


/***************************************************

  Funciones auxiliares para trabajar con los datos
  de FAT32.

***************************************************/


uint32_t pfs_fat_fetchChar(LDirEntry *D, int8_t n) {
    int i = (n % 13);

    if ( i <= 4  ) return D->LDIR_Name1[i];
    if ( i <= 10 ) return D->LDIR_Name2[i-5];
    return D->LDIR_Name3[i-11];
}

int8_t pfs_fat_getNameLength(LDirEntry * ldirentry){
	uint8_t i;
	uint16_t character;

    for ( i=0 ; i<13 ; i++) {
    	character = pfs_fat_fetchChar(ldirentry , i);
		if ( LFN_ISNULL(character) )
			return (i + 1);
    }

    return (i + 1);
}

void pfs_fat_extractName( LDirEntry * d, uint16_t * dest, int8_t length) {
	int8_t i;

	for (i=0; i < (length - 1); i++) {
    	dest[i] = pfs_fat_fetchChar(d , i);
    }

    dest[length - 1] = 0x00;

    return;
}

char *  pfs_get_fileName(LDirEntry * l){
	uint8_t nameLength = pfs_fat_getNameLength(l);

	uint16_t utf16name[13];
	pfs_fat_extractName(l , utf16name , nameLength);

	char * utf8name = (char *)calloc(nameLength,sizeof(char));
	size_t utf8length = 0;

	unicode_utf16_to_utf8_inbuffer(utf16name , nameLength - 1 , utf8name , &utf8length);

	return utf8name;
}


