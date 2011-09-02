/**
 * logging.h
 *
 * Declaracion de las funciones de logging (Linux).
 * 
 *
 */
#ifndef COMMONS_LOGGING_H
#define COMMONS_LOGGING_H

#define BUFFSIZE 2048

#define LOGGING_LEVEL_DEBUG 			3
#define LOGGING_LEVEL_INFO				2
#define LOGGING_LEVEL_ERROR				1

#define DEBUG_LVL "DEBUG"
#define INFO_LVL "INFO"
#define ERROR_LVL "ERROR"

/**
 * Niveles de logging.
 */
void commons_logging_setLoggingLevelEnabled(int level);
int commons_logging_getLoggingLevelEnabled();
Boolean commons_logging_isDebugEnabled();
Boolean commons_logging_isInfoEnabled();

/**
 * Strings de logging.
 */
char *commons_logging_getLogString(char *, char *, char *);
void commons_logging_writeLogString(char *, char *);
void commons_logging_disablePrintf();
void commons_logging_initLog(char *);
void commons_logging_regSignals();
void commons_logging_shutdownLog();

void commons_logging_println(char *, char *, char *);
void commons_logging_logInfo(char *, char *);
void commons_logging_logError(char *, char *);
void commons_logging_logDebug(char *, char *);

#endif
