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
	Boolean test_ppd_configuration_configureSuite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite("Grage PPD Suite" , suiteInitAndClean , suiteInitAndClean);

		if ( unit_testing_addToSuite(suite , "Config Setup" , test_ppd_configuration_setup) ==NULL )
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}

	/*void test_ppd_configuration_setup(){
		char * p;
		ppd_configuration_setup();
		p=getPpdMode();
		CU_ASSERT_EQUAL(p,"connect");
		p=getPpdAlgoritmo();
		CU_ASSERT_EQUAL(p,"scan");
		p=getPpdPort();
		CU_ASSERT_EQUAL(p,"666");
		p=getPpdIdDisk();
		CU_ASSERT_EQUAL(p,"7");
		p=getPpdDiskCilinder();
		CU_ASSERT_EQUAL(p,"10");
		p=getPpdDiskHead();
		CU_ASSERT_EQUAL(p,"9");
		p=getPpdDiskSector();
		CU_ASSERT_EQUAL(p,"8");
		p=getPpdReadTimeMs();
		CU_ASSERT_EQUAL(p,"2");
		p=getPpdWriteTimeMs();
		CU_ASSERT_EQUAL(p,"3");
		p=getPpdRpm();
		CU_ASSERT_EQUAL(p,"7200");
		p=getPpdSaltoPistaMs();
		CU_ASSERT_EQUAL(p,"5");
		CU_ASSERT_EQUAL(commons_logging_getLoggingLevelEnabled(),LOGGING_LEVEL_INFO);
	}*/

	void test_ppd_configuration_setup(){
		char * value;

		ppd_configuration_setConfigurationFile("../conf/grage-ppd.properties");
		ppd_configuration_setup();

		value = getPpdMode();
		CU_ASSERT_STRING_EQUAL(value,"connect");

		value = getPpdAlgoritmo();
		CU_ASSERT_STRING_EQUAL(value , "scan");

		value = getPpdPort();
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
	}



