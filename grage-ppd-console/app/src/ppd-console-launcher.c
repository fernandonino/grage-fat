/*
 * ppd-console-launcher.c
 *
 *  Created on: 10/10/2011
 *      Author: joaquin
 */

#include <stdlib.h>
#include <stdio.h>
#include <linux-commons-logging.h>
#include "ppd_console_interpreter.h"

	void ppd_console_launcher_initialize(){
		log_create("ppd-console","../logs/ppd-console.log",INFO|WARNING|ERROR|DEBUG,M_CONSOLE_DISABLE);
	}

	void ppd_console_launcher_exit() {
		log_destroy();
	}

	int main(int argc, char *args[]){


		ppd_console_launcher_initialize();
		ppd_console_interpreter();
		ppd_console_launcher_exit();

		return EXIT_SUCCESS;
	}
