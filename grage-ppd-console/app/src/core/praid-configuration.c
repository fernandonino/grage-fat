/*
 * praid-configuration.c
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo
 */
#include <string.h>
#include "linux-commons-file.h"

#include "praid-configuration.h"

	char * deviceAddress = NULL;
	char * consoleStatus = NULL;


	char * praid_configuration_getDevicePort(void){
		return deviceAddress;
	}

	void praid_configuration_setDevicePort(char * devAddress){
		deviceAddress = devAddress;
	}

	char * praid_configuration_getConsoleStatus(void){
		return consoleStatus;
	}

	void praid_configuration_setConsoleStatus(char * consoleStatusValue){
		consoleStatus = consoleStatusValue;
	}

	void praid_configuration_processEntries(char * key , char * value , void * object){
		if( !strcmp(key, PRAID_DEVICE_PORT)){
			praid_configuration_setDevicePort(value);
		}else if(!strcmp(key , PRAID_CONSOLE_STATUS)){
			praid_configuration_setConsoleStatus(value);
		}
	}

	/*
	 * Inicializacion de la configuracion
	 */

	char * configurationFile = PRAID_DEFAULT_CONFIGURATION_FILE;

	void praid_configuration_setConfigurationFile(char * s){
		configurationFile = s;
	}
	char * praid_configuration_getConfigurationFile(){
		return configurationFile;
	}

	void praid_configuration_setup(){
		File * file = commons_file_openFile(configurationFile);
		commons_file_loadConfiguration(file , praid_configuration_processEntries);
		commons_file_closeFile(file);
	}