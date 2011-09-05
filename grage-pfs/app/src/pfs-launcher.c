/*
 * pfs-launcher.c
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo
 */

#include <stdlib.h>
#include <stdio.h>

#include "pfs-configuration.h"


	void pfs_launcher_initialize(){
		pfs_configuration_setup();
	}


	void pfs_launcher_doLaunch(){

	}


	int main(int argc, char *args[]){

		/*
		 * Parametro de la aplicacion - path al archivo de conf
		 */
		if(argc > 0){
			pfs_configuration_setConfigurationFile(args[0]);
		}

		pfs_launcher_initialize();

		pfs_launcher_doLaunch();

		return EXIT_SUCCESS;
	}
