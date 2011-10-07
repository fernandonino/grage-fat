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

	void test_praid_getConsoleStatus();
	void test_praid_getDevicePort();
	void test_praid_setDevicePort();
	void test_praid_setConsoleStatus();
	void test_praid_fullConfigurationTest();


	/*
	 * Realiza la configuracion de la suite
	 */
	Boolean test_praid_configurationSuite(){

		int suiteInitAndClean(){
			praid_configuration_setDevicePort(NULL);
			praid_configuration_setDevicePort(NULL);
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

		if ( unit_testing_addToSuite(suite , "Levantar la configuracion del archivo" , test_praid_fullConfigurationTest ) ==NULL )
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}



	void test_praid_getDevicePort(){
		char * port = praid_configuration_getDevicePort();
		CU_ASSERT_PTR_NULL(port);
	}

	void test_praid_getConsoleStatus(){
		char * port = praid_configuration_getConsoleStatus();
		CU_ASSERT_PTR_NULL(port);
	}

	void test_praid_setDevicePort(){
		praid_configuration_setDevicePort("10000");
		char * port = praid_configuration_getDevicePort();
		CU_ASSERT_PTR_NOT_NULL(port);
		CU_ASSERT_STRING_EQUAL(port,"10000");
	}

	void test_praid_setConsoleStatus(){
		praid_configuration_setConsoleStatus("true");
		char * port = praid_configuration_getConsoleStatus();
		CU_ASSERT_PTR_NOT_NULL(port);
		CU_ASSERT_STRING_EQUAL(port,"true");
	}

	void test_praid_fullConfigurationTest(){
		char * value;

		praid_configuration_setConfigurationFile("../conf/grage-praid.properties");
		praid_configuration_setup();

		value = praid_configuration_getConsoleStatus();
		CU_ASSERT_PTR_NOT_NULL(value);
		CU_ASSERT_STRING_EQUAL(value,"false");

		value = praid_configuration_getDevicePort();
		CU_ASSERT_PTR_NOT_NULL(value);
		CU_ASSERT_STRING_EQUAL(value,"10000");
	}

