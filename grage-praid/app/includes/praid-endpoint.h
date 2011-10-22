/*
 * praid-endpoint.h
 *
 *  Created on: 10/10/2011
 *      Author: gonzalo
 */

#ifndef PRAID_ENDPOINT_H_
#define PRAID_ENDPOINT_H_

	void praid_endpoint_pfs_responseGetSectors(ListenSocket ls , NipcMessage m);
	void praid_endpoint_ppd_sendMessage(ListenSocket ppdSocket , NipcMessage m);

#endif /* PRAID_ENDPOINT_H_ */
