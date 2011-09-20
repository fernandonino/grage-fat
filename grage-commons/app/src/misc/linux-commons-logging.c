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
#include <signal.h>

#include "linux-commons.h"
#include "linux-commons-logging.h"
#include "linux-commons-strings.h"

	Boolean printfEnabled = TRUE;
	int levelEnabled;
	FILE *logfile;
	pthread_mutex_t logmutex=PTHREAD_MUTEX_INITIALIZER;


	void commons_logging_setLoggingLevelEnabled(int level) {
		levelEnabled = level;
	}

	void commons_logging_disablePrintf(){
		printfEnabled = FALSE;
	}

	int commons_logging_getLoggingLevelEnabled() {
		return levelEnabled;
	}

	Boolean commons_logging_isDebugEnabled() {
		if (levelEnabled >= LOGGING_LEVEL_DEBUG) {
			return TRUE;
		} else {
			return FALSE;
		}
	}

	Boolean commons_logging_isInfoEnabled() {
		if (levelEnabled >= LOGGING_LEVEL_INFO) {
			return TRUE;
		} else {
			return FALSE;
		}
	}

	char * commons_logging_getLogString(char * procName, char * type, char * desc) {
		char * auxLogString = (char *) malloc(BUFFSIZE);
		char * logString = NULL;
		char inst[12];
		int pid = getpid();
		pthread_t tid = pthread_self();
		struct timeb tmili;

		time_t curtime = time(NULL);
		ftime(&tmili);
		struct tm * loctime = localtime(&curtime);
		sprintf(inst, "%d:%d:%d:%d", loctime->tm_hour, loctime->tm_min, loctime->tm_sec, tmili.millitm);
		desc = commons_string_trimd(desc);
		sprintf(auxLogString, "[%s] [Proc:%s] [PID:%d] [TID:%d] [Type:%s] %s\r\n", inst, procName, pid, (int) tid, type, desc);
		free(desc);
		//Por si la cadena ocupa menos de 2048 caracteres.
		logString = strdup(auxLogString);
		free(auxLogString);

		return logString;
	}

	void commons_logging_initLog(char * procName) {
		char logname[20];

		bzero(logname, sizeof(logname));
		sprintf(logname, "../logs/%s.log", procName);

		logfile = fopen(logname, "a+");

		if (logfile == NULL)
			perror("No se pudo abrir el logfile.\n");

		//seteo el nivel info por default
		commons_logging_setLoggingLevelEnabled(LOGGING_LEVEL_INFO);
	}

	void commons_logging_regSignals() {
		signal(SIGINT,  commons_logging_defaultSigHandler);
		signal(SIGKILL, commons_logging_defaultSigHandler);
		signal(SIGTERM, commons_logging_defaultSigHandler);
		signal(SIGSTOP, commons_logging_defaultSigHandler);
	}

	void commons_logging_defaultSigHandler() {
		fclose(logfile);
		signal(SIGINT, SIG_DFL);
		signal(SIGKILL, SIG_DFL);
		signal(SIGTERM, SIG_DFL);
		signal(SIGSTOP, SIG_DFL);
		kill(getpid(),SIGINT);
	}

	void commons_logging_shutdownLog() {
		fclose(logfile);
	}

	void commons_logging_writeLogString(char * procName, char * logString) {
		pthread_mutex_lock(&logmutex);
		fwrite(logString, strlen(logString), 1, logfile);
		fflush(logfile);
		fsync(fileno(logfile));
		pthread_mutex_unlock(&logmutex);
	}

	void commons_logging_println(char * proc, char * type, char * description) {
		char * logString = commons_logging_getLogString(proc, type, description);
		printf("%s", logString);
		free(logString);
	}

	void commons_logging_logInfo(char * proc, char * description) {
		if (commons_logging_isInfoEnabled()) {
			char * logString = commons_logging_getLogString(proc, INFO_LVL, description);
			if(printfEnabled)
				printf("%s", logString);
			commons_logging_writeLogString(proc, logString);
			free(logString);
		}
	}

	void commons_logging_logError(char * proc, char * description) {
		char * logString = commons_logging_getLogString(proc, ERROR_LVL, description);
		if(printfEnabled)
			printf("%s", logString);
		commons_logging_writeLogString(proc, logString);
		free(logString);
	}

	void commons_logging_logDebug(char * proc, char * description) {
		if (commons_logging_isDebugEnabled()) {
			char * logString = commons_logging_getLogString(proc, DEBUG_LVL, description);
			if(printfEnabled)
				printf("%s", logString);
			commons_logging_writeLogString(proc, logString);
			free(logString);
		}
	}
