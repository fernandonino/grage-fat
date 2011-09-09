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
	void setPpdPort(int p);
	void setPpdIdDisk(int p);
	void setPpdDiskCilinder(int p);
	void setPpdDiskHead(int p);
	void setPpdDiskSector(int p);
	void setPpdReadTimeMs(int p);
	void setPpdWriteTimeMs(int p);
	void setPpdRpm(int p);
	void setPpdSaltoPistaMs(int p);
	char * getPpdMode();
	char * getPpdAlgoritmo();
	int getPpdPort();
	int getPpdIdDisk();
	int getPpdDiskCilinder();
	int getPpdDiskHead();
	int getPpdDiskSector();
	int getPpdReadTimeMs();
	int getPpdWriteTimeMs();
	int getPpdRpm();
	int getPpdSaltoPistaMs();
#endif /* PFS_CONFIGURATION_H_ */
