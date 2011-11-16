/*
 * pfs-state.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */


#include <stdlib.h>
#include <stdio.h>

#include "pfs-state.h"
#include "pfs-configuration.h"


	ListenSocket dataSocket;
	BPB biosParameterBlock;
	Volume * volume;

	void pfs_state_setDataSocket(ListenSocket s){
		dataSocket = s;
	}
	ListenSocket pfs_state_getDataSocket(){
		return dataSocket;

	}


	void pfs_state_setBiosParameterBlock(BPB bpb){
		biosParameterBlock = bpb;
	}
	BPB pfs_state_getBiosParameterBlock(){
		return biosParameterBlock;
	}


	void pfs_state_setVolume(Volume * v){
		volume = v;
	}
	Volume * pfs_state_getVolume(){
		return volume;
	}





	Boolean pfs_state_isPooledConnectionsEnabled(){
		return (pfs_configuration_getMaxConnections() > 0);
	}
