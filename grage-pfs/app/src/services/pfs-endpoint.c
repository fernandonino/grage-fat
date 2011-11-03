/*
 * pfs-endpoint.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "grage-commons.h"
#include "pfs-endpoint.h"

	void pfs_endpoint_callPutSectors(){

	}

	DiskSector pfs_endpoint_callGetSectors(uint32_t sectorNumber){
		DiskSector s;
		s.sectorNumber = sectorNumber;

		return s;
	}

	Cluster pfs_endpoint_getCluster(uint32_t s){
		uint8_t i;
		Cluster cluster;

		for( i = 0 ; i < 8 ; i++){
			cluster.sectors[i] = pfs_endpoint_callGetSectors(s + i);
		}

		return cluster;
	}


