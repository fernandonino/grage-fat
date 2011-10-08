/*
 * integ-linux-commons-loader.c
 *
 *  Created on: 28/08/2011
 *      Author: gonzalo
 */


#include "linux-testing-integration.h"


	int commons_testing_integration_doInitialize(){

		/*
		 * Aca va cualquier cosa o recurso q sea necesario
		 * inicializar para todas las corridas de integracion
		 */

		return SUCCESS;
	}


	void commons_testing_integration_doFinalize(){
		/*
		 * finalizacion de los recursos creados
		 */
	}

	int commons_testing_integration_doRun(){

		/*
		 * Ejemplo de como ejecutar una suite de tests
		 * de integracion...
		 *
		 * Si hay mas suites se cuelgan de aca tambien...
		 *
		 */

		 integ_commons_example_runSuite();


		 return SUCCESS;

	}

