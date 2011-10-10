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

	extern pthread_t entrypointThread;
	extern pthread_t readingJobThread;
	extern pthread_t writingJobThread;


	void ppd_launcher_initialize(){
		log_create("ppd","../logs/ppd.log",INFO|WARNING|ERROR|DEBUG,M_CONSOLE_DISABLE);
		ppd_configuration_setup();
	}


	void ppd_launcher_joinAllThread(){

		printf("joineando hilos\n");

		pthread_join(entrypointThread , NULL);
		pthread_join(readingJobThread , NULL);
		pthread_join(writingJobThread , NULL);
	}

	void ppd_launcher_launchConnections(){
		if(commons_string_equals(ppd_conf_getPpdMode() ,
				PPD_CONFIGURATION_MODE_CONNECT)){
			ppd_connections_connectToPraid();
		}else{
			ppd_connections_waitForPfsConnections();
		}
	}


	void ppd_launcher_doLaunch(){
		ppd_queues_initialize();
 		ppd_planifier_worker_doJobs();
 		ppd_launcher_launchConnections();
		ppd_entrypoint_launch();
		ppd_launcher_joinAllThread();
	}


	void ppd_laucher_exit(){
		log_destroy();
	}


	int main(int argc, char *args[]){

		ppd_launcher_initialize();
		ppd_launcher_doLaunch();

		//TODO: esta funcion hay q colgarla del evento del CTRL+C
		//TODO: para ser ejecutada ahi.
		// ppd_laucher_exit();

		return EXIT_SUCCESS;
	}
