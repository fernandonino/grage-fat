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
