/*
 * ppd-state.h
 *
 *  Created on: 08/10/2011
 *      Author: gonzalo
 */


#include "grage-commons.h"
#include "linux-commons.h"
#include "linux-commons-socket.h"


#ifndef PPD_STATE_H_
#define PPD_STATE_H_

	void ppd_state_setPraidSocket(ListenSocket ls);
	ListenSocket ppd_state_getPraidSocket();

	void ppd_state_setPfsConnection(ServerSocket * s);
	ServerSocket * ppd_state_getPfsConnection();

	ListenSocket ppd_state_getActiveSocket();

#endif /* PPD_STATE_H_ */
