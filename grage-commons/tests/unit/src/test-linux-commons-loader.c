/*
 * test-linux-commons-loader.c
 *
 *  Created on: 13/08/2011
 *      Author: gonzalo
 */


#include "linux-commons.h"

#include "linux-testing-unit.h"

#include "test-linux-commons-suites.h"

	char * unit_testing_getProjectName(){
		return "grage-commons";
	}


	int unit_testing_loadAll(){

		if(linux_commons_errors_configureSuite())
			return 1;

		if(linux_commons_list_configureSuite())
			return 1;

		if(linux_commons_file_configureSuite())
			return 1;

		if(linux_commons_logging_configureSuite())
			return 1;

		if(linux_commons_configureSuite())
			return 1;

		if(linux_commons_string_configureSuite())
			return 1;

//		if(linux_commons_socket_configureSuite())
//			return 1;
//
		return 0;
	}
