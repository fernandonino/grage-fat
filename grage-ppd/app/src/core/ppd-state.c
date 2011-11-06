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

	Boolean connectionStatusActive;
	ListenSocket praidSocket;
	ServerSocket * pfsConnection;


	void ppd_state_setPraidSocket(ListenSocket ls){
		praidSocket = ls;
		connectionStatusActive = TRUE;
	}
	ListenSocket ppd_state_getPraidSocket(){
		return praidSocket;
	}
	void ppd_state_setPfsConnection(ServerSocket * p){
		pfsConnection = p;
		connectionStatusActive = TRUE;
	}
	ServerSocket * ppd_state_getPfsConnection(){
		return pfsConnection;
	}
	ListenSocket ppd_state_getActiveSocket(){
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


	char * diskStartAddress;

	char * ppd_state_getDiskStartAddress(){
		return diskStartAddress;
	}
	void ppd_state_setDiskStartAddress(char * anAddress){
		diskStartAddress = anAddress;
	}



	uint32_t sectorsCount;

	void ppd_state_setSectorsCount(uint32_t count){
		sectorsCount = count;
	}
	uint32_t ppd_state_getSectorsCount(){
		return sectorsCount;
	}



	ListenSocket ppdConsoleSocket;

	void ppd_state_setPpdConsoleSocket(ListenSocket ls){
		ppdConsoleSocket = ls;
	}
	ListenSocket ppd_state_getPpdConsoleSocket(){
		return ppdConsoleSocket;
	}

