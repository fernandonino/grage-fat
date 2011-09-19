/*
 * test-linux-commons-logging.c
 *
 *  Created on: 22/08/2011
 *      Author: gonzalo
 */



#include <stdlib.h>
#include <string.h>
#include "linux-commons.h"
#include "linux-commons-logging.h"

#include "linux-testing-unit.h"


	void test_commons_logging_isDebugEnabled();
	void test_commons_logging_isInfoEnabled();
	void test_commons_logging_getLogString();
	void test_commons_logging_writeLogString();
	void test_commons_logging_println();
	void test_commons_logging_logInfo ();
	void test_commons_logging_logError();
	void test_commons_logging_logDebug();
	/*
	 * Realiza la configuracion de la suite
	 */
	Boolean linux_commons_logging_configureSuite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite(
				"Suite Linux Commons Logging" , suiteInitAndClean , suiteInitAndClean);

		if( unit_testing_addToSuite(suite , "Verificacion de loggeo por Debug" ,
				test_commons_logging_isDebugEnabled) == NULL)
			return EXIT_FAILURE;
		if( unit_testing_addToSuite(suite , "Verificacion de loggeo de informacion" ,
				test_commons_logging_isInfoEnabled) == NULL)
			return EXIT_FAILURE;
		if( unit_testing_addToSuite(suite , "Creacion correcta de string de log" ,
				test_commons_logging_getLogString) == NULL)
			return EXIT_FAILURE;
		if( unit_testing_addToSuite(suite , "Apertura y escritura en log" ,
				test_commons_logging_writeLogString) == NULL)
			return EXIT_FAILURE;
		if( unit_testing_addToSuite(suite , "Impresion en pantalla de la linea de log" ,
				test_commons_logging_println) == NULL)
			return EXIT_FAILURE;
		if( unit_testing_addToSuite(suite , "Impresion en pantalla de la linea de log de tipo INFO y escribirla en su archivo" ,
				test_commons_logging_logInfo) == NULL)
			return EXIT_FAILURE;
		if( unit_testing_addToSuite(suite , "Impresion en pantalla de la linea de log de tipo ERROR y escribirla en su archivo" ,
				test_commons_logging_logError) == NULL)
			return EXIT_FAILURE;
		if( unit_testing_addToSuite(suite , "Impresion en pantalla de la linea de log de tipo DEBUG y escribirla en su archivo" ,
				test_commons_logging_logDebug) == NULL)
			return EXIT_FAILURE;
		return EXIT_SUCCESS;
	}

	/*
	 * Metodos que realizan los tests
	 */


	void test_commons_logging_isDebugEnabled(){
		int value = commons_logging_isDebugEnabled();
		if(commons_logging_isDebugEnabled()){
			CU_ASSERT_EQUAL(value,TRUE);
		}
		else{
			CU_ASSERT_EQUAL(value,FALSE);
		}
	}

	void test_commons_logging_isInfoEnabled(){
		int value = commons_logging_isInfoEnabled();
		if(commons_logging_isInfoEnabled()){
			CU_ASSERT_EQUAL(value,TRUE);
		}
		else{
			CU_ASSERT_EQUAL(value,FALSE);
		}
	}

	void test_commons_logging_getLogString(){
		String value = commons_logging_getLogString("unProc", "unTipo", "unaDesc");
		CU_ASSERT(strlen(value));
	}

	void test_commons_logging_writeLogString(){
		FILE *logfilePrueba;
		char path[50];
		char line[250];
		char testingProcedure[5];

		bzero( path , sizeof(path) );
		bzero( line , sizeof(line) );
		strcpy(testingProcedure, "init");
		sprintf( path , "app/logs/%s.log" , testingProcedure );
		logfilePrueba = fopen(path, "r+");
		CU_ASSERT_PTR_NULL(logfilePrueba);

		if(!logfilePrueba){
			commons_logging_initLog(testingProcedure);
			commons_logging_logError(testingProcedure , "Testeo de escritura");
			logfilePrueba = fopen(path, "a+");
			CU_ASSERT_PTR_NOT_NULL(logfilePrueba);

			rewind(logfilePrueba);
			fgets(line, sizeof(line), logfilePrueba);
			CU_ASSERT(strlen(line) > 0 );

			if(logfilePrueba){
				remove(path);
				logfilePrueba = fopen(path, "r+");
				CU_ASSERT_PTR_NULL(logfilePrueba);
			}
		}
	}

	void test_commons_logging_println(){
		//No requiere test
	}

	void test_commons_logging_logError(){
		int testLevelEnabled = 1;
		char testingProcedure[10], path[50];
		FILE *logfilePrueba;

		bzero(testingProcedure, sizeof(testingProcedure));
		strcpy(testingProcedure, "init");
		sprintf(path, "app/logs/%s.log", testingProcedure);

		commons_logging_setLoggingLevelEnabled(testLevelEnabled);
		commons_logging_initLog(testingProcedure);
		commons_logging_logError(testingProcedure , "ERROR LOG TESTING");
		logfilePrueba = fopen(path, "r+");
		CU_ASSERT_PTR_NOT_NULL(logfilePrueba);
		if(logfilePrueba){
			remove(path);
			logfilePrueba = fopen(path, "r+");
			CU_ASSERT_PTR_NULL(logfilePrueba);
		}

		commons_logging_logInfo(testingProcedure , "INFO LOG TESTING");
		logfilePrueba = fopen(path, "r+");
		CU_ASSERT_PTR_NULL(logfilePrueba);

		commons_logging_logDebug(testingProcedure , "DEBUG LOG TESTING");
		logfilePrueba = fopen(path, "r+");
		CU_ASSERT_PTR_NULL(logfilePrueba);
	}

	void test_commons_logging_logInfo(){
		int testLevelEnabled = 2;
		char testingProcedure[10], path[50];
		FILE *logfilePrueba;

		bzero(testingProcedure, sizeof(testingProcedure));
		strcpy(testingProcedure, "init");
		sprintf(path, "app/logs/%s.log", testingProcedure);

		commons_logging_setLoggingLevelEnabled(testLevelEnabled);
		commons_logging_initLog(testingProcedure);
		commons_logging_logError(testingProcedure , "ERROR LOG TESTING");
		logfilePrueba = fopen(path, "r+");
		CU_ASSERT_PTR_NOT_NULL(logfilePrueba);
		if(logfilePrueba){
			remove(path);
			logfilePrueba = fopen(path, "r+");
			CU_ASSERT_PTR_NULL(logfilePrueba);
		}

		commons_logging_initLog(testingProcedure);
		commons_logging_logInfo(testingProcedure , "INFO LOG TESTING");
		logfilePrueba = fopen(path, "r+");
		CU_ASSERT_PTR_NOT_NULL(logfilePrueba);
		if(logfilePrueba){
			remove(path);
			logfilePrueba = fopen(path, "r+");
			CU_ASSERT_PTR_NULL(logfilePrueba);
		}

		commons_logging_logDebug(testingProcedure , "DEBUG LOG TESTING");
		logfilePrueba = fopen(path, "r+");
		CU_ASSERT_PTR_NULL(logfilePrueba);

	}

	void test_commons_logging_logDebug(){

		int testLevelEnabled = 3;
		char testingProcedure[10], path[50];
		FILE *logfilePrueba;

		bzero(testingProcedure, sizeof(testingProcedure));
		strcpy(testingProcedure, "init");
		sprintf(path, "app/logs/%s.log", testingProcedure);

		commons_logging_setLoggingLevelEnabled(testLevelEnabled);
		commons_logging_initLog(testingProcedure);
		commons_logging_logError(testingProcedure , "ERROR LOG TESTING");
		logfilePrueba = fopen(path, "r+");
		CU_ASSERT_PTR_NOT_NULL(logfilePrueba);
		if(logfilePrueba){
			remove(path);
			logfilePrueba = fopen(path, "r+");
			CU_ASSERT_PTR_NULL(logfilePrueba);
		}

		commons_logging_initLog(testingProcedure);
		commons_logging_logInfo(testingProcedure , "INFO LOG TESTING");
		logfilePrueba = fopen(path, "r+");
		CU_ASSERT_PTR_NOT_NULL(logfilePrueba);		
		if(logfilePrueba){
			remove(path);
			logfilePrueba = fopen(path, "r+");
			CU_ASSERT_PTR_NULL(logfilePrueba);
		}

		commons_logging_initLog(testingProcedure);
		commons_logging_logDebug(testingProcedure , "DEBUG LOG TESTING");
		logfilePrueba = fopen(path, "r+");
		CU_ASSERT_PTR_NOT_NULL(logfilePrueba);
		if(logfilePrueba){
			remove(path);
			logfilePrueba = fopen(path, "r+");
			CU_ASSERT_PTR_NULL(logfilePrueba);
		}

	}




