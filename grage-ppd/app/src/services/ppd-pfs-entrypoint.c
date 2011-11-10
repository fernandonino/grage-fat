/*
 * ppd-pfs-entrypoint.c
 *
 *  Created on: 10/11/2011
 *      Author: gonzalo
 */

#include <linux-commons.h>
#include <linux-commons-socket.h>

#include <nipc-messaging.h>

#include "ppd-entrypoint.h"
#include "ppd-state.h"
#include "ppd-connection.h"

	void ppd_pfs_entrypoint_serviceThread(ListenSocket * pfsSocket);


	void ppd_entrypoint_launchPfsPpdEntrypoint(){

		while(TRUE){

			puts("[ Quedando en escucha de conexiones PFS] ");

			ListenSocket pfsSocket = commons_socket_acceptConnection(ppd_state_getPfsConnection());

			Boolean status = ppd_connections_handshake(pfsSocket);

			pthread_t newPfsThread;

			if(status){

				ListenSocket * newPfsSocket = (ListenSocket *) malloc(sizeof(ListenSocket));
				*newPfsSocket = pfsSocket;

				pthread_create( &newPfsThread , NULL , (void* (*) (void*))ppd_pfs_entrypoint_serviceThread , newPfsSocket);
			}
		}
	}


	void ppd_pfs_entrypoint_serviceThread(ListenSocket * pfsSocket){

		NipcMessage message = nipc_messaging_receive(*pfsSocket);

		if(message.header.operationId == NIPC_OPERATION_ID_GET_SECTORS){

			message.payload.pfsSocket = *pfsSocket;

			ppd_entrypoint_executeGetSector(message);

		}else if(message.header.operationId == NIPC_OPERATION_ID_PUT_SECTORS){

			message.payload.pfsSocket = *pfsSocket;

			ppd_entrypoint_executePutSector(message);
		}
	}




