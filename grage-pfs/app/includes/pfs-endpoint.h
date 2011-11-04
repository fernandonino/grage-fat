/*
 * pfs-endpoint.h
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */

#include <grage-commons.h>
#include <stdint.h>

#ifndef PFS_ENDPOINT_H_
#define PFS_ENDPOINT_H_

	void pfs_endpoint_callPutSector(void);
	DiskSector pfs_endpoint_callGetSector(uint32_t);

#endif /* PFS_ENDPOINT_H_ */
