/*
 * ppd-utils.c
 *
 *  Created on: 05/09/2011
 *      Author: EL Seba
 */
#include <stdint.h>
#include "linux-commons-logging.h"



	//temporales para tenerlas hechas, no se usan todavia
	int ppd_utils_get_cilinder_from_sector(int sector){
		int cantidadSectoresPorCilindro;
		return (sector%cantidadSectoresPorCilindro)-1;
	}
	int ppd_utils_get_sectorofcilinder_from_sector(int sector){
		int cantidadSectoresPorCilindro;
		return sector-(sector%cantidadSectoresPorCilindro)*cantidadSectoresPorCilindro;
	}
	int ppd_utils_get_sector_from_sectorofcilinder(int sector, int cilinder){
		int cantidadSectoresPorCilindro;
		return (cilinder*cantidadSectoresPorCilindro)+sector;
	}
