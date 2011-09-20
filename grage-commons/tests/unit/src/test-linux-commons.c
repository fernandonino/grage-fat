/*
 * test-linux-commons.c
 *
 *  Created on: Aug 23, 2011
 *      Author: Fernando
 */

#include <stdlib.h>
#include "linux-commons.h"
#include "linux-commons-errors.h"

#include "linux-testing-unit.h"

	/*
	 * Prototipo de funciones de test
	 */
	void test_commons_misc_intToString();
	void test_commons_misc_longToString();
	void test_commons_misc_generateRandomKey();
	void test_commons_misc_digitsCount();


	/*
	 * Realiza la configuracion de la suite
	 */
	Boolean linux_commons_configureSuite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite("Linux Common" , suiteInitAndClean , suiteInitAndClean);

		if ( unit_testing_addToSuite(suite , "Funcion intToString" ,
				test_commons_misc_intToString) ==NULL )
					return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "Funcion longToString" ,
				test_commons_misc_longToString) ==NULL )
					return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "Funcion generateRandomKey" ,
				test_commons_misc_generateRandomKey) ==NULL )
					return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "Funcion digitsCount" ,
				test_commons_misc_digitsCount) ==NULL )
					return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}




	void test_commons_misc_digitsCount(){

		int someNumber = 30;
		int count = commons_misc_digitsCount(someNumber);
		CU_ASSERT_EQUAL(count  , 2);

		someNumber = 312;
		count = commons_misc_digitsCount(someNumber);
		CU_ASSERT_EQUAL(count  , 3);

		someNumber = 51234;
		count = commons_misc_digitsCount(someNumber);
		CU_ASSERT_EQUAL(count  , 5);

		someNumber = 0;
		count = commons_misc_digitsCount(someNumber);
		CU_ASSERT_EQUAL(count  , 1);

	}


	void test_commons_misc_intToString(){

		char * number = commons_misc_intToString(32);

		CU_ASSERT_PTR_NOT_NULL(number);
		CU_ASSERT_STRING_EQUAL(number,"32");

		free(number);
	}

	void test_commons_misc_longToString(){

		char * number = commons_misc_longToString(65539);

		CU_ASSERT_PTR_NOT_NULL(number);
		CU_ASSERT_STRING_EQUAL(number,"65539");

		free(number);
	}

	void test_commons_misc_generateRandomKey(){

		char * randomKey = commons_misc_generateRandomKey(16);
		CU_ASSERT_PTR_NOT_NULL(randomKey);

		randomKey = commons_misc_generateRandomKey(0);
		CU_ASSERT_PTR_NULL(randomKey);

		free(randomKey);
	}


