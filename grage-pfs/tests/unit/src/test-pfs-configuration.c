/*
 * test-pfs-configuration.c
 *
 *  Created on: Sep 09, 2011
 *      Author: Fernando
 */

#include <stdlib.h>

#include "linux-commons.h"
#include "linux-testing-unit.h"

#include "pfs-configuration.h"

	/*
	 * Prototipo de funciones de test
	 */

	void test_pfs_setDevicePort();
	void test_pfs_setDeviceAddress();
	void test_pfs_setMaxConnections();
	void test_pfs_setCacheSize();
	void test_pfs_getDevicePort();
	void test_pfs_getDeviceAddress();
	void test_pfs_getMaxConnections();
	void test_pfs_getCacheSize();


	/*
	 * Realiza la configuracion de la suite
	 */
	Boolean test_pfs_configurationSuite(){

		int suiteInitAndClean(){
			pfs_configuration_setCacheSize(NULL);
			pfs_configuration_setDeviceAddress(NULL);
			pfs_configuration_setMaxConnections(NULL);
			pfs_configuration_setDevicePort(NULL);
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite("Grage PFS Configuration Suite" , suiteInitAndClean , suiteInitAndClean);


		if ( unit_testing_addToSuite(suite , "Obtener puerto del device" , test_pfs_getDevicePort ) ==NULL )
			return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "Obtener direccion del device" , test_pfs_getDeviceAddress ) ==NULL )
			return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "Obtener maxima cantidad de conecciones" , test_pfs_getMaxConnections ) ==NULL )
			return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "Obtener size del cache" , test_pfs_getCacheSize ) ==NULL )
			return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "Setear puerto del device" , test_pfs_setDevicePort ) ==NULL )
			return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "Setear direccion del device" , test_pfs_setDeviceAddress ) ==NULL )
			return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "Setear maxima cantidad de conecciones" , test_pfs_setMaxConnections ) ==NULL )
			return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "Setear size del cache" , test_pfs_setCacheSize ) ==NULL )
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}


	void test_pfs_setDevicePort(){
		pfs_configuration_setDevicePort("10000");
		char * port = pfs_configuration_getDevicePort();
		CU_ASSERT_PTR_NOT_NULL(port);
		CU_ASSERT_STRING_EQUAL(port,"10000");
	}

	void test_pfs_setDeviceAddress(){
		pfs_configuration_setDeviceAddress("127.0.0.1");
		char * address = pfs_configuration_getDeviceAddress();
		CU_ASSERT_PTR_NOT_NULL(address);
		CU_ASSERT_STRING_EQUAL(address,"127.0.0.1");
	}

	void test_pfs_setMaxConnections(){
		pfs_configuration_setMaxConnections("5");
		char * connections = pfs_configuration_getMaxConnections();
		CU_ASSERT_PTR_NOT_NULL(connections);
		CU_ASSERT_STRING_EQUAL(connections,"5");
	}

	void test_pfs_setCacheSize(){
		pfs_configuration_setCacheSize("32");
		char * size = pfs_configuration_getCacheSize();
		CU_ASSERT_PTR_NOT_NULL(size);
		CU_ASSERT_STRING_EQUAL(size,"32");
	}

	void test_pfs_getDevicePort(){
		char * port = pfs_configuration_getDevicePort();

/*		if(NULL == pfs_configuration_getDevicePort())
			printf("**************************************************************");
		else
			printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
*/

		CU_ASSERT_PTR_NULL(pfs_configuration_getDevicePort());
	}

	void test_pfs_getDeviceAddress(){
		char * address = pfs_configuration_getDeviceAddress();
		//CU_ASSERT_PTR_NULL(address);
	}

	void test_pfs_getMaxConnections(){
		char * connections = pfs_configuration_getMaxConnections();
		//CU_ASSERT_PTR_NULL(connections);
	}

	void test_pfs_getCacheSize(){
		char * size = pfs_configuration_getCacheSize();
		//CU_ASSERT_PTR_NULL(size);

	}
