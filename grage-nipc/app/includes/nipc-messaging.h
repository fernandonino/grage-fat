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
#include "linux-commons-errors.h"

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
#define NIPC_PROCESS_ID_PFS						4
#define NIPC_PROCESS_ID_PPD						3




	typedef struct {

		uint32_t offset;
		char data[NIPC_MESSAGE_PAYLOAD_BUFFER_SIZE];

	} NipcStream;



	typedef struct {

		uint8_t messageType;
		uint16_t payloadLength;

		uint8_t operationId;
		uint8_t responseCode;

		uint8_t processHandshakeId;

	} __attribute__((packed)) NipcHeader;


	typedef struct {

		DiskSector diskSector;
		uint32_t pfsSocket;

	} __attribute__((packed)) NipcPayload;


	typedef struct {

		NipcHeader header;
		NipcPayload payload;

	} __attribute__((packed)) NipcMessage;




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
	NipcMessage nipc_mbuilder_addProcessId(NipcMessage , uint8_t);

	NipcStream nipc_stream_buildNipcStream();


	NipcMessage nipc_stream_deserializeNipcMessageStream(NipcStream aStream);
	NipcStream nipc_stream_serializeNipcPayload(NipcPayload aPayload);
	NipcStream nipc_stream_serializeNipcMessage(NipcMessage aMessage);
	NipcPayload nipc_stream_deserializeNipcPayloadStream(NipcStream aStream );


	void nipc_sendHandshake(ListenSocket lSocket , uint8_t processId , RuntimeErrorValidator * validator);
	NipcMessage nipc_receiveHandshake(ListenSocket lSocket , RuntimeErrorValidator * validator);
	void nipc_sendPutSectorRequest(ListenSocket l , DiskSector diskSector , RuntimeErrorValidator * validator);
	void nipc_sendGetSectorRequest(ListenSocket l , uint32_t sector , RuntimeErrorValidator * validator);
	void nipc_sendGetSectorResponse(ListenSocket l , DiskSector diskSector, uint32_t sector , RuntimeErrorValidator * validator);


#endif /* NIPC_PROTOCOL_H_ */
