/*
 * test-ppd-loader.c
 *
 *  Created on: 13/08/2011
 *      Author: gonzalo
 */


#include "linux-testing-unit.h"
#include <linux-commons-logging.h>
#include "test-ppd-suites.h"

	void suite_setup(){
		if(log_create("test","../logs/tests.log",INFO,M_CONSOLE_DISABLE)){
			perror("Error creando la configuracion de los logs.");
		}
	}

	void suite_tearDown(){
		log_destroy();
	}

	char * unit_testing_getProjectName(){
		return "grage-ppd";
	}


	int unit_testing_loadAll(){
		if(test_ppd_configuration_configureSuite())
			return 1;

		if(test_ppd_queues_configureSuite())
			return 1;
		if(test_ppd_persistanceSuite())
			return 1;

		return 0;
	}
