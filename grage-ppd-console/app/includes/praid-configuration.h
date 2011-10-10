/*
 * praid-configuration.h
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo
 */

#ifndef PRAID_CONFIGURATION_H_
#define PRAID_CONFIGURATION_H_

#define PRAID_DEFAULT_CONFIGURATION_FILE		"../conf/grage-praid.properties"
#define PRAIND_DEFAULT_LOG_FILE "../logs/grage-praid.log"
#define PRAID_DEVICE_PORT	"praid.device.port"
#define PRAID_CONSOLE_STATUS "praid.console.status"

	char * praid_configuration_getDevicePort(void);
	char * praid_configuration_getConsoleStatus(void);

	void praid_configuration_setDevicePort(char * devAddress);
	void praid_configuration_setConsoleStatus(char * devAddress);

	void praid_configuration_setup();
	char * praid_configuration_getConfigurationFile();
	void praid_configuration_setConfigurationFile(char * s);

#endif /* PFS_CONFIGURATION_H_ */
