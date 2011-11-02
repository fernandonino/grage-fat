/*
 * integration.c
 *
 *  Created on: 02/11/2011
 *      Author: gonzalo
 */


#include <time.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux-commons-logging.h>
#include <linux-commons-socket.h>
#include <nipc-messaging.h>
#include <linux-commons.h>

#include "pfs-configuration.h"
#include "pfs-console.h"
#include "pfs-fat-utils.h"



void pfs_test_villa_21_get();
void pfs_test_villa_21_put();
void launchThreadTestsPut();
void launchThreadTestsGet();

	void integ_pfs_integrateFlowPut(){
		launchThreadTestsPut();
	}

	void integ_pfs_integrateFlowGet(){
		launchThreadTestsGet();

	}

	void launchThreadTestsGet(){
		pthread_t th1;
		pthread_t th2;
		pthread_t th3;
		pthread_t th4;
		pthread_t th5;
		pthread_t th6;
		pthread_t th7;
		pthread_t th8;
		pthread_t th9;
		pthread_t th10;
		pthread_t th11;
		pthread_t th12;
		pthread_t th13;

		pthread_create(&th1 , NULL , pfs_test_villa_21_get , NULL);
		pthread_create(&th2 , NULL , pfs_test_villa_21_get , NULL);
		pthread_create(&th3 , NULL , pfs_test_villa_21_get , NULL);
		pthread_create(&th4 , NULL , pfs_test_villa_21_get , NULL);
		pthread_create(&th5 , NULL , pfs_test_villa_21_get , NULL);
		pthread_create(&th6 , NULL , pfs_test_villa_21_get , NULL);
		pthread_create(&th7 , NULL , pfs_test_villa_21_get , NULL);
		pthread_create(&th8 , NULL , pfs_test_villa_21_get , NULL);
		pthread_create(&th9 , NULL , pfs_test_villa_21_get , NULL);
		pthread_create(&th10 , NULL , pfs_test_villa_21_get , NULL);
		pthread_create(&th11 , NULL , pfs_test_villa_21_get , NULL);
		pthread_create(&th12 , NULL , pfs_test_villa_21_get , NULL);
		pthread_create(&th13 , NULL , pfs_test_villa_21_get , NULL);

		pthread_join(th1 ,NULL);
		pthread_join(th2 ,NULL);
		pthread_join(th3 ,NULL);
		pthread_join(th4 ,NULL);
		pthread_join(th5 ,NULL);
		pthread_join(th6 ,NULL);
		pthread_join(th7 ,NULL);
		pthread_join(th8 ,NULL);
		pthread_join(th9 ,NULL);
		pthread_join(th10 ,NULL);
		pthread_join(th11 ,NULL);
		pthread_join(th12 ,NULL);
		pthread_join(th13 ,NULL);

	}

	void launchThreadTestsPut(){
		pthread_t th1;
		pthread_t th2;
		pthread_t th3;
		pthread_t th4;
		pthread_t th5;
		pthread_t th6;
		pthread_t th7;
		pthread_t th8;

		pthread_create(&th1 , NULL , pfs_test_villa_21_put , NULL);
		pthread_create(&th2 , NULL , pfs_test_villa_21_put , NULL);
		pthread_create(&th3 , NULL , pfs_test_villa_21_put , NULL);
		pthread_create(&th4 , NULL , pfs_test_villa_21_put , NULL);
		pthread_create(&th5 , NULL , pfs_test_villa_21_put , NULL);
		pthread_create(&th6 , NULL , pfs_test_villa_21_put , NULL);
		pthread_create(&th7 , NULL , pfs_test_villa_21_put , NULL);
		pthread_create(&th8 , NULL , pfs_test_villa_21_put , NULL);

		pthread_join(th1 ,NULL);
		pthread_join(th2 ,NULL);
		pthread_join(th3 ,NULL);
		pthread_join(th4 ,NULL);
		pthread_join(th5 ,NULL);
		pthread_join(th6 ,NULL);
		pthread_join(th7 ,NULL);
		pthread_join(th8 ,NULL);
	}


	void pfs_test_villa_21_get() {
		char * host = pfs_configuration_getDeviceAddress();
		char * port = pfs_configuration_getDevicePort();
		ListenSocket lsocket = commons_socket_openClientConnection(host, port);

		RuntimeErrorValidator * validator = commons_errors_buildSuccessValidator();
		nipc_sendHandshake(lsocket, NIPC_PROCESS_ID_PFS, validator);
		NipcMessage message = nipc_receiveHandshake(lsocket, validator);

		if (message.header.responseCode == NIPC_RESPONSE_CODE_ERROR
				|| commons_errors_hasError(validator)){

			puts("error en el response code");
			exit(1);
		}

		message = nipc_mbuilder_buildNipcMessage();
		message = nipc_mbuilder_addOperationId(message,
				NIPC_OPERATION_ID_GET_SECTORS);
		message = nipc_mbuilder_addMessageType(message,
				NIPC_MESSAGE_TYPE_SECTOR_ID);
		message = nipc_mbuilder_addDiskSectorId(message, 5);

		nipc_messaging_send(lsocket, message);

		message = nipc_messaging_receive(lsocket);

		printf("SectorId: %i , SectorContent: %s\n",
				message.payload.diskSector.sectorNumber,
				message.payload.diskSector.sectorContent);

	}




	void pfs_test_villa_21_put() {
		char * host = pfs_configuration_getDeviceAddress();
			char * port = pfs_configuration_getDevicePort();
		ListenSocket lsocket = commons_socket_openClientConnection(host, port);

		RuntimeErrorValidator * validator = commons_errors_buildSuccessValidator();
		nipc_sendHandshake(lsocket, NIPC_PROCESS_ID_PFS, validator);
		NipcMessage message = nipc_receiveHandshake(lsocket, validator);

		if (message.header.responseCode == NIPC_RESPONSE_CODE_ERROR
				|| commons_errors_hasError(validator)){

			puts("error en el response code");
			exit(1);
		}

		DiskSector sector;
		sector.sectorNumber = 1;
		strcpy(sector.sectorContent, "hola mundo");

		message = nipc_mbuilder_buildNipcMessage();
		message = nipc_mbuilder_addOperationId(message,
				NIPC_OPERATION_ID_PUT_SECTORS);
		message = nipc_mbuilder_addMessageType(message,
				NIPC_MESSAGE_TYPE_SECTOR_DISK);
		message = nipc_mbuilder_addDiskSector(message, sector);

		nipc_messaging_send(lsocket, message);

	}


