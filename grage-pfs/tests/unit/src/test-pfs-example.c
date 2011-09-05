/*
 * test-pfs-example.c
 *
 *  Created on: Aug 23, 2011
 *      Author: Fernando
 */

#include <stdlib.h>

#include "linux-commons.h"
#include "linux-testing-unit.h"

	/*
	 * Prototipo de funciones de test
	 */
	void test_pfs_example();


	/*
	 * Realiza la configuracion de la suite
	 */
	Boolean test_pfs_example_configureSuite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite("Grage PFS Suite" , suiteInitAndClean , suiteInitAndClean);

		if ( unit_testing_addToSuite(suite , "Metodo de ejemplo" , test_pfs_example ) ==NULL )
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}



	void test_pfs_example(){

	}


