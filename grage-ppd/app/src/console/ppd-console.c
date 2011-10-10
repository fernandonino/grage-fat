/*
 * ppd-console.c
 *
 *  Created on: 10/10/2011
 *      Author: joaquin
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "linux-commons.h"

uint32 ppd_console_initialize(){

	pid_t  pid;
	char *args[0];
	pid = fork();
	if (pid == -1){
		printf("No se pudo fork()ear la consola");
	}
	if (pid == 0){
		{
    	  int code = execv("/opt/grage-repository/lib/grage-ppd-console", args);
		}
	}
	return 0;
}
