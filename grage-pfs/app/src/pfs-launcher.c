/*
 * pfs-launcher.c
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo-joaco
 */
#include <time.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <linux-commons-logging.h>
#include <linux-commons-socket.h>
#include <linux-commons-errors.h>
#include <linux-commons-list.h>
#include <linux-commons.h>

#include "nipc-messaging.h"

#include "pfs-endpoint.h"
#include "pfs-configuration.h"
#include "pfs-console.h"
#include "pfs-state.h"
#include "pfs-fat32.h"
#include "pfs-fuse.h"

void createTmpFile(uint16_t, uint32_t);
void estoyProbandoComoMierdaSeLeeUnArchivo(Volume * , FatFile * , uint16_t , uint32_t);

void launch_pfs_tests(void);




	void pfs_launcher_launchConnections(){

		char * host = pfs_configuration_getDeviceAddress();
		char * port = pfs_configuration_getDevicePort();

		ListenSocket dataSocket = commons_socket_openClientConnection(host , port);

		pfs_state_setDataSocket(dataSocket);

		nipc_sendHandshake(dataSocket , NIPC_PROCESS_ID_PFS);

		NipcMessage message =  nipc_receiveHandshake(pfs_state_getDataSocket());

		if(message.header.responseCode == NIPC_RESPONSE_CODE_ERROR){
			puts("Fallo el handshake");
			exit(1);
		}

	}



	void pfs_launcher_initialize() {
		log_create("pfs", "/opt/grage-repository/logs/grage-pfs.log",
				INFO , M_CONSOLE_DISABLE);
		pfs_configuration_setConfigurationFile(PFS_DEFAULT_CONFIGURATION_FILE);
		pfs_configuration_initialize();

		pfs_launcher_launchConnections();
		//pfs_console_initialize();
		//ppd_initializeDisk();
	}





	void pfs_launcher_initializeBPB(){
		DiskSector sector = pfs_endpoint_callGetSector(0);

		BPB biosParameterBlock;

		memcpy(&biosParameterBlock , sector.sectorContent , sizeof(BPB));

		pfs_state_setBiosParameterBlock(biosParameterBlock);

		Volume * volume = pfs_fat_utils_loadVolume(&biosParameterBlock);

		if(volume == NULL){
			puts("El volumen es invalido");
			exit(1);
		}

		pfs_state_setVolume(volume);
	}





	void pfs_launcher_launch(int argc, char *argv[]) {


		pfs_launcher_initializeBPB();

		pfs_fuse_launchFuse(argc,argv);

		//launch_pfs_tests();

	}

	void pfs_launcher_exit() {
		log_destroy();
	}

	int main(int argc, char *argv[]) {

		pfs_launcher_initialize();

		pfs_launcher_launch(argc , argv);

		pfs_launcher_exit();


		return EXIT_SUCCESS;
	}

	void launch_pfs_tests(void){
		Volume * v = pfs_state_getVolume();
		FatFile * file = pfs_fat32_open("/face-01.png");
		FatFile * directory = pfs_fat32_open("/");
		struct dirent de;
		struct stat st;
		int16_t result;


		/* Modificando la fecha y hora del archivo "file"

		time_t currentTime = time(NULL);

		pfs_fat32_utils_fillTime( &(file->shortEntry.DIR_CrtDate) , &(file->shortEntry.DIR_CrtTime) , currentTime);
		pfs_fat32_utils_fillTime( &(file->shortEntry.DIR_WrtDate) , &(file->shortEntry.DIR_WrtTime) , currentTime);

		uint32_t sectorNumber = pfs_fat_utils_getFirstSectorOfCluster(v , file->source);
		DiskSector sector = pfs_endpoint_callGetSector(sectorNumber);
		memcpy(sector.sectorContent + file->sourceOffset + 32 , &(file->shortEntry) , FAT_32_DIR_ENTRY_SIZE);

		pfs_endpoint_callPutSector(sector);

		*/


		/* Listando el directorio "directory" */

		while( (result = pfs_fat32_readDirectory(&de , directory , v)) == 0 ){
			memset(&st, 0, sizeof(st));
			st.st_ino = de.d_ino;
			if (de.d_type == DT_DIR) {
				st.st_mode = S_IFDIR;
			} else st.st_mode = S_IFREG;

			printf("%s\n" , de.d_name);
		}


		/* Borrando un archivo y un directorio "file" y "directory", respectivamente */
/*
		pfs_fat32_unlink(v,file);

		if (directory == NULL || directory->shortEntry.DIR_Attr != FAT_32_ATTR_DIRECTORY){
			puts("Directorio no exite o no es un directorio.");
		}
		if(pfs_fat32_isDirectoryEmpty(v, directory)){
			pfs_fat32_rmdir(v,directory);
			puts("Directorio borrado para el CodeMaster que lo mira por TV.");
		} else {
			puts("El directorio no esta vacio.");
		}
*/


		/* Borrando un archivo y un directorio "file" y "directory", respectivamente */
		//estoyProbandoComoMierdaSeLeeUnArchivo(v , file , 512, 265044);

		commons_misc_doFreeNull((void **)file);
		commons_misc_doFreeNull((void **)directory);
	}

	void createTmpFile(uint16_t size, uint32_t offset){

		char buffer[size];
		int photo = open("/vfs/face-01.png" , O_RDONLY);
		lseek(photo, offset, SEEK_SET);
		read(photo , buffer , size);
		FILE * forro = fopen("/vfs/face-01-1024.png" , "w");
		fwrite(buffer , 1 , size , forro);
		close(photo);
		fclose(forro);
	}

	void estoyProbandoComoMierdaSeLeeUnArchivo(Volume * v , FatFile * file , uint16_t size, uint32_t offset){

		createTmpFile(size, offset);

		char buffer[size];

		uint32_t filesize = file->shortEntry.DIR_FileSize;
		if ( (offset + size) > filesize )
			size = filesize - offset;

		uint16_t result = pfs_fat32_utils_seek(v , file , offset , filesize);
		if ( result == EXIT_FAILURE )
			return;

		uint16_t read = pfs_fat32_read(v , file , buffer , size);
		printf("Se leyeron %d\n" , read);

		FILE * forro = fopen("/vfs/face-01-1024-test.png" , "w");
		fwrite(buffer , 1 , size , forro);
		fclose(forro);
	}
