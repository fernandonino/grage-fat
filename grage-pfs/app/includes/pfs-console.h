/*
 * pfs-console.h
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */

#ifndef PFS_CONSOLE_H_
#define PFS_CONSOLE_H_

#define NUM_THREADS     1

	int pfs_console_initialize();
	void * pfs_console_thread(void *thread_init_parameter);

#endif /* PFS_CONSOLE_H_ */
