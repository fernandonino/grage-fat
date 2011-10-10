/*
 * ppd-planifier.h
 *
 *  Created on: 09/10/2011
 *      Author: gonzalo
 */

#include <pthread.h>

#ifndef PPD_PLANIFIER_H_
#define PPD_PLANIFIER_H_

	pthread_t ppd_planifier_worker_getReadingThread();
	pthread_t ppd_planifier_worker_getWritingThread();


	void ppd_planifier_worker_doJobs();



#endif /* PPD_PLANIFIER_H_ */
