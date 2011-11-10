/*
 * ppd-state.c
 *
 *  Created on: 08/10/2011
 *      Author: gonzalo
 */

#include "ppd-configuration.h"
#include "grage-commons.h"
#include "linux-commons.h"
#include "linux-commons-socket.h"
#include "linux-commons-strings.h"
#include "ppd-state.h"
#include "ppd-persistance.h"



	Boolean connectionStatusActive;
	ListenSocket praidSocket;

	void ppd_state_setPraidSocket(ListenSocket ls){
		praidSocket = ls;
		connectionStatusActive = TRUE;
	}
	ListenSocket ppd_state_getPraidSocket(){
		return praidSocket;
	}



	ServerSocket * pfsConnection;

	void ppd_state_setPfsConnection(ServerSocket * p){
		pfsConnection = p;
		connectionStatusActive = TRUE;
	}
	ServerSocket * ppd_state_getPfsConnection(){
		return pfsConnection;
	}


	Boolean ppd_state_isListenMode(){
		return ( commons_string_equals(
				ppd_conf_getPpdMode() ,
				PPD_CONFIGURATION_MODE_LISTEN));
	}

	/*

	ListenSocket ppd_state_getPraidSocket(){
		if(connectionStatusActive )
			if(commons_string_equals( ppd_conf_getPpdMode() ,
							PPD_CONFIGURATION_MODE_CONNECT)){
				return ppd_state_getPraidSocket();
			}else{
				return ppd_state_getPfsConnection()->listenSocket;
			}
		else
			return 0;
	}

	*/



	char * diskStartAddress = NULL;

	char * ppd_state_getDiskStartAddress(){
		return diskStartAddress;
	}
	void ppd_state_setDiskStartAddress(char * anAddress){
		diskStartAddress = anAddress;
	}




	ListenSocket ppdConsoleSocket;

	void ppd_state_setPpdConsoleSocket(ListenSocket ls){
		ppdConsoleSocket = ls;
	}
	ListenSocket ppd_state_getPpdConsoleSocket(){
		return ppdConsoleSocket;
	}



	File * replicationDiskVolume = NULL;

	void ppd_state_setReplicationDiskVolume(File * f){
		replicationDiskVolume = f;
	}
	File * ppd_state_getReplicationDiskVolume(){
		return replicationDiskVolume;
	}




	uint32_t volumeSize;

	void ppd_state_setVolumeSize(uint32_t v){
		volumeSize = v;
	}
	uint32_t ppd_state_getVolumeSize(){
		return volumeSize;
	}



	Boolean workerRunning = FALSE;

	Boolean ppd_state_isWorkerRunning(){
		return workerRunning;
	}
	void ppd_state_setWorkerRunning(Boolean status){
		workerRunning = status;
	}



	Boolean replicationProcessActive = FALSE;

	Boolean ppd_state_isReplicationProcessActive(){
		return replicationProcessActive;
	}
	void ppd_state_setReplicationProcessActive(Boolean s){
		replicationProcessActive = s;
	}
