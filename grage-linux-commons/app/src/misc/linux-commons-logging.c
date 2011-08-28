/*
 *  logging.c
 *  
 *
 *  Created by Fernando Nino on 4/11/11.
 *
 *  Pendiente:
 *  Verificar si en commons_logging_writeLogString la ruta es correcta
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/timeb.h>
#include <pthread.h>

#include "linux-commons.h"
#include "linux-commons-logging.h"

int levelEnabled;

	void commons_logging_setLoggingLevelEnabled(int level){
		levelEnabled = level;
	}

	int commons_logging_getLoggingLevelEnabled(){
		return levelEnabled;
	}

	Boolean commons_logging_isDebugEnabled(){
		if(levelEnabled >= LOGGING_LEVEL_DEBUG){
			return TRUE;
		}else{
			return FALSE;
		}
	}

	Boolean commons_logging_isInfoEnabled(){
		if(levelEnabled >= LOGGING_LEVEL_INFO){
			return TRUE;
		}else{
			return FALSE;
		}
	}
	
	char * commons_logging_getLogString(char * procName , char * type, char * desc) {
		char * logString = (char *)malloc(BUFFSIZE);
		char inst[12];
		int pid = getpid();
		pthread_t tid = pthread_self();
		struct timeb tmili;

		time_t curtime = time(NULL);
		ftime (&tmili);
		struct tm * loctime = localtime(&curtime);
		sprintf( inst , "%d:%d:%d:%d" , loctime->tm_hour , loctime->tm_min , loctime->tm_sec , tmili.millitm );

		sprintf( logString , "[%s] [Proc:%s] [PID:%d] [TID:%d] [Type:%s] %s\r\n" , inst,procName , pid , (int)tid , type , desc );

		return logString;
	}
	
	void commons_logging_writeLogString(char * procName , char * logString ){
		FILE *logfile;
		char logname[20];

		bzero( logname , sizeof(logname) );
		sprintf( logname , "app/logs/%s.log" , procName );

		logfile = fopen( logname , "a+");

		if ( logfile == NULL ) {
			printf("No se pudo abrir el logfile.\n");
		} else {
			fwrite( logString, strlen(logString) , 1 , logfile );
			fclose(logfile);
		}
	}
	
	void commons_logging_println(char * proc , char * type , char * description ){
		printf("%s" , commons_logging_getLogString(proc , type , description) );
	}
	
	void commons_logging_logInfo(char * proc , char * description ){
		if(commons_logging_isInfoEnabled()){
			commons_logging_println(proc , "INFO" , description );
			commons_logging_writeLogString(proc , commons_logging_getLogString(proc , "INFO" , description));
		}
	}

	void commons_logging_logError(char * proc , char * description ){
		commons_logging_println(proc , "ERROR" , description );
		commons_logging_writeLogString(proc , commons_logging_getLogString(proc , "ERROR" , description));
	}

	void commons_logging_logDebug(char * proc , char * description ){
		if(commons_logging_isDebugEnabled()){
			commons_logging_println(proc , "DEBUG" , description );
			commons_logging_writeLogString(proc , commons_logging_getLogString(proc , "DEBUG" , description));
		}
	}
