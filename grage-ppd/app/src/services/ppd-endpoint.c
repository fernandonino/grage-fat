/*
 * ppd-endpoint.c
 *
 *  Created on: 31/10/2011
 *      Author: gonzalo
 */

#include <nipc-messaging.h>

#include "ppd-state.h"

	void ppd_endpoint_responseGetSector( NipcMessage message){
		nipc_messaging_send(ppd_state_getActiveSocket() , message);
	}
