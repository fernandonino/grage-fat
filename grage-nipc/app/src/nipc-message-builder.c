/*
 * nipc-handshake.c
 *
 *  Created on: 03/10/2011
 *      Author: gonzalo
 */

#include "nipc-messaging.h"

	NipcMessage nipc_mbuilder_buildNipcMessage(){
		NipcMessage builder ;
		return builder;
	}

	NipcMessage nipc_mbuilder_buildNipcMessageFromHeader(NipcHeader header){
		NipcMessage builder ;
		builder.header = header;
		return builder;
	}

	NipcMessage nipc_mbuilder_buildNipcMessageFromHeaderAndPayload(NipcHeader header , NipcPayload payload){
		NipcMessage builder ;
		builder.payload = payload;
		builder.header = header;
		return builder;
	}

	NipcMessage nipc_mbuilder_addMessageType(NipcMessage aMessage , uint8_t aType){
		aMessage.header.messageType = aType;
		return aMessage;
	}

	NipcMessage nipc_mbuilder_addResponseCode(NipcMessage aMessage , uint8_t aCode){
		aMessage.header.responseCode = aCode;
		return aMessage;
	}

	NipcMessage nipc_mbuilder_addPayloadLength(NipcMessage aMessage , uint16_t lenth){
		aMessage.header.payloadLength = lenth;
		return aMessage;
	}

	NipcMessage nipc_mbuilder_addOperationId(NipcMessage aMessage , uint8_t operationId){
		aMessage.header.operationId = operationId;
		return aMessage;
	}

	NipcMessage nipc_mbuilder_addDiskSector(NipcMessage aMessage , DiskSector aSector){
		aMessage.payload.diskSector = aSector;
		return aMessage;
	}

	NipcMessage nipc_mbuilder_addDiskSectorId(NipcMessage aMessage , uint32_t sectorId){
		aMessage.payload.diskSector.sectorNumber = sectorId;
		return aMessage;
	}

	NipcMessage nipc_mbuilder_addDiskSectorContent(NipcMessage aMessage , char * aContent , uint16_t aLength){
		memcpy( aMessage.payload.diskSector.sectorContent , aContent , aLength);
		return aMessage;
	}



