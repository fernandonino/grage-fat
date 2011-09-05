/*
 * ppd-configuration.c
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo
 */

#include "linux-commons-file.h"

#include "ppd-configuration.h"


	/*
	 * Configuracion
	 */


	void ppd_configuration_processEntries(char * key , char * value , void * object){

		/**
		 * aca va el procesamiento de los keys y values.
		 */
	}







	/*
	 * Inicializacion de la configuracion
	 */

	char * configurationFile = PPD_DEFAULT_CONFIGURATION_FILE;

	void ppd_configuration_setConfigurationFile(char * s){
		configurationFile = s;
	}
	char * ppd_configuration_getConfigurationFile(){
		return configurationFile;
	}

	void ppd_configuration_setup(){
		File * file = commons_file_openFile(configurationFile);
		commons_file_loadConfiguration(file , ppd_configuration_processEntries);
		commons_misc_doFreeNull((void**) &file);
	}
