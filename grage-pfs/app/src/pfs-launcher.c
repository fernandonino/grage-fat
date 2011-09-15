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
		pfs_configuration_initialize();
	}


	void pfs_launcher_launch(){

	}


	int main(int argc, char *argv[]){

		/*
		 * Parametro de la aplicacion - path al archivo de conf
		 */
		if(argc > 0){
			pfs_configuration_setConfigurationFile(argv[1]);
		}

		pfs_launcher_initialize();

		pfs_launcher_launch();

		return EXIT_SUCCESS;
	}
