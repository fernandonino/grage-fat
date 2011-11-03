/*
 * nipc-facade.c
 *
 *  Created on: 08/10/2011
 *      Author: gonzalo
 */

#include "nipc-messaging.h"
#include "linux-commons-errors.h"


	void nipc_sendHandshake(ListenSocket lSocket ,
			uint8_t processId ,
			RuntimeErrorValidator * validator){

		NipcMessage message = nipc_mbuilder_buildNipcMessage();

		message = nipc_mbuilder_addMessageType(message , NIPC_MESSAGE_TYPE_HANDSHAKE);
		message = nipc_mbuilder_addProcessId(message , processId);

		nipc_messaging_send(lSocket , message);

	}


	void nipc_sendPpdHandshake(ListenSocket lSocket ,
			uint8_t ppdId ,
			uint32_t sectorsCount ,
			RuntimeErrorValidator * validator){

		NipcMessage message = nipc_mbuilder_buildNipcMessage();

		message = nipc_mbuilder_addMessageType(message , NIPC_MESSAGE_TYPE_HANDSHAKE);
		message = nipc_mbuilder_addProcessId(message , NIPC_PROCESS_ID_PPD);
		message = nipc_mbuilder_addOperationId(message , ppdId);
		message = nipc_mbuilder_addResponseCode(message , sectorsCount);

		nipc_messaging_send(lSocket , message);

	}


	NipcMessage nipc_receiveHandshake(ListenSocket lSocket , RuntimeErrorValidator * validator){

		NipcMessage message = nipc_messaging_receive(lSocket);

		if(message.header.responseCode == NIPC_RESPONSE_CODE_ERROR){
			commons_errors_setError(validator , "Ha ocurrido un error en el handshake");
		}

		return message;
	}


