/*
* commons.h
*
*  Created on: 29/05/2011
*      Author: gonzalo
*/
#include <pthread.h>
#include <stdio.h>
#include <dirent.h>
#include <stdint.h>


#ifndef COMMONS_DEFINITIONS
#define COMMONS_DEFINITIONS

#define FALSE 0
#define TRUE 1
#define SOCKET_ERROR -1

#define SYSTEM_FILE_LINE_LENGTH 					100
#define SYSTEM_INT_CONVERSION_MAX_DIGITS_COUNT 		12
#define SYSTEM_STRING_CONCATENATION_SIZE			2048

	/**
	* Alias para darle nombres mas bonitos ^^
	*/

	typedef pthread_t Thread;
	typedef pthread_mutex_t ThreadMutex;
	typedef FILE File;
	typedef struct dirent FilesIterator;
	typedef DIR Directory;
	typedef int Boolean;
	typedef char * String;
	typedef void * Object;


	typedef uint32_t uint32;
	typedef int32_t int32;


	/*
	 * Miscelaneas
	 */
	String commons_misc_generateRandomKey(int size);
	String commons_misc_longToString(long int l);
	String commons_misc_intToString(int i);
	void commons_misc_lockThreadMutex(ThreadMutex * m);
	void commons_misc_unlockThreadMutex(ThreadMutex * m);
	int commons_misc_digitsCount(int i);

	void commons_misc_doFreeNull(void **pointer);
#endif




