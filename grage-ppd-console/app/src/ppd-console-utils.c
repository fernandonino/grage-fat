/*
 * ppd-console-utils.c
 *
 *  Created on: Nov 12, 2011
 *      Author: seba
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <grage-commons.h>

MessageConsolePPD armarMensaje(uint32 messageID, uint32 pista, uint32 sector, uint32 timeMiliSeconds){
	MessageConsolePPD mensaje;
	mensaje.messageID=messageID;
	mensaje.pistaSector.sectorNumber=sector;
	mensaje.pistaSector.pista=pista;
	mensaje.timeInMiliseconds=timeMiliSeconds;
	return mensaje;
}
int ppd_console_utils_get_cilinder_from_sector(int sector){
	int cantidadSectoresPorCilindro;
	return (sector%cantidadSectoresPorCilindro)-1;
}
int ppd_console_utils_get_sectorofcilinder_from_sector(int sector){
	int cantidadSectoresPorCilindro;
	return sector-(sector%cantidadSectoresPorCilindro)*cantidadSectoresPorCilindro;
}
int ppd_console_utils_get_sector_from_sectorofcilinder(int sector, int cilinder){
	int cantidadSectoresPorCilindro;
	return (cilinder*cantidadSectoresPorCilindro)+sector;
}
