/*
 * test-ppd-example.c
 *
 *  Created on: Aug 23, 2011
 *      Author: gonzalo
 */

#include <stdlib.h>

#include "linux-commons.h"
#include "linux-testing-unit.h"

	/*
	 * Prototipo de funciones de test
	 */
	void test_ppd_example();


	/*
	 * Realiza la configuracion de la suite
	 */
	Boolean test_ppd_example_configureSuite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite("Grage PPD Suite" , suiteInitAndClean , suiteInitAndClean);

		if ( unit_testing_addToSuite(suite , "Metodo de ejemplo" , test_ppd_example ) ==NULL )
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}



	void test_ppd_example(){

	}


