/*
 * test-praid-configuration.c
 *
 *  Created on: 12/09/2011
 *      Author: joaquin
 */


#include <stdlib.h>

#include "linux-commons.h"
#include "linux-testing-unit.h"

#include "praid-configuration.h"
	/*
	 * Prototipo de funciones de test
	 */

	void test_praid_setDevicePort();
	void test_praid_setConsoleStatus();
	void test_praid_getDevicePort();
	void test_praid_getConsoleStatus();



	/*
	 * Realiza la configuracion de la suite
	 */
	Boolean test_praid_configurationSuite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite("Grage PFS Configuration Suite" , suiteInitAndClean , suiteInitAndClean);


		if ( unit_testing_addToSuite(suite , "Obtener puerto del dispositivo RAID" , test_praid_getDevicePort ) ==NULL )
			return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "Obtener estado de consola del dispositivo RAID" , test_praid_getConsoleStatus ) ==NULL )
			return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "Setear puerto del dispositivo RAID" , test_praid_setDevicePort ) ==NULL )
			return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "Setear estado de consola del dispositivo RAID" , test_praid_setConsoleStatus ) ==NULL )
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}



	void test_praid_getDevicePort(){
		char * port = getDevicePort();
		CU_ASSERT_PTR_NULL(port);
		setDevicePort("10000");
		port = getDevicePort();
		CU_ASSERT_PTR_NOT_NULL(port);
		CU_ASSERT_STRING_EQUAL(port,"10000");
		setDevicePort("");
	}

	void test_praid_getConsoleStatus(){
		char * port = getConsoleStatus();
		CU_ASSERT_PTR_NULL(port);
		setConsoleStatus("1");
		port = getConsoleStatus();
		CU_ASSERT_PTR_NOT_NULL(port);
		CU_ASSERT_STRING_EQUAL(port,"1");
		setConsoleStatus("");
	}

	void test_praid_setDevicePort(){
		setDevicePort("10000");
		char * port = getDevicePort();
		CU_ASSERT_PTR_NOT_NULL(port);
		CU_ASSERT_STRING_EQUAL(port,"10000");
	}

	void test_praid_setConsoleStatus(){
		setConsoleStatus("1");
		char * port = getConsoleStatus();
		CU_ASSERT_PTR_NOT_NULL(port);
		CU_ASSERT_STRING_EQUAL(port,"1");
	}
