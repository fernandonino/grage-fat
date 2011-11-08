/*
 * praid-configuration.h
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo
 */


#include <linux-commons.h>

#ifndef PRAID_CONFIGURATION_H_
#define PRAID_CONFIGURATION_H_

//#define PRAID_DEFAULT_CONFIGURATION_FILE		"../conf/grage-praid.properties"
//#define PRAIND_DEFAULT_LOG_FILE 				"../logs/grage-praid.log"

#define PRAID_DEFAULT_CONFIGURATION_FILE		"/opt/grage-repository/grage-praid.properties"
#define PRAIND_DEFAULT_LOG_FILE 				"/opt/grage-repository/logs/grage-praid.log"

#define PRAID_CONF_DEVICE_PORT						"praid.device.port"
#define PRAID_CONF_CONSOLE_STATUS 					"praid.enable.console"
#define PRAID_CONF_ENABLE_REPLICATION				"praid.enable.replication"

#define PRAID_CONF_VALUE_TRUE						"true"
#define PRAID_CONF_VALUE_FALSE						"false"




	char * praid_configuration_getDevicePort(void);
	Boolean praid_configuration_getConsoleStatus(void);
	Boolean praid_configuration_getEnableReplication(void);

	void praid_configuration_setDevicePort(char * devAddress);
	void praid_configuration_setConsoleStatus(Boolean);
	void praid_configuration_setEnableReplication(Boolean);

	void praid_configuration_setup();
	char * praid_configuration_getConfigurationFile();
	void praid_configuration_setConfigurationFile(char * s);

#endif /* PFS_CONFIGURATION_H_ */
