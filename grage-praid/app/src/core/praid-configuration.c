/*
 * praid-configuration.c
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo
 */
#include <string.h>
#include <stdlib.h>

#include <linux-commons-file.h>
#include <linux-commons-strings.h>
#include <linux-commons-console-logging.h>

#include "praid-configuration.h"



	char * devicePort = NULL;

	char * praid_configuration_getDevicePort(void){
		return devicePort;
	}
	void praid_configuration_setDevicePort(char * devAddress){
		devicePort = devAddress;
	}


	Boolean enableReplication = TRUE;

	Boolean praid_configuration_getEnableReplication(void){
		return enableReplication;
	}
	void praid_configuration_setEnableReplication(Boolean status){
		enableReplication = status;
	}



	void praid_configuration_processEntries(char * key , char * value , void * object){
		if( !strcmp(key, PRAID_CONF_DEVICE_PORT)){
			praid_configuration_setDevicePort(value);
		}else if(!strcmp(key , PRAID_CONF_CONSOLE_LEVEL)){

			commons_console_logging_setLevel(value);

		}else if( !strcmp(key , PRAID_CONF_ENABLE_REPLICATION)){
			if( !strcmp(value , PRAID_CONF_VALUE_FALSE)){
				praid_configuration_setEnableReplication(FALSE);
			}else if (!strcmp(value , PRAID_CONF_VALUE_TRUE)){
				praid_configuration_setEnableReplication(TRUE);
			}
		}else if(commons_string_equals(key , PRAID_CONF_POOLED_CONNECTIONS_ENABLED)){
			if( commons_string_equals(value , "true")){
				praid_conf_setPooledConnections(TRUE);
			}else if(commons_string_equals(value , "false")){
				praid_conf_setPooledConnections(FALSE);
			}
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

		if(file == NULL){
			puts("Ha ocurrido un error buscando el archivo de configuracion");
			exit(EXIT_FAILURE);
		}

		commons_file_loadConfiguration(file , praid_configuration_processEntries);
		commons_file_closeFile(file);

		if(commons_console_logging_isDefault())
			puts("[ Se ha cargado la configuracion ]");
	}




	Boolean pooledConnections = FALSE;

	void praid_conf_setPooledConnections(Boolean s){
		pooledConnections = s;
	}
	Boolean praid_conf_isPooledConnections(){
		return pooledConnections;
	}

