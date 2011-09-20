/*
 * test-linux-commons-errors.c
 *
 *  Created on: 08/08/2011
 *      Author: gonzalo
 */




#include <stdlib.h>
#include "linux-commons.h"
#include "linux-commons-errors.h"

#include "linux-testing-unit.h"


	void test_commons_errors_buildFaultValidator(void);
	void test_commons_errors_buildSuccessValidator(void);
	void test_commons_errors_setError(void);



	/*
	 * Realiza la configuracion de la suite
	 */
	Boolean linux_commons_errors_configureSuite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite(
				"Suite Linux Commons Errors" , suiteInitAndClean , suiteInitAndClean);

		if( unit_testing_addToSuite(suite , "Se establece Fault en runtime y se inserta descripcion del error" ,
				test_commons_errors_buildFaultValidator) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Se establece Success en runtime y se inserta NULL en la descripcion" ,
				test_commons_errors_buildSuccessValidator) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Seteo de error" ,
				test_commons_errors_setError) == NULL)
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}



	/*
	 * Metodos que realizan los tests
	 */

	void test_commons_errors_buildFaultValidator(){

		RuntimeErrorValidator * r = commons_errors_buildFaultValidator("Algun error");

		CU_ASSERT_PTR_NOT_NULL(r);
		CU_ASSERT_PTR_NOT_NULL(r->errorDescription);
		CU_ASSERT_EQUAL(r->errorDescription , "Algun error");
	}
	void test_commons_errors_buildSuccessValidator(){

		RuntimeErrorValidator * r = commons_errors_buildSuccessValidator();

		CU_ASSERT_PTR_NOT_NULL(r);
		CU_ASSERT_PTR_NULL(r->errorDescription);

	}

	void test_commons_errors_setError(){

	}



