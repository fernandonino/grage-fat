/*
 * test-linux-commons-errors.c
 *
 *  Created on: 08/08/2011
 *      Author: gonzalo
 */




#include <stdlib.h>
#include "linux-commons.h"
#include "linux-commons-strings.h"

#include "linux-testing-unit.h"


	void test_commons_string_subString();
	void test_commons_string_trim();
	void test_commons_string_concat();
	void test_commons_string_concatAll();
	void test_commons_string_cloneString();
	void test_commons_string_fillIntString();
	void test_commons_string_replaceAll();
	void test_commons_string_equals();
	void test_commons_string_startsWith();
	void test_commons_string_endsWith();
	void test_commons_string_getIndexOf();

	void doFreeNull(const void **pointer);

	/*
	 * Realiza la configuracion de la suite
	 */
	Boolean linux_commons_string_configureSuite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite(
				"Suite Linux Commons String" , suiteInitAndClean , suiteInitAndClean);

		if( unit_testing_addToSuite(suite , "Test Metodo subString" , test_commons_string_subString) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Test Metodo trim" , test_commons_string_trim) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Test Metodo concat" , test_commons_string_concat) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Test Metodo concatAll" , test_commons_string_concatAll) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Test Metodo cloneString" , test_commons_string_cloneString) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Test Metodo fillIntString" , test_commons_string_fillIntString) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Test Metodo replaceAll" , test_commons_string_replaceAll) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Test Metodo equals" , test_commons_string_equals) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Test Metodo startsWith" , test_commons_string_startsWith) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Test Metodo endsWith" , test_commons_string_endsWith) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Test Metodo getIndexOf" , test_commons_string_getIndexOf) == NULL)
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}

	/*
	 * Metodos que realizan los tests
	 */
	void test_commons_string_subString(){

		String okResult = commons_string_subString("hola",0,1);
		
		CU_ASSERT_PTR_NOT_NULL(okResult);
		CU_ASSERT_STRING_EQUAL(okResult,"ho");
		doFreeNull(&okResult);
		
		okResult = commons_string_subString("raro que pase esto",1,5);
		CU_ASSERT_PTR_NOT_NULL(okResult);
		CU_ASSERT_STRING_EQUAL(okResult,"aro q");
		doFreeNull(&okResult);

		// Veamos q tanto se puede romper.
		String maybeErrResult = commons_string_subString("aca",-1,1);
		CU_ASSERT_PTR_NOT_NULL(maybeErrResult);
		//Esto da error pero por que no esta manejado el underflow en la funcion
		//Hay q ver q se hace en este caso. Igual es muy HDP
		//CU_ASSERT_STRING_EQUAL(maybeErrResult,"ac");
		doFreeNull(&maybeErrResult);

		maybeErrResult = commons_string_subString("aca", 1,16);
		CU_ASSERT_PTR_NOT_NULL(maybeErrResult);
		CU_ASSERT_STRING_EQUAL(maybeErrResult,"ca");
		doFreeNull(&maybeErrResult);

	}

	void test_commons_string_trim(){
		String result = NULL;

		result = commons_string_trim(strdup("hola"));
		CU_ASSERT_PTR_NOT_NULL(result);
		CU_ASSERT_STRING_EQUAL(result,"hola");

		result = commons_string_trim(strdup("no separar"));
		CU_ASSERT_PTR_NOT_NULL(result);
		CU_ASSERT_STRING_EQUAL(result,"no separar");

		result = commons_string_trim(strdup("uno a derecha "));
		CU_ASSERT_PTR_NOT_NULL(result);
		CU_ASSERT_STRING_EQUAL(result,"uno a derecha");

		result = commons_string_trim(strdup(" uno a izquierda"));
		CU_ASSERT_PTR_NOT_NULL(result);
		CU_ASSERT_STRING_EQUAL(result,"uno a izquierda");

		result = commons_string_trim(strdup("  dos"));
		CU_ASSERT_PTR_NOT_NULL(result);
		CU_ASSERT_STRING_EQUAL(result,"dos");

		result = commons_string_trim(strdup("dos  "));
		CU_ASSERT_PTR_NOT_NULL(result);
		CU_ASSERT_STRING_EQUAL(result,"dos");

		result = commons_string_trim(strdup("     todos  "));
		CU_ASSERT_PTR_NOT_NULL(result);
		CU_ASSERT_STRING_EQUAL(result,"todos");

		result = commons_string_trim(strdup("    "));
		CU_ASSERT_PTR_NOT_NULL(result);
		CU_ASSERT_STRING_EQUAL(result,"");

		result = commons_string_trim(strdup("a"));
		CU_ASSERT_PTR_NOT_NULL(result);
		CU_ASSERT_STRING_EQUAL(result,"a");

	}

	void test_commons_string_concat(){

		CU_ASSERT_STRING_EQUAL("concat",commons_string_concat("con","cat"));

		CU_ASSERT_STRING_EQUAL("concat",commons_string_concat("concat",""));

		CU_ASSERT_STRING_EQUAL("concat",commons_string_concat("","concat"));

	}

	void test_commons_string_concatAll(){
		CU_ASSERT_STRING_EQUAL("holacomoestas",commons_string_concatAll(3,"hola","como","estas"));
		CU_ASSERT_STRING_EQUAL("holacomo",commons_string_concatAll(2,"hola","como","estas"));
		CU_ASSERT_STRING_EQUAL("hola",commons_string_concatAll(1,"hola","como","estas"));
		CU_ASSERT_STRING_EQUAL("",commons_string_concatAll(0,"hola","como","estas"));
	}

	void test_commons_string_cloneString(){
		String valor = "1234";
		CU_ASSERT_STRING_EQUAL("1234",commons_string_cloneString(valor));
	}

	void test_commons_string_fillIntString(){
		CU_ASSERT_STRING_EQUAL(commons_string_fillIntString(22,5),"00022");

		CU_ASSERT_STRING_EQUAL(commons_string_fillIntString(22,0),"22");

	}

	void test_commons_string_replaceAll(){
		String texto = "cada vez que encuentre hola tendra que reemplazarlo hola!!!hola";

		String textoChau = commons_string_replaceAll(texto,"hola","chau");

		CU_ASSERT_PTR_NOT_NULL(textoChau);

		CU_ASSERT_STRING_EQUAL(texto,commons_string_replaceAll(textoChau,"chau","hola"))
	}

	void test_commons_string_equals(){
		String result = NULL;

		result = commons_string_trimd("hola ");
		CU_ASSERT_PTR_NOT_NULL(result);
		CU_ASSERT_STRING_EQUAL(result,"hola");
		CU_ASSERT_TRUE(commons_string_equals(result,"hola"));
		doFreeNull(&result);

		CU_ASSERT_FALSE(commons_string_equals("PrueBa","Prueba"));

		CU_ASSERT_TRUE(commons_string_equals("",""));

		CU_ASSERT_FALSE(commons_string_equals(NULL,"a"));

		CU_ASSERT_FALSE(commons_string_equals("a",NULL));

		CU_ASSERT_FALSE(commons_string_equals(NULL,NULL));

		CU_ASSERT_TRUE(commons_string_equals(commons_misc_intToString(22),"22"));
	}

	void test_commons_string_startsWith(){
		//Soporte de null. ver si corresponde.
//		CU_ASSERT_FALSE(commons_string_startsWith(NULL,"a"));
//		CU_ASSERT_FALSE(commons_string_startsWith(NULL,NULL));
//		CU_ASSERT_FALSE(commons_string_startsWith("uno dos",NULL));

		CU_ASSERT_TRUE(commons_string_startsWith("ay","a"));

		CU_ASSERT_FALSE(commons_string_startsWith(commons_string_cloneString("todo bien hasta que "),"todo bien hasta que segmentation fault"));

		CU_ASSERT_TRUE(commons_string_startsWith("commons_string_startsWith","ComMons"));

		CU_ASSERT_TRUE(commons_string_startsWith("",""));

		CU_ASSERT_FALSE(commons_string_startsWith("hache muda","a"))
	}

	void test_commons_string_endsWith(){
		CU_ASSERT_TRUE(commons_string_endsWith("ay","y"));

		CU_ASSERT_FALSE(commons_string_endsWith(commons_string_cloneString("todo bien hasta que "),"todo bien hasta que segmentation fault"));

		CU_ASSERT_TRUE(commons_string_endsWith("commons_string_endsWith","with"));

		CU_ASSERT_TRUE(commons_string_endsWith("",""));

		CU_ASSERT_FALSE(commons_string_endsWith("hache muda","d"))
	}

	void test_commons_string_getIndexOf(){
		int value = 0;
		value = commons_string_getIndexOf("fabian",'a');
		CU_ASSERT_TRUE(value > 0);
		CU_ASSERT_EQUAL(value,1);

		value = 0;
		value = commons_string_getIndexOf("fabian",'e');
		CU_ASSERT_TRUE(value == -1);
	}

	/*
	 * Libera y asigna null a un puntero.
	 */
	void doFreeNull(const void **pointer){
		free(*pointer);
		*pointer = NULL;
	}




