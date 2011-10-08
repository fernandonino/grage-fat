/*
 * test-ppd-example.c
 *
 *  Created on: Aug 23, 2011
 *      Author: gonzalo
 */

#include <stdlib.h>

#include "linux-commons.h"
#include "linux-testing-unit.h"

#include "nipc-messaging.h"
	/*
	 * Prototipo de funciones de test
	 */
	void test_nipc_mbuilder_addMessageType();
	void test_nipc_mbuilder_addResponseCode();
	void test_nipc_mbuilder_addDiskSector();
	void test_nipc_serializationAndDeserializationPayload();
	void test_nipc_mbuilder_addPayloadLength();
	void test_nipc_mbuilder_addOperationId();


	DiskSector test_nipc_all_mockDiskSector(char * content , uint32 s);


	/*
	 * Realiza la configuracion de la suite
	 */
	Boolean test_nipc_configureSuite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite("Grage NIPC Suite" , suiteInitAndClean , suiteInitAndClean);

		if ( unit_testing_addToSuite(suite , "nipc_mbuilder_addMessageType" , test_nipc_mbuilder_addMessageType ) ==NULL )
			return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "nipc_mbuilder_addResponseCode" , test_nipc_mbuilder_addResponseCode ) ==NULL )
			return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "nipc_mbuilder_addPayloadLength" , test_nipc_mbuilder_addPayloadLength ) ==NULL )
			return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "nipc_mbuilder_addOperationId" , test_nipc_mbuilder_addOperationId ) ==NULL )
			return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "nipc_mbuilder_addDiskSector" , test_nipc_mbuilder_addDiskSector ) ==NULL )
			return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "nipc_serializationAndDeserializationPayload" , test_nipc_serializationAndDeserializationPayload ) ==NULL )
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}




	void test_nipc_mbuilder_addMessageType(){

		NipcMessage message = nipc_mbuilder_buildNipcMessage();

		message = nipc_mbuilder_addMessageType(message , NIPC_MESSAGE_TYPE_SECTOR_DISK);

		CU_ASSERT_EQUAL(message.header.messageType , NIPC_MESSAGE_TYPE_SECTOR_DISK);

		message = nipc_mbuilder_addMessageType(message , NIPC_MESSAGE_TYPE_SECTOR_ID);

		CU_ASSERT_EQUAL(message.header.messageType , NIPC_MESSAGE_TYPE_SECTOR_ID);
	}




	void test_nipc_mbuilder_addResponseCode(){

		NipcMessage m = nipc_mbuilder_buildNipcMessage();

		m = nipc_mbuilder_addResponseCode(m , NIPC_RESPONSE_CODE_SUCCESS);

		CU_ASSERT_EQUAL(m.header.responseCode , NIPC_RESPONSE_CODE_SUCCESS);

		m = nipc_mbuilder_addResponseCode(m , NIPC_RESPONSE_CODE_ERROR);

		CU_ASSERT_EQUAL(m.header.responseCode , NIPC_RESPONSE_CODE_ERROR);
	}


	void test_nipc_mbuilder_addPayloadLength(){

		NipcMessage m ;

		m = nipc_mbuilder_addPayloadLength(m , 512);
		CU_ASSERT_EQUAL(m.header.payloadLength , 512);

		m = nipc_mbuilder_addPayloadLength(m , 345);
		CU_ASSERT_EQUAL(m.header.payloadLength , 345);

	}


	void test_nipc_mbuilder_addOperationId(){

		NipcMessage m ;

		m = nipc_mbuilder_addOperationId(m , NIPC_OPERATION_ID_GET_SECTORS);
		CU_ASSERT_EQUAL(m.header.operationId , NIPC_OPERATION_ID_GET_SECTORS);

		m = nipc_mbuilder_addOperationId(m , NIPC_OPERATION_ID_PUT_SECTORS);
		CU_ASSERT_EQUAL(m.header.operationId , NIPC_OPERATION_ID_PUT_SECTORS);
	}

	void test_nipc_mbuilder_addDiskSector(){


		char * content = "CONTENIDO DEL SECTOR";

		NipcMessage m = nipc_mbuilder_buildNipcMessage();

		m = nipc_mbuilder_addDiskSector(m , test_nipc_all_mockDiskSector(content , 416));

		CU_ASSERT_STRING_EQUAL( content , m.payload.diskSector.sectorContent);
		CU_ASSERT_EQUAL(416 , m.payload.diskSector.sectorNumber);

	}



	void test_nipc_mbuilder_addDiskSectorId(){

		uint32 sectorId = 5432;

		NipcMessage m = nipc_mbuilder_buildNipcMessage();

		m = nipc_mbuilder_addDiskSectorId(m , sectorId);

		CU_ASSERT_EQUAL(sectorId, m.payload.diskSector.sectorNumber);
	}



	void test_nipc_serializationAndDeserializationPayload(){

		char * content = "CONTENIDO DEL SECTOR";
		uint32 sectorId = 5643;

		NipcMessage m = nipc_mbuilder_buildNipcMessage();

		m = nipc_mbuilder_addMessageType(m , NIPC_MESSAGE_TYPE_SECTOR_DISK);
		m = nipc_mbuilder_addDiskSector(m , test_nipc_all_mockDiskSector(content , sectorId));

		NipcStream s = nipc_stream_serializeNipcPayload(m.payload);

		NipcPayload payload = nipc_stream_deserializeNipcPayloadStream(s );

		CU_ASSERT_STRING_EQUAL(m.payload.diskSector.sectorContent , payload.diskSector.sectorContent);
		CU_ASSERT_EQUAL(m.payload.diskSector.sectorNumber ,payload.diskSector.sectorNumber );
		CU_ASSERT_STRING_EQUAL(content ,  payload.diskSector.sectorContent);

	}







	DiskSector test_nipc_all_mockDiskSector(char * content , uint32 s){

		DiskSector d;

		d.sectorNumber = s;
		memcpy( &d.sectorContent , content , strlen(content));

		return d;
	}

