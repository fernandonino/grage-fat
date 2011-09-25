/*
 * ppd-configuration.c
 *
 *  Created on: 05/09/2011
 *      Author: EL Seba
 */

#include <stdlib.h>
#include <linux-commons-logging.h>
#include "linux-commons-file.h"
#include "linux-commons-strings.h"
#include "ppd-configuration.h"
#include "ppd-utils.h"
	extern t_log * logstruct;
	char * ppdMode;
	char * ppdAlgoritmo;
	char * ppdPort;
	char * ppdIdDisk;
	char * ppdDiskCilinder;
	char * ppdDiskHead;
	char * ppdDiskSector;
	char * ppdReadTimeMs;
	char * ppdWriteTimeMs;
	char * ppdRpm;
	char * ppdSaltoPistaMs;
	/*
	 * Configuracion
	 */
	void setPpdMode(char * p){
		ppdMode = p;
	}
	void setPpdAlgoritmo(char * p){
		ppdAlgoritmo = p;
	}
	void setPpdPort(char * p){
		ppdPort = p;
	}
	void setPpdIdDisk(char * p){
		ppdIdDisk = p;
	}
	void setPpdDiskCilinder(char * p){
		ppdDiskCilinder = p;
	}
	void setPpdDiskHead(char * p){
		ppdDiskHead = p;
	}
	void setPpdDiskSector(char * p){
		ppdDiskSector = p;
	}
	void setPpdReadTimeMs(char * p){
		ppdReadTimeMs = p;
	}
	void setPpdWriteTimeMs(char * p){
		ppdWriteTimeMs = p;
	}
	void setPpdRpm(char * p){
		ppdRpm = p;
	}
	void setPpdSaltoPistaMs(char * p){
		ppdSaltoPistaMs = p;
	}
	char * getPpdMode(){
		return ppdMode;
	}
	char * getPpdAlgoritmo(){
		return ppdAlgoritmo;
	}
	char * getPpdPort(){
		return ppdPort;
	}
	char * getPpdIdDisk(){
		return ppdIdDisk;
	}
	char * getPpdDiskCilinder(){
		return ppdDiskCilinder;
	}
	char * getPpdDiskHead(){
		return ppdDiskHead;
	}
	char * getPpdDiskSector(){
		return ppdDiskSector;
	}
	char * getPpdReadTimeMs(){
		return ppdReadTimeMs;
	}
	char * getPpdWriteTimeMs(){
		return ppdWriteTimeMs;
	}
	char * getPpdRpm(){
		return ppdRpm;
	}
	char * getPpdSaltoPistaMs(){
		return ppdSaltoPistaMs;
	}
	void setPpdLoggingLevel(char * v){
	}

	void ppd_configuration_processEntries(char * key , char * value , void * object){
		log_debug_t(logstruct,"Seteando [%s]=[%s]", key, value);

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
		if(commons_string_equals(key,PPD_CONFIGURATION_LISTEN_PORT)){
			setPpdPort(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_ID_DISK)){
			setPpdIdDisk(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_CILINDER)){
			setPpdDiskCilinder(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_HEAD)){
			setPpdDiskHead(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_SECTOR)){
			setPpdDiskSector(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_READ_TIME)){
			setPpdReadTimeMs(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_WRITE_TIME)){
			setPpdWriteTimeMs(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_RPM)){
			setPpdRpm(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_SALTO_PISTA)){
			setPpdSaltoPistaMs(value);
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
		commons_file_closeFile(file);
	}
