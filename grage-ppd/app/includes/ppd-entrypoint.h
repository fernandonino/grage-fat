/*
 * ppd-entrypoint.h
 *
 *  Created on: 09/10/2011
 *      Author: gonzalo
 */


#include <pthread.h>

#ifndef PPD_ENTRYPOINT_H_
#define PPD_ENTRYPOINT_H_

	void ppd_entrypoint_launch();


	pthread_t ppd_entrypoint_getEntrypointThread();

#endif /* PPD_ENTRYPOINT_H_ */
