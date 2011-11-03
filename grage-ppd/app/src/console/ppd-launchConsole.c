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
#include <sys/socket.h>
#include <sys/un.h>
#include "linux-commons.h"
#include "ppd-configuration.h"
#include "ppd-console-entreypoint.h"
#define SOCK_PATH "/opt/.echo_socket"

void ppd_launchConsole_startUNIX(){

	int s, len, newfd, t;
	struct sockaddr_un local, remote;

	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		system("pause");
		exit(1);
	}

	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, SOCK_PATH);
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);
	if (bind(s, (struct sockaddr *)&local, len) == -1) {
		perror("bind");
		system("pause");
		exit(1);
	}
	if (listen(s, 5) == -1) {
		perror("listen");
		system("pause");
		exit(1);
	}

	t = sizeof(remote);

	if ((newfd = accept(s, (struct sockaddr *)&remote, &t)) == -1) {
		perror("accept");
		exit(1);
	}
	if(newfd == -1){
		puts("error de conexion");
	}
	else{
		printf("conexion establecida.\n>");
	}
}

uint32 ppd_launchConsole_initialize(){

	pid_t  pid;
	char *args[0];

	pid = fork();
	if (pid == -1){
		printf("No se pudo fork()ear la consola");
	}
	if (pid == 0){
		{
			puts("Iniciando consola");
			uint32 code = execv("/opt/grage-repository/lib/grage-ppd-console", args);
			if(code == -1){
				puts("No se pudo levantar la consola");
			}else{ ppd_console_entrypoint_setearPosicionCabezal(0,0); }
		}
	}
	return 0;
}
