/*
 * ppd-configuration.c
 *
 *  Created on: 05/09/2011
 *      Author: EL Seba
 */

#include <stdlib.h>
#include "linux-commons-file.h"
#include "linux-commons-strings.h"
#include "linux-commons-logging.h"
#include "ppd-configuration.h"
#include "ppd-utils.h"
	char * ppdMode;
	char * ppdAlgoritmo;
	int ppdPort;
	int ppdIdDisk;
	int ppdDiskCilinder;
	int ppdDiskHead;
	int ppdDiskSector;
	int ppdReadTimeMs;
	int ppdWriteTimeMs;
	int ppdRpm;
	int ppdSaltoPistaMs;
	/*
	 * Configuracion
	 */
	void setPpdMode(char * p){
		ppdMode = p;
	}
	void setPpdAlgoritmo(char * p){
		ppdAlgoritmo = p;
	}
	void setPpdPort(int p){
		ppdPort = p;
	}
	void setPpdIdDisk(int p){
		ppdIdDisk = p;
	}
	void setPpdDiskCilinder(int p){
		ppdDiskCilinder = p;
	}
	void setPpdDiskHead(int p){
		ppdDiskHead = p;
	}
	void setPpdDiskSector(int p){
		ppdDiskSector = p;
	}
	void setPpdReadTimeMs(int p){
		ppdReadTimeMs = p;
	}
	void setPpdWriteTimeMs(int p){
		ppdWriteTimeMs = p;
	}
	void setPpdRpm(int p){
		ppdRpm = p;
	}
	void setPpdSaltoPistaMs(int p){
		ppdSaltoPistaMs = p;
	}
	char * getPpdMode(){
		return ppdMode;
	}
	char * getPpdAlgoritmo(){
		return ppdAlgoritmo;
	}
	int getPpdPort(){
		return ppdPort;
	}
	int getPpdIdDisk(){
		return ppdIdDisk;
	}
	int getPpdDiskCilinder(){
		return ppdDiskCilinder;
	}
	int getPpdDiskHead(){
		return ppdDiskHead;
	}
	int getPpdDiskSector(){
		return ppdDiskSector;
	}
	int getPpdReadTimeMs(){
		return ppdReadTimeMs;
	}
	int getPpdWriteTimeMs(){
		return ppdWriteTimeMs;
	}
	int getPpdRpm(){
		return ppdRpm;
	}
	int getPpdSaltoPistaMs(){
		return ppdSaltoPistaMs;
	}
	void setPpdLoggingLevel(char * v){
		if(commons_string_equals(v , DEBUG_LVL)){
			commons_logging_setLoggingLevelEnabled(LOGGING_LEVEL_DEBUG);
		}else if(commons_string_equals(v , INFO_LVL)){
			commons_logging_setLoggingLevelEnabled(LOGGING_LEVEL_INFO);
		}else if (commons_string_equals(v , ERROR_LVL)){
			commons_logging_setLoggingLevelEnabled(LOGGING_LEVEL_ERROR);
		}
	}
	void ppd_configuration_processEntries(char * key , char * value , void * object){
		if (commons_logging_isDebugEnabled()) ppd_utils_debug(commons_string_concatAll(4, "Seteando ", key , "=" , value));

		/**
		 * aca va el procesamiento de los keys y values.
		 */
		if(commons_string_equals(key , PPD_CONFIGURATION_LOGGING_LEVEL)){
			setPpdLoggingLevel(value);
		}
		if(commons_string_equals(key , PPD_CONFIGURATION_MODE)){
			setPpdMode(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_ALGORITMO)){
			setPpdAlgoritmo(value);
		}
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

		if(file == NULL){
			//error("No existe el archivo de configuracion");
			exit(EXIT_FAILURE);
		}

		commons_file_loadConfiguration(file , ppd_configuration_processEntries);
		commons_misc_doFreeNull((void**) &file);
	}
