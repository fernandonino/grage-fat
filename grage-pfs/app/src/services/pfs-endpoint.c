/*
 * pfs-endpoint.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

#include <linux-commons-errors.h>

#include "nipc-messaging.h"

#include "pfs-endpoint.h"
#include "pfs-state.h"






	ListenSocket pfs_endpoint_doHandshake(){

		char * host = pfs_configuration_getDeviceAddress();
		char * port = pfs_configuration_getDevicePort();

		ListenSocket dataSocket = commons_socket_openClientConnection(host , port);

		nipc_sendHandshake(dataSocket , NIPC_PROCESS_ID_PFS);

		NipcMessage message =  nipc_receiveHandshake(dataSocket);

		if(message.header.responseCode == NIPC_RESPONSE_CODE_ERROR){
			puts("Fallo el handshake");
			exit(1);
		}

		return dataSocket;
	}




	void pfs_endpoint_callPutSector( DiskSector diskSector){

		ListenSocket ds = pfs_endpoint_doHandshake();

		NipcMessage message = nipc_mbuilder_buildNipcMessage();
		message = nipc_mbuilder_addOperationId(message , NIPC_OPERATION_ID_PUT_SECTORS);
		message = nipc_mbuilder_addDiskSector(message , diskSector);

		nipc_messaging_send(ds , message );

		close(ds);
	}


	DiskSector pfs_endpoint_callGetSector(uint32_t sectorNumber){

		ListenSocket ds = pfs_endpoint_doHandshake();

		DiskSector diskSector;

		NipcMessage message = nipc_mbuilder_buildNipcMessage();
		message = nipc_mbuilder_addOperationId(message , NIPC_OPERATION_ID_GET_SECTORS);
		message = nipc_mbuilder_addDiskSectorId(message , sectorNumber);

		nipc_messaging_send(ds , message);

		message = nipc_messaging_receive(ds);

		close(ds);

		return message.payload.diskSector;
	}


	char * ppd_persistance_mapDisk(char * diskId){
		struct stat filestat;

		int32_t fd =  open(diskId , O_RDWR);
		if (fd == -1){
			perror("Error en open del disco");
			return NULL;
		}

		if ( fstat(fd , &filestat) ){
			perror("fstat - error al obtener atributos del disco");
			return NULL;
		}

		char * map = mmap((caddr_t)0 , filestat.st_size , PROT_WRITE , MAP_SHARED , fd , 0);
		if( map == MAP_FAILED ){
			perror("Error al mapear el disco");
			return NULL;
		}

		if( posix_madvise(map , filestat.st_size , POSIX_MADV_SEQUENTIAL) ){
			perror("Error en posix_madvise");
			return NULL;
		}

		close(fd);

		return map;
	}



	char * ppd_persistance_unmapDisk(char * diskId , char * mapping){
		struct stat filestat;

		int32_t fd =  open(diskId , O_RDONLY);
		if (fd == -1){
			perror("Error en open del disco");
		}

		if ( fstat(fd , &filestat) ){
			perror("fstat - error al obtener atributos del disco");
		}

		msync(mapping , filestat.st_size , MS_SYNC );

		if( munmap(mapping , filestat.st_size) ){
			perror("Error en unmapping.");
		}

		close(fd);

		return NULL;
	}



	char * diskStartAddress;

	char * ppd_state_getDiskStartAddress(){
		return diskStartAddress;
	}

	void ppd_state_setDiskStartAddress(char * anAddress){
		diskStartAddress = anAddress;
	}

	void ppd_initializeDisk(){
		ppd_state_setDiskStartAddress( ppd_persistance_mapDisk("/vfs/fat32.disk.1") );
	}


	/*
	DiskSector pfs_endpoint_callGetSector(uint32_t sectorNumber){

		if(commons_errors_hasError(v)){
			puts(v->errorDescription);
		}
		DiskSector d;
		return d;
	}

	*/
