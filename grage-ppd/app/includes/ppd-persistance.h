/*
 * ppd-persistance.h
 *
 *  Created on: 29/09/2011
 *      Author: Fernando
 */

#ifndef PPD_PERSISTANCE_H_
#define PPD_PERSISTANCE_H_

#include "grage-commons.h"

	char * ppd_persistance_mapDisk(char * );
	char * ppd_persistance_unmapDisk(char * , char * );
	void ppd_persistence_writeSector(DiskSector , void *);
	void ppd_persistence_readSector(DiskSector , void *);

#endif /* PPD_PERSISTANCE_H_ */
