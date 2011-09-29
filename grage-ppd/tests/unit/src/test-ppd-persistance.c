/*
 * test-ppd-persistance.c
 *
 *  Created on: 29/09/2011
 *      Author: Fernando
 */


#include <stdlib.h>
#include "linux-commons-logging.h"
#include "linux-commons.h"
#include "linux-testing-unit.h"
#include "ppd-persistance.h"
	/*
	 * Prototipo de funciones de test
	 */
	void test_ppd_persistance_mapDisk(void);
	void test_ppd_persistance_unmapDisk(void);

	/*
	 * Realiza la configuracion de la suite
	 */
	Boolean test_ppd_persistanceSuite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite("Grage PPD Suite" , suiteInitAndClean , suiteInitAndClean);

		if ( unit_testing_addToSuite(suite , "Mapeando el disco en memoria" , test_ppd_persistance_mapDisk) ==NULL )
			return EXIT_FAILURE;

		if ( unit_testing_addToSuite(suite , "Haciendo unmapping del disco" , test_ppd_persistance_unmapDisk) ==NULL )
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}

	void test_ppd_persistance_mapDisk(){

		char * mapping = ppd_persistance_mapDisk("/vfs/fat32.disk.0");
		CU_ASSERT_PTR_NOT_NULL(mapping);

		mapping = ppd_persistance_mapDisk("discoQueNoExiste");
		CU_ASSERT_PTR_NULL(mapping);
	}

	void test_ppd_persistance_unmapDisk(){

		char * mapping = ppd_persistance_mapDisk("/vfs/fat32.disk.0");
		CU_ASSERT_PTR_NOT_NULL(mapping);

		mapping = ppd_persistance_unmapDisk("/vfs/fat32.disk.0" , mapping);
		CU_ASSERT_PTR_NULL(mapping);
	}
