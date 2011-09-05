/*
 * ppd-launcher.c
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo
 */

#include <stdlib.h>
#include <stdio.h>

#include "ppd-configuration.h"


	void ppd_launcher_initialize(){
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
