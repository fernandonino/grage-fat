/*
 * test-linux-commons-loader.c
 *
 *  Created on: 13/08/2011
 *      Author: gonzalo
 */


#include "linux-commons.h"
#include <linux-commons-logging.h>
#include "linux-testing-unit.h"

#include "test-linux-commons-suites.h"

t_log * logstruct;

	char * unit_testing_getProjectName(void){
		return "grage-commons";
	}


	int unit_testing_loadAll(void){

		logstruct = log_create("test","../logs/tests.log",INFO,M_CONSOLE_DISABLE);

		if(linux_commons_errors_configureSuite())
			return 1;

		if(linux_commons_list_configureSuite())
			return 1;

		if(linux_commons_file_configureSuite())
			return 1;

		if(linux_commons_configureSuite())
			return 1;

		if(linux_commons_string_configureSuite())
			return 1;

		if(linux_commons_queue_configureSuite())
			return 1;

//		if(linux_commons_socket_configureSuite())
//			return 1;
//
		return 0;
	}
