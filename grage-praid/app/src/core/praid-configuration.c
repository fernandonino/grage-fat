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


	char * getDevicePort(void){
		return deviceAddress;
	}

	void setDevicePort(char * devAddress){
		deviceAddress = devAddress;
	}

	char * getConsoleStatus(void){
		return consoleStatus;
	}

	void setConsoleStatus(char * consoleStatusValue){
		consoleStatus = consoleStatusValue;
	}

	void praid_configuration_processEntries(char * key , char * value , void * object){
		if( !strcmp(key, PRAID_DEVICE_PORT)){
			setDevicePort(value);
		}else if(!strcmp(key , PRAID_CONSOLE_STATUS)){
			setConsoleStatus(value);
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
		commons_misc_doFreeNull((void**) &file);
	}
