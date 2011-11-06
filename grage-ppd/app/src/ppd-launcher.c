/*
 * ppd-launcher.c
 *
 *  Created on: 05/09/2011
 *      Author: EL Seba
 */

#include <strings.h>
#include <sys/types.h>
#include <wchar.h>
#include <iconv.h>
#include <fcntl.h>
#include <dirent.h>


#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <linux-commons-logging.h>
#include <linux-commons-strings.h>
#include <linux-commons-file.h>
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
	extern pthread_t ppdConsoleThread;


	char * diskPath;

	void ppd_launcher_initializeBPB(){

		BPB biosParameterBlock;
		DiskSector diskSector = ppd_persistence_readSector(0 , ppd_state_getDiskStartAddress());
		memcpy(&biosParameterBlock , diskSector.sectorContent , sizeof(BPB));

		ppd_state_setSectorsCount(biosParameterBlock.BPB_TotSec32);
	}



	void ppd_launcher_console(){
		//ppd_launchConsole_initialize();
		ppd_launchConsole_launchSlaveThread();
	}


	void ppd_initializeDisk(){
		ppd_state_setDiskStartAddress( ppd_persistance_mapDisk(ppd_conf_getDiskPath()) );
		ppd_launcher_initializeBPB();
	}

	void ppd_launcher_initialize(){
		int status = log_create("ppd", PPD_DEFAULT_LOG_FILE ,INFO|WARNING|ERROR|DEBUG,M_CONSOLE_DISABLE);

		if(status == 0)
			puts("Log creado con exito");
		else
			puts("Fallo la creacion del log");

		ppd_configuration_setup();

		/*
		 * Si existe la configuracion se mapea el disco
		 */
		if( commons_file_isValidConfValue( ppd_conf_getDiskPath()))
			ppd_initializeDisk();

		ppd_queues_initialize();
	}


	void ppd_launcher_joinAllThread(){

		printf("joineando hilos\n");

		//pthread_join(entrypointThread , NULL);
		pthread_join(jobsThread , NULL);
		pthread_join(ppdConsoleThread , NULL);
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
 		//ppd_launcher_launchConnections();
		//ppd_entrypoint_launch();
		ppd_launcher_console();
		ppd_planifier_worker_doJobs();
		ppd_launcher_joinAllThread();
	}



	void ppd_launcher_exit(){
		puts("finalizando todo");

		/*
		 * Desmapeamos el disco solo si fue mapeado
		 */
		if( commons_file_isValidConfValue( ppd_conf_getDiskPath()))
			ppd_persistance_unmapDisk( ppd_conf_getDiskPath() , ppd_state_getDiskStartAddress() );

		log_destroy();
	}


	int main(){

		ppd_launcher_initialize();
		ppd_launcher_doLaunch();

		ppd_launcher_exit();

		return EXIT_SUCCESS;
	}
