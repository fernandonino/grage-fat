/*
 * test-linux-commons-file.c
 *
 *  Created on: 22/08/2011
 *      Author: gonzalo
 */


#include <stdlib.h>
#include "linux-commons.h"
#include "linux-commons-file.h"
#include "linux-commons-strings.h"
#include "linux-commons-list.h"

#include "linux-testing-unit.h"

	String nameFile = "/opt/grage-repository/hudson/reports/archivo.txt";
	String nameDirectory = "/opt/grage-repository/hudson/reports/directorio.txt";

	void test_commons_file_createRemoveDirectory(void);
	void test_commons_file_existsOpenRemoveCreateFile(void);
	void test_commons_file_createStartNameFile(void);
	void test_commons_file_workWithConfFile(void);
	void test_commons_file_getKey(void);
	void test_commons_file_getValue(void);
	void test_commons_file_parseKeyAndValueFile(void);
	void test_commons_file_parseFileAndModifyValueForKey(void);
	void test_commons_file_updateEntry(void);
	void test_commons_file_createOrTruncateFile(void);
	void test_commons_file_insertLine(void);
	//void test_commons_file_getDirectoryFiles(void);

	/*
	 * Realiza la configuracion de la suite
	 */
	Boolean linux_commons_file_configureSuite(){

		int suiteInitAndClean(){
			return EXIT_SUCCESS;
		}

		TestSuite suite = unit_testing_createSuite(
				"Suite Linux Commons File" , suiteInitAndClean , suiteInitAndClean);

		if( unit_testing_addToSuite(suite , "Test createRemoveDirectory" , test_commons_file_createRemoveDirectory) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Test existsOpenRemoveCreateFile" , test_commons_file_existsOpenRemoveCreateFile) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Test createStartNameFile" , test_commons_file_createStartNameFile) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Trabajar con configuration file" , test_commons_file_workWithConfFile) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Obtener el key de un string de archivo de configuracion" , test_commons_file_getKey) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Obtener el value de un string de archivo de configuracion" , test_commons_file_getValue) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Abrir un archivo, escribir y truncar" , test_commons_file_createOrTruncateFile) == NULL)
			return EXIT_FAILURE;

		if( unit_testing_addToSuite(suite , "Abrir un archivo e insertar una linea" , test_commons_file_insertLine) == NULL)
			return EXIT_FAILURE;

		//if( unit_testing_addToSuite(suite , "Abrir un directorio y listar archivos" , test_commons_file_getDirectoryFiles) == NULL)
		//	return EXIT_FAILURE;

		/*
		 * TODO: Agregar las entradas para levantar los tests
		 */

		return EXIT_SUCCESS;
	}


	void test_commons_file_createRemoveDirectory() {
		Boolean b = commons_file_createDirectory(nameDirectory);
		CU_ASSERT_TRUE(b);
		b = commons_file_removeDirectory(nameDirectory);
		CU_ASSERT_TRUE(b);
	}


	void test_commons_file_existsOpenRemoveCreateFile() {

		Boolean b = commons_file_existsFile(nameFile);
	    CU_ASSERT_FALSE(b);

	    FILE * f = commons_file_openFile(nameFile);
	    CU_ASSERT_PTR_NULL(f);

	    f = commons_file_openOrCreateFile(nameFile);
	    CU_ASSERT_PTR_NOT_NULL(f);

	    commons_file_removeFile(nameFile);
	    b = commons_file_existsFile(nameFile);
	    CU_ASSERT_FALSE(b);
	}


	void test_commons_file_createStartNameFile() {
		String otroFile = "archivo2.txt";

	    File * file = commons_file_openOrCreateFile( commons_string_concatAll(3,"/opt/grage-repository/hudson/reports" , "/" , otroFile) );
	    CU_ASSERT_PTR_NOT_NULL(file);

	    String fullFileName = commons_file_getStartNameMatchingFile( "/opt/grage-repository/hudson/reports" , "archivo2" );
	    CU_ASSERT_STRING_EQUAL( fullFileName , otroFile );

	    fullFileName = commons_file_getStartNameMatchingFile( "/opt/grage-repository/hudson/reports" , "esteArchivoNoExiste" );
	    CU_ASSERT_PTR_NULL(fullFileName);

	    commons_file_closeFile(file);
	    commons_file_removeFile( commons_string_concatAll(3,"/opt/grage-repository/hudson/reports" , "/" , otroFile) );
	    free(fullFileName);
	}

	void test_commons_file_workWithConfFile() {

		void processKeyAndValues(char * key , char * value , void * storage){
			if ( !strcmp(key , "nombre") )
				CU_ASSERT_STRING_EQUAL(value , "fernando");
		}

		String fileName = "/opt/grage-repository/hudson/reports/process.properties";

		File * confFile = commons_file_openOrCreateFile(fileName);
		CU_ASSERT_PTR_NOT_NULL(confFile);

		commons_file_insertEntry( "nombre" , "fernando" , confFile);

		commons_file_loadConfiguration(confFile , &processKeyAndValues);

		commons_file_closeFile(confFile);
		commons_file_removeFile(fileName);
	}

	/*
	 * TODO: Terminar con estas funciones
	 */

	void test_commons_file_getKey() {
		String line = strdup("nombre=hudson");

		String key = commons_file_getKey(line);
		CU_ASSERT_PTR_NOT_NULL(key);
		CU_ASSERT_STRING_EQUAL(key,"nombre");
		free(line);

		line = strdup("sinKeyNiValue");
		key = commons_file_getKey(line);
		CU_ASSERT_PTR_NULL(key);
		free(line);
	}

	void test_commons_file_getValue() {
		String line = strdup("nombre=hudson");

		String value = commons_file_getValue(line);
		CU_ASSERT_PTR_NOT_NULL(value);
		CU_ASSERT_STRING_EQUAL(value,"hudson");
		free(line);

		line = strdup("sinKeyNiValue");
		value = commons_file_getKey(line);
		CU_ASSERT_PTR_NULL(value);
		free(line);
	}

	void test_commons_file_parseKeyAndValueFile() {
	}

	void test_commons_file_parseFileAndModifyValueForKey() {
	}

	void test_commons_file_updateEntry() {
	}

	void test_commons_file_createOrTruncateFile() {

		File * file = commons_file_createOrTruncateFile(nameFile);
		CU_ASSERT_PTR_NOT_NULL(file);

		fseek(file, 0, SEEK_END); //Ir al final del archivo
		int size = ftell(file); //Obtener la posicion del final
		CU_ASSERT_EQUAL(size , 0);

		fseek(file, 0, SEEK_SET); //Volver al inicio del archivo
		commons_file_insertEntry( "nombre" , "hudson" , file);
		commons_file_closeFile(file);

		file = commons_file_createOrTruncateFile(nameFile);
		CU_ASSERT_PTR_NOT_NULL(file);

		fseek(file, 0, SEEK_END); //Ir al final del archivo
		size = ftell(file); //Obtener la posicion del final
		CU_ASSERT_EQUAL(size , 0);

		commons_file_closeFile(file);
		commons_file_removeFile(nameFile);
	}

	void test_commons_file_insertLine() {

		File * file = commons_file_openOrCreateFile(nameFile);
		CU_ASSERT_PTR_NOT_NULL(file);

		commons_file_insertLine("unStringHardcodeado",file);
		int expectedSize = strlen("unStringHardcodeado");
		fseek(file, 0, SEEK_END);
		int actualSize = ftell(file);
		CU_ASSERT_EQUAL(actualSize-1 , expectedSize);

		commons_file_closeFile(file);
		commons_file_removeFile(nameFile);
	}

	/*void test_commons_file_getDirectoryFiles(){
		String file;

		String directory = "/opt/grage-repository/includes";
		List files = commons_file_getDirectoryFiles(directory , NULL , NULL);
		CU_ASSERT_PTR_NOT_NULL(files);

		Iterator * ite = commons_iterator_buildIterator(files);
		while( commons_iterator_hasMoreElements(ite) ){
			file = commons_iterator_next(ite);
			printf("commons_file_getDirectoryTest hallo el archivo: %s\n" , file);
		}
		free(file);
		free(ite);
	}*/
