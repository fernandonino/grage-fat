/*
 * ppd-utils.c
 *
 *  Created on: 05/09/2011
 *      Author: EL Seba
 */
#include "linux-commons-logging.h"

	void ppd_utils_info(char * m){
		commons_logging_logInfo("PPD" , m);
	}

	void ppd_utils_debug(char * m){
		commons_logging_logDebug("PPD" , m);
	}

	void ppd_utils_error(char * m){
		commons_logging_logError("PPD" , m);
	}

