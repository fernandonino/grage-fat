/*
 * ppd-configuration.h
 *
 *  Created on: 05/09/2011
 *      Author: gonzalo
 */

#ifndef PPD_CONFIGURATION_H_
#define PPD_CONFIGURATION_H_

#define PPD_DEFAULT_CONFIGURATION_FILE		"../conf/grage-ppd.properties"
#define PPD_CONFIGURATION_MODE			"ppd.configuration.mode"
#define PPD_CONFIGURATION_ALGORITMO			"ppd.configuration.algoritmo"
#define PPD_CONFIGURATION_LISTEN_PORT		"ppd.configuration.port"
#define PPD_CONFIGURATION_LOGGING_LEVEL			"ppd.configuration.logging.level"
#define PPD_CONFIGURATION_ID_DISK			"ppd.configuration.id.disk"
#define PPD_CONFIGURATION_CILINDER 			"ppd.configuration.cilinder"
#define PPD_CONFIGURATION_HEAD				"ppd.configuration.head"
#define PPD_CONFIGURATION_SECTOR			"ppd.configuration.sector"
#define PPD_CONFIGURATION_READ_TIME			"ppd.configuration.read.time"
#define PPD_CONFIGURATION_WRITE_TIME		"ppd.configuration.write.time"
#define PPD_CONFIGURATION_RPM				"ppd.configuration.rpm"
#define PPD_CONFIGURATION_SALTO_PISTA		"ppd.configuration.salto.pista"

	void ppd_configuration_setup();

	void ppd_configuration_setConfigurationFile(char * s);
	void setPpdMode(char * p);
	void setPpdAlgoritmo(char * p);
	void setPpdPort(char * p);
	void setPpdIdDisk(char * p);
	void setPpdDiskCilinder(char * p);
	void setPpdDiskHead(char * p);
	void setPpdDiskSector(char * p);
	void setPpdReadTimeMs(char * p);
	void setPpdWriteTimeMs(char * p);
	void setPpdRpm(char * p);
	void setPpdSaltoPistaMs(char * p);
	char * getPpdMode();
	char * getPpdAlgoritmo();
	char * getPpdPort();
	char * getPpdIdDisk();
	char * getPpdDiskCilinder();
	char * getPpdDiskHead();
	char * getPpdDiskSector();
	char * getPpdReadTimeMs();
	char * getPpdWriteTimeMs();
	char * getPpdRpm();
	char * getPpdSaltoPistaMs();
#endif /* PFS_CONFIGURATION_H_ */
