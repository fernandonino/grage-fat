/*
 * praid-entrypoint-listener.c
 *
 *  Created on: 06/10/2011
 *      Author: utn_so
 */



#include "linux-commons-socket.h"

#include "praid-configuration.h"

#include "nipc-messaging.h"

#include "praid-state.h"

	pthread_t entrypointListenerThread;



	void praid_entry_startEntrypointListening(){

		ServerSocket * serverSocket = commons_socket_openServerConnection(praid_configuration_getDevicePort());

		while (TRUE){

			ListenSocket listenSocket = commons_socket_acceptConnection(serverSocket);

			RuntimeErrorValidator * validator = commons_errors_buildSuccessValidator();

			NipcMessage handshake = nipc_receiveHandshake(listenSocket , validator);
			nipc_sendHandshake(listenSocket , validator);

			if(commons_errors_hasError(validator)){
				printf("ha ocurrido un error en el handshake");
				return;
			}

			if(handshake.header.processHandshakeId == NIPC_PROCESS_ID_PFS){

				praid_pfs_launchNewSlaveThread(listenSocket);

			}else if(handshake.header.processHandshakeId == NIPC_PROCESS_ID_PPD){

				PPDConnectionStorage * storage = praid_state_buildPPDConnectionStorage(listenSocket);

				praid_state_addPpdStorage(storage);

				praid_ppd_thread_launchNewSlaveThread(storage);
			}

			//liberar la memoria del validator
		}
	}



	void praid_entrypoint_listener_launch(){
		pthread_create(&entrypointListenerThread , NULL , (void * (*)(void *))praid_entry_startEntrypointListening , NULL);
	}
