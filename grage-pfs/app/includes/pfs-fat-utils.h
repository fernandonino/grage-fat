/*
 * pfs-fat-utils.h
 *
 *  Created on: 24/10/2011
 *      Author: joaquin
 */

#ifndef PFS_FAT_UTILS_H_
#define PFS_FAT_UTILS_H_

typedef struct Node {
    uint32_t Cluster;
    struct Node * Next;
} rsvCluster;

//Ejemplifico abajo de cada una, como invocar a cada funcion
void pfs_fat_utils_CreateList(rsvCluster **);
	//pfs-fat-utils-crearLista(&firstNode);
void pfs_fat_utils_InsertNodo(rsvCluster ** , uint32_t );
	//insertNode(&firstNode, Cluster);
void pfs_fat_utils_ShowList(rsvCluster * );
	//pfs_fat_utils_ShowList(firstNode);
void pfs_fat_utils_FreeList(rsvCluster **);
	//pfs_fat_utils_FreeList(&firstNode);
uint32_t pfs_fat_utils_GetNextCluster(rsvCluster ** );
	//pfs_fat_utils_GetNextCluster(&firstNode);
uint32_t pfs_fat_utils_IsListEmpty(rsvCluster * );
	//pfs_fat_utils_IsListEmpty(*firstCluster);

#endif /* PFS_FAT_UTILS_H_ */
