/*
 * ppd-console-launcher.c
 *
 *  Created on: 10/10/2011
 *      Author: joaquin
 */

#include <stdlib.h>
#include <stdio.h>
#include <linux-commons-logging.h>

	void praid_launcher_initialize(){
		log_create("ppd-console","../logs/ppd-console.log",INFO|WARNING|ERROR|DEBUG,M_CONSOLE_DISABLE);

	}


	void praid_launcher_doLaunch(){

		//lanzar el thread que atiende peticiones
	}

	void praid_launcher_exit() {
		log_destroy();
	}

	int main(int argc, char *args[]){

		/*
		 * Parametro de la aplicacion - path al archivo de conf
		 */

		praid_launcher_initialize();

		praid_launcher_doLaunch();

		praid_launcher_exit();

		return EXIT_SUCCESS;
	}
