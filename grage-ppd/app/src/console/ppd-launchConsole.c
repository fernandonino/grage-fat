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
#include "ppd-launchConsole.h"


#define SOCK_PATH "/opt/grage-repository/.echo_socket"




	pthread_t ppdConsoleThread;


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

			ppd_state_setPpdConsoleSocket(newfd);

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




	void ppd_launchConsole_launchSlaveThread(){

		puts("inicializando consola");
		ppd_launchConsole_startUNIX();

		puts("lanzando consola");
		pthread_create(&ppdConsoleThread , NULL , ( void *(*) (void *)) ppdConsoleServiceThread , NULL);
	}




	void ppdConsoleServiceThread(){

		while(TRUE){

			char buffer[1024];
			bzero(buffer , 1024);

			puts("recibiendo bytes de la consola");
			int receivedCount = commons_socket_receiveBytes( ppd_state_getPpdConsoleSocket() , buffer , 1024);

			//hace lo q quieras con el contenido del buffer

			commons_socket_sendBytes(ppd_state_getPpdConsoleSocket()  , buffer , 1024);
		}
	}


