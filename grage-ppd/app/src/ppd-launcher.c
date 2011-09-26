/*
 * ppd-launcher.c
 *
 *  Created on: 05/09/2011
 *      Author: EL Seba
 */

#include <stdlib.h>
#include <stdio.h>
#include "linux-commons-logging.h"
#include "ppd-configuration.h"

t_log * logstruct;


	void ppd_launcher_initialize(){
		logstruct = log_create("ppd", PPD_DEFAULT_LOG_FILE, DEBUG | INFO | WARNING | ERROR, M_CONSOLE_DISABLE);
		ppd_configuration_setup();
	}


	void ppd_launcher_doLaunch(){

	}


	int main(int argc, char *args[]){

		/*
		 * Parametro de la aplicacion - path al archivo de conf
		 */
		if(argc > 0){
			ppd_configuration_setConfigurationFile(args[0]);
		}

		ppd_launcher_initialize();

		ppd_launcher_doLaunch();

		return EXIT_SUCCESS;
	}
