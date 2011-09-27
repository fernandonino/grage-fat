/*
 * ppd-launcher.c
 *
 *  Created on: 05/09/2011
 *      Author: EL Seba
 */

#include <stdlib.h>
#include <stdio.h>
#include <linux-commons-logging.h>
#include "ppd-configuration.h"


	void ppd_launcher_initialize(){
		log_create("ppd","../logs/ppd.log",INFO|WARNING|ERROR|DEBUG,M_CONSOLE_DISABLE);
		ppd_configuration_setup();
	}


	void ppd_launcher_doLaunch(){

	}

	void ppd_laucher_exit(){
		log_destroy();
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

		ppd_laucher_exit();

		return EXIT_SUCCESS;
	}
