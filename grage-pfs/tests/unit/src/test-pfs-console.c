/*
 * test-pfs-console.c
 *
 *  Created on: 27/09/2011
 *      Author: joaquin
 */

#include <stdlib.h>
#include <strings.h>
#include "linux-commons.h"
#include "linux-testing-unit.h"
#include "grage-commons.h"
#include "pfs-console.h"
#include "pfs-console-utils.h"

	/*
	 * Prototipo de funciones de test
	 */

	void test_pfs_console_initialize();
	void test_pfs_console_utils_parseCMD();
	void test_pfs_console_utils_get_cmd_parameter();


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
		if ( unit_testing_addToSuite(suite , "Obtener comando" , test_pfs_console_utils_parseCMD ) ==NULL )
			return EXIT_FAILURE;
		if ( unit_testing_addToSuite(suite , "Obtener parametro de comando" , test_pfs_console_utils_get_cmd_parameter ) ==NULL )
			return EXIT_FAILURE;
		return EXIT_SUCCESS;
	}

	void test_pfs_console_initialize(){
		int test_thread = pfs_console_initialize();
		CU_ASSERT(test_thread == 0);
	}

	void test_pfs_console_utils_parseCMD(){
		String cmd;
		cmd = pfs_console_utils_parseCMD("comandoPrueba");
		CU_ASSERT_STRING_EQUAL(cmd, "comandoPrueba");
		bzero(cmd, sizeof(cmd));
		cmd = pfs_console_utils_parseCMD("comandoPrueba()");
		CU_ASSERT_STRING_EQUAL(cmd, "comandoPrueba");
		bzero(cmd, sizeof(cmd));
		cmd = pfs_console_utils_parseCMD("comandoPrueba ");
		CU_ASSERT_STRING_EQUAL(cmd, "comandoPrueba");
	}

	void test_pfs_console_utils_get_cmd_parameter(){
		String parameter;
		parameter = pfs_console_utils_get_cmd_parameter("comandoPrueba(parametroPrueba)", strlen("comandoPrueba"));
		puts(parameter);
		CU_ASSERT_STRING_EQUAL(parameter, "parametroPrueba");
	}


