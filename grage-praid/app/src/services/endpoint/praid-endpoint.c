/*
 * praid-pfs-endpoint.c
 *
 *  Created on: 06/10/2011
 *      Author: gonzalo
 */

#include "nipc-messaging.h"

	void praid_endpoint_pfs_responseGetSectors(ListenSocket ls , NipcMessage m){
		nipc_messaging_send(ls , m);
	}

	void praid_endpoint_ppd_sendMessage(ListenSocket ppdSocket , NipcMessage m){
		nipc_messaging_send(ppdSocket , m);
	}
