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
	char * ppdWriteDelay;
	char * ppdReadDelay;

	char * praidAddress;
	char * praidPort;
	char * diskPath;



	void ppd_conf_setPraidPort(char * p){
		praidPort = p;
	}
	char * ppd_conf_getPraidPort(){
		return praidPort;
	}

	void ppd_conf_setPraidAddress(char * p){
		praidAddress = p;
	}
	char * ppd_conf_getPraidAddress(){
		return praidAddress;
	}



	/*
	 * Configuracion
	 */
	void ppd_conf_setPpdMode(char * p){
		ppdMode = p;
	}
	void setPpdAlgoritmo(char * p){
		ppdAlgoritmo = p;
	}
	void ppd_conf_setPpdPort(char * p){
		ppdPort = p;
	}
	void ppd_conf_setPpdIdDisk(char * p){
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
	void setPddWriteDelay(char * secs){
		ppdWriteDelay = secs;
	}
	void setPpdReadDelay(char * secs){
		ppdReadDelay = secs;
	}
	void setPpdLoggingLevel(char * v){
	}

	void ppd_conf_setDiskPath(char * path){
		diskPath = path;
	}



	char * ppd_conf_getPpdMode(){
		return ppdMode;
	}
	char * getPpdAlgoritmo(){
		return ppdAlgoritmo;
	}
	char * ppd_conf_getPpdPort(){
		return ppdPort;
	}
	char * ppd_conf_getPpdIdDisk(){
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
	char * getPpdWriteDelay(){
		return ppdWriteDelay;
	}
	char * getPpdReadDelay(){
		return ppdReadDelay;
	}

	char * ppd_conf_getDiskPath(){
		return diskPath;
	}


	void ppd_configuration_processEntries(char * key , char * value , void * object){
		//log_debug_t("Seteando [%s]=[%s]", key, value);

		/**
		 * aca va el procesamiento de los keys y values.
		*/
		if(commons_string_equals(key , PPD_CONFIGURATION_LOGGING_LEVEL)){
			setPpdLoggingLevel(value);
		}
		if(commons_string_equals(key , PPD_CONFIGURATION_MODE)){
			 ppd_conf_setPpdMode(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_ALGORITMO)){
			setPpdAlgoritmo(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_LISTEN_PORT)){
			 ppd_conf_setPpdPort(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_ID_DISK)){
			ppd_conf_setPpdIdDisk(value);
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
		if(commons_string_equals(key,PPD_CONFIGURATION_READ_DELAY)){
			setPpdReadDelay(value);
		}
		if(commons_string_equals(key,PPD_CONFIGURATION_WRITE_DELAY)){
			setPddWriteDelay(value);
		}
		if(commons_string_equals(key , PPD_CONFIGURATION_PRAID_PORT)){
			ppd_conf_setPraidPort(value);
		}
		if(commons_string_equals(key , PPD_CONFIGURATION_PRAID_HOST)){
			ppd_conf_setPraidAddress(value);
		}
		if(commons_string_equals(key , PPD_CONFIGURATION_DISK_PATH)){
			ppd_conf_setDiskPath(value);
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
			printf("No existe el archivo de configuracion\n");
			exit(EXIT_FAILURE);
		}

		commons_file_loadConfiguration(file , ppd_configuration_processEntries);
		commons_file_closeFile(file);
	}
