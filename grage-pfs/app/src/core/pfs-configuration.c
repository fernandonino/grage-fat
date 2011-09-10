/*
 * pfs-configuration.c
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo
 */

#include <string.h>
#include "linux-commons-file.h"

#include "pfs-configuration.h"


	/*
	 * Configuracion
	 */

	char * deviceAddress = NULL;
	char * devicePort = NULL;
	char * maxConnections = NULL;
	char * cacheSize = NULL;

	char * getDeviceAddress(void){
		return deviceAddress;
	}

	void setDeviceAddress(char * devAddress){
		deviceAddress = devAddress;
	}

	char * getDevicePort(void){
		return devicePort;
	}

	void setDevicePort(char * devPort){
		devicePort = devPort;
	}

	char * getMaxConnections(void){
		return maxConnections;
	}

	void setMaxConnections(char * numberOfConnections){
		maxConnections = numberOfConnections;
	}

	char * getCacheSize(void){
		return cacheSize;
	}

	void setCacheSize(char * sizeOfCache){
		cacheSize = sizeOfCache;
	}

	/*
	 * Procesamiento de la configuracion
	 */

	void pfs_configuration_processEntries(char * key , char * value , void * object){

		if( !strcmp(key , PFS_DEVICE_ADDRESS)){
                setDeviceAddress(value);
        }else if(!strcmp(key , PFS_DEVICE_PORT)){
                setDevicePort(value);
        }else if(!strcmp(key , PFS_MAX_CONNECTIONS)){
                setMaxConnections(value);
        }else if(!strcmp(key , PFS_CACHE_SIZE)){
                setCacheSize(value);
        }
	}


	/*
	 * Inicializacion de la configuracion
	 */

	char * configurationFile = PFS_DEFAULT_CONFIGURATION_FILE;

	void pfs_configuration_setConfigurationFile(char * s){
		configurationFile = s;
	}
	char * pfs_configuration_getConfigurationFile(){
		return configurationFile;
	}

	void pfs_configuration_setup(){
		File * file = commons_file_openFile(configurationFile);
		commons_file_loadConfiguration(file , pfs_configuration_processEntries);
		commons_misc_doFreeNull((void**) &file);
	}
