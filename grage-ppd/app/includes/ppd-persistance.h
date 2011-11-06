/*
 * ppd-persistance.h
 *
 *  Created on: 29/09/2011
 *      Author: Fernando
 */

#include <stdint.h>
#include "grage-commons.h"

#ifndef PPD_PERSISTANCE_H_
#define PPD_PERSISTANCE_H_


	char * ppd_persistance_mapDisk(char * );
	char * ppd_persistance_unmapDisk(char * , char * );
	void ppd_persistence_writeSector(DiskSector , char *);
	DiskSector ppd_persistence_readSector(uint32_t sectorId , char *);

#endif /* PPD_PERSISTANCE_H_ */
