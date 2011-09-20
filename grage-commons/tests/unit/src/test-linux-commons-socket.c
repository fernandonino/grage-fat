/*
 * test-linux-commons-errors.c
 *
 *  Created on: 08/08/2011
 *      Author: gonzalo
 */




#include <stdlib.h>
#include <unistd.h>
#include "linux-commons.h"
#include "linux-commons-strings.h"
#include "linux-commons-socket.h"

#include "linux-testing-unit.h"

	void test_commons_socket_srv_bind();
	void test_commons_client_connect();

	/*
	 * Realiza la configuracion de la suite
	 */
	Boolean linux_commons_socket_configureSuite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite(
				"Suite Linux Commons Socket" , suiteInitAndClean , suiteInitAndClean);

		if( unit_testing_addToSuite(suite , "Test prueba binding del server" , test_commons_socket_srv_bind) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Test pruebo cliente" , test_commons_client_connect) == NULL)
			return EXIT_FAILURE;

//		if( unit_testing_addToSuite(suite , "Test Metodo concat" , test_commons_string_concat) == NULL)
//			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}


	void test_commons_socket_srv_bind(){
		ServerSocket * srvSocket = commons_socket_openServerConnection("9000");

		CU_ASSERT_PTR_NOT_NULL(srvSocket);

		//CU_ASSERT_TRUE(srvSocket->listenSocket > 0);
	}

	void test_commons_client_connect(){
		CU_PASS("ok");
	}

//int main(void){
//
//	commons_socket_openServerConnection("32657");
//	sleep(15);
//	return EXIT_SUCCESS;
//}

