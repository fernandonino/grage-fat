/*
 * test-linux-commons-queue.c
 *
 *  Created on: 26/09/2011
 *      Author: gonzalo
 */

#include <stdlib.h>
#include <stdio.h>

#include "linux-testing-unit.h"
#include "linux-commons-queue.h"



	void test_commons_queue_buildQueue();
	void test_commons_queue_destroy();
	void test_commons_queue_putingtAndGetting();



	/*
	 * Start Mock Objects .............
	 */

	typedef struct {
		int mockId;
		char * mockName;
	}Mock;

	Boolean areEquals(Mock * m1 , Mock * m2){
		return (m1->mockId == m2->mockId);
	};

	Mock * mock(int id , char * name){
		Mock * m = malloc(sizeof(Mock));
		m->mockId = id;
		m->mockName = name;
		return m;
	}

	/*
	 * End mock objects...........
	 */





	Boolean linux_commons_queue_configureSuite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite(
				"Suite Linux Commons Queues" , suiteInitAndClean , suiteInitAndClean);

		if( unit_testing_addToSuite(suite , "Creacion de una cola .. hmm... colas..." ,
				test_commons_queue_buildQueue) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Colocando y Sacando elementos en la cola" ,
				test_commons_queue_putingtAndGetting) == NULL)
			return EXIT_FAILURE;


		if( unit_testing_addToSuite(suite , "Destruyendo la cola" ,
				test_commons_queue_destroy) == NULL)
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}





	void test_commons_queue_buildQueue(){


		Queue theQueue = commons_queue_buildQueue(areEquals);

		CU_ASSERT_PTR_NOT_NULL(theQueue);
		CU_ASSERT_EQUAL(theQueue->size , 0);

		commons_queue_destroy(theQueue);
	}




	void test_commons_queue_putingtAndGetting(){
		Queue theQueue = commons_queue_buildQueue(areEquals);

		commons_queue_put(theQueue , mock(1,"Elemento 1"));
		commons_queue_put(theQueue , mock(2,"Elemento 2"));
		commons_queue_put(theQueue , mock(3,"Elemento 3"));
		commons_queue_put(theQueue , mock(4,"Elemento 4"));

		CU_ASSERT_EQUAL( theQueue->size , 4);

		CU_ASSERT_EQUAL(((Mock *)commons_queue_get(theQueue))->mockId , 1);
		CU_ASSERT_EQUAL(((Mock *)commons_queue_get(theQueue))->mockId , 2);
		CU_ASSERT_EQUAL(((Mock *)commons_queue_get(theQueue))->mockId , 3);
		CU_ASSERT_EQUAL(((Mock *)commons_queue_get(theQueue))->mockId , 4);

		CU_ASSERT_EQUAL( theQueue->size , 0);
	}


	void test_commons_queue_destroy(){

	}
