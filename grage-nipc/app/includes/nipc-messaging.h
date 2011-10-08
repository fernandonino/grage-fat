/*
 * nipc-protocol.h
 *
 *  Created on: 01/10/2011
 *      Author: gonzalo
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "grage-commons.h"
#include "linux-commons-socket.h"


#ifndef NIPC_PROTOCOL_H_
#define NIPC_PROTOCOL_H_

#define NIPC_MESSAGE_PAYLOAD_BUFFER_SIZE		1024

#define NIPC_MESSAGE_TYPE_HANDSHAKE 			0
#define NIPC_MESSAGE_TYPE_SECTOR_ID				'I'
#define NIPC_MESSAGE_TYPE_SECTOR_DISK			'D'
#define NIPC_RESPONSE_CODE_SUCCESS				0
#define NIPC_RESPONSE_CODE_ERROR				1
#define NIPC_OPERATION_ID_PUT_SECTORS			'P'
#define NIPC_OPERATION_ID_GET_SECTORS			'G'





	typedef struct {

		uint32_t offset;
		char data[NIPC_MESSAGE_PAYLOAD_BUFFER_SIZE];

	} NipcStream;



	typedef struct {

		uint8_t messageType;
		uint16_t payloadLength;

		uint8_t operationId;
		uint8_t responseCode;

	} NipcHeader;


	typedef struct {

		DiskSector diskSector;

	} NipcPayload;


	typedef struct {

		NipcHeader header;
		NipcPayload payload;

	} NipcMessage;




	void nipc_messaging_send(ListenSocket aSocket , NipcMessage aMessage);
	NipcMessage nipc_messaging_receive(ListenSocket aSocket);



	NipcMessage nipc_mbuilder_buildNipcMessage();
	NipcMessage nipc_mbuilder_buildNipcMessageFromHeader(NipcHeader header);
	NipcMessage nipc_mbuilder_buildNipcMessageFromHeaderAndPayload(NipcHeader header , NipcPayload payload);
	NipcMessage nipc_mbuilder_addMessageType(NipcMessage aMessage , uint8_t aType);
	NipcMessage nipc_mbuilder_addResponseCode(NipcMessage aMessage , uint8_t aCode);
	NipcMessage nipc_mbuilder_addPayloadLength(NipcMessage , uint16_t lenth);
	NipcMessage nipc_mbuilder_addOperationId(NipcMessage , uint8_t);
	NipcMessage nipc_mbuilder_addDiskSector(NipcMessage aMessage , DiskSector aSector);
	NipcMessage nipc_mbuilder_addDiskSectorId(NipcMessage aMessage , uint32_t sectorId);
	NipcMessage nipc_mbuilder_addDiskSectorContent(NipcMessage aMessage , char * aContent , uint16_t aLength);


	NipcStream nipc_stream_buildNipcStream();


	NipcMessage nipc_stream_deserializeNipcMessageStream(NipcStream aStream);
	NipcStream nipc_stream_serializeNipcPayload(NipcPayload aPayload);
	NipcStream nipc_stream_serializeNipcMessage(NipcMessage aMessage);
	NipcPayload nipc_stream_deserializeNipcPayloadStream(NipcStream aStream );



#endif /* NIPC_PROTOCOL_H_ */