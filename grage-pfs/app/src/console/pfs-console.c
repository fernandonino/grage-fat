/*
 * pfs-console.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <linux-commons.h>
#include <grage-commons.h>
#include "pfs-console-utils.h"
#include "pfs-cache.h"
#include "pfs-state.h"
#include "pfs-fat32.h"



	pthread_t fuckingThread;


	void pfs_console_fsinfo(){
		Volume * v = pfs_state_getVolume();

		printf("Cantidad de Clusters ocupados: %d \n",pfs_fat_utils_BusyClustersQuantity());
		printf("Cantidad de Clusters libres: %d\n",pfs_fat_utils_FreeClustersQuantity());
		printf("Tamaño de un Sector: %db \n",v->bps);
		printf("Tamaño de un Cluster: %db \n",v->bpc);
		printf("Tamaño de la FAT: %d kb \n", pfs_fat_utils_FATsizeKilobytes());

	}

	void pfs_console_finfo(String parameter){
		FatFile * fatFile = pfs_fat32_open(parameter);
		Volume * v = pfs_state_getVolume();
		uint32_t currentCluster = fatFile->nextCluster;

		int i = 0;
		printf("#Clusters: \n");
		while (!(FAT_32_ISEOC(currentCluster)) && i<20){
			printf("%d ",currentCluster);
			currentCluster=pfs_fat32_utils_getNextClusterInChain(v,currentCluster);
			i++;
		}
		printf("\n");
	}

	void *pfs_console_thread(void *argument)
	{
		char buffer[250];
		String cmd, parameter;

		while(1){

			if (signal(SIGINT, pfs_cache_sectores_dump) == SIG_ERR){
				perror ("No se puede cambiar signal");
			}

			fgets(buffer, 250, stdin);
			printf(">%s\n", buffer);
			cmd = pfs_console_utils_parseCMD(buffer);
			if(!strcmp(cmd,"exit")) exit(1);
			if(!strcmp(cmd, "fsinfo")) pfs_console_fsinfo();
			if(!strcmp(cmd, "finfo")){
				parameter = pfs_console_utils_get_cmd_parameter(buffer, strlen(cmd));
				pfs_console_finfo(parameter);
			}
			if(strcmp(cmd, "fsinfo") && strcmp(cmd, "finfo") && strcmp(cmd,"exit")) puts("Comando incorrecto");
		}
		return NULL;
	}

	void pfs_console_initialize(){



		pthread_create(&fuckingThread, NULL, pfs_console_thread, NULL);

	}
