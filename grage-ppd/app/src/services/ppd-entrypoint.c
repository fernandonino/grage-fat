/*
 * ppd-entrypoint.c
 *
 *  Created on: 08/10/2011
 *      Author: gonzalo
 */

#include <unistd.h>
#include <pthread.h>

#include "grage-commons.h"
#include "linux-commons.h"
#include "linux-commons-socket.h"

#include "nipc-messaging.h"
#include "ppd-queues.h"
#include "ppd-state.h"

	void ppd_entrypoint_executePutSector(NipcMessage message);
	void ppd_entrypoint_executeGetSector(NipcMessage message);
	void ppd_entrypoint_doLunch();



	pthread_t entrypointThread;



	void ppd_entrypoint_launch(){
		pthread_create(&entrypointThread , NULL , (void * (*) (void* )) ppd_entrypoint_doLunch , NULL);
	}

	void ppd_entrypoint_executePutSector(NipcMessage message){
		ppd_queues_putInQueue(message);
	}

	void ppd_entrypoint_executeGetSector(NipcMessage message){
		ppd_queues_putInQueue(message);
	}

	void ppd_entrypoint_doLunch(){

		while(TRUE){

			printf("esperando peticiones de PRAID/PFS\n");

			NipcMessage m = nipc_messaging_receive(ppd_state_getActiveSocket());

			/*
			 * Si se desconecta el PRAID
			 */
			if(m.header.operationId == NIPC_FIELD_BLANK)
				break;

			if(m.header.operationId == NIPC_OPERATION_ID_GET_SECTORS){
				ppd_entrypoint_executeGetSector(m);
			}else if (m.header.operationId == NIPC_OPERATION_ID_PUT_SECTORS){
				ppd_entrypoint_executePutSector(m);
			}
		}

		puts("finalizando la aplicacion por desconexion del PRAID");
		close(ppd_state_getActiveSocket());
		exit(1);

	}
