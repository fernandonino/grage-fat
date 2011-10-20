/*
 * test-ppd-configuration.c
 *
 *  Created on: Sep 9, 2011
 *      Author: seba
 */
#include <stdlib.h>
#include "linux-commons-logging.h"
#include "linux-commons.h"
#include "linux-testing-unit.h"
#include "ppd-configuration.h"
	/*
	 * Prototipo de funciones de test
	 */
	void test_ppd_configuration_setup();


	/*
	 * Realiza la configuracion de la suite
	 */
	Boolean test_ppd_configurationSuite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite("Grage PPD Suite" , suiteInitAndClean , suiteInitAndClean);

		if ( unit_testing_addToSuite(suite , "Config Setup" , test_ppd_configuration_setup) ==NULL )
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}


	void test_ppd_configuration_setup(){
		char * value;

		ppd_configuration_setConfigurationFile("../conf/grage-ppd.properties");
		ppd_configuration_setup();

		value =  ppd_conf_getPpdMode();
		CU_ASSERT_STRING_EQUAL(value,"connect");

		value = getPpdAlgoritmo();
		CU_ASSERT_STRING_EQUAL(value , "look");

		value =  ppd_conf_getPpdPort();
		CU_ASSERT_STRING_EQUAL(value , "666");

		value = getPpdIdDisk();
		CU_ASSERT_STRING_EQUAL(value , "7");

		value = getPpdDiskCilinder();
		CU_ASSERT_STRING_EQUAL(value , "10");

		value = getPpdDiskHead();
		CU_ASSERT_STRING_EQUAL(value , "9");

		value = getPpdDiskSector();
		CU_ASSERT_STRING_EQUAL(value , "8");

		value = getPpdReadTimeMs();
		CU_ASSERT_STRING_EQUAL(value , "2");

		value = getPpdWriteTimeMs();
		CU_ASSERT_STRING_EQUAL(value , "3");

		value = getPpdRpm();
		CU_ASSERT_STRING_EQUAL(value , "7200");

		value = getPpdSaltoPistaMs();
		CU_ASSERT_STRING_EQUAL(value , "5");

		value = getPpdReadDelay();
		CU_ASSERT_STRING_EQUAL(value , "10");

		value = getPpdWriteDelay();
		CU_ASSERT_STRING_EQUAL(value , "10");
	}



