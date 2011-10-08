/*
 * nipc-messaging.c
 *
 *  Created on: 01/10/2011
 *      Author: gonzalo
 */


#include "nipc-messaging.h"


	void nipc_messaging_send(ListenSocket aSocket , NipcMessage aMessage){

		NipcStream aStream = nipc_stream_serializeNipcMessage(aMessage);
		commons_socket_sendBytes(aSocket , aStream.data , aStream.offset);
	}



	NipcMessage nipc_messaging_receive(ListenSocket aSocket){

		NipcHeader header;

		NipcStream stream = nipc_stream_buildNipcStream();

		commons_socket_receiveBytes(aSocket , &header , sizeof(NipcHeader));
		commons_socket_receiveBytes(aSocket , stream.data , header.payloadLength);

		stream.offset = header.payloadLength;

		NipcPayload payload = nipc_stream_deserializeNipcPayloadStream(stream );

		return nipc_mbuilder_buildNipcMessageFromHeaderAndPayload(header , payload);
	}


