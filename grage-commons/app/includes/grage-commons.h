/*
 * grage-commons.h
 *
 *  Created on: 19/09/2011
 *      Author: utn_so
 */


#include "linux-commons.h"

#ifndef GRAGE_COMMONS_H_
#define GRAGE_COMMONS_H_


	typedef struct {

		uint32 sectorNumber;
		char sectorContent[512];

	}DiskSector;


	typedef struct {
		uint32 clusterNumber;
		DiskSector sectors[8];
	}Cluster;


#endif /* GRAGE_COMMONS_H_ */
