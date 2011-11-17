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
#include "ppd-configuration.h"



	void ppd_pfs_entrypoint_serviceThread(ListenSocket * pfsSocket);
	Boolean ppd_pfs_entrypoint_processRequest(ListenSocket * pfsSocket);



	void ppd_entrypoint_launchPfsPpdEntrypoint(){

		puts("[ Quedando en escucha de conexiones PFS] ");

		while(TRUE){

			ListenSocket pfsSocket = commons_socket_acceptConnection(ppd_state_getPfsConnection());

			Boolean status = ppd_connections_handshake(pfsSocket);

			pthread_t newPfsThread;

			if(status){

				ListenSocket * newPfsSocket = (ListenSocket *) malloc(sizeof(ListenSocket));
				*newPfsSocket = pfsSocket;

				pthread_create( &newPfsThread , NULL , (void* (*) (void*))ppd_pfs_entrypoint_serviceThread , newPfsSocket);

				puts("Se creo un hilo");
			}
		}
	}


	void ppd_pfs_entrypoint_serviceThread(ListenSocket * pfsSocket){

		if( ppd_conf_isPooledConnections()){

			while(ppd_pfs_entrypoint_processRequest(pfsSocket));

		}else{
			ppd_pfs_entrypoint_processRequest(pfsSocket);
		}

		puts("Se murio un hilo");
	}


	Boolean ppd_pfs_entrypoint_processRequest(ListenSocket * pfsSocket){

		NipcMessage message = nipc_messaging_receive(*pfsSocket);

		if(message.header.operationId == NIPC_OPERATION_ID_GET_SECTORS){

			puts("[ Recibiendo peticion GET Sectores ]");
			printf("[ Solicitando sectorId: %i ]\n" , message.payload.diskSector.sectorNumber);

			message.payload.pfsSocket = *pfsSocket;

			ppd_entrypoint_executeGetSector(message);

		}else if(message.header.operationId == NIPC_OPERATION_ID_PUT_SECTORS){

			puts("[ Recibiendo peticion PUT Sectores ]");
			printf("[ Solicitando sectorId: %i ]\n" , message.payload.diskSector.sectorNumber);

			message.payload.pfsSocket = *pfsSocket;

			ppd_entrypoint_executePutSector(message);

		}else if(nipc_mbuilder_isBlanckMessage(message)){

			return FALSE;
		}

		return TRUE;
	}



