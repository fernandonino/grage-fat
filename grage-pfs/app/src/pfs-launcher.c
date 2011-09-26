/*
 * pfs-launcher.c
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo
 */

#include <stdlib.h>
#include <stdio.h>
#include <linux-commons-logging.h>
#include "pfs-configuration.h"
t_log * logstruct;

	void pfs_launcher_initialize(){
		log_create("pfs","../logs/pfs.log",DEBUG | INFO | WARNING | ERROR, M_CONSOLE_DISABLE);
		pfs_configuration_initialize();
	}


	void pfs_launcher_launch(){

	}


	int main(int argc, char *argv[]){

		/*
		 * Parametro de la aplicacion - path al archivo de conf
		 */
		if(argc > 1){
			pfs_configuration_setConfigurationFile(argv[1]);
		} else {
			pfs_configuration_setConfigurationFile(PFS_DEFAULT_CONFIGURATION_FILE);
		}

		pfs_launcher_initialize();

		pfs_launcher_launch();

		return EXIT_SUCCESS;
	}
