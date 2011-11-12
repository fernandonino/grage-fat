/*
 * ppd-console-entrypoint.c
 *
 *  Created on: Nov 3, 2011
 *      Author: seba
 */
#include <string.h>
#include <stdio.h>
#include "grage-commons.h"
#include "ppd-configuration.h"
#include "ppd-utils.h"

PistaSector posicionActual;
MessageConsolePPD messageConsolePPD;
PistaSector ppd_console_entrypoint_getPosicionCabezal(){
	return posicionActual;
}
void ppd_console_entrypoint_setearPosicionCabezal(uint32 pista, uint32 sector){
	posicionActual.pista=pista;
	posicionActual.sectorNumber=sector;
}
uint32 nuevoSectorActual(float sector, float ms, uint32 cantidadSectoresPista){
	uint32 sectorFinal;
	uint32 sectoresMovidos;
	float  sectoresPostaMovidos;

	sectoresMovidos = (ms / atoi(getPpdReadTimeMs())) + 1;
	sectoresPostaMovidos = sectoresMovidos % cantidadSectoresPista;
	sectorFinal = sector + sectoresPostaMovidos;
	if (sectorFinal >= cantidadSectoresPista)
	{
		return sectorFinal - cantidadSectoresPista;
	}
	else
	{
		return sectorFinal;
	}

}
void ppd_console_entrypoint_TiempoConsumido(uint32 pistaSolicitada, uint32 sectorSolicitado){

	uint32 sectorActual = posicionActual.sectorNumber;
	uint32 pistaActual=posicionActual.pista;
	//char * a = (char *)malloc(3);
	//a=getPpdDiskSector();
	uint32 cantidadSectoresPista = atoi(getPpdDiskSector());
	uint32 costoPista = atoi(getPpdSaltoPistaMs());
	uint32 costoSector =60*1000/atoi(getPpdRpm())/cantidadSectoresPista;
	float tiempoPistas = 0;
	float tiempoSectores = 0;
	float tiempoTotal;

	while (!(pistaActual == pistaSolicitada))
	{
		if (pistaActual < pistaSolicitada)
		{
			pistaActual++;
		}
		else
		{
			pistaActual--;
		}
		sectorActual = nuevoSectorActual(sectorActual,costoPista,cantidadSectoresPista);
		tiempoPistas = costoPista + tiempoPistas;
	}
	while (sectorActual != sectorSolicitado)
	{
		sectorActual = sectorActual + 1;
		if (sectorActual == cantidadSectoresPista)
		{
			sectorActual = 0;
		}
		printf("%d:%d ",pistaActual, sectorActual);
		tiempoSectores = tiempoSectores + costoSector;
	}

	tiempoTotal = tiempoPistas + tiempoSectores;
	printf("\nTiempo Consumido: %f ms\n\n" , tiempoTotal);
}


