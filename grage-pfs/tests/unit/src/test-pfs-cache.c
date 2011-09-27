/*
 * test-pfs-cache.c
 *
 *  Created on: Sep 27, 2011
 *      Author: seba
 */

#include <stdlib.h>

#include "linux-commons.h"
#include "linux-testing-unit.h"
#include "grage-commons.h"
#include "pfs-cache.h"

	/*
	 * Prototipo de funciones de test
	 */

	void test_pfs_cache_put_get();



	/*
	 * Realiza la configuracion de la suite
	 */
	Boolean test_pfs_cache_configurationSuite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite("Grage PFS Cache Suite" , suiteInitAndClean , suiteInitAndClean);


		if ( unit_testing_addToSuite(suite , "Obtener cluster" , test_pfs_cache_put_get ) ==NULL )
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}
	Cluster * generarClusterForTesting()
	{
		Cluster * clusterForTests;
		clusterForTests =(Cluster *)malloc(32768);

		clusterForTests->clusterNumber=69;
		strcpy(clusterForTests->sectors[0].sectorContent,"hola");
		clusterForTests->sectors[0].sectorNumber=43;
		strcpy(clusterForTests->sectors[1].sectorContent,"que");
		clusterForTests->sectors[1].sectorNumber=44;
		strcpy(clusterForTests->sectors[2].sectorContent,"tal");
		clusterForTests->sectors[2].sectorNumber=45;
		free(clusterForTests);
		return clusterForTests;
	}
	void test_pfs_cache_put_get()
	{
		Cluster * clusterPut = generarClusterForTesting();
		pfs_cache_initialize();
		pfs_cache_put(clusterPut);
		Cluster * clusterGet = pfs_cache_get(clusterPut->clusterNumber);
		CU_ASSERT_PTR_EQUAL(clusterPut,clusterGet);
	}
