/*
 * pfs-fat32.c
 *
 *  Created on: 04/11/2011
 *      Author: gonzalo-joaco
 */

#include "pfs-fat32.h"



	FatFile pfs_fat32_utils_openRootDirectory(Volume * v) {
		FatFile fatFile;

		uint32_t nextCluster;
		fatFile.source = v->root;
		fatFile.sourceOffset = 0;

		uint32_t sector = pfs_fat_utils_getFatEntrySector(v, v->root);
		uint32_t offset = pfs_fat_utils_getFatEntryOffset(v, v->root);

		DiskSector diskSector = pfs_endpoint_callGetSector(sector);

		memcpy(&nextCluster, diskSector.sectorContent + offset, sizeof(uint32_t));

		if (FAT32_ISEOC(nextCluster))
			fatFile.nextCluster = 0;
		else
			fatFile.nextCluster = nextCluster;

		fatFile.dirEntryOffset = 0;
		fatFile.dirType = 0;

		return fatFile;
	}




	FatFile pfs_fat32_utils_openNonRootDirectory(char * path , Volume * v ){

		uint32_t next;
		FatFile fatFile;
		LongDirEntry longEntry;
		DirEntry sDirEntry;

		uint32_t offset = v->fds * v->bps;
		uint32_t currentCluster;

		uint32_t sector = pfs_fat_utils_getFirstSectorOfCluster(volume,
				v->root);

		DiskSector diskSector = pfs_endpoint_callGetSector(sector);
		memcpy(&longEntry, diskSector.sectorContent, FAT_32_DIR_ENTRY_SIZE);

		char * utf8name = pfs_fat_utils_getFileName(&longEntry);

		List directories = commons_list_tokenize(path, '/');
		Iterator * ite = commons_iterator_buildIterator(directories);

		while (commons_iterator_hasMoreElements(ite)) {

			char * token = commons_iterator_next(ite);

			while (longEntry.LDIR_Ord != FAT_32_ENDOFDIR
					&& !commons_string_equals(token, utf8name)) {

				offset += FAT_32_BLOCK_ENTRY_SIZE;

				if (offset < FAT_32_SECTOR_SIZE) {

					memcpy(&longEntry,
							diskSector.sectorContent + FAT_32_BLOCK_ENTRY_SIZE,
							FAT_32_DIR_ENTRY_SIZE);

					utf8name = pfs_get_fileName(&longEntry);

				} else if (offset == FAT_32_SECTOR_SIZE) {

					diskSector = pfs_endpoint_callGetSector(++sector);

					offset = 0;
				}
			}

			if (longEntry.LDIR_Ord == FAT_32_ENDOFDIR) {
				return NULL;
			} else if (commons_string_equals(utf8name, token)) {
				offset += 32;
				memcpy(&sDirEntry, diskSector.sectorContent + offset,
						FAT_32_BLOCK_ENTRY_SIZE);

				currentCluster = next;
				next = getFirstClusterFromDirEntry(&sDirEntry);
				sector = getFirstSectorOfCluster(v, next);

				offset = sector * v->bps + FAT_32_BLOCK_ENTRY_SIZE; //quiza es 32

			}
		}

		commons_misc_doFreeNull(utf8name);

		fatFile.source = currentCluster;
		fatFile.sourceOffset = currentCluster;
		fatFile.nextCluster = getFirstClusterFromDirEntry(&sDirEntry);
		fatFile.dirEntryOffset = offset -= 64;
		fatFile.flag = 1;
		fatFile.dirType = 1;

		return fatFile;
	}



	FatFile pfs_fat32_open(char * path) {

		Volume * v = pfs_state_getVolume();

		if (commons_string_equals(FAT_32_ROOT_FORWARD_SLASH, path)) {
			return pfs_fat32_utils_openRootDirectory(v);
		} else {
			return pfs_fat32_utils_openNonRootDirectory(path , v);
		}
	}
