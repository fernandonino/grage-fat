/*
 * praid-launcher.c
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo
 */

#include <stdlib.h>
#include <stdio.h>

#include "praid-configuration.h"


	void praid_launcher_initialize(){
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
