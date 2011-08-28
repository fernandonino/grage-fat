/**
 * logging.h
 *
 * Declaracion de las funciones de logging (Linux).
 * 
 *
 */

#define BUFFSIZE 2048

#define LOGGING_LEVEL_DEBUG 			3
#define LOGGING_LEVEL_INFO				2
#define LOGGING_LEVEL_ERROR				1

#define LOGGING_LEVEL_STRING_DEBUG		"debug"
#define LOGGING_LEVEL_STRING_INFO		"info"
#define LOGGING_LEVEL_STRING_ERROR		"error"

	/**
	 * Niveles de logging.
	 */
	void commons_logging_setLoggingLevelEnabled(int level);
	int  commons_logging_getLoggingLevelEnabled();
	Boolean commons_logging_isDebugEnabled();
	Boolean commons_logging_isInfoEnabled ();

	/**
	 * Strings de logging.
	 */
	char * commons_logging_getLogString  (char * , char * , char * );
	void   commons_logging_writeLogString(char * , char * );

	void commons_logging_println (char * proc , char * type , char * description );
	void commons_logging_logInfo (char * proc , char * description );
	void commons_logging_logError(char * proc , char * description );
	void commons_logging_logDebug(char * proc , char * description );
