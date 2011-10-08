/*
 * test-ppd-example.c
 *
 *  Created on: Aug 23, 2011
 *      Author: gonzalo
 */

#include <stdlib.h>

#include "linux-commons.h"
#include "linux-testing-unit.h"

#include "nipc-messaging.h"
	/*
	 * Prototipo de funciones de test
	 */
	void test_nipc_mbuilder_addMessageType();


	/*
	 * Realiza la configuracion de la suite
	 */
	Boolean test_nipc_configureSuite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite("Grage NIPC Suite" , suiteInitAndClean , suiteInitAndClean);

		if ( unit_testing_addToSuite(suite , "Metodo de ejemplo" , test_nipc_mbuilder_addMessageType ) ==NULL )
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}



	void test_nipc_mbuilder_addMessageType(){

		NipcMessage message = nipc_mbuilder_buildNipcMessage();

		message = nipc_mbuilder_addMessageType(message , NIPC_MESSAGE_TYPE_SECTOR_DISK);

		CU_ASSERT_EQUAL(message.header.messageType , NIPC_MESSAGE_TYPE_SECTOR_DISK);

		message = nipc_mbuilder_addMessageType(message , NIPC_MESSAGE_TYPE_SECTOR_ID);

		CU_ASSERT_EQUAL(message.header.messageType , NIPC_MESSAGE_TYPE_SECTOR_ID);
	}


