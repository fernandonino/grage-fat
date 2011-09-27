/*
 * test-linux-commons-loader.c
 *
 *  Created on: 13/08/2011
 *      Author: gonzalo
 */


#include "linux-testing-unit.h"
#include <linux-commons-logging.h>
#include "test-pfs-suites.h"

	void suite_setup(){
		if(log_create("test","../logs/tests.log",INFO,M_CONSOLE_DISABLE)){
			perror("Error creando la configuracion de los logs.");
		}
	}

	void suite_tearDown(){
		log_destroy();
	}

	char * unit_testing_getProjectName(){
		return "grage-pfs";
	}


	int unit_testing_loadAll(){
		if(test_pfs_configurationSuite())
			return 1;
		if(test_pfs_cache_configurationSuite())
			return 1;
		if(test_pfs_console_Suite())
			return 1;
		return 0;
	}
