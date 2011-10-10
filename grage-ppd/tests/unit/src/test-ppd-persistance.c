/*
 * test-ppd-persistance.c
 *
 *  Created on: 29/09/2011
 *      Author: Fernando
 */


#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>



#include "linux-commons-logging.h"
#include "linux-commons.h"
#include "linux-testing-unit.h"
#include "ppd-persistance.h"

	/*
	 * Prototipo de funciones de test
	 */
	void test_ppd_persistance_mapDisk(void);
	void test_ppd_persistance_unmapDisk(void);


	/* Funciones auxiliares para readAndWriteSector*/
	unsigned long get_size_by_fd(int);

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

	void test_ppd_persistance_readAndWriteSector(void){

		DiskSector testSector;
		testSector.sectorNumber = 0;

		char * oneFile =  ppd_persistance_mapDisk("../resources/face-01.png");
		CU_ASSERT_PTR_NOT_NULL(oneFile);
		ppd_persistence_readSector(&testSector , oneFile);
		oneFile = ppd_persistance_unmapDisk("../resources/face-01.png" , oneFile);
		CU_ASSERT_PTR_NULL(oneFile);

		char * anotherFile = ppd_persistance_mapDisk("../resources/face-02.png");
		CU_ASSERT_PTR_NOT_NULL(anotherFile);
		ppd_persistence_writeSector(&testSector , anotherFile);
		anotherFile = ppd_persistance_unmapDisk("../resources/face-02.png" , anotherFile);
		CU_ASSERT_PTR_NULL(anotherFile);


	}


	/* Funciones auxiliares para readAndWriteSector*/


	unsigned long get_size_by_fd(int fd) {
		struct stat statbuf;
		if(fstat(fd, &statbuf) < 0) exit(-1);
		return statbuf.st_size;
	}

