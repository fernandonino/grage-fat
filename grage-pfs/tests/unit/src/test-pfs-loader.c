/*
 * test-linux-commons-loader.c
 *
 *  Created on: 13/08/2011
 *      Author: gonzalo
 */


#include "linux-testing-unit.h"

#include "test-pfs-suites.h"

	char * unit_testing_getProjectName(){
		return "grage-pfs";
	}


	int unit_testing_loadAll(){

		if(test_pfs_configurationSuite())
			return 1;

		return 0;
	}
