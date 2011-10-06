/*
 * test-ppd-persistance.c
 *
 *  Created on: 29/09/2011
 *      Author: Fernando
 */


#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/md5.h>
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
	void test_ppd_persistance_readAndWriteSector(void);


	/* Funciones auxiliares para readAndWriteSector*/
	int compareMd5Sum(unsigned char * , unsigned char * );
	int compareFiles(char * , char * );
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

		if ( unit_testing_addToSuite(suite , "Copiando un archivo mapeado" , test_ppd_persistance_readAndWriteSector) ==NULL )
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

		char * oneFile =  ppd_persistance_mapDisk("/vfs/face-01.png");
		CU_ASSERT_PTR_NOT_NULL(oneFile);
		ppd_persistence_readSector(&testSector , oneFile);
		oneFile = ppd_persistance_unmapDisk("/vfs/face-01.png" , oneFile);
		CU_ASSERT_PTR_NULL(oneFile);

		char * anotherFile = ppd_persistance_mapDisk("/vfs/face-02.png");
		CU_ASSERT_PTR_NOT_NULL(anotherFile);
		ppd_persistence_writeSector(&testSector , anotherFile);
		anotherFile = ppd_persistance_unmapDisk("/vfs/face-02.png" , anotherFile);
		CU_ASSERT_PTR_NULL(anotherFile);

		int result = compareFiles("/vfs/face-01.png" , "/vfs/face-02.png");
		CU_ASSERT_EQUAL(result , 0);

	}


	/* Funciones auxiliares para readAndWriteSector*/

	int compareMd5Sum(unsigned char * stringOne , unsigned char * stringTwo) {
		int i;
		int compare = 0;
		for(i=0 ; i < MD5_DIGEST_LENGTH ; i++) {
			if ( stringOne[i] != stringTwo[i] )
				compare = 1;
		}
		return compare;
	}

	unsigned long get_size_by_fd(int fd) {
		struct stat statbuf;
		if(fstat(fd, &statbuf) < 0) exit(-1);
		return statbuf.st_size;
	}

	int compareFiles(char * fileOne , char * fileTwo){
		int file_descript1, file_descript2;
		unsigned long file_size1 , file_size2;
		char * file_buffer1;
		char * file_buffer2;
		unsigned char result1[MD5_DIGEST_LENGTH];
		unsigned char result2[MD5_DIGEST_LENGTH];

		file_descript1 = open(fileOne, O_RDONLY);
		if(file_descript1 < 0) exit(-1);
		file_descript2 = open(fileTwo, O_RDONLY);
		if(file_descript2 < 0) exit(-1);

		file_size1 = get_size_by_fd(file_descript1);
		file_size2 = get_size_by_fd(file_descript2);

		file_buffer1 = mmap(0, file_size1, PROT_READ, MAP_SHARED, file_descript1, 0);
		MD5((unsigned char*) file_buffer1, file_size1, result1);
		file_buffer2 = mmap(0, file_size2, PROT_READ, MAP_SHARED, file_descript2, 0);
		MD5((unsigned char*) file_buffer2, file_size2, result2);

		close(file_descript1);
		close(file_descript2);

		return compareMd5Sum(result1,result2);
	}


