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

		uint32_t cluster = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v, v->root);

		fatFile.source = v->root;
		fatFile.sourceOffset = 0;

		if (FAT_32_ISEOC(cluster))
			fatFile.nextCluster = 0;
		else
			fatFile.nextCluster = cluster;

		fatFile.dirEntryOffset = 0;
		fatFile.dirType = 0;

		return &fatFile;
	}



	FatFile * pfs_fat32_utils_openNonRootDirectory(const char * path , Volume * v ){

		uint32_t next = v->root;
		FatFile fatFile;
		LongDirEntry longEntry;
		DirEntry sDirEntry;

		uint32_t offset = v->fds * v->bps;

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

			char * token = commons_iterator_next(ite);

			while (longEntry.LDIR_Ord != FAT_32_ENDOFDIR
					&& !commons_string_equals(token, utf8name)) {

				offset += FAT_32_BLOCK_ENTRY_SIZE;

				if (offset < FAT_32_SECTOR_SIZE) {

					memcpy(&longEntry,
							diskSector.sectorContent + FAT_32_BLOCK_ENTRY_SIZE,
							FAT_32_DIR_ENTRY_SIZE);

					utf8name = pfs_fat_utils_getFileName(&longEntry);

				} else if (offset == FAT_32_SECTOR_SIZE) {

					if(pfs_fat32_utils_isLastSectorFromCluster(v , sector)){

						sector = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v , next);
						diskSector = pfs_endpoint_callGetSector(sector);
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

				offset += FAT_32_DIR_ENTRY_SIZE;

				memcpy(&sDirEntry, diskSector.sectorContent + offset,
						FAT_32_DIR_ENTRY_SIZE);
				offset = sector * v->bps + FAT_32_BLOCK_ENTRY_SIZE;

				fatFile.longEntry = longEntry;
				fatFile.shortEntry = sDirEntry;

				fatFile.source = next;
				next = pfs_fat_getFirstClusterFromDirEntry(&sDirEntry);
				sector = pfs_fat_utils_getFirstSectorOfCluster(v, next);

			}
		}

		commons_misc_doFreeNull((void **)utf8name);


		fatFile.sourceOffset = pfs_fat32_utils_getDirEntryOffset(
				diskSector.sectorNumber , originalSector , offset );
		fatFile.nextCluster = pfs_fat_getFirstClusterFromDirEntry(&sDirEntry);
		fatFile.currentSector.sectorNumber = pfs_fat_utils_getFirstSectorOfCluster(v , fatFile.nextCluster);
		fatFile.dirEntryOffset = 0;
		fatFile.dirType = 1;

		return &fatFile;
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
		uint8_t lfncount = 0;

		DiskSector diskSector = file->currentSector;

		if( file->dirEntryOffset >= volume->bps){
			uint32_t sectorId = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(volume , file->nextCluster);
			file->currentSector = diskSector = pfs_endpoint_callGetSector(sectorId);
		}

		//lfnentry.LDIR_Ord = 0x00; // Fuerzo la entrada al ciclo
		do {
			memcpy(&lfnentry , diskSector.sectorContent + file->dirEntryOffset , sizeof(LongDirEntry));
			file->dirEntryOffset += 32;

			if( file->dirEntryOffset >= volume->bps ){
				uint32_t sectorId = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(volume , file->nextCluster);
				diskSector = pfs_endpoint_callGetSector(sectorId);
			}
		} while ( lfnentry.LDIR_Ord != 0x00 );

		if( FAT_32_LDIR_ISLAST(lfnentry.LDIR_Ord) ){
			lfncount++;
			memcpy(&sfnentry , diskSector.sectorContent + file->dirEntryOffset  , sizeof(DirEntry));
			file->dirEntryOffset += 32;

			pfs_fat32_utils_toDirent(direntry , sfnentry , lfnentry, volume);
			return EXIT_SUCCESS;

		} else if ( lfncount == 0 ){ //La entrada es solo DirEntry ( . o ..)
			memcpy(&sfnentry , diskSector.sectorContent + file->dirEntryOffset - 32 , sizeof(DirEntry));

			pfs_fat32_utils_toDirent(direntry , sfnentry , lfnentry, volume);
			return EXIT_SUCCESS;

		}

		return EXIT_FAILURE;
	}

	void pfs_fat32_unlink(Volume * v , FatFile * fd){

		uint32_t sector = fd->sourceOffset / v->bps;
		DiskSector diskSector = pfs_endpoint_callGetSector(sector);

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

		uint32_t sector = fd->sourceOffset / v->bps;
		DiskSector diskSector = pfs_endpoint_callGetSector(sector);

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
		else
			return 1;
	}

	/*
	void fat_stat(Volume * v , FatFile * fatFile , struct stat * st) {

		memset((char *) st, 0, sizeof(struct stat));

	    st->st_dev = (int)(v);
	    st->st_nlink = 1;
	    st->st_rdev = 0;
	    st->st_blksize = v->bpc;

	  if(fatFile->dirType == 0) {
	  	st->st_ino = v->root;
		st->st_mode = S_IFDIR | S_IRWXU;
		st->st_size = 0;
		st->st_blocks=0;
	    st->st_ctim = st->st_atim = st->st_mtim = 0;
	  } else {
		st->st_ino = pfs_fat_getFirstClusterFromDirEntry(fatFile->shortEntry);

		if ( fatFile->shortEntry->DIR_Attr == FAT_32_ATTR_DIRECTORY ) {
			st->st_mode = S_IFDIR | S_IRWXU;
		} else {
			st->st_mode = S_IFREG | S_IRWXU;
		}
		st->st_size = fatFile->shortEntry->DIR_FileSize;
		st->st_blocks = (st->st_size / v->bpc) + 1;
	    st->st_ctim = st->st_atim = st->st_mtim = pfs_fat32_utils_getTime(fatFile->shortEntry);
	  }

	}*/

	uint16_t pfs_fat32_read(Volume * v , FatFile * f , char * buf , size_t size){
		uint16_t bytesRead = 0;
		uint16_t bytesLeft = size;
		uint16_t bytesToRead;

		DiskSector sector = pfs_fat32_utils_getSectorFromNthCluster(f);

		if ( f->sectorByteOffset + size <= v->bps ){
			memcpy(buf , sector.sectorContent + f->sectorByteOffset , size);
			return bytesRead += size;
		} else {
			memcpy(buf , sector.sectorContent + f->sectorByteOffset , v->bps - f->sectorByteOffset);
			bytesRead += v->bps - f->sectorByteOffset;
			bytesLeft -= bytesRead;
		}

		uint32_t nextNeededCluster = f->fileAbsoluteClusterNumber;


		// El siguiente algoritmo alcanza para leer un sector + 1 cluster entero
		// Si el size es suficientemente grande como para abarcar mas de un cluster, entonces falla.
		// Otro detalle: si size pedido es mayor al tamanio del archivo menos la posicion (del fat_seek),
		// entonces se estan devolviendo bytes de mas!
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
				bytesToRead = bytesLeft;
			} else {
				bytesToRead = v->bps;
			}

			memcpy( buf + bytesRead , sector.sectorContent , bytesToRead);
			bytesRead += bytesToRead;
			bytesLeft -= bytesToRead;
		}

		return bytesRead; //Nunca se deberia llegar aca
	}









