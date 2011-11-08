/*
 * ppd-connection-launcher.c
 *
 *  Created on: 08/10/2011
 *      Author: gonzalo
 */

#include "linux-commons-strings.h"
#include "linux-commons.h"
#include "linux-commons-socket.h"
#include "grage-commons.h"

#include "nipc-messaging.h"

#include "ppd-state.h"
#include "ppd-configuration.h"
#include "ppd-entrypoint.h"

	void ppd_connections_handshake();


	void ppd_connections_connectToPraid(){

		ListenSocket praidSocket = commons_socket_openClientConnection(
				ppd_conf_getPraidAddress() ,
				ppd_conf_getPraidPort());

		if(praidSocket > 0){
			puts("Se ha logrado conectar con el PRAID");

			puts("Salvando el estado de la coneccion");
			ppd_state_setPraidSocket(praidSocket);

			puts("Realizando handshake");
			ppd_connections_handshake();


		}else{
			puts("Fallo la coneccion con el proceso PRAID");
			puts("Finalizando");
			exit(1);
		}
	}

	void ppd_connections_waitForPfsConnections(){

		ServerSocket * pfsConnection = commons_socket_openServerConnection(
				ppd_conf_getPpdPort());

		ppd_state_setPfsConnection(pfsConnection);

		while(TRUE){

			ListenSocket pfsSocket = commons_socket_acceptConnection(pfsConnection);

			ppd_connections_handshake();

		}
	}


	void ppd_connections_handshake(){

		NipcMessage message;

		if(commons_string_equals( ppd_conf_getPpdMode() ,
				PPD_CONFIGURATION_MODE_CONNECT)){

			uint8_t ppdId = atoi(ppd_conf_getPpdIdDisk());

			nipc_sendPpdHandshake(ppd_state_getPraidSocket(), ppdId ,
					ppd_state_getVolumeSize() );
			message = nipc_receiveHandshake(ppd_state_getPraidSocket());

			if(message.header.responseCode == NIPC_RESPONSE_CODE_ERROR){
				puts("El proceso RAID denego la coneccion del proceso PPD, vuelva a intentarlo mas tarde");
				exit(1);
			}

		}else{
			ServerSocket * s = ppd_state_getPfsConnection();
			message = nipc_receiveHandshake(s->listenSocket );
		}
	}

