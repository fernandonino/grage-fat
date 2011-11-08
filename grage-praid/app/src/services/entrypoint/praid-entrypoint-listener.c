/*
 * praid-entrypoint-listener.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */
#include <stdlib.h>
#include <unistd.h>

#include "linux-commons-socket.h"

#include "nipc-messaging.h"

#include "praid-configuration.h"
#include "praid-entrypoint.h"
#include "praid-state.h"
#include "praid-sync.h"



	pthread_t entrypointListenerThread;




	void praid_entry_processNewPpdConnection(NipcMessage message , ListenSocket listenSocket){

		uint8_t ppdId = message.header.operationId;

		PPDConnectionStorage * storage = praid_state_buildPPDConnectionStorageFromId(listenSocket , ppdId);

		storage->volumeSize = message.header.responseCode;

		praid_state_addPpdStorage(storage);

		if(praid_ppd_sync_isValidReplication())
			praid_ppd_sync_synchronize(storage);
		else
			praid_endpoint_ppd_callFinishReplication(storage->connection);

		praid_ppd_thread_launchNewSlaveThread(storage);
	}


	void praid_entry_denegateConnection(uint8_t processId , ListenSocket aListenSocket){

		NipcMessage message = nipc_mbuilder_buildNipcMessage();

		message = nipc_mbuilder_addResponseCode(message , NIPC_RESPONSE_CODE_ERROR);
		message = nipc_mbuilder_addMessageType(message , NIPC_MESSAGE_TYPE_HANDSHAKE);
		message = nipc_mbuilder_addProcessId(message , processId);

		nipc_messaging_send(aListenSocket , message);

		close(aListenSocket);
	}


	void praid_entry_startEntrypointListening(){

		puts("Queda en escucha");
		ServerSocket * serverSocket = commons_socket_openServerConnection(praid_configuration_getDevicePort());

		while (TRUE){

			ListenSocket listenSocket = commons_socket_acceptConnection(serverSocket);
			puts("Se conecto algo");

			NipcMessage handshake = nipc_receiveHandshake(listenSocket );

			if(handshake.header.messageType == NIPC_MESSAGE_TYPE_HANDSHAKE){

				/*
				 * Si la replicacion no esta en proceso se aceptan conecciones de nuevos ppds
				 * de lo contrario las mismas son denegadas para no generar inconsistencias				 *
				 */
				if(handshake.header.processHandshakeId == NIPC_PROCESS_ID_PPD
						&& praid_sync_isReplicationActive()){

					praid_entry_denegateConnection(handshake.header.processHandshakeId , listenSocket);

					continue;
				}

				printf("tipo de proceso conectado: %i\n" , handshake.header.processHandshakeId);

				nipc_sendHandshake(listenSocket ,  handshake.header.processHandshakeId );

				if(handshake.header.processHandshakeId == NIPC_PROCESS_ID_PFS){

					praid_pfs_launchNewSlaveThread(listenSocket);

				}else if(handshake.header.processHandshakeId == NIPC_PROCESS_ID_PPD){

					praid_entry_processNewPpdConnection(handshake , listenSocket);
				}
			}
		}
	}



	void praid_entrypoint_listener_launch(){
		pthread_create(&entrypointListenerThread , NULL , (void * (*)(void *))praid_entry_startEntrypointListening , NULL);
	}
