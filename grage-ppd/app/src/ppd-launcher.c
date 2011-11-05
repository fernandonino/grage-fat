/*
 * ppd-launcher.c
 *
 *  Created on: 05/09/2011
 *      Author: EL Seba
 */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <linux-commons-logging.h>
#include <linux-commons-strings.h>
#include <nipc-messaging.h>
#include <unistd.h>

#include "ppd-configuration.h"
#include "ppd-queues.h"
#include "ppd-planifier.h"
#include "ppd-persistance.h"
#include "ppd-state.h"
#include "ppd-launchConsole.h"
#include "ppd-connection.h"
#include "ppd-entrypoint.h"


	void ppd_launcher_exit();


	extern pthread_t entrypointThread;
	extern pthread_t jobsThread;
	extern pthread_t jobsThread;

	char * diskPath;



	void ppd_launcher_initialize(){
		int status = log_create("ppd", PPD_DEFAULT_LOG_FILE ,INFO|WARNING|ERROR|DEBUG,M_CONSOLE_DISABLE);

		if(status == 0)
			puts("Log creado con exito");
		else
			puts("Fallo la creacion del log");

		ppd_configuration_setup();
		//ppd_launchConsole_initialize();
		//ppd_launchConsole_startUNIX();

		//ppd_state_setDiskStartAddress( ppd_persistance_mapDisk(ppd_conf_getDiskPath()) );
	}


	void ppd_launcher_joinAllThread(){

		printf("joineando hilos\n");

		pthread_join(entrypointThread , NULL);
		pthread_join(jobsThread , NULL);
		pthread_join(jobsThread , NULL);
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
 		ppd_launcher_launchConnections();
		ppd_entrypoint_launch();
		ppd_planifier_worker_doJobs();
		ppd_launcher_joinAllThread();
	}



	void ppd_launcher_exit(){
		puts("finalizando todo");
		//ppd_persistance_unmapDisk( ppd_conf_getDiskPath() , ppd_state_getDiskStartAddress() );
		log_destroy();
	}


	int main(){

		ppd_launcher_initialize();
		ppd_launcher_doLaunch();

		ppd_launcher_exit();

		return EXIT_SUCCESS;
	}
