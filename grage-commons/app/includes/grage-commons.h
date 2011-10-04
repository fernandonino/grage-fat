/*
 * grage-commons.h
 *
 *  Created on: 19/09/2011
 *      Author: utn_so
 */


#ifndef GRAGE_COMMONS_H_
#define GRAGE_COMMONS_H_

#include "linux-commons.h"

#define SECTOR_SIZE	512

	typedef struct {

		uint32 sectorNumber;
		char sectorContent[SECTOR_SIZE];

	}DiskSector;


	typedef struct {
		uint32 clusterNumber;
		DiskSector sectors[8];
	}Cluster;


#endif /* GRAGE_COMMONS_H_ */
