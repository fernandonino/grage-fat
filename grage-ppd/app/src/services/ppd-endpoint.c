/*
 * ppd-endpoint.c
 *
 *  Created on: 31/10/2011
 *      Author: gonzalo
 */

#include <nipc-messaging.h>

#include "ppd-state.h"

	void ppd_endpoint_responseGetSector( NipcMessage message){
		nipc_messaging_send(ppd_state_getActiveSocket() , message);
	}

	void ppd_endpoint_buildAndSendSyncMessage(uint16_t payLength , DiskSector diskSector){
		NipcMessage message = nipc_mbuilder_buildNipcMessage();

		message = nipc_mbuilder_addOperationId(message , NIPC_OPERATION_ID_SYNC_PUT_SECTOR);
		message = nipc_mbuilder_addPayloadLength(message , payLength);
		message = nipc_mbuilder_addDiskSector(message , diskSector);
		message = nipc_mbuilder_addMessageType( message , NIPC_MESSAGE_TYPE_SYNC_PROCESS);

		nipc_messaging_send(ppd_state_getActiveSocket() , message);

	}

	void ppd_endpoint_sendFinishReplication(){
		NipcMessage message = nipc_mbuilder_buildNipcMessage();
		message = nipc_mbuilder_addMessageType( message , NIPC_MESSAGE_TYPE_SYNC_PROCESS);
		message = nipc_mbuilder_addOperationId( message , NIPC_OPERATION_ID_SYNC_END);

		nipc_messaging_send(ppd_state_getActiveSocket() , message);
	}
