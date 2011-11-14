/*
 * pfs-fat32.c
 *
 *  Created on: 04/11/2011
 *      Author: gonzalo-joaco
 */


#include <stdlib.h>

#include "pfs-fat32.h"


	FatFile * pfs_fat32_utils_openRootDirectory(Volume * v) {
		FatFile * fatFile = (FatFile *)calloc(1,sizeof(FatFile));

		uint32_t cluster = pfs_fat32_utils_getNextClusterInChain(v , v->root);

		fatFile->source = v->root;
		fatFile->sourceOffset = 0;

		if (FAT_32_ISEOC(cluster))
			fatFile->nextCluster = 0;
		else
			fatFile->nextCluster = cluster;

		fatFile->currentSector.sectorNumber = v->fds;
		fatFile->dirEntryOffset = 0;
		fatFile->dirType = 0;

		return fatFile;
	}



	FatFile * pfs_fat32_utils_openNonRootDirectory(const char * path , Volume * v ){

		uint32_t next = v->root;
		FatFile * fatFile = (FatFile *)calloc(1,sizeof(FatFile));
		LongDirEntry longEntry;
		DirEntry sDirEntry;

		int16_t offset;

		uint32_t sector = pfs_fat_utils_getFirstSectorOfCluster(v ,
				v->root);

		DiskSector diskSector = pfs_endpoint_callGetSector(sector);
		memcpy(&longEntry, diskSector.sectorContent, FAT_32_DIR_ENTRY_SIZE);

		char * utf8name = pfs_fat_utils_getFileName(&longEntry);

		List directories = commons_list_tokenize((char *)path, '/');
		Iterator * ite = commons_iterator_buildIterator(directories);
		uint32_t originalSector ;

		while (commons_iterator_hasMoreElements(ite)) {

			originalSector = diskSector.sectorNumber;

			offset = 0;

			char * token = commons_iterator_next(ite);

			while (longEntry.LDIR_Ord != FAT_32_ENDOFDIR
					&& !commons_string_equals(token, utf8name)) {

				offset += FAT_32_BLOCK_ENTRY_SIZE;

				if (offset < v->bps) {

					memcpy(&longEntry,
							diskSector.sectorContent + offset,
							FAT_32_DIR_ENTRY_SIZE);

					utf8name = pfs_fat_utils_getFileName(&longEntry);

				} else if (offset == v->bps) {

					if(pfs_fat32_utils_isLastSectorFromCluster(v , sector)){

						sector = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v , next);
						diskSector = pfs_endpoint_callGetSector(sector);
						next = pfs_fat32_utils_getNextClusterInChain(v , next);
						offset = -FAT_32_BLOCK_ENTRY_SIZE;  //Porque si es un cluster que continua cuando sumo 64 al ppio del while me salteo 1 directorio
					}else{

						diskSector = pfs_endpoint_callGetSector(++sector);
						offset = 0;

					}
				}

			}

			if (longEntry.LDIR_Ord == FAT_32_ENDOFDIR) {
				commons_misc_doFreeNull((void **)utf8name);
				return NULL;
			} else if (commons_string_equals(utf8name, token)) {

				offset += FAT_32_DIR_ENTRY_SIZE;

				memcpy(&sDirEntry, diskSector.sectorContent + offset,
						FAT_32_DIR_ENTRY_SIZE);

				fatFile->longEntry = longEntry;
				fatFile->shortEntry = sDirEntry;

				fatFile->source = next;
				next = pfs_fat_getFirstClusterFromDirEntry(&sDirEntry);

				if ( commons_iterator_hasMoreElements(ite) ){
					sector = pfs_fat_utils_getFirstSectorOfCluster(v, next);
					diskSector = pfs_endpoint_callGetSector(sector);
				}
			}
		}

		commons_misc_doFreeNull((void **)utf8name);


		fatFile->sourceOffset = pfs_fat32_utils_getDirEntryOffset(
				diskSector.sectorNumber , originalSector , offset );
		fatFile->nextCluster = pfs_fat_getFirstClusterFromDirEntry(&sDirEntry);
		fatFile->currentSector.sectorNumber = pfs_fat_utils_getFirstSectorOfCluster(v , fatFile->nextCluster);
		fatFile->dirEntryOffset = 0;
		fatFile->dirType = 1;

		return fatFile;
	}




	FatFile * pfs_fat32_open(const char * path) {

		Volume * v = pfs_state_getVolume();

		if (commons_string_equals(FAT_32_ROOT_FORWARD_SLASH, (char *)path)) {
			return pfs_fat32_utils_openRootDirectory(v);
		} else {
			return pfs_fat32_utils_openNonRootDirectory(path , v);
		}
	}




	int8_t pfs_fat32_readDirectory( struct dirent * direntry , FatFile * file , Volume * volume){
		LongDirEntry lfnentry;
		DirEntry  sfnentry;

		DiskSector diskSector = pfs_endpoint_callGetSector(file->currentSector.sectorNumber);


		if( file->dirEntryOffset >= volume->bps){
			if ( file->nextCluster != 0 ){
			uint32_t sectorId = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(volume , file->nextCluster);
			file->currentSector = diskSector = pfs_endpoint_callGetSector(sectorId);
			} else {

			}
		}

		do {
			memcpy(&lfnentry , diskSector.sectorContent + file->dirEntryOffset , sizeof(LongDirEntry));
			file->dirEntryOffset += 32;

			if( file->dirEntryOffset >= volume->bps ){
				uint32_t sectorId = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(volume , file->nextCluster);
				diskSector = pfs_endpoint_callGetSector(sectorId);
			}
		} while ( lfnentry.LDIR_Ord == FAT_32_FREEENT );

		if( FAT_32_LDIR_ISLAST(lfnentry.LDIR_Ord) ){
			memcpy(&sfnentry , diskSector.sectorContent + file->dirEntryOffset  , sizeof(DirEntry));
			file->dirEntryOffset += 32;

			pfs_fat32_utils_toDirent(direntry , &sfnentry , &lfnentry, volume);
			return EXIT_SUCCESS;

		} else if ( lfnentry.LDIR_Ord == FAT_32_ENDOFDIR ){

			return EXIT_FAILURE;

		} else { //La entrada es solo DirEntry ( . o ..)
			memcpy(&sfnentry , diskSector.sectorContent + file->dirEntryOffset - 32 , sizeof(DirEntry));

			pfs_fat32_utils_toDirent(direntry , &sfnentry , NULL , volume);
			return EXIT_SUCCESS;

		}

		return EXIT_FAILURE;
	}

	void pfs_fat32_unlink(Volume * v , FatFile * fd){

		uint32_t sectorNumberInCluster = fd->sourceOffset / v->bps;
		uint32_t sector = pfs_fat_utils_getFirstSectorOfCluster(v , fd->source);
		DiskSector diskSector = pfs_endpoint_callGetSector(sector + sectorNumberInCluster);

		//Borrado de DirEntry
		pfs_fat32_unlink_dirEntry(v , fd , &diskSector);
		pfs_endpoint_callPutSector(diskSector);

		//Borrado de la chain en la FAT
		uint8_t result;
		do {
			result = pfs_fat32_unlink_FatEntryChain(v , fd);
		} while( result );

	}


	void pfs_fat32_rmdir(Volume * v , FatFile * fd){

		uint32_t sectorNumberInCluster = fd->sourceOffset / v->bps;
		uint32_t sector = pfs_fat_utils_getFirstSectorOfCluster(v , fd->source);
		DiskSector diskSector = pfs_endpoint_callGetSector(sector + sectorNumberInCluster);

		//Borrado de DirEntry
		pfs_fat32_unlink_dirEntry(v , fd , &diskSector);
		pfs_endpoint_callPutSector(diskSector);

		//Borrado de la chain en la FAT
		pfs_fat32_unlink_FatEntryChain(v , fd);
	}

	void pfs_fat32_unlink_dirEntry(Volume * v , FatFile * fd , DiskSector * diskSector){
		DirEntry sDirEntry;
		LongDirEntry lDirEntry , auxLDirEntry;

		uint32_t sectorOffset = fd->sourceOffset % v->bps;

		memcpy(&lDirEntry, diskSector->sectorContent + sectorOffset, sizeof(LongDirEntry));
		memcpy(&auxLDirEntry, diskSector->sectorContent + sectorOffset + FAT_32_BLOCK_ENTRY_SIZE, sizeof(LongDirEntry));
		memcpy(&sDirEntry, diskSector->sectorContent + sectorOffset + FAT_32_DIR_ENTRY_SIZE, sizeof(DirEntry));
		if(auxLDirEntry.LDIR_Ord != 0x00){
			diskSector->sectorContent[sectorOffset] = FAT_32_FREEENT;
			diskSector->sectorContent[sectorOffset + FAT_32_DIR_ENTRY_SIZE] = FAT_32_FREEENT;
		}
		else{
			diskSector->sectorContent[sectorOffset] = FAT_32_ENDOFDIR;
			diskSector->sectorContent[sectorOffset + FAT_32_DIR_ENTRY_SIZE] = FAT_32_ENDOFDIR;
		}

	}

	int8_t pfs_fat32_unlink_FatEntryChain(Volume * v , FatFile * fd){
		uint32_t nextCluster , offset , EntryValue , sector;
		uint32_t phantomValue = FAT_32_FAT_FREE_ENTRY;

		nextCluster = pfs_fat32_utils_getNextClusterInChain(v ,fd->nextCluster);
		sector = pfs_fat_utils_getFatEntrySector(v, fd->nextCluster);
		DiskSector diskSector = pfs_endpoint_callGetSector(sector);
		offset = pfs_fat_utils_getFatEntryOffset(v, fd->nextCluster);

		memcpy(&EntryValue, diskSector.sectorContent + offset, sizeof(uint32_t));
		memcpy(diskSector.sectorContent + offset, &phantomValue, sizeof(uint32_t));
		pfs_endpoint_callPutSector(diskSector);

		if( FAT_32_ISEOC(EntryValue) )
			return 0;
		else{
			fd->nextCluster = nextCluster;
			return 1;
		}
	}


	uint16_t pfs_fat32_read(Volume * v , FatFile * f , char * buf , size_t size){
		uint16_t bytesRead = 0;
		uint16_t bytesLeft = size;
		uint16_t bytesToRead;

		DiskSector sector = pfs_fat32_utils_getSectorFromNthCluster(f);

		if ( f->sectorByteOffset + size <= v->bps ){				// Lo que se pide para leer + el offset caen dentro de un sector
			memcpy(buf , sector.sectorContent + f->sectorByteOffset , size);
			return bytesRead += size;
		} else {													// El contenido de un sector no alcanza; se lee lo necesario y se sigue
			memcpy(buf , sector.sectorContent + f->sectorByteOffset , v->bps - f->sectorByteOffset);
			bytesRead += v->bps - f->sectorByteOffset;
			bytesLeft -= bytesRead;
		}

		uint32_t nextNeededCluster = f->fileAbsoluteClusterNumber;


		// El siguiente algoritmo alcanza para leer un sector + 1 cluster entero
		// Si el size es suficientemente grande como para abarcar mas de un cluster, entonces falla.
		while( bytesRead < size ){

			if( pfs_fat32_utils_isLastSectorFromCluster(v , sector.sectorNumber) ){
				uint32_t sectorId = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v , nextNeededCluster);
				sector = pfs_endpoint_callGetSector(sectorId);
			} else {
				sector = pfs_endpoint_callGetSector(sector.sectorNumber + 1);
			}

			if ( bytesLeft <= v->bps ){
				bytesToRead = bytesLeft;
			} else if ( pfs_fat32_utils_isLastSectorFromCluster(v , sector.sectorNumber) && FAT_32_ISEOC(nextNeededCluster) ){
				bytesToRead = bytesLeft; //Verificar el uso de FAT_32_ISEOC ---> creo que habria que preguntar por el siguiente cluster
			} else {
				bytesToRead = v->bps;
			}

			memcpy( buf + bytesRead , sector.sectorContent , bytesToRead);
			bytesRead += bytesToRead;
			bytesLeft -= bytesToRead;
		}

		return bytesRead;
	}






