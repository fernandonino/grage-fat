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

	while(comando[i] != ')' && i < (250 - cmd_length)){
		parametro[j] = comando[i];
		i++;
		j++;
	}
	return parametro;
}

uint32 ppd_console_obtenerNroParametros(char * comando){
	uint32 cant = 0;
	uint32 i=0;

	while(comando[i] !=')' && comando[i] != '\n' && i < 250 && cant < 5){
		if(comando[i] == ' ') cant++;
		i++;
	}
	if(cant > 0) return ++cant;
	else{
		puts("trace debe reecibir parametros");
		return cant;
	}
}

void ppd_console_llenarVector(char * comando, uint32 nro_parametros){
	uint32 i = 6;
	uint32 j = 0;
	uint32 k = 0;
	char aux[6];
	while(comando[i] != ')' && i < 244 && j <= nro_parametros){
		while(comando[i] != ' ' && comando[i] != ')'){
			aux[k] = comando[i];
			k++;
			i++;
		}
		k = 0;
		sectores[j] = atoi(aux);
		j++;
		i++;
	}
}

void ppd_console_info(){

}

void ppd_console_clean(uint32 clusterInicial, uint32 clusterFinal){

}

void ppd_console_trace(uint32 sectorPedido){

}

void ppd_console_interpreter(){

	char buffer[250];
	String cmd, parameter, parameter2;

	while(!feof(stdin)){
		printf(">");
		fgets(buffer, 250, stdin);
		printf(">%s\n", buffer);
		cmd = ppd_console_parseCMD(buffer);
		if(!strcmp(cmd, "info")) ppd_console_info();
		if(!strcmp(cmd, "clean")){
			parameter  = ppd_console_get_cmd_parameter(buffer, strlen(cmd));
			parameter2 = ppd_console_get_cmd_parameter(buffer, (strlen(cmd) + 1 + strlen(parameter)));
			pfs_console_clean(atoi(parameter), atoi(parameter2));
		}
		if(!strcmp(cmd, "trace")){
			uint32 nro_parametros, i = 0;
			nro_parametros = ppd_console_obtenerNroParametros(buffer);
			ppd_console_llenarVector(buffer, nro_parametros);
			while(i <= nro_parametros){
				ppd_console_trace(sectores[i]);
				i++;
			}
		}
		if(strcmp(cmd, "info") && strcmp(cmd, "clean") && strcmp(cmd, "trace")) puts("Comando incorrecto");
	}
}
