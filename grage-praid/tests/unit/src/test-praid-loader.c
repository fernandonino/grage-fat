/*
 * test-praid-loader.c
 *
 *  Created on: 13/08/2011
 *      Author: gonzalo
 */


#include "linux-testing-unit.h"
#include <linux-commons-logging.h>
#include "test-praid-suites.h"

t_log * logstruct;

	char * unit_testing_getProjectName(){
		return "grage-praid";
	}


	int unit_testing_loadAll(){

		logstruct = log_create("test","../logs/tests.log",INFO,M_CONSOLE_DISABLE);

		if(test_praid_configurationSuite())
			return 1;

		return 0;
	}
