/*
 * pfs-endpoint.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */


#include <stdlib.h>
#include <stdio.h>

#include <linux-commons-errors.h>

#include "pfs-endpoint.h"
#include "nipc-messaging.h"

	void pfs_endpoint_callPutSector(){

	}

	DiskSector pfs_endpoint_callGetSector(uint32_t sectorNumber){
		RuntimeErrorValidator * v = commons_errors_buildSuccessValidator();

		if(commons_errors_hasError(v)){
			puts(v->errorDescription);
		}
		DiskSector d;
		return d;
	}






