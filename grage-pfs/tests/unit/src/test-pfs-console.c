/*
 * test-pfs-console.c
 *
 *  Created on: 27/09/2011
 *      Author: joaquin
 */

#include <stdlib.h>

#include "linux-commons.h"
#include "linux-testing-unit.h"
#include "grage-commons.h"
#include "pfs-console.h"

	/*
	 * Prototipo de funciones de test
	 */

	void test_pfs_console_initialize();



	/*
	 * Realiza la configuracion de la suite
	 */
	Boolean test_pfs_console_Suite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite("Grage PFS Console Suite" , suiteInitAndClean , suiteInitAndClean);


		if ( unit_testing_addToSuite(suite , "Crear hilo de consola" , test_pfs_console_initialize ) ==NULL )
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}

	void test_pfs_console_initialize(){
		//int test_thread = pfs_console_initialize();
		//CU_ASSERT(test_thread == 0);
	}

