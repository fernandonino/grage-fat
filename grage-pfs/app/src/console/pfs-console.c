/*
 * pfs-console.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux-commons.h>

#define NUM_THREADS     1

void *pfs_console_thread(void *argument)
{
	char buffer[250];

	while(!feof(stdin)){
		fgets(buffer, 250, stdin);
		printf(">%s", buffer);
	}
	return NULL;
}

int pfs_console_initialize(){
	Thread threads[NUM_THREADS];
	int console_thread;
	console_thread = pthread_create(&threads[NUM_THREADS], NULL, pfs_console_thread, NULL);

	//pthread_exit(NULL);
	return console_thread;

}
