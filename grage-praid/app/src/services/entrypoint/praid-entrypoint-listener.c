/*
 * praid-entrypoint-listener.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */
#include <stdlib.h>

#include "linux-commons-socket.h"

#include "nipc-messaging.h"

#include "praid-configuration.h"
#include "praid-entrypoint.h"
#include "praid-state.h"



	pthread_t entrypointListenerThread;



	void praid_entry_startEntrypointListening(){

		puts("Queda en escucha");
		ServerSocket * serverSocket = commons_socket_openServerConnection(praid_configuration_getDevicePort());

		while (TRUE){

			ListenSocket listenSocket = commons_socket_acceptConnection(serverSocket);
			puts("Se conecto algo");

			RuntimeErrorValidator * validator = commons_errors_buildSuccessValidator();

			NipcMessage handshake = nipc_receiveHandshake(listenSocket , validator);

			if(handshake.header.messageType == NIPC_MESSAGE_TYPE_HANDSHAKE){

				printf("tipo de proceso conectado: %i\n" , handshake.header.processHandshakeId);

				nipc_sendHandshake(listenSocket ,  handshake.header.processHandshakeId, validator);

				if(commons_errors_hasError(validator)){
					puts("ha ocurrido un error en el handshake");
					exit(1);
				}

				if(handshake.header.processHandshakeId == NIPC_PROCESS_ID_PFS){

					praid_pfs_launchNewSlaveThread(listenSocket);

				}else if(handshake.header.processHandshakeId == NIPC_PROCESS_ID_PPD){

					uint8_t ppdId = handshake.header.operationId;

					PPDConnectionStorage * storage = praid_state_buildPPDConnectionStorageFromId(listenSocket , ppdId);

					storage->sectorsCount = handshake.header.responseCode;

					praid_state_addPpdStorage(storage);

					praid_ppd_thread_launchNewSlaveThread(storage);
				}
			}

			free(validator);
		}
	}



	void praid_entrypoint_listener_launch(){
		pthread_create(&entrypointListenerThread , NULL , (void * (*)(void *))praid_entry_startEntrypointListening , NULL);
	}
