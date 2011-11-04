/*
 * praid-sync.h
 *
 *  Created on: 03/11/2011
 *      Author: gonzalo
 */

#ifndef PRAID_SYNC_H_
#define PRAID_SYNC_H_


	void praid_ppd_sync_synchronizeStorage(PPDConnectionStorage * source ,
				PPDConnectionStorage * dest);

	void praid_ppd_thread_synchronize(PPDConnectionStorage * storage);

	Boolean praid_ppd_sync_isValidReplication();

#endif /* PRAID_SYNC_H_ */
