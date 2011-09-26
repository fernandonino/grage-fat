/*
 * praid-launcher.c
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo
 */

#include <stdlib.h>
#include <stdio.h>
#include <linux-commons-logging.h>
#include "praid-configuration.h"
t_log * logstruct;

	void praid_launcher_initialize(){
		logstruct = log_create("praid", PRAIND_DEFAULT_LOG_FILE, DEBUG | INFO | WARNING | ERROR, M_CONSOLE_DISABLE);
		praid_configuration_setup();
	}


	void praid_launcher_doLaunch(){

	}


	int main(int argc, char *args[]){

		/*
		 * Parametro de la aplicacion - path al archivo de conf
		 */
		if(argc > 0){
			praid_configuration_setConfigurationFile(args[0]);
		}

		praid_launcher_initialize();

		praid_launcher_doLaunch();

		return EXIT_SUCCESS;
	}
