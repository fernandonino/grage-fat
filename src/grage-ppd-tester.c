/*
 ============================================================================
 Name        : grage-ppd-tester.c
 Author      : yo
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include <linux-commons-socket.h>
#include <grage-commons.h>
#include <linux-commons-logging.h>

#include <nipc-messaging.h>



#define LOCAL_SERVER_PORT		"10000"
#define PPD_DEFAULT_LOG_FILE	"/opt/grage-repository/logs/grage-ppd-tester.log"



	int getSectorsCount(int size){
		int sectorsCount = size / 512;
		if(size % 512 != 0)
			sectorsCount += 1;

		return sectorsCount;
	}


	ListenSocket wait_for_Pfs(ServerSocket * serverSocket , uint32_t * sectorsCount){
		ListenSocket listenSocket = commons_socket_acceptConnection(serverSocket);

		NipcMessage handshake = nipc_receiveHandshake(listenSocket );

		nipc_sendHandshake(listenSocket ,  handshake.header.processHandshakeId );

		*sectorsCount = handshake.header.responseCode;

		return listenSocket;
	}


	int main(void) {


		int status = log_create("ppd", PPD_DEFAULT_LOG_FILE ,INFO|WARNING|ERROR|DEBUG,M_CONSOLE_DISABLE);

		if(status == 0)
			puts("[ Log creado con exito ]");
		else{
			puts("[ Fallo la creacion del log ]");
			exit(EXIT_FAILURE);
		}

		uint32_t bytesCountPpd1;
		uint32_t bytesCountPpd2;

		ServerSocket * serverSocket = commons_socket_openServerConnection(LOCAL_SERVER_PORT);

		ListenSocket listenSocket1 = wait_for_Pfs(serverSocket , &bytesCountPpd1);
		ListenSocket listenSocket2 = wait_for_Pfs(serverSocket , &bytesCountPpd2);

		NipcMessage message ;

		uint32_t sectorsCount = getSectorsCount(bytesCountPpd1);

		for(int i=0 ; i<sectorsCount ; i++){

			printf("[ sector: %i de %i]\n" , i , sectorsCount);

			message = nipc_mbuilder_buildNipcMessage();
			message = nipc_mbuilder_addOperationId(message , NIPC_OPERATION_ID_GET_SECTORS);
			message = nipc_mbuilder_addDiskSectorId(message , i);
			nipc_messaging_send(listenSocket1 , message);

			message = nipc_messaging_receive(listenSocket1);

			if( nipc_mbuilder_isBlanckMessage(message)){
				puts("[ finalizando por un error al cerrarse el socket] ");
				break;
			}

			message = nipc_mbuilder_addOperationId(message , NIPC_OPERATION_ID_PUT_SECTORS);

			/*
			if( i == 15){
				printf("[ Enviando basura en el sector %i ]\n" , i);
				memset(message.payload.diskSector.sectorContent , '*' , sizeof(message.payload.diskSector.sectorContent));
			}
			*/
			nipc_messaging_send(listenSocket2 , message);

		}

		log_destroy();

		nipc_messaging_receive(listenSocket1);
		return EXIT_SUCCESS;
	}
