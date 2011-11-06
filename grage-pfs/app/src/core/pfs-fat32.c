/*
 * pfs-fat32.c
 *
 *  Created on: 04/11/2011
 *      Author: gonzalo-joaco
 */


#include <stdlib.h>
#include "pfs-fat32.h"



	FatFile * pfs_fat32_utils_openRootDirectory(Volume * v) {
		FatFile fatFile;

		uint32_t cluster = pfs_fat32_utils_getFatClusterEntry(v);

		fatFile.source = v->root;
		fatFile.sourceOffset = 0;

		if (FAT32_ISEOC(cluster))
			fatFile.nextCluster = 0;
		else
			fatFile.nextCluster = cluster;

		fatFile.dirEntryOffset = 0;
		fatFile.dirType = 0;

		return &fatFile;
	}



	FatFile * pfs_fat32_utils_openNonRootDirectory(char * path , Volume * v ){

		uint32_t next = v->root;
		FatFile fatFile;
		LongDirEntry longEntry;
		DirEntry sDirEntry;

		uint32_t offset = v->fds * v->bps;

		uint32_t sector = pfs_fat_utils_getFirstSectorOfCluster(volume,
				v->root);

		DiskSector diskSector = pfs_endpoint_callGetSector(sector);
		memcpy(&longEntry, diskSector.sectorContent, FAT_32_DIR_ENTRY_SIZE);

		char * utf8name = pfs_fat_utils_getFileName(&longEntry);

		List directories = commons_list_tokenize(path, '/');
		Iterator * ite = commons_iterator_buildIterator(directories);
		uint32_t originalSector ;

		while (commons_iterator_hasMoreElements(ite)) {

			originalSector = diskSector.sectorNumber;

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

					if(pfs_fat32_utils_isLastSectorFromCluster(sector)){

						uint32_t sectorId = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v , next);
						diskSector = pfs_endpoint_callGetSector(sectorId);
						next = pfs_fat32_utils_getNextClusterInChain(v , next);

					}else{

						diskSector = pfs_endpoint_callGetSector(++sector);

					}
					offset = 0;
				}

			}

			if (longEntry.LDIR_Ord == FAT_32_ENDOFDIR) {
				return NULL;
			} else if (commons_string_equals(utf8name, token)) {
				offset += 32;
				memcpy(&sDirEntry, diskSector.sectorContent + offset,
						FAT_32_BLOCK_ENTRY_SIZE);

				fatFile.source = next;
				next = pfs_fat_getFirstClusterFromDirEntry(&sDirEntry);
				sector = getFirstSectorOfCluster(v, next);

				offset = sector * v->bps + FAT_32_BLOCK_ENTRY_SIZE; //quiza es 32
			}
		}

		commons_misc_doFreeNull(utf8name);


		fatFile.sourceOffset = pfs_fat32_utils_getDirEntryOffset(
				diskSector.sectorNumber , originalSector , offset );;
		fatFile.nextCluster = pfs_fat_getFirstClusterFromDirEntry(&sDirEntry);
		fatFile.currentSector = pfs_fat_utils_getFirstSectorOfCluster(volume,fatFile.nextCluster);
		fatFile.dirEntryOffset = 0;
		fatFile.dirType = 1;

		return &fatFile;
	}




	FatFile * pfs_fat32_open(char * path) {

		Volume * v = pfs_state_getVolume();

		if (commons_string_equals(FAT_32_ROOT_FORWARD_SLASH, path)) {
			return pfs_fat32_utils_openRootDirectory(v);
		} else {
			return pfs_fat32_utils_openNonRootDirectory(path , v);
		}
	}




	int8_t pfs_fat32_readDirectory( struct dirent * direntry , FatFile * file , Volume * volume){
		LongDirEntry lfnentry;
		DirEntry  sfnentry;
		uint8_t lfncount = 0;
		uint16_t offset = 0;

		DiskSector diskSector = file->currentSector;

		if( file->dirEntryOffset >= volume->bps){
			uint32_t sectorId = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(volume , file->nextCluster);
			file->currentSector = diskSector = pfs_endpoint_callGetSector(sectorId);
		}

		lfnentry.LDIR_Ord = 0x00; // Fuerzo la entrada al ciclo
		while ( LDIR_ISFREE(lfnentry.LDIR_Ord) ) {
			memcpy(&lfnentry , diskSector->sectorContent + file->dirEntryOffset , sizeof(LongDirEntry));
			file->dirEntryOffset += 32;

			if( file->dirEntryOffset >= volume->bps ){
				uint32_t sectorId = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(volume , file->nextCluster);
				diskSector = pfs_endpoint_callGetSector(sectorId);
			} else
				return EXIT_FAILURE;
		}

		if( LDIR_ISLASTLONG(lfnentry.LDIR_Ord) ){
			lfncount++;
			memcpy(&sfnentry , diskSector->sectorContent + file->dirEntryOffset  , sizeof(DirEntry));
			file->dirEntryOffset += 32;

			pfs_fat_toDirent(direntry , sfnentry , lfnentry);
			return EXIT_SUCCESS;

		} else if ( lfncount == 0 ){ //La entrada es solo DirEntry ( . o ..)
			memcpy(&sfnentry , diskSector->sectorContent + file->dirEntryOffset - 32 , sizeof(DirEntry));

			pfs_fat_toDirent(direntry , sfnentry , lfnentry);
			return EXIT_SUCCESS;

		}

		return EXIT_FAILURE;
	}
