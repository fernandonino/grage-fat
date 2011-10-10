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

	void praid_launcher_initialize(){
		log_create("praid","../logs/praid.log",INFO|WARNING|ERROR|DEBUG,M_CONSOLE_DISABLE);
		praid_configuration_setup();
	}


	void praid_launcher_doLaunch(){

		//lanzar el thread que atiende peticiones
	}

	void praid_launcher_exit() {
		log_destroy();
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

		praid_launcher_exit();

		return EXIT_SUCCESS;
	}
