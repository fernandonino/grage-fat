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
#include <linux-commons-socket.h>
#include <grage-commons.h>
#include "ppd-configuration.h"
#include "ppd-console-entreypoint.h"
#include "ppd-launchConsole.h"
#include "ppd-state.h"
#include "ppd-queues.h"
#include "ppd-planifier.h"
#include "ppd-utils.h"

#define SOCK_PATH "/opt/grage-repository/.echo_socket"

pthread_t ppdConsoleThread;




	pid_t childProcessId = -1;

	void ppd_launch_console_setChildProcessId(pid_t p){
		childProcessId = p;
	}
	pid_t ppd_launch_console_getChildProcessId(){
		return childProcessId;
	}


	void ppd_launchConsole_startUNIX() {

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
		if (bind(s, (struct sockaddr *) &local, len) == -1) {
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

		if ((newfd = accept(s, (struct sockaddr *) &remote, &t)) == -1) {
			perror("accept");
			exit(1);
		}
		if (newfd == -1) {
			puts("error de conexion");
			exit(1);
		} else {
			//printf("conexion establecida.\n>");

			ppd_state_setPpdConsoleSocket(newfd);

		}
	}

	uint32 ppd_launchConsole_initialize() {

		char *args[0];

		pid_t pid = fork();
		if (pid == -1) {
			printf("No se pudo fork()ear la consola");
			exit(1);
		}
		if (pid == 0) {
			//puts("Iniciando consola");
			uint32 code = execv("/opt/grage-repository/lib/grage-ppd-console", args);
			if (code == -1) {
				puts("No se pudo levantar la consola.");
				puts("Error lanzando /opt/grage-repository/lib/grage-ppd-console.");
				exit(1);
			}

			ppd_launch_console_setChildProcessId(pid);

		}

		return 0;
	}

	void ppd_launchConsole_launchSlaveThread() {

		//puts("Inicializando consola");
		ppd_launchConsole_startUNIX();

		//puts("lanzando consola");
		pthread_create(&ppdConsoleThread, NULL,
				(void *(*)(void *)) ppdConsoleServiceThread, NULL);
	}

	void ppdConsoleServiceThread() {
		puts("[ Recibiendo bytes de la consola ]");
		MessageConsolePPD mensaje;
		PistaSector posicionCabezal;
		ppd_console_entrypoint_setearPosicionCabezal(0, 0);
		while (TRUE) {

			char buffer[1024];
			bzero(buffer, 1024);

			if (commons_socket_receiveBytes(ppd_state_getPpdConsoleSocket(), &mensaje,sizeof mensaje)==0){
				break;
			}

			if (mensaje.messageID == MESSAGE_ID_SECTORES_POR_CILINDRO) {
				mensaje.pistaSector.sectorNumber = atoi(getPpdDiskSector());
			}

			if (mensaje.messageID == MESSAGE_ID_POSICION_ACTUAL) {

				posicionCabezal = ppd_console_entrypoint_getPosicionCabezal();
				mensaje.pistaSector.pista = posicionCabezal.pista;
				mensaje.pistaSector.sectorNumber = posicionCabezal.sectorNumber;

			}
			if (mensaje.messageID == MESSAGE_ID_SECTORES_RECORRIDOS) {
				Queue listQueues ;

				listQueues = ppd_queues_getJobsQueue();
				Iterator * queues = commons_iterator_buildIterator(listQueues);
				Job * queue = commons_iterator_next(queues);
				Job * newJob = (Job *) malloc (sizeof (Job));
				newJob->sectorId=ppd_utils_get_sector_from_sectorofcilinder(mensaje.pistaSector.sectorNumber,mensaje.pistaSector.pista);
				if(commons_string_equals(getPpdAlgoritmo() , "sstf") && commons_iterator_hasMoreElements(queues)){
					while (ppd_alg_planif_strategy_sstf(newJob,	queue) != TRUE){
						listQueues = ppd_queues_getJobsQueue();
						Iterator * queues = commons_iterator_buildIterator(listQueues);
						Job * queue = commons_iterator_next(queues);
						Job * newJob = (Job *) malloc (sizeof (Job));
						newJob->sectorId=ppd_utils_get_sector_from_sectorofcilinder(mensaje.pistaSector.sectorNumber,mensaje.pistaSector.pista);
					}
					if ((mensaje.timeInMiliseconds = ppd_console_entrypoint_TiempoConsumido(
									mensaje.pistaSector.pista,
									mensaje.pistaSector.sectorNumber))==-1){
						mensaje.messageID = MESSAGE_ID_ERROR;
					}else{
						mensaje.messageID = MESSAGE_ID_TIEMPO_CONSUMIDO;
					}
				}else{
					if ((mensaje.timeInMiliseconds = ppd_console_entrypoint_TiempoConsumido(
							mensaje.pistaSector.pista,
							mensaje.pistaSector.sectorNumber))==-1){
						mensaje.messageID = MESSAGE_ID_ERROR;
					}else{
						mensaje.messageID = MESSAGE_ID_TIEMPO_CONSUMIDO;
					}
				}
				free(newJob);
			}

			if (mensaje.messageID == MESSAGE_ID_CLEAN_SECTORS){

			}
			//hace lo q quieras con el contenido del buffer

			commons_socket_sendBytes(ppd_state_getPpdConsoleSocket(), &mensaje,
					sizeof mensaje);

		}
	}
