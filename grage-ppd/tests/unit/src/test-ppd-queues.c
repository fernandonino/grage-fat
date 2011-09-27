/*
 * test-ppd-queues.c
 *
 *  Created on: 26/09/2011
 *      Author: gonzalo
 */


#include <stdlib.h>

#include "linux-commons.h"
#include "linux-testing-unit.h"

#include "ppd-queues.h"

	/*
	 * Prototipo de funciones de test
	 */
	void test_ppd_queues_buildJob();
	void test_ppd_queues_isTheSameJob();
	void test_ppd_queues_pickForWrite();
	void test_ppd_queues_pickForRead();
	void test_ppd_queues_putForRead();
	void test_ppd_queues_putForWrite();


	/*
	 * Realiza la configuracion de la suite
	 */
	Boolean test_ppd_queues_configureSuite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite("Grage PPD Suite Queue" , suiteInitAndClean , suiteInitAndClean);

		if ( unit_testing_addToSuite(suite , "Creacion de un Job" , test_ppd_queues_buildJob ) ==NULL )
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}



	void test_ppd_queues_buildJob(){}
	void test_ppd_queues_isTheSameJob(){}
	void test_ppd_queues_pickForWrite(){}
	void test_ppd_queues_pickForRead(){}
	void test_ppd_queues_putForRead(){}
	void test_ppd_queues_putForWrite(){}
