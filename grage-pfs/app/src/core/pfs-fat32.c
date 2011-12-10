/*
 * pfs-fat32.c
 *
 *  Created on: 04/11/2011
 *      Author: gonzalo-joaco
 */


#include <stdlib.h>
#include "pfs-cache.h"

#include "pfs-fat32.h"


	FatFile * pfs_fat32_utils_openRootDirectory(Volume * v) {
		FatFile * fatFile = (FatFile *)calloc(1,sizeof(FatFile));

		fatFile->cache = pfs_cache_sectors_initialize();

		uint32_t cluster = pfs_fat32_utils_getNextClusterInChain(v , v->root);

		fatFile->source = v->root;
		fatFile->sourceOffset = 0;

		if (FAT_32_ISEOC(cluster))
			fatFile->nextCluster = 0;
		else
			fatFile->nextCluster = cluster;

		fatFile->currentSector = v->fds;
		fatFile->dirEntryOffset = 0;
		fatFile->dirType = 0;

		pfs_state_addOpenFile(fatFile);

		return fatFile;
	}



    FatFile * pfs_fat32_utils_openNonRootDirectory(const char * path , Volume * v ){

            uint32_t next = v->root;
            FatFile * fatFile = (FatFile *)calloc(1,sizeof(FatFile));
            LongDirEntry longEntry;
            DirEntry sDirEntry;
            uint32_t offset;
            uint32_t blockNumber;
            char utf8name[14];

            if(pfs_cache_habilitada())
            	fatFile->cache = pfs_cache_sectors_initialize();

            Block block;
            block = pfs_fat32_utils_callGetBlock(v->root, fatFile);

            List directories = commons_list_tokenize((char *)path, '/');
            Iterator * ite = commons_iterator_buildIterator(directories);

            while (commons_iterator_hasMoreElements(ite)) {

				offset = 0;

				char * token = commons_iterator_next(ite);

				do{
					if (offset < v->bpc) {
						memcpy(&longEntry, block.content + offset, FAT_32_DIR_ENTRY_SIZE);
						if(FAT_32_LDIR_ISLONG(longEntry.LDIR_Attr)){
							pfs_fat_utils_getFileName(&longEntry , utf8name);
							offset += FAT_32_BLOCK_ENTRY_SIZE;
						}
						else{
							memcpy(&sDirEntry, block.content + offset, FAT_32_DIR_ENTRY_SIZE);
							pfs_fat32_utils_getShortName(&sDirEntry , utf8name);
							offset += FAT_32_DIR_ENTRY_SIZE;
						}
					} else if (offset >= v->bpc) {
						blockNumber = pfs_fat32_utils_getNextClusterInChain(v, block.id);
						block = pfs_fat32_utils_callGetBlock(blockNumber, fatFile);
						offset = 0;
					}

				}while (longEntry.LDIR_Ord != FAT_32_ENDOFDIR && !commons_string_equals(token, utf8name));

				if (longEntry.LDIR_Ord == FAT_32_ENDOFDIR) {
						//commons_misc_doFreeNull((void **)utf8name);
						return NULL;
				} else if (commons_string_equals(utf8name, token)) {

					if (offset >= v->bpc) {
						Block auxBlock;
						blockNumber = pfs_fat32_utils_getNextClusterInChain(v, block.id);
						auxBlock = pfs_fat32_utils_callGetBlock(blockNumber, fatFile);

						memcpy(&sDirEntry, auxBlock.content, FAT_32_DIR_ENTRY_SIZE);
						offset -= FAT_32_BLOCK_ENTRY_SIZE;
					}
					else{
						if(FAT_32_LDIR_ISLONG(longEntry.LDIR_Attr)){
							offset -= FAT_32_DIR_ENTRY_SIZE;
							memcpy(&sDirEntry, block.content + offset, FAT_32_DIR_ENTRY_SIZE);
							offset -= FAT_32_DIR_ENTRY_SIZE; //Esto es para el calculo del dirEntryOffset
						}
					}

					fatFile->longEntry = longEntry;
					fatFile->shortEntry = sDirEntry;

					fatFile->source = next;
					next = pfs_fat_getFirstClusterFromDirEntry(&sDirEntry);

					if ( commons_iterator_hasMoreElements(ite) ){
						block = pfs_fat32_utils_callGetBlock(next, fatFile);
					}
				}
            }

            fatFile->sourceOffset = offset;
            fatFile->nextCluster = pfs_fat_getFirstClusterFromDirEntry(&sDirEntry);
            //fatFile->currentSector = pfs_fat_utils_getFirstSectorOfCluster(v , fatFile->nextCluster);
            fatFile->dirEntryOffset = 0;
            fatFile->dirType = 1;
            fatFile->EOC = 0;

            //El siguiente if es un cambio para arreglar un tema con el unlink.
            //No esta probado con el resto de las funciones
            if(!FAT_32_LDIR_ISLONG(fatFile->longEntry.LDIR_Attr)){
            	fatFile->sourceOffset -= FAT_32_DIR_ENTRY_SIZE;
            }

            pfs_state_addOpenFile(fatFile);

            return fatFile;
    }





	FatFile * pfs_fat32_open(const char * path) {

		Volume * v = pfs_state_getVolume();

		if (commons_string_equals(FAT_32_ROOT_FORWARD_SLASH, (char *)path) || path[1] == 0) {
			return pfs_fat32_utils_openRootDirectory(v);
		} else {
			return pfs_fat32_utils_openNonRootDirectory(path , v);
		}
	}




	int8_t pfs_fat32_readDirectory( struct dirent * direntry , FatFile * file , Volume * volume){
		LongDirEntry lfnentry;
		DirEntry  sfnentry;
		Block block;


		if ( file->dirType == 0) {
			if( file->dirEntryOffset >= volume->bpc ){
				file->nextCluster = pfs_fat32_utils_getNextClusterInChain(volume , file->nextCluster);
				file->dirEntryOffset = 0;
				block = pfs_fat32_utils_callGetBlock(file->nextCluster, file);
			} else {
				block = pfs_fat32_utils_callGetBlock(file->source, file);
			}
		} else {
			if( file->dirEntryOffset >= volume->bpc ){
				file->nextCluster = pfs_fat32_utils_getNextClusterInChain(volume , file->nextCluster);
				file->dirEntryOffset = 0;
				block = pfs_fat32_utils_callGetBlock(file->nextCluster, file);
			} else {
				block = pfs_fat32_utils_callGetBlock(file->nextCluster, file);
			}
		}

		do {
			memcpy(&lfnentry , block.content + file->dirEntryOffset , sizeof(LongDirEntry));
			file->dirEntryOffset += 32;

			if( file->dirEntryOffset >= volume->bpc ){
				if ( FAT_32_ISEOC(file->nextCluster) ){
					return EXIT_FAILURE;
				} else {
					file->nextCluster = pfs_fat32_utils_getNextClusterInChain(volume, file->nextCluster);
					file->dirEntryOffset = 0;
				}
			}

		} while ( lfnentry.LDIR_Ord == FAT_32_FREEENT );

		if( FAT_32_LDIR_ISLONG(lfnentry.LDIR_Attr) ){
			memcpy(&sfnentry , block.content + file->dirEntryOffset  , sizeof(DirEntry));
			file->dirEntryOffset += 32;

			pfs_fat32_utils_toDirent(direntry , &sfnentry , &lfnentry, volume);
			return EXIT_SUCCESS;

		} else if ( lfnentry.LDIR_Ord == FAT_32_ENDOFDIR ){

			return EXIT_FAILURE;

		} else { //La entrada es solo DirEntry ( . o ..)
			memcpy(&sfnentry , block.content + file->dirEntryOffset - 32 , sizeof(DirEntry));
			pfs_fat32_utils_toDirent(direntry , &sfnentry , NULL , volume);
			return EXIT_SUCCESS;

		}

		return EXIT_FAILURE;
	}


	void pfs_fat32_unlink(Volume * v , FatFile * fd){

		uint32_t sectorNumberInCluster = fd->sourceOffset / v->bps;
		uint32_t sector = pfs_fat_utils_getFirstSectorOfCluster(v , fd->source);
		DiskSector diskSector = pfs_endpoint_callCachedGetSector(sector + sectorNumberInCluster , fd);

		//Borrado de DirEntry
		pfs_fat32_unlink_dirEntry(v , fd , diskSector);

		//Borrado de la chain en la FAT
		uint8_t result;
		do {
			result = pfs_fat32_unlink_FatEntryChain(v , fd);
		} while( result );

	}


	void pfs_fat32_rmdir(Volume * v , FatFile * fd){

		uint32_t sectorNumberInCluster = fd->sourceOffset / v->bps;
		uint32_t sector = pfs_fat_utils_getFirstSectorOfCluster(v , fd->source);
		DiskSector diskSector = pfs_endpoint_callCachedGetSector(sector + sectorNumberInCluster , fd);

		//Borrado de DirEntry
		pfs_fat32_unlink_dirEntry(v , fd , diskSector);

		//Borrado de la chain en la FAT
		pfs_fat32_unlink_FatEntryChain(v , fd);
	}

	void pfs_fat32_unlink_dirEntry(Volume * v , FatFile * fd , DiskSector diskSector){
		DirEntry sDirEntry;
		LongDirEntry lDirEntry , auxLDirEntry;
		uint32_t sector = diskSector.sectorNumber;
		uint32_t next = fd->source;
		DiskSector  auxDiskSector;
		uint8_t twoWritesFlag = 0;

		uint32_t sectorOffset = fd->sourceOffset % v->bps;
		memcpy(&lDirEntry, diskSector.sectorContent + sectorOffset, sizeof(LongDirEntry));
		if(FAT_32_LDIR_ISLONG(lDirEntry.LDIR_Attr)){
			if((sectorOffset += FAT_32_DIR_ENTRY_SIZE) < v->bps){
				memcpy(&sDirEntry, diskSector.sectorContent + sectorOffset, sizeof(DirEntry));
				if((sectorOffset += FAT_32_DIR_ENTRY_SIZE) < v->bps){
					memcpy(&auxLDirEntry, diskSector.sectorContent + sectorOffset, sizeof(LongDirEntry));
				}
				else{
					if(pfs_fat32_utils_isLastSectorFromCluster(v , sector)){
						sector = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v , next);
						auxDiskSector = pfs_endpoint_callCachedGetSector(sector , fd);
						next = pfs_fat32_utils_getNextClusterInChain(v , next);
					}
					else{

						auxDiskSector = pfs_endpoint_callCachedGetSector(++sector , fd);
					}
					memcpy(&auxLDirEntry, auxDiskSector.sectorContent, sizeof(LongDirEntry));
				}
			}
			else{
				twoWritesFlag = 1;
				if(pfs_fat32_utils_isLastSectorFromCluster(v , sector)){
					sector = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v , next);
					auxDiskSector = pfs_endpoint_callCachedGetSector(sector , fd);
					next = pfs_fat32_utils_getNextClusterInChain(v , next);
				}
				else{

					auxDiskSector = pfs_endpoint_callCachedGetSector(++sector , fd);
				}
				memcpy(&sDirEntry, auxDiskSector.sectorContent, sizeof(DirEntry));
				memcpy(&auxLDirEntry, auxDiskSector.sectorContent + FAT_32_DIR_ENTRY_SIZE, sizeof(LongDirEntry));
			}
		}
		else{
			memcpy(&sDirEntry, diskSector.sectorContent + sectorOffset, sizeof(DirEntry));
			if((sectorOffset += FAT_32_DIR_ENTRY_SIZE) < v->bps){
				memcpy(&auxLDirEntry, diskSector.sectorContent + sectorOffset, sizeof(LongDirEntry));
			}
			else{
				if(pfs_fat32_utils_isLastSectorFromCluster(v , diskSector.sectorNumber)){
					sector = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v , next);
					auxDiskSector = pfs_endpoint_callCachedGetSector(sector , fd);
					next = pfs_fat32_utils_getNextClusterInChain(v , next);
				}
				else{

					auxDiskSector = pfs_endpoint_callCachedGetSector(++sector , fd);
				}
				memcpy(&auxLDirEntry, auxDiskSector.sectorContent, sizeof(LongDirEntry));
			}
		}
		if(twoWritesFlag){
			if(auxLDirEntry.LDIR_Ord != 0x00){
				uint8_t phantomValue = FAT_32_FREEENT;
				memcpy(diskSector.sectorContent + v->bps - FAT_32_DIR_ENTRY_SIZE, &phantomValue, sizeof(LongDirEntry));
				memcpy(auxDiskSector.sectorContent, &phantomValue, sizeof(DirEntry));
			}
			else{
				uint8_t phantomValue = FAT_32_ENDOFDIR;
				memcpy(diskSector.sectorContent + v->bps - FAT_32_DIR_ENTRY_SIZE, &phantomValue, sizeof(LongDirEntry));
				memcpy(auxDiskSector.sectorContent, &phantomValue, sizeof(DirEntry));

			}
			pfs_endpoint_callPutSector(diskSector , NULL);
			pfs_endpoint_callPutSector(auxDiskSector , NULL);
		}
		else{
			sectorOffset = fd->sourceOffset % v->bps;

			if(auxLDirEntry.LDIR_Ord != 0x00){
				uint8_t phantomValue = FAT_32_FREEENT;
				if(FAT_32_LDIR_ISLONG(lDirEntry.LDIR_Attr)){
					memcpy(diskSector.sectorContent + sectorOffset, &phantomValue, sizeof(LongDirEntry));
					memcpy(diskSector.sectorContent + sectorOffset + FAT_32_DIR_ENTRY_SIZE, &phantomValue, sizeof(DirEntry));
				}
				else{
					memcpy(diskSector.sectorContent + sectorOffset, &phantomValue, sizeof(DirEntry));
				}
			}
			else{
				uint8_t phantomValue = FAT_32_ENDOFDIR;
				if(FAT_32_LDIR_ISLONG(lDirEntry.LDIR_Attr)){
					memcpy(diskSector.sectorContent + sectorOffset, &phantomValue, sizeof(LongDirEntry));
					memcpy(diskSector.sectorContent + sectorOffset + FAT_32_DIR_ENTRY_SIZE, &phantomValue, sizeof(DirEntry));
				}
				else{
					memcpy(diskSector.sectorContent + sectorOffset, &phantomValue, sizeof(DirEntry));
				}
			}
			pfs_endpoint_callPutSector(diskSector , NULL);
		}
	}


	int8_t pfs_fat32_unlink_FatEntryChain(Volume * v , FatFile * fd){
		uint32_t nextCluster , offset , EntryValue , sector;
		uint32_t phantomValue = FAT_32_FAT_FREE_ENTRY;

		nextCluster = pfs_fat32_utils_getNextClusterInChain(v ,fd->nextCluster);
		sector = pfs_fat_utils_getFatEntrySector(v, fd->nextCluster);
		DiskSector diskSector = pfs_endpoint_callCachedGetSector(sector , NULL);
		offset = pfs_fat_utils_getFatEntryOffset(v, fd->nextCluster);

		memcpy(&EntryValue, diskSector.sectorContent + offset, sizeof(uint32_t));
		memcpy(diskSector.sectorContent + offset, &phantomValue, sizeof(uint32_t));
		pfs_endpoint_callPutSector(diskSector , NULL);

		v->freeClusterCount++;

		if( FAT_32_ISEOC(EntryValue) )
			return 0;
		else{
			fd->nextCluster = nextCluster;
			return 1;
		}
	}

	uint32_t pfs_fat32_write(Volume * v , FatFile * f , const char * buf , size_t size){
/*
		uint32_t bytesWritten = 0 ;
		uint32_t bytesLeft = size;
		uint32_t bytesToWrite , sectorId;


		DiskSector sector;
		if (f->fileAbsoluteClusterNumberWrite == 0){
			sector = pfs_fat32_utils_getSectorFromNthClusterWrite(f);
		}
		else{
			f->fileAbsoluteClusterNumberWrite = pfs_fat32_utils_getNextClusterInChain(v,f->fileAbsoluteClusterNumberWrite);
			sectorId = pfs_fat_utils_getFirstSectorOfCluster(v , f->fileAbsoluteClusterNumberWrite);
			sector = pfs_endpoint_callCachedGetSector(sectorId, f);
		}

		if( size + f->sectorByteOffset <= v->bps ){
			memcpy(sector.sectorContent + f->sectorByteOffset , buf , size);
			pfs_endpoint_callPutSector(sector , f);
			return bytesWritten += size;
		} else {
			memcpy(sector.sectorContent + f->sectorByteOffset , buf , v->bps - f->sectorByteOffset);
			bytesWritten = v->bps - f->sectorByteOffset;
			bytesLeft -= bytesWritten;
		}

		pfs_endpoint_callPutSector(sector , f);

		uint32_t current = f->fileAbsoluteClusterNumberWrite;

		while( bytesWritten < size ){

			if( pfs_fat32_utils_isLastSectorFromCluster(v , sector.sectorNumber) ){
				uint32_t sectorId = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v , current);
				current = pfs_fat_utils_getClusterBySector(v , sectorId);
				sector = pfs_endpoint_callCachedGetSector(sectorId , f);
			} else {
				sector = pfs_endpoint_callCachedGetSector(sector.sectorNumber + 1 , f);

			}

			if ( (bytesLeft <= v->bps) || FAT_32_ISEOC(current) ){
				bytesToWrite = bytesLeft;
			} else {
				bytesToWrite = v->bps;
			}

			memcpy(sector.sectorContent , buf + bytesWritten , bytesToWrite);
			bytesWritten += bytesToWrite;
			bytesLeft -= bytesToWrite;
			pfs_endpoint_callPutSector(sector , f);
		}

		return bytesWritten;
*/return 0;
	}

	uint32_t pfs_fat32_read(Volume * v , FatFile * f , char * buf , size_t size){
		uint32_t bytesRead = 0;
		uint32_t bytesLeft = size;
		uint32_t bytesToRead , current;
		Block block;

		if (f->fileAbsoluteClusterNumberRead == 0){
			block = pfs_fat32_utils_getBlockFromNthClusterRead(f);
			current = block.id;
		} else {
			current = f->fileAbsoluteClusterNumberRead;
			current = pfs_fat32_utils_getNextClusterInChain(v , current);
			if ( ! FAT_32_ISEOC(current) ){
				block = pfs_fat32_utils_callGetBlock(current , f);
			}
		}

		if ( f->fileClusterOffset + size <= v->bpc ){	// Lo que se pide para leer + el offset caen dentro de un cluster
			memcpy(buf , block.content + f->fileClusterOffset , size);
			return bytesRead += size;
		} else {										// El contenido de un cluster no alcanza; se lee lo necesario y se sigue
			memcpy(buf , block.content + f->fileClusterOffset , v->bpc - f->fileClusterOffset);
			bytesRead += v->bpc - f->fileClusterOffset;
			bytesLeft -= bytesRead;
		}

//		current = f->fileAbsoluteClusterNumberRead;

		while( bytesRead < size ){

			if (FAT_32_ISEOC(current)){
				return bytesRead;
			} else {
				current = pfs_fat32_utils_getNextClusterInChain(v , block.id);
				block = pfs_fat32_utils_callGetBlock(current , f);
			}

			if ( (bytesLeft <= v->bpc)){
				bytesToRead = bytesLeft;
			} else {
				bytesToRead = v->bpc;
			}

			memcpy( buf + bytesRead , block.content , bytesToRead);
			bytesRead += bytesToRead;
			bytesLeft -= bytesToRead;
		}

		f->fileAbsoluteClusterNumberRead = current;

		return bytesRead;
	}

	int8_t pfs_fat32_mknod(Volume * v , FatFile * destination , char * filename){
		DirEntry shortEntry;
		LongDirEntry longEntry , auxEntry;
		uint32_t sectorId;
		uint16_t offset = 0;
		uint8_t freeCount = 0;

		/* Completamos los campos del nombre */
		longEntry.LDIR_Ord = 0x41; 						//Ultimo (y unico) long entry
		pfs_fat32_utils_loadLongEntryFilename(&longEntry , filename);
		pfs_fat32_utils_loadEntryFilename(&shortEntry , filename);

		/* Completamos los campos de fecha y hora */
		time_t tim = time(NULL);
		pfs_fat32_utils_fillTime(&(shortEntry.DIR_WrtDate), &(shortEntry.DIR_WrtTime), tim);
		pfs_fat32_utils_fillTime(&(shortEntry.DIR_CrtDate), &(shortEntry.DIR_CrtTime), tim);
		shortEntry.DIR_LstAccDate = shortEntry.DIR_CrtDate;
		shortEntry.DIR_CrtTimeTenth = 0;

		/* Completamos el resto de los atributos */
		shortEntry.DIR_Attr = 0; 								 //Archivo comun y silvestre
		longEntry.LDIR_Attr = FAT_32_ATTR_LONG_NAME;			 //Archivo comun y silvestre, es long entry
		shortEntry.DIR_FileSize = 0; 							 //Por ahora esta vacio
		shortEntry.DIR_FstClusHI = shortEntry.DIR_FstClusLO = 0; //Como esta vacio, no tiene contenido

		/* Buscamos 64 bytes contiguos libres dentro del cluster
		 * 32 para el LongDirEntry y 32 para el DirEntry
		 */

		DiskSector sector = pfs_endpoint_callCachedGetSector(destination->currentSector , destination);

		while( freeCount < 2 ) {
			do {
				memcpy(&auxEntry , sector.sectorContent + offset , FAT_32_DIR_ENTRY_SIZE);
				offset += 32;

				if ( FAT_32_DIRENT_ISFREE(auxEntry.LDIR_Ord) ){
					freeCount++;
					break;
				}

				if( offset >= v->bps ){
					if ( pfs_fat32_utils_isLastSectorFromCluster(v , sector.sectorNumber) ) {
						if ( FAT_32_ISEOC(destination->nextCluster)){
							uint32_t newCluster = pfs_fat32_utils_allocateNewCluster(v , destination->nextCluster);
							sectorId = pfs_fat_utils_getFirstSectorOfCluster(v , newCluster);
							sector = pfs_endpoint_callCachedGetSector(sectorId , destination);
							freeCount = 2;
							offset = 64;
							break;
						} else {
							uint32_t sectorId = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v , destination->nextCluster);
							sector = pfs_endpoint_callCachedGetSector(sectorId , destination);
							offset = 0;
						}
					} else {
						sector = pfs_endpoint_callCachedGetSector(sector.sectorNumber + 1 , destination);
						offset = 0;
					}
				}
			} while ( ! FAT_32_DIRENT_ISFREE(auxEntry.LDIR_Ord) ); //Sale cuando encontro 32 bytes libres

			memcpy(&auxEntry , sector.sectorContent + offset , FAT_32_DIR_ENTRY_SIZE); //Se buscan los siguientes 32 bytes libres
			offset += 32;

			if ( FAT_32_DIRENT_ISFREE(auxEntry.LDIR_Ord) )
				freeCount++; //Listo, tenemos 64 bytes contiguos libres
			else
				freeCount = 0; // Solo se encontraron 32 bytes libres - hay que empezar de nuevo
		}

		/* Escribimos los entries en el sector y lo mandamos a escribir en el disco */

		uint16_t longOffset = offset - FAT_32_BLOCK_ENTRY_SIZE;
		uint16_t shortOffset = offset - FAT_32_DIR_ENTRY_SIZE;

		memcpy(sector.sectorContent + longOffset , &longEntry , FAT_32_DIR_ENTRY_SIZE);
		memcpy(sector.sectorContent + shortOffset , &shortEntry , FAT_32_DIR_ENTRY_SIZE);

		pfs_endpoint_callPutSector(sector , NULL);

		return EXIT_SUCCESS;
	}

	int8_t pfs_fat32_mkdir(Volume * v , FatFile * destination , char * filename){
		DirEntry shortEntry;
		LongDirEntry longEntry , auxEntry;
		uint32_t sectorId = destination->currentSector;
		uint16_t offset = 0;
		uint8_t freeCount = 0;

		/* Completamos los campos del nombre */
		longEntry.LDIR_Ord = 0x41; 						//Ultimo (y unico) long entry
		pfs_fat32_utils_loadLongEntryFilename(&longEntry , filename);
		pfs_fat32_utils_loadEntryFilename(&shortEntry , filename);


		/* Completamos los campos de fecha y hora */
		time_t tim = time(NULL);
		pfs_fat32_utils_fillTime(&(shortEntry.DIR_WrtDate), &(shortEntry.DIR_WrtTime), tim);
		pfs_fat32_utils_fillTime(&(shortEntry.DIR_CrtDate), &(shortEntry.DIR_CrtTime), tim);
		shortEntry.DIR_LstAccDate = shortEntry.DIR_CrtDate;
		shortEntry.DIR_CrtTimeTenth = 0;


		/* Completamos el resto de los atributos */
		uint32_t newCluster = pfs_fat32_utils_assignCluster(v);
		pfs_fat_setFirstClusterToDirEntry(&shortEntry , newCluster);	//Seteamos LO + HI
		shortEntry.DIR_Attr = FAT_32_ATTR_DIRECTORY; 			 		//Es un directorio
		longEntry.LDIR_Attr = FAT_32_ATTR_LONG_NAME;					 //Es un directorio, es long entry
		shortEntry.DIR_FileSize = 0; 							 		//Los directorios siempre estan en 0


		/* Buscamos 64 bytes contiguos libres dentro del cluster
		 * 32 para el LongDirEntry y 32 para el DirEntry
		 */

		DiskSector sector = pfs_endpoint_callCachedGetSector(destination->currentSector , destination);

		while( freeCount < 2 ) {
			do {
				memcpy(&auxEntry , sector.sectorContent + offset , FAT_32_DIR_ENTRY_SIZE);
				offset += 32;

				if ( FAT_32_DIRENT_ISFREE(auxEntry.LDIR_Ord) ){
					freeCount++;
					break;
				}

				if( offset >= v->bps ){
					if ( pfs_fat32_utils_isLastSectorFromCluster(v , sector.sectorNumber) ) {
						if ( FAT_32_ISEOC(destination->nextCluster)){
							uint32_t newCluster = pfs_fat32_utils_allocateNewCluster(v,destination->nextCluster);
							sectorId = pfs_fat_utils_getFirstSectorOfCluster(v , newCluster);
							sector = pfs_endpoint_callCachedGetSector(sectorId , destination);
							offset = 0;
						} else {
							sectorId = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v , destination->nextCluster);
							sector = pfs_endpoint_callCachedGetSector(sectorId , destination);
							offset = 0;
						}
					} else {
						sector = pfs_endpoint_callCachedGetSector(++sectorId , destination);
						offset = 0;
					}
				}
			} while ( ! FAT_32_DIRENT_ISFREE(auxEntry.LDIR_Ord) ); //Sale cuando encontro 32 bytes libres

			memcpy(&auxEntry , sector.sectorContent + offset , FAT_32_DIR_ENTRY_SIZE); //Se buscan los siguientes 32 bytes libres
			offset += 32;

			if ( FAT_32_DIRENT_ISFREE(auxEntry.LDIR_Ord) )
				freeCount++; //Listo, tenemos 64 bytes contiguos libres
			else
				freeCount = 0; // Solo se encontraron 32 bytes libres - hay que empezar de nuevo
		}

		/* Escribimos los entries en el sector y lo mandamos a escribir en el disco */

		uint16_t longOffset = offset - FAT_32_BLOCK_ENTRY_SIZE;
		uint16_t shortOffset = offset - FAT_32_DIR_ENTRY_SIZE;

		memcpy(sector.sectorContent + longOffset , &longEntry , FAT_32_DIR_ENTRY_SIZE);
		memcpy(sector.sectorContent + shortOffset , &shortEntry , FAT_32_DIR_ENTRY_SIZE);

		pfs_endpoint_callPutSector(sector , NULL);


		/* Por ultimo, creamos entradas . y .. */
		DirEntry punto , puntoPunto;
		sectorId = pfs_fat_utils_getFirstSectorOfCluster(v , newCluster);
		sector = pfs_endpoint_callCachedGetSector(sectorId , destination);

		pfs_fat32_utils_fillDotEntry(&punto , &shortEntry);
		memcpy( sector.sectorContent ,  &punto , FAT_32_DIR_ENTRY_SIZE);

		if (destination->dirType == 0)
			pfs_fat32_utils_fillDotDotEntry(&puntoPunto , NULL);
		else
			pfs_fat32_utils_fillDotDotEntry(&puntoPunto , &(destination->shortEntry));
		memcpy( sector.sectorContent + 32, &puntoPunto , FAT_32_DIR_ENTRY_SIZE);

		pfs_endpoint_callPutSector(sector , NULL);

		return EXIT_SUCCESS;
	}


	uint8_t pfs_fat32_truncate(Volume * v, FatFile * f, off_t newsize){

		DiskSector diskSector;
		DirEntry sDirEntry;
		uint8_t setEndOfData = FAT_32_ENDOFDIR;
		uint32_t setFatEntryFree = FAT_32_FAT_FREE_ENTRY;
		uint32_t clusterCount = 0;
		uint32_t currentCluster = f->nextCluster;

		if(f->nextCluster == v->root) return -1; //No se puede truncar el root

		//Actualizar fecha y hora de modificacion
		time_t tim = time(NULL);
		pfs_fat32_utils_fillTime(&(f->shortEntry.DIR_WrtDate), &(f->shortEntry.DIR_WrtTime), tim);
		f->shortEntry.DIR_LstAccDate = f->shortEntry.DIR_WrtDate;

		//Extendiendo el archivo
		if(newsize > f->shortEntry.DIR_FileSize) {
			pfs_fat32_utils_extendFileTruncate( v , f , newsize);
			return 0;
		}

		//Actualizar tamanio de archivo
		uint32_t dirSector = pfs_fat_utils_getFirstSectorOfCluster(v, f->source);
		uint32_t secInClus = f->sourceOffset / v->bps;
		dirSector = dirSector + secInClus;
		uint32_t sectorOffset = f->sourceOffset % v->bps;

		if(FAT_32_LDIR_ISLONG(f->longEntry.LDIR_Attr)){
			if(sectorOffset + FAT_32_DIR_ENTRY_SIZE >= v->bps){
				if(pfs_fat32_utils_isLastSectorFromCluster(v, dirSector)){
					dirSector = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v, f->source);
				}
				else{
					dirSector++;
				}
				sectorOffset = 0;
			}
			else{
				sectorOffset += FAT_32_DIR_ENTRY_SIZE;
			}
		}

		diskSector = pfs_endpoint_callCachedGetSector(dirSector , f);
		memcpy(&sDirEntry, diskSector.sectorContent + sectorOffset, sizeof(DirEntry));
		sDirEntry.DIR_FileSize = newsize;
		memcpy(diskSector.sectorContent + sectorOffset, &sDirEntry, sizeof(DirEntry));
		pfs_endpoint_callPutSector(diskSector , NULL);

		//Obtenemos el cluster a partir del cual se va a truncar
		while(clusterCount * v->bpc < newsize){
			clusterCount++;
			currentCluster = pfs_fat32_utils_getNextClusterInChain(v, currentCluster);
		}

		uint32_t offsetInCluster = newsize % v->bpc;
		uint32_t offsetInSector = offsetInCluster % v->bps;
		uint32_t sectorInCluster = offsetInCluster / v->bps;
		uint32_t sector = pfs_fat_utils_getFirstSectorOfCluster(v, currentCluster) + sectorInCluster;

		//Borrado del primer cluster considerando que sea a partir del medio de un sector
		diskSector = pfs_endpoint_callCachedGetSector(sector , f);
		if(v->bps - offsetInSector == 512){
			memset(diskSector.sectorContent + offsetInSector, setEndOfData, v->bps - offsetInSector);
		}
		else{
			memset(diskSector.sectorContent + offsetInSector + 1, setEndOfData, v->bps - offsetInSector);
		}

		pfs_endpoint_callPutSector(diskSector , f);

		uint8_t absoluteSectorNumber = sector % 8 + 1;

		for(; absoluteSectorNumber != 8; absoluteSectorNumber++){
			sector++;
			diskSector = pfs_endpoint_callCachedGetSector(sector , f);
			memset(diskSector.sectorContent, setEndOfData, v->bps);
			pfs_endpoint_callPutSector(diskSector , f);
		}

		if( offsetInCluster == 0)
			v->freeClusterCount++;


		//Seteo de EOC a la EntryFat correspondiente al cluster obtenido y borrado de datos
		uint32_t nextCluster;
		nextCluster = pfs_fat32_utils_getNextClusterInChain(v, currentCluster);
		pfs_fat32_utils_markEndOfChain(v , currentCluster);

		if(FAT_32_ISEOC(nextCluster)){
			return EXIT_SUCCESS;
		}
		else{
			uint32_t fatEntryOffset;
			while(!FAT_32_ISEOC(nextCluster)){
				sector = pfs_fat_utils_getFirstSectorOfCluster(v, nextCluster);
				for(absoluteSectorNumber = 0; absoluteSectorNumber != 8; absoluteSectorNumber++){
					diskSector = pfs_endpoint_callCachedGetSector(sector , f);
					memset(diskSector.sectorContent, setEndOfData, v->bps);
					pfs_endpoint_callPutSector(diskSector , f);
					sector++;
				}
				sector = pfs_fat_utils_getFatEntrySector(v, nextCluster);
				diskSector = pfs_endpoint_callCachedGetSector(sector , NULL);
				fatEntryOffset = pfs_fat_utils_getFatEntryOffset(v, nextCluster);

				memcpy(&nextCluster, diskSector.sectorContent + fatEntryOffset, sizeof(uint32_t));
				memcpy(diskSector.sectorContent + fatEntryOffset, &setFatEntryFree, sizeof(uint32_t));
				pfs_endpoint_callPutSector(diskSector , NULL);

				v->freeClusterCount++;
			}
			return EXIT_SUCCESS;
		}

	return EXIT_SUCCESS;
	}


	void pfs_fat32_moveFile(Volume * v, FatFile * oldFatFile, const char * path, const char * newpath){
		uint8_t freeCount = 0;
		uint32_t offset = FAT_32_BLOCK_ENTRY_SIZE;
		LongDirEntry auxEntry;

		char newDirName[66];
		char dest[14];
		pfs_fat32_utils_getDirNameFromPath(newpath, newDirName);
		FatFile * newFatFile = pfs_fat32_open(newDirName);

		uint32_t sectorIdToWrite = pfs_fat_utils_getFirstSectorOfCluster(v, newFatFile->nextCluster);
		uint32_t sectorId = sectorIdToWrite;
		DiskSector sector = pfs_endpoint_callCachedGetSector(sectorId , newFatFile);

		while( freeCount < 2 ) {
			do {
				memcpy(&auxEntry , sector.sectorContent + offset , FAT_32_DIR_ENTRY_SIZE);
				offset += 32;

				if ( FAT_32_DIRENT_ISFREE(auxEntry.LDIR_Ord) ){
					freeCount++;
					break;
				}

				if( offset >= v->bps ){
					if ( pfs_fat32_utils_isLastSectorFromCluster(v , sector.sectorNumber) ) {
						if ( FAT_32_ISEOC(newFatFile->nextCluster)){
							uint32_t newCluster = pfs_fat32_utils_allocateNewCluster(v , newFatFile->nextCluster);
							sectorId = pfs_fat_utils_getFirstSectorOfCluster(v , newCluster);
							sector = pfs_endpoint_callCachedGetSector(sectorId , newFatFile);
							freeCount = 2;
							offset = 64;
							break;
						} else {
							uint32_t sectorId = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v , newFatFile->nextCluster);
							sector = pfs_endpoint_callCachedGetSector(sectorId , newFatFile);
							offset = 0;
						}
					} else {
						sector = pfs_endpoint_callCachedGetSector(sector.sectorNumber + 1 , newFatFile);
						offset = 0;
					}
				}
			} while ( ! FAT_32_DIRENT_ISFREE(auxEntry.LDIR_Ord) ); //Sale cuando encontro 32 bytes libres

			memcpy(&auxEntry , sector.sectorContent + offset , FAT_32_DIR_ENTRY_SIZE); //Se buscan los siguientes 32 bytes libres
			offset += 32;

			if ( FAT_32_DIRENT_ISFREE(auxEntry.LDIR_Ord) )
				freeCount++; //Listo, tenemos 64 bytes contiguos libres
			else
				freeCount = 0; // Solo se encontraron 32 bytes libres - hay que empezar de nuevo
		}
		uint16_t longOffset = offset - FAT_32_BLOCK_ENTRY_SIZE;
		uint16_t shortOffset = offset - FAT_32_DIR_ENTRY_SIZE;

		pfs_fat32_utils_getFileNameFromPath(newpath, dest);
		pfs_fat32_utils_loadEntryFilename(&oldFatFile->shortEntry , dest);
		pfs_fat32_utils_loadLongEntryFilename(&oldFatFile->longEntry ,dest);

		if(sectorId != sectorIdToWrite) sector = pfs_endpoint_callCachedGetSector(sectorIdToWrite , newFatFile);

		if(FAT_32_LDIR_ISLONG(oldFatFile->longEntry.LDIR_Attr)){
			memcpy(sector.sectorContent + longOffset , &oldFatFile->longEntry , FAT_32_DIR_ENTRY_SIZE);
			if(longOffset + FAT_32_DIR_ENTRY_SIZE >= v->bps){
				if(pfs_fat32_utils_isLastSectorFromCluster(v, sectorId)){
					sectorId = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v, oldFatFile->source);
				}
				else{
					sectorId++;
				}
				sector = pfs_endpoint_callCachedGetSector(sectorId , oldFatFile);
				shortOffset = 0;
			}

			memcpy(sector.sectorContent + shortOffset , &oldFatFile->shortEntry , FAT_32_DIR_ENTRY_SIZE);
		}
		else{
			shortOffset -= FAT_32_DIR_ENTRY_SIZE;
			memcpy(sector.sectorContent + shortOffset , &oldFatFile->shortEntry , FAT_32_DIR_ENTRY_SIZE);
		}
		pfs_endpoint_callPutSector(sector , NULL);

		uint32_t sectorNumberInCluster = oldFatFile->sourceOffset / v->bps;
		uint32_t sectorToDelete = pfs_fat_utils_getFirstSectorOfCluster(v , oldFatFile->source);
		DiskSector diskSectorToDelete = pfs_endpoint_callCachedGetSector(sectorToDelete + sectorNumberInCluster , oldFatFile);

		pfs_fat32_unlink_dirEntry(v, oldFatFile, diskSectorToDelete);
	}


	void pfs_fat32_rename(Volume * v, FatFile * fatFile, char * dest){

		uint32_t sectorInCluster = fatFile->sourceOffset / v->bps;
		uint32_t sectorOffset = fatFile->sourceOffset % v->bps;
		uint32_t sector = pfs_fat_utils_getFirstSectorOfCluster(v, fatFile->source);
		sector = sector + sectorInCluster;
		DiskSector diskSector = pfs_endpoint_callCachedGetSector(sector , fatFile);

		pfs_fat32_utils_loadEntryFilename(&fatFile->shortEntry , dest);
		if(FAT_32_LDIR_ISLONG(fatFile->longEntry.LDIR_Attr)){
			pfs_fat32_utils_loadLongEntryFilename(&fatFile->longEntry ,dest);
			memcpy(diskSector.sectorContent + sectorOffset, &fatFile->longEntry, sizeof(LongDirEntry));
			if(sectorOffset + FAT_32_DIR_ENTRY_SIZE >= 512){
				if(pfs_fat32_utils_isLastSectorFromCluster(v, sector)){
					sector = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v, fatFile->source);
				}
				else{
					sector++;
				}
				DiskSector auxDiskSector = pfs_endpoint_callCachedGetSector(sector , fatFile);
				memcpy(auxDiskSector.sectorContent, &fatFile->shortEntry, sizeof(DirEntry));
				pfs_endpoint_callPutSector(auxDiskSector , NULL);
			}
			else{
				memcpy(diskSector.sectorContent + sectorOffset + FAT_32_DIR_ENTRY_SIZE, &fatFile->shortEntry, sizeof(DirEntry));
			}
		}
		else{
			memcpy(diskSector.sectorContent + sectorOffset, &fatFile->shortEntry, sizeof(DirEntry));
		}
		pfs_endpoint_callPutSector(diskSector , NULL);
	}


	void pfs_fat32_flush(Volume * v){
		DiskSector sector = pfs_endpoint_callCachedGetSector(1 , NULL);
		uint32_t nextFree = v->nextFreeCluster - 1;
		memcpy(sector.sectorContent + 488 , &(v->freeClusterCount) , sizeof(uint32_t));
		memcpy(sector.sectorContent + 492 , &(nextFree) , sizeof(uint32_t));
		pfs_endpoint_callPutSector(sector , NULL);
	}



