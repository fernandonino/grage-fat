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

	NipcMessage nipc_mbuilder_addMessageType(NipcMessage aMessage , uint8_t aType){
		aMessage.header.messageType = aType;
		return aMessage;
	}

	NipcMessage nipc_mbuilder_addResponseCode(NipcMessage aMessage , uint8_t aCode){
		aMessage.header.errorCode = aCode;
		return aMessage;
	}

	NipcMessage nipc_mbuilder_addDiskSector(NipcMessage aMessage , DiskSector aSector){
		aMessage.payload.diskSector = aSector;
		return aMessage;
	}

	NipcMessage nipc_mbuilder_addSectorId(NipcMessage aMessage , uint32_t sectorId){
		aMessage.payload.sectorId = sectorId;
		return aMessage;
	}



