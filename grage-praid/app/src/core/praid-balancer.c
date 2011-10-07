/*
 * praid-queue.c
 *
 *  Created on: 06/10/2011
 *      Author: utn_so
 */


#include "praid-state.h"



	PPDConnectionStorage praid_balancer_selectStorage(){

		//debe decidir basandose en la disponibilidad de cada storage
		//cual es el mas idoneo para ser utilizado

		//el criterio de la disponibilidad debe basarse en:
		//1) si esta en uso actualmente
		//2) la cantidad de usos realizados (el que tenga menos usos es mejor candidato)
		PPDConnectionStorage unStorage;

		return unStorage;
	}
