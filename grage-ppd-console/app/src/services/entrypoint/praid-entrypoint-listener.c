/*
 * praid-entrypoint-listener.c
 *
 *  Created on: 06/10/2011
 *      Author: utn_so
 */



#include "linux-commons-socket.h"

	void startEntrypointListening(){

		char * port;

		ServerSocket * serverSocket = commons_socket_openServerConnection(port);

		while (TRUE){

			// recibir mensaje en serverSocket->listenSocket

			// si es un handshake del pfs guardar el estado

			// si es un handshake del ppd agregar a la lista de ppds conectados uno nuevo con el estado
			// y sincronizarlo

			// si es una respuesta de un ppd enviarla a travez del endpoint al pfs

			// si es una peticion del pfs derivarla al entrypoint correspondiente
		}


	}
