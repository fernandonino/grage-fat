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


	void praid_launcher_initialize(){
		log_create("praid","../logs/praid.log",INFO|WARNING|ERROR|DEBUG,M_CONSOLE_DISABLE);
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
