/*
 * pfs-configuration.h
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo
 */

#ifndef PFS_CONFIGURATION_H_
#define PFS_CONFIGURATION_H_

#define PFS_DEFAULT_CONFIGURATION_FILE		"../conf/grage-pfs.properties"

#define PFS_DEVICE_ADDRESS	"pfs.device.address"
#define PFS_DEVICE_PORT		"pfs.device.port"
#define	PFS_MAX_CONNECTIONS	"pfs.max.connections"
#define PFS_CACHE_SIZE		"pfs.cache.size"

	char * getDeviceAddress(void);
	char * getMaxConnections(void);
	char * getDevicePort(void);
	char * getCacheSize(void);

	void setDevicePort(char * devPort);
	void setDeviceAddress(char *);
	void setMaxConnections(char * numberOfConnections);
	void setCacheSize(char * sizeOfCache);

	void pfs_configuration_processEntries(char * key , char * value , void * object);

	void pfs_configuration_setConfigurationFile(char * s);
	char * pfs_configuration_getConfigurationFile();

	void pfs_configuration_setup();

#endif /* PFS_CONFIGURATION_H_ */
