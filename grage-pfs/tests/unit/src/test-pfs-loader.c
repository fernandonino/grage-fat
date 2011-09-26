/*
 * test-linux-commons-loader.c
 *
 *  Created on: 13/08/2011
 *      Author: gonzalo
 */


#include "linux-testing-unit.h"
#include <linux-commons-logging.h>
#include "test-pfs-suites.h"
t_log * logstruct;
	char * unit_testing_getProjectName(){
		return "grage-pfs";
	}


	int unit_testing_loadAll(){

		logstruct = log_create("test","../logs/tests.log",INFO,M_CONSOLE_DISABLE);

		if(test_pfs_configurationSuite())
			return 1;

		return 0;
	}
