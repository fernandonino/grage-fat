/*
 * pfs-launcher.c
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo
 */

#include <stdlib.h>
#include <stdio.h>
#include <linux-commons-logging.h>
#include <linux-commons-socket.h>
#include <nipc-messaging.h>

#include "pfs-configuration.h"
#include "pfs-console.h"
#include "pfs-fat-utils.h"




	void pfs_test_villa_21(){
		char * host = pfs_configuration_getDeviceAddress();
		char * port = pfs_configuration_getDevicePort();
		ListenSocket lsocket = commons_socket_openClientConnection(host , port);

		RuntimeErrorValidator * validator = commons_errors_buildSuccessValidator();
		nipc_sendHandshake(lsocket , NIPC_PROCESS_ID_PFS , validator);
		NipcMessage message = nipc_receiveHandshake(lsocket , validator);

		if(message.header.responseCode == NIPC_RESPONSE_CODE_ERROR
				|| commons_errors_hasError(validator))
			exit(1);

		DiskSector sector;
		sector.sectorNumber = 1;
		strcpy(sector.sectorContent , "hola mundo");

		message = nipc_mbuilder_buildNipcMessage();
		message = nipc_mbuilder_addOperationId(message , NIPC_OPERATION_ID_PUT_SECTORS);
		message = nipc_mbuilder_addMessageType(message , NIPC_MESSAGE_TYPE_SECTOR_DISK);
		message = nipc_mbuilder_addDiskSector(message , sector);

		nipc_messaging_send(lsocket , message);

	}




	void pfs_launcher_initialize(){
		log_create("pfs","/opt/grage-repository/logs/pfs.log",INFO|DEBUG|WARNING|ERROR,M_CONSOLE_DISABLE);
		pfs_configuration_initialize();
		//pfs_console_initialize();
	}


	void pfs_launcher_launch(){
		pfs_test_villa_21();
	}

	void pfs_launcher_exit(){
		log_destroy();
	}

	int main(int argc, char *argv[]){

		/*
		 * Parametro de la aplicacion - path al archivo de conf
		 */

		if(argc > 1){
			pfs_configuration_setConfigurationFile(argv[1]);
		} else {
			pfs_configuration_setConfigurationFile(PFS_DEFAULT_CONFIGURATION_FILE);
		}

		pfs_launcher_initialize();

		pfs_launcher_launch();

		pfs_launcher_exit();
		return EXIT_SUCCESS;
	}
