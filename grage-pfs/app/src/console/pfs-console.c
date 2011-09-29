/*
 * pfs-console.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux-commons.h>
#include "pfs-console-utils.h"



void pfs_console_fsinfo(){

}

void pfs_console_finfo(String parameter){

}

void *pfs_console_thread(void *argument)
{
	char buffer[250];
	String cmd, parameter;

	while(!feof(stdin)){
		fgets(buffer, 250, stdin);
		printf(">%s\n", buffer);
		cmd = pfs_console_utils_parseCMD(buffer);
		if(!strcmp(cmd, "fsinfo")) pfs_console_fsinfo();
		if(!strcmp(cmd, "finfo")){
			parameter = pfs_console_utils_get_cmd_parameter(buffer, strlen(cmd));
			pfs_console_finfo(parameter);
		}
		if(strcmp(cmd, "fsinfo") && strcmp(cmd, "finfo")) puts("Comando incorrecto");
	}
	return NULL;
}

int pfs_console_initialize(){
	Thread threads[NUM_THREADS];
	int console_thread;
	console_thread = pthread_create(&threads[NUM_THREADS], NULL, pfs_console_thread, NULL);

	//pthread_exit(NULL);	SI SE DESCOMENTA NO PASA EL TEST
	return console_thread;

}
