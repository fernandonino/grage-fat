/*
 * ppd-launcher.c
 *
 *  Created on: 05/09/2011
 *      Author: EL Seba
 */

#include <stdlib.h>
#include <stdio.h>
#include <linux-commons-logging.h>
#include "ppd-configuration.h"
#include "ppd-queues.h"


	void ppd_launcher_initialize(){
		log_create("ppd","../logs/ppd.log",INFO|WARNING|ERROR|DEBUG,M_CONSOLE_DISABLE);
		ppd_configuration_setup();
	}


	void ppd_launcher_doLaunch(){

		//ppd_queues_initialize();

		if(commons_string_equals(ppd_conf_getPpdMode() ,
				PPD_CONFIGURATION_MODE_CONNECT)){
			ppd_connections_connectToPraid();
		}else{
			ppd_connections_waitForPfsConnections();
		}
	}

	void ppd_laucher_exit(){
		log_destroy();
	}


	int main(int argc, char *args[]){

		ppd_launcher_initialize();

		ppd_launcher_doLaunch();

		ppd_laucher_exit();

		return EXIT_SUCCESS;
	}
