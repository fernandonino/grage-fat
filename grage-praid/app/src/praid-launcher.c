/*
 * praid-launcher.c
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo
 */

#include <stdlib.h>
#include <stdio.h>

#include "linux-commons-logging.h"
#include "linux-commons.h"

#include "praid-configuration.h"
#include "praid-entrypoint.h"
#include "praid-state.h"

#include <time.h>



	void praid_launcher_initialize(){

		if(log_create("praid", PRAIND_DEFAULT_LOG_FILE ,INFO|WARNING|ERROR|DEBUG,M_CONSOLE_DISABLE)){
			puts("[ Falló la inicialización del log ]");
			exit(EXIT_FAILURE);
		}

		praid_configuration_setup();

		praid_state_initializeStorages();
	}


	void praid_launcher_doLaunch(){
		praid_entry_startEntrypointListening();
	}


	void praid_launcher_exit() {
		log_destroy();
	}


	int main(int argc, char *args[]){

		praid_launcher_initialize();

		praid_launcher_doLaunch();

		praid_launcher_exit();

		return EXIT_SUCCESS;
	}
