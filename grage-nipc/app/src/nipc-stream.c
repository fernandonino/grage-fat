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


	NipcMessage nipc_stream_deserializeNipcPayloadStream(NipcStream aStream , NipcHeader aHeader){
		NipcMessage message;
		message.header = aHeader;

		if(aHeader.messageType == NIPC_MESSAGE_TYPE_SECTOR_DISK){
			memcpy(&message.payload.diskSector , aStream.data , aStream.offset);
		}else if(aHeader.messageType == NIPC_MESSAGE_TYPE_SECTOR_ID){
			memcpy(&message.payload.sectorId , aStream.data , aStream.offset);
		}

		return message;
	}

	NipcStream nipc_stream_serializeNipcMessage(NipcMessage aMessage){

		NipcStream stream = nipc_stream_buildNipcStream();

		if(aMessage.header.messageType == NIPC_MESSAGE_TYPE_HANDSHAKE){
			memcpy(stream.data , &(aMessage.header) , stream.offset += sizeof(NipcHeader));
		}else if(aMessage.header.messageType == NIPC_MESSAGE_TYPE_SECTOR_DISK){
			memcpy(&stream.data , &(aMessage.header) , stream.offset += sizeof(NipcHeader));
			memcpy(&stream.data , &(aMessage.payload.diskSector) , stream.offset += sizeof(aMessage.payload.diskSector));
		}else if(aMessage.header.messageType == NIPC_MESSAGE_TYPE_SECTOR_ID){
			memcpy(&stream.data , &(aMessage.header) , stream.offset += sizeof(NipcHeader));
			memcpy(&stream.data , &(aMessage.payload.sectorId) , stream.offset += sizeof(aMessage.payload.sectorId));
		}
		return stream;
	}


