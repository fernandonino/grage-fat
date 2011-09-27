/*
 * ppd-queues.h
 *
 *  Created on: 26/09/2011
 *      Author: gonzalo
 */

#include "linux-commons.h"

#ifndef PPD_PLANIFIER_H_
#define PPD_PLANIFIER_H_

	typedef struct {
		uint32 jobId;
		Object jobData;
	} Job;

	Job * ppd_queues_buildJob(Object aData);

	Boolean ppd_queues_isTheSameJob(Job * j1 , Job * j2);

	Object ppd_queues_pickForWrite();
	Object ppd_queues_pickForRead();
	void ppd_queues_putForRead(Object aData);
	void ppd_queues_putForWrite(Object aData);

#endif /* PPD_PLANIFIER_H_ */
