/*
 * nipc-facade.c
 *
 *  Created on: 08/10/2011
 *      Author: gonzalo
 */

#include "nipc-messaging.h"
#include "linux-commons-errors.h"

	void nipc_sendHandshake(ListenSocket lSocket , uint8_t processId , RuntimeErrorValidator * validator){
		NipcMessage message;

		message = nipc_mbuilder_addMessageType(message , NIPC_MESSAGE_TYPE_HANDSHAKE);
		message = nipc_mbuilder_addProcessId(message , processId);

		nipc_messaging_send(lSocket , message);

/*		message = nipc_messaging_receive(lSocket);

		if(message.header.responseCode == NIPC_RESPONSE_CODE_ERROR){
			commons_errors_setError(validator , "Ha ocurrido un error en el handshake");
			return;
		}
		*/
	}

	NipcMessage nipc_receiveHandshake(ListenSocket lSocket , RuntimeErrorValidator * validator){

		NipcMessage message = nipc_messaging_receive(lSocket);

		if(message.header.responseCode == NIPC_RESPONSE_CODE_ERROR){
			commons_errors_setError(validator , "Ha ocurrido un error en el handshake");
		}

		return message;
	}


	void nipc_sendPutSectorRequest(ListenSocket l , DiskSector diskSector , RuntimeErrorValidator * validator){

		NipcMessage message;
		message = nipc_mbuilder_addMessageType(message , NIPC_MESSAGE_TYPE_SECTOR_DISK);
		message = nipc_mbuilder_addOperationId(message , NIPC_OPERATION_ID_PUT_SECTORS);
		message = nipc_mbuilder_addDiskSector(message , diskSector);

		nipc_messaging_send(l , message);

	}

	void nipc_sendGetSectorRequest(ListenSocket l , uint32_t sector , RuntimeErrorValidator * validator){

		NipcMessage message;
		message = nipc_mbuilder_addMessageType(message , NIPC_MESSAGE_TYPE_SECTOR_DISK);
		message = nipc_mbuilder_addOperationId(message , NIPC_OPERATION_ID_GET_SECTORS);
		message = nipc_mbuilder_addDiskSectorId(message , sector);

		nipc_messaging_send(l , message);

	}


	void nipc_sendGetSectorResponse(ListenSocket l , DiskSector diskSector, uint32_t sector , RuntimeErrorValidator * validator){

		NipcMessage message;
		message = nipc_mbuilder_addMessageType(message , NIPC_MESSAGE_TYPE_SECTOR_DISK);
		message = nipc_mbuilder_addOperationId(message , NIPC_OPERATION_ID_GET_SECTORS);
		message = nipc_mbuilder_addDiskSector(message , diskSector);

		nipc_messaging_send(l , message);

	}


