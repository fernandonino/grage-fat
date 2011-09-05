/*
 * praid-configuration.c
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo
 */

#include "linux-commons-file.h"

#include "praid-configuration.h"


	/*
	 * Configuracion
	 */


	void praid_configuration_processEntries(char * key , char * value , void * object){

		/**
		 * aca va el procesamiento de los keys y values.
		 */
	}







	/*
	 * Inicializacion de la configuracion
	 */

	char * configurationFile = PPD_DEFAULT_CONFIGURATION_FILE;

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
