/*
 * linux-commons-socket.c
 *
 *  Created on: 29/05/2011
 *      Author: gonzalo
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "linux-commons-strings.h"
#include "linux-commons-socket.h"
#include "linux-commons-logging.h"


	ServerSocket * commons_socket_openServerConnection(char * port) {

		ServerSocket * serverSocket = (ServerSocket *) malloc(sizeof(ServerSocket));

		serverSocket->listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

		if (serverSocket->listenSocket == SOCKET_ERROR) {
			commons_logging_logError("linux-commons" , "No se pudo crear el socket con la llamada 'socket'");
			return NULL;
		}

		int on = 1;
		setsockopt(serverSocket->listenSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );

		bzero(&serverSocket->address, sizeof(struct sockaddr_in));
		serverSocket->address.sin_addr.s_addr = INADDR_ANY;
		serverSocket->address.sin_port = (in_port_t) htons(atoi(port));
		serverSocket->address.sin_family = AF_INET;

		if (bind(serverSocket->listenSocket,
				(struct sockaddr*) &serverSocket->address,
				sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
			commons_logging_logError("linux-commons" , "no se pudo bindear el socket");
			return NULL;
		}

		if (listen(serverSocket->listenSocket, 5) == SOCKET_ERROR) {
			commons_logging_logError("linux-commons" , "no se pudo poner el socket en escucha");
			return NULL;
		}

		return serverSocket;
	}


	ListenSocket commons_socket_acceptConnection(ServerSocket * serverSocket) {
		int nAddressSize = sizeof(struct sockaddr_in);
		return accept(serverSocket->listenSocket,
				(struct sockaddr*) &serverSocket->address,
				(socklen_t *) &nAddressSize);
	}


	ListenSocket commons_socket_openClientConnection(char * host , char * port ){

		if(host == NULL || port == NULL){
			commons_logging_logError("linux-commons" , "invocando 'openClientConnection' con parametros {host} o {port} en NULL");
			return INVALID_SOCKET;
		}

		struct hostent * serverHost;
		struct sockaddr_in clientSocket;
		bzero((char *) &clientSocket , sizeof(clientSocket));
		serverHost = gethostbyname(host);
		clientSocket.sin_port = htons(atoi(port));
		clientSocket.sin_family = AF_INET;
		clientSocket.sin_addr.s_addr = ((struct in_addr*)serverHost->h_addr_list[0])->s_addr;
		ListenSocket clientDescriptor = socket(AF_INET , SOCK_STREAM , IPPROTO_IP);
		int status = connect(clientDescriptor , (struct  sockaddr *)&clientSocket , sizeof(clientSocket));

		return (status == 0)? clientDescriptor : INVALID_SOCKET;
	}


	ClientConnection * commons_socket_buildClientConnection(ListenSocket l){
		ClientConnection * clientConnection = malloc(sizeof(ClientConnection));
		clientConnection->clientSocket = l;
		return clientConnection;
	}


	int commons_socket_sendBytes(ListenSocket ls , void * bytesToSend , int bytesCount){
		int allSsended = 0;
		int sended = 0;
		while(allSsended < bytesCount ){
			sended = send(ls , bytesToSend, bytesCount - allSsended , 0);
			allSsended += sended;

			if(sended <= 0)
				break;
		}

		if(commons_logging_isDebugEnabled())
			commons_logging_logDebug("linux-commons" , commons_string_concatAll(3 , "Se han enviado " ,
					commons_misc_intToString(allSsended) , " bytes"));
		return allSsended;
	}


	int commons_socket_receiveBytes(ListenSocket ls , void * bytesToReceive , int bytesCount){

		int allReceived = 0;
		int received = 0;
		while( allReceived < bytesCount ){
			received = recv(ls , bytesToReceive , bytesCount - allReceived  , 0);
			allReceived += received;

			if(received <= 0)
				break;
		}

		if(commons_logging_isDebugEnabled())
			commons_logging_logDebug("linux-commons" , commons_string_concatAll(3, "Se han recibido " ,
					commons_misc_intToString(allReceived) , " bytes") );
		return allReceived;
	}


