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
						offset = -FAT_32_BLOCK_ENTRY_SIZE;

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

		if (commons_string_equals(FAT_32_ROOT_FORWARD_SLASH, (char *)path) || path[1] == 0) {
			return pfs_fat32_utils_openRootDirectory(v);
		} else {
			return pfs_fat32_utils_openNonRootDirectory(path , v);
		}
	}




	int8_t pfs_fat32_readDirectory( struct dirent * direntry , FatFile * file , Volume * volume){
		LongDirEntry lfnentry;
		DirEntry  sfnentry;

		DiskSector diskSector = pfs_endpoint_callGetSector(file->currentSector.sectorNumber);

		if( file->dirEntryOffset >= volume->bps ){
			if(pfs_fat32_utils_isLastSectorFromCluster(volume, file->currentSector.sectorNumber)){
				if ( FAT_32_ISEOC(file->nextCluster) ){
					return EXIT_FAILURE;
				} else {
					file->currentSector.sectorNumber = pfs_fat_utils_getFirstSectorOfCluster(volume,file->nextCluster);
					file->nextCluster = pfs_fat32_utils_getNextClusterInChain(volume, file->nextCluster);
				}
			} else {
				file->currentSector.sectorNumber++;
			}
			diskSector = pfs_endpoint_callGetSector(file->currentSector.sectorNumber);
			file->dirEntryOffset = 0;
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


	uint32_t pfs_fat32_read(Volume * v , FatFile * f , char * buf , size_t size){
		uint32_t bytesRead = 0;
		uint32_t bytesLeft = size;
		uint32_t bytesToRead;

		DiskSector sector = pfs_fat32_utils_getSectorFromNthCluster(f);

		if ( f->sectorByteOffset + size <= v->bps ){	// Lo que se pide para leer + el offset caen dentro de un sector
			memcpy(buf , sector.sectorContent + f->sectorByteOffset , size);
			return bytesRead += size;
		} else {										// El contenido de un sector no alcanza; se lee lo necesario y se sigue
			memcpy(buf , sector.sectorContent + f->sectorByteOffset , v->bps - f->sectorByteOffset);
			bytesRead += v->bps - f->sectorByteOffset;
			bytesLeft -= bytesRead;
		}

		uint32_t current = f->fileAbsoluteClusterNumber;

		while( bytesRead < size ){

			if( pfs_fat32_utils_isLastSectorFromCluster(v , sector.sectorNumber) ){
				uint32_t sectorId = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v , current);
				current = pfs_fat_utils_getClusterBySector(v , sectorId);
				sector = pfs_endpoint_callGetSector(sectorId);
			} else {
				sector = pfs_endpoint_callGetSector(sector.sectorNumber + 1);
			}

			if ( (bytesLeft <= v->bps) || FAT_32_ISEOC(current) ){
				bytesToRead = bytesLeft;
			//} else if ( pfs_fat32_utils_isLastSectorFromCluster(v , sector.sectorNumber) && FAT_32_ISEOC(current) ){
			//	bytesToRead = bytesLeft; //Verificar el uso de FAT_32_ISEOC ---> creo que habria que preguntar por el siguiente cluster
			} else {
				bytesToRead = v->bps;
			}

			memcpy( buf + bytesRead , sector.sectorContent , bytesToRead);
			bytesRead += bytesToRead;
			bytesLeft -= bytesToRead;
		}

		return bytesRead;
	}

	int8_t pfs_fat32_mknod(Volume * v , FatFile * destination , char * filename){
		DirEntry shortEntry;
		LongDirEntry longEntry , auxEntry;
		uint32_t sectorId;
		uint16_t offset = 0;
		uint8_t freeCount = 0;

		/* Completamos los campos del nombre */
		longEntry.LDIR_Ord = 0x40; 						//Ultimo (y unico) long entry
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
		shortEntry.DIR_FileSize = 0; 							 //Por ahora esta vacio
		shortEntry.DIR_FstClusHI = shortEntry.DIR_FstClusLO = 0; //Como esta vacio, no tiene contenido

		/* Buscamos 64 bytes contiguos libres dentro del cluster
		 * 32 para el LongDirEntry y 32 para el DirEntry
		 */

		DiskSector sector = pfs_endpoint_callGetSector(destination->currentSector.sectorNumber);

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
							sector = pfs_endpoint_callGetSector(sectorId);
							freeCount = 2;
							offset = 64;
							break;
						} else {
							uint32_t sectorId = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v , destination->nextCluster);
							sector = pfs_endpoint_callGetSector(sectorId);
							offset = 0;
						}
					} else {
						sector = pfs_endpoint_callGetSector(sector.sectorNumber + 1);
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

		pfs_endpoint_callPutSector(sector);

		return EXIT_SUCCESS;
	}

	int8_t pfs_fat32_mkdir(Volume * v , FatFile * destination , char * filename){
		DirEntry shortEntry;
		LongDirEntry longEntry , auxEntry;
		uint32_t sectorId;
		uint16_t offset = 0;
		uint8_t freeCount = 0;

		/* Completamos los campos del nombre */
		longEntry.LDIR_Ord = 0x40; 						//Ultimo (y unico) long entry
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
		shortEntry.DIR_FileSize = 0; 							 		//Los directorios siempre estan en 0


		/* Buscamos 64 bytes contiguos libres dentro del cluster
		 * 32 para el LongDirEntry y 32 para el DirEntry
		 */

		DiskSector sector = pfs_endpoint_callGetSector(destination->currentSector.sectorNumber);

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
							sector = pfs_endpoint_callGetSector(sectorId);
							offset = 0;
						} else {
							sectorId = pfs_fat32_utils_getFirstSectorFromNextClusterInChain(v , destination->nextCluster);
							sector = pfs_endpoint_callGetSector(sectorId);
							offset = 0;
						}
					} else {
						sector = pfs_endpoint_callGetSector(sector.sectorNumber + 1);
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

		pfs_endpoint_callPutSector(sector);


		/* Por ultimo, creamos entradas . y .. */
		DirEntry punto , puntoPunto;
		sectorId = pfs_fat_utils_getFirstSectorOfCluster(v , newCluster);
		sector = pfs_endpoint_callGetSector(sectorId);

		pfs_fat32_utils_fillDotEntry(&punto , &shortEntry);
		memcpy( sector.sectorContent ,  &punto , FAT_32_DIR_ENTRY_SIZE);

		if (destination->dirType == 0)
			pfs_fat32_utils_fillDotDotEntry(&puntoPunto , NULL);
		else
			pfs_fat32_utils_fillDotDotEntry(&puntoPunto , &(destination->shortEntry));
		memcpy( sector.sectorContent + 32, &puntoPunto , FAT_32_DIR_ENTRY_SIZE);

		pfs_endpoint_callPutSector(sector);

		return EXIT_SUCCESS;
	}

	uint8_t pfs_fat32_utils_truncate(Volume * v, FatFile * f, off_t newsize){

			DiskSector diskSector;
			DirEntry sDirEntry;
			uint8_t result = 1;
			uint8_t setEndOfData = FAT_32_ENDOFDIR;
			uint32_t setFatEntryFree = FAT_32_FAT_FREE_ENTRY;
			uint32_t clusterCount = 0;
			uint32_t currentCluster = f->nextCluster;

			if(newsize > f->shortEntry.DIR_FileSize) return -1;
			if(f->nextCluster == v->root) return -1; //No se puede truncar el root

			//Actualizar tamaño de archivo
			uint32_t dirSector = pfs_fat_utils_getFirstSectorOfCluster(v, f->source);
			diskSector = pfs_endpoint_callGetSector(dirSector);
			memcpy(&sDirEntry, diskSector.sectorContent + (f->sourceOffset + FAT_32_DIR_ENTRY_SIZE), sizeof(DirEntry));
			sDirEntry.DIR_FileSize = newsize;
			memcpy(diskSector.sectorContent + f->sourceOffset + FAT_32_DIR_ENTRY_SIZE, &sDirEntry, sizeof(DirEntry));
			pfs_endpoint_callPutSector(diskSector);

			//Obtenemos el cluster a partir del cual se va a truncar
			while(clusterCount * v->bpc < newsize){
				clusterCount++;
				currentCluster = pfs_fat32_utils_getNextClusterInChain(v, currentCluster);
			}

			uint32_t offsetInCluster = newsize % v->bpc;
			uint32_t offsetInSector = offsetInCluster % v->bps;
			uint32_t sectorInCluster = offsetInCluster / v->bps;
			uint32_t sector = pfs_fat_utils_getFirstSectorOfCluster(v, currentCluster) + sectorInCluster;

			//Seteo de fin de datos en cluster obtenido
			diskSector = pfs_endpoint_callGetSector(sector);
			memcpy(diskSector.sectorContent + offsetInSector + 1, &setEndOfData, sizeof(setEndOfData));
			pfs_endpoint_callPutSector(diskSector);

			//Seteo de EOC a la EntryFat correspondiente al cluster obtenido
			sector = pfs_fat_utils_getFatEntrySector(v, currentCluster);
			diskSector = pfs_endpoint_callGetSector(sector);
			offsetInSector = pfs_fat_utils_getFatEntryOffset(v, currentCluster);
			memcpy(&currentCluster, diskSector.sectorContent + offsetInSector, sizeof(uint32_t));
			memcpy(diskSector.sectorContent + offsetInSector, &setFatEntryFree, sizeof(setFatEntryFree));
			pfs_endpoint_callPutSector(diskSector);

			//Borrado de sectores de datos
			if(FAT_32_ISEOC(currentCluster)){
				return 0;
			}
			else{
				f->nextCluster = currentCluster;
				while(!FAT_32_ISEOC(currentCluster) && result){
							result = pfs_fat32_unlink_FatEntryChain(v , f);
							sector = pfs_fat_utils_getFirstSectorOfCluster(v, currentCluster);
							diskSector = pfs_endpoint_callGetSector(sector);
							memcpy(diskSector.sectorContent, &setEndOfData, sizeof(setEndOfData));
							pfs_endpoint_callPutSector(diskSector);
							currentCluster = pfs_fat32_utils_getNextClusterInChain(v, currentCluster);
				}
				return 0;
			}
		}



