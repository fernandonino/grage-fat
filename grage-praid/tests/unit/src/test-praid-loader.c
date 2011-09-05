/*
 * test-praid-loader.c
 *
 *  Created on: 13/08/2011
 *      Author: gonzalo
 */


#include "linux-testing-unit.h"

#include "test-praid-suites.h"

	char * unit_testing_getProjectName(){
		return "grage-praid";
	}


	int unit_testing_loadAll(){

		if(test_praid_example_configureSuite())
			return 1;

		return 0;
	}
