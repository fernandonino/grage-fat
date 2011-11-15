/*
 * pfs-endpoint.h
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */

#include "grage-commons.h"
#include <stdint.h>

#ifndef PFS_ENDPOINT_H_
#define PFS_ENDPOINT_H_

	void pfs_endpoint_callPutSector(DiskSector);
	DiskSector pfs_endpoint_callGetSector(uint32_t);

	//Funciones prestadas del PPD
	char * ppd_persistance_unmapDisk(char * diskId , char * mapping);
	char * ppd_persistance_mapDisk(char * diskId);
	char * ppd_state_getDiskStartAddress(void);
	void ppd_state_setDiskStartAddress(char *);
	void ppd_initializeDisk(void);


	ListenSocket pfs_endpoint_doHandshake();

#endif /* PFS_ENDPOINT_H_ */
