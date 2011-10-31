/*
 * nipc-stream.c
 *
 *  Created on: 04/10/2011
 *      Author: gonzalo
 */

#include "nipc-messaging.h"


	NipcStream nipc_stream_buildNipcStream(){
		NipcStream stream;
		bzero(stream.data , sizeof(stream.data));
		return stream;
	}


	NipcPayload nipc_stream_deserializeNipcPayloadStream(NipcStream aStream ){

		NipcPayload payload;
		memcpy(&payload.diskSector , aStream.data , aStream.offset);

		return payload;
	}

	NipcStream nipc_stream_serializeNipcMessage(NipcMessage aMessage){

		NipcStream stream = nipc_stream_buildNipcStream();

		memcpy(stream.data , &aMessage.header , stream.offset += sizeof(NipcHeader));

		NipcStream payloadStream = nipc_stream_serializeNipcPayload(aMessage.payload);

		char * buffer = stream.data ;
		memcpy( buffer + stream.offset , payloadStream.data , stream.offset += payloadStream.offset);

		return stream;
	}


	NipcMessage nipc_stream_deserializeNipcMessageStream(NipcStream aStream){
		NipcMessage m = nipc_mbuilder_buildNipcMessage();

		uint32 offset = 0;

		memcpy(&m.header, aStream.data , offset += sizeof(m.header));
		memcpy(&m.payload , aStream.data + offset , sizeof(m.payload));

		return m;
	}


	NipcStream nipc_stream_serializeNipcPayload(NipcPayload aPayload){
		NipcStream stream = nipc_stream_buildNipcStream();
		memcpy(stream.data , &aPayload.diskSector , stream.offset += sizeof(aPayload.diskSector));
		return stream;
	}

