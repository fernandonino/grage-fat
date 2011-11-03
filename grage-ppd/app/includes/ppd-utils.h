/*
 * ppd-utils.h
 *
 *  Created on: Nov 3, 2011
 *      Author: seba
 */

#ifndef PPD_UTILS_H_
#define PPD_UTILS_H_

int ppd_utils_get_cilinder_from_sector(int sector);
int ppd_utils_get_sectorofcilinder_from_sector(int sector);
int ppd_utils_get_sector_from_sectorofcilinder(int sector, int cilinder);

#endif /* PPD_UTILS_H_ */
