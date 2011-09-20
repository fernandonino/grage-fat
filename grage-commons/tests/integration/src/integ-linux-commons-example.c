/*
 * integ-linux-commons-example.c
 *
 *  Created on: 28/08/2011
 *      Author: gonzalo
 */

#include <stdlib.h>
#include <stdio.h>


	void integ_commons_integrateFlowExample1();
	void integ_commons_integrateFlowExample2();
	void integ_commons_integrateFlowExample3();





	/*
	 * *********** Inicializacion de la Suite **************
	 */

	void integ_commons_example_runSuite(){

		integ_commons_integrateFlowExample1();
		integ_commons_integrateFlowExample2();
		integ_commons_integrateFlowExample3();
	}







	/*
	 * *********** Metodos de tests de integracion **************
	 */



	void integ_commons_integrateFlowExample1(){

		printf("call 1\n");
		printf("call 2\n");
		printf("call 3\n");
		printf("call 4\n");

	}


	void integ_commons_integrateFlowExample2(){

		printf("call 1\n");
		printf("call 2\n");
		printf("call 3\n");
		printf("call 4\n");

	}


	void integ_commons_integrateFlowExample3(){

		printf("call 1\n");
		printf("call 2\n");
		printf("call 3\n");
		printf("call 4\n");

	}
