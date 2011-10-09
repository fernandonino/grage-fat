/*
 * ppd-entrypoint.c
 *
 *  Created on: 08/10/2011
 *      Author: gonzalo
 */


#include <pthread.h>

#include "grage-commons.h"
#include "linux-commons.h"
#include "linux-commons-socket.h"

#include "nipc-messaging.h"

	void ppd_entrypoint_executePutSector(NipcMessage message);
	void ppd_entrypoint_executeGetSector(NipcMessage message);



	void ppd_entrypoint_launch(){

		while(TRUE){

			NipcMessage m = nipc_messaging_receive(ppd_state_getActiveSocket());

			if(m.header.operationId == NIPC_OPERATION_ID_GET_SECTORS){
				ppd_entrypoint_executeGetSector(m);
			}else if (m.header.operationId == NIPC_OPERATION_ID_PUT_SECTORS){
				ppd_entrypoint_executePutSector(m);
			}
		}
	}



	void ppd_entrypoint_executePutSector(NipcMessage message){
		ppd_queues_putForWrite(message);
	}

	void ppd_entrypoint_executeGetSector(NipcMessage message){
		ppd_queues_putForRead(message);
	}
