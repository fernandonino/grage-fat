/*
 * ppd_console_interpreter.c
 *
 *  Created on: 10/10/2011
 *      Author: joaquin
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux-commons.h>
#include <linux-commons-socket.h>
#include "ppd_console_launcher.h"
#include <grage-commons.h>
#include "ppd-console-utils.h"
uint32 sectores[5];

String ppd_console_parseCMD(char * comando){
	uint32 i=0;
	uint32 j=0;
	String cmd;
	cmd = (String)malloc(255);

	memset(cmd, '\0', sizeof cmd);

	while(comando[i] != '(' && comando[i] != '\n' && comando[i] != ' ' && i <= 250){
		cmd[j] = comando[i];
		i++;
		j++;
	}

	return cmd;
}

String ppd_console_get_cmd_parameter(char * comando, uint32 cmd_length){
	uint32 i=cmd_length + 1;
	uint32 j=0;
	String parametro;
	parametro = (String)malloc(255);
	memset(parametro, '\0', sizeof parametro);

	while(comando[i] != ')' && comando[i] != ' ' && i < (250 - cmd_length)
			&& comando[i] >= 48 && comando[i] <= 57)
	{
		parametro[j] = comando[i];
		i++;
		j++;
	}
	if(comando[i] >= 57 || comando[i] <= 48) puts("Valor de Cluster ingresado incorrecto");
	return parametro;
}

uint32 ppd_console_obtenerNroParametros(char * comando){
	uint32 cant = 0;
	uint32 i=0;

	while(comando[i] !=')' && comando[i] != '\n' && i < 250 && cant < 5){
		if(comando[i] == ' ') cant++;
		i++;
	}
	printf("AAAAAAAAAAa %d\n", cant);
	if(cant > 0 && cant < 5 &&comando[6] != ' ') {
		cant++;
		return cant;
	}
	if(cant == 0 && atoi(ppd_console_get_cmd_parameter(comando, strlen(ppd_console_parseCMD(comando)))) > 0){
		return 1;
	}
	if(comando[6] == ' '){
		puts("El primer parametro no debe ser vacio");
		return 0;
	}
	if (cant == 0 && atoi(ppd_console_get_cmd_parameter(comando, strlen(ppd_console_parseCMD(comando)))) <= 0){
		puts("trace debe recibir parametros");
		return 0;
	}
	if(cant >= 5){
		puts("El maximo de clusters es 5");
		return 0;
	}
	return 0;
}

void ppd_console_llenarVector(char * comando, uint32 nro_parametros){
	if(nro_parametros != 0){
		uint32 i = 6;
		uint32 j = 0;
		uint32 k = 0;
		char aux[6];
		while(comando[i] != ')' && i < 244 && j < nro_parametros){
			while(comando[i] >= 48 && comando[i] <= 57){
				aux[k] = comando[i];
				k++;
				i++;
			}
			k = 0;
			sectores[j] = atoi(aux);
			memset(aux, '\0', sizeof(aux));
			j++;
			i++;
		}
	}
}

void ppd_console_info(){
	puts("console info");
}

void ppd_console_clean(uint32 clusterInicial, uint32 clusterFinal){
	puts("console clean");
	printf("clusterInicial %d\n", clusterInicial);
	printf("clusterFinal %d\n", clusterFinal);
}

void ppd_console_trace(uint32 sectorPedido){
	char buf[1024];

	MessageConsolePPD mensaje = armarMensaje(MESSAGE_ID_POSICION_ACTUAL,
								4,
								2,
								-1);
	puts("console trace");

	commons_socket_sendBytes(ppd_console_launcher_getSocketPPD(),&mensaje,sizeof mensaje);

	int receivedCount = commons_socket_receiveBytes( ppd_console_launcher_getSocketPPD() ,&mensaje , sizeof mensaje);
	printf("ID: %d \n PISTA: %d \n SECTOR: %d \n TIEMPO: %d \n",
				mensaje.menssageID,
				mensaje.pistaSector.pista,
				mensaje.pistaSector.sectorNumber,
				mensaje.timeInMiliseconds);
	printf("sectorPedido: %d\n", sectorPedido);
}

void ppd_console_interpreter(){

	char buffer[250];

	String cmd, parameter, parameter2;

	while(!feof(stdin)){
		printf(">");
		fgets(buffer, 250, stdin);
		cmd = ppd_console_parseCMD(buffer);
		//INFO
		if(!strcmp(cmd, "info"))ppd_console_info();
		//CLEAN
		if(!strcmp(cmd, "clean") && buffer[5]!= ' '){
			if(buffer[5]=='(' && buffer[6]!=')'){
				parameter  = ppd_console_get_cmd_parameter(buffer, strlen(cmd));
				parameter2 = ppd_console_get_cmd_parameter(buffer, (strlen(cmd) + 1 + strlen(parameter)));
				if(atoi(parameter2)>0) ppd_console_clean(atoi(parameter), atoi(parameter2));
				else puts("Indique clusters a limpiar");
				bzero(parameter, sizeof(parameter));
				bzero(parameter2,sizeof(parameter2));
			}
			else puts("Indique clusters a limpiar");
		}
		//TRACE
		if(!strcmp(cmd, "trace") && buffer[5]!= ' '){
			uint32 nro_parametros;
			uint32 i = 1;
			nro_parametros = ppd_console_obtenerNroParametros(buffer);
			ppd_console_llenarVector(buffer, nro_parametros);
			while(i <= nro_parametros){
				ppd_console_trace(sectores[i - 1]);
				i++;
			}

		}

		if((strcmp(cmd, "info") && strcmp(cmd, "clean") && strcmp(cmd, "trace")) || buffer[5]== ' ')
			puts("Comando incorrecto");
		bzero(buffer, sizeof(buffer));
	}
}
