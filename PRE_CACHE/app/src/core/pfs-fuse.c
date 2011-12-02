/*
 * pfs-fuse.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */


#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>

#include <fuse.h>

#include <pfs-fuse.h>
#include "grage-commons.h"
#include "pfs-fat32.h"

	struct fuse_operations grage_oper = {
	  .getattr = pfs_fuse_getattr,			//Revisada
	  .mkdir = pfs_fuse_mkdir,
	  .unlink = pfs_fuse_unlink,			//Revisada
	  .rmdir = pfs_fuse_rmdir,				//Revisada
	  .rename = pfs_fuse_rename,
	  .open = pfs_fuse_open,				//Revisada
	  .read = pfs_fuse_read,				//Revisada
	  .write = pfs_fuse_write,
	  .flush = pfs_fuse_flush,
	  .release = pfs_fuse_release,			//Revisada
	  .releasedir = pfs_fuse_releasedir,
	  .readdir = pfs_fuse_readdir,			//Revisada
	  .mknod = pfs_fuse_mknod,
	  .truncate = pfs_fuse_truncate,
	};


	void pfs_fuse_launchFuse( int argc , char * argv[] ){
		fuse_main(argc, argv, &grage_oper);
	}


	int pfs_fuse_mknod(const char *path, mode_t mode, dev_t dev){
		char filename[14];
		char basedir[128];
		Volume * v = pfs_state_getVolume();

		pfs_fat32_utils_getDirNameFromPath(path,basedir);
		pfs_fat32_utils_getFileNameFromPath(path,filename);

		FatFile * destination = pfs_fat32_open(path);

		if( strlen(filename) > 13 || destination != NULL ) //Si no es NULL, ya existe un archivo con ese nombre
			return -EXIT_FAILURE;

		destination = pfs_fat32_open(basedir);
		if (destination-> dirType == 0 ){
			pfs_fat32_mknod(v , destination , filename);
		} else if ( destination->shortEntry.DIR_Attr != FAT_32_ATTR_DIRECTORY ) {//Intenta crear un archivo adentro de otro
			return EXIT_FAILURE;
		} else {
			pfs_fat32_mknod(v , destination , filename);
		}

		return EXIT_SUCCESS;
	}

	int pfs_fuse_open(const char *path, struct fuse_file_info *fi){

		FatFile * file = pfs_fat32_open(path);

		if(file != NULL){
			fi->fh = (long)file;
			return EXIT_SUCCESS;
		} else
			return -ENOENT;

	}

	int pfs_fuse_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){

		Volume * volume = pfs_state_getVolume();
		FatFile * file = (FatFile *)fi->fh;

		uint32_t filesize = file->shortEntry.DIR_FileSize;
		if ( (offset + size) > filesize )
			size = filesize - offset;

		if (size == 0)
			return size;

		uint8_t result = pfs_fat32_utils_seek(volume , file , offset , filesize);
		if ( result == EXIT_FAILURE )
			return -ESPIPE;

		uint32_t read = pfs_fat32_read(volume , file , buf , size);

		return read;
	}

	int pfs_fuse_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){

		uint32_t result;
		Volume * volume = pfs_state_getVolume();
		FatFile * file = (FatFile *)fi->fh;
		uint32_t filesize= file->shortEntry.DIR_FileSize;
		uint32_t total = size + offset;
		uint32_t amount = size + offset - filesize;

		if ( size == 0)
			return size;

		uint32_t usedBytes = filesize % volume->bpc;
		if ( amount > volume->bpc - usedBytes || usedBytes == 0) {
			pfs_fat32_utils_extendFile(volume , file , size);

		} else {
			filesize = total;
			pfs_fat32_utils_updateFilesize(volume , file , filesize);
		}

		result = pfs_fat32_utils_seekWrite(volume , file , offset , file->shortEntry.DIR_FileSize);
		if ( result == EXIT_FAILURE )
			return -ESPIPE;

		result = pfs_fat32_write(volume , file , buf , size);

		return result;
	}

	int pfs_fuse_flush(const char *path, struct fuse_file_info *fi){

		Volume * v = pfs_state_getVolume();
		pfs_fat32_flush(v);

		return EXIT_SUCCESS;
	}

	int pfs_fuse_release(const char *path, struct fuse_file_info *fi){

		FatFile * file = (FatFile *)fi->fh;
		if (file != NULL)
			commons_misc_doFreeNull((void**)file);

		return EXIT_SUCCESS;
	}

	int pfs_fuse_releasedir(const char *path, struct fuse_file_info *fi){

		FatFile * file = (FatFile *)fi->fh;
		if (file != NULL)
			commons_misc_doFreeNull((void**)file);

		return EXIT_SUCCESS;
	}

	int pfs_fuse_truncate(const char *path, off_t newsize){
		Volume * v = pfs_state_getVolume();
		FatFile * f = pfs_fat32_open(path);

		if( f == NULL ){
			return -ENOENT;
		}

		uint8_t res = pfs_fat32_truncate(v, f, newsize);
		if(res != 0){
			return -ENOENT;
		}

		return 0;
	}

	int pfs_fuse_unlink(const char * path){

		Volume * v = pfs_state_getVolume();
		FatFile * f = pfs_fat32_open(path);
		if( f == NULL ){
			return -ENOENT;
		}

		pfs_fat32_unlink(v , f);

		return EXIT_SUCCESS;
	}

	int pfs_fuse_mkdir(const char *path, mode_t mode){
		char dirname[14];
		char basedir[128];
		Volume * v = pfs_state_getVolume();

		pfs_fat32_utils_getDirNameFromPath(path,basedir);
		pfs_fat32_utils_getFileNameFromPath(path,dirname);

		FatFile * destination = pfs_fat32_open(path);

		if( strlen(dirname) > 13 || destination != NULL ) //Si no es NULL, ya existe un archivo con ese nombre
			return -EXIT_FAILURE;

		destination = pfs_fat32_open(basedir);
		if ( destination->dirType == 0 ){
			pfs_fat32_mkdir(v , destination , dirname);
		} else if ( destination->shortEntry.DIR_Attr != FAT_32_ATTR_DIRECTORY || destination == NULL){ //Intenta crear un archivo dentro de otro
			return -EXIT_FAILURE;																// o el directorio "padre" no existe
		} else {
			pfs_fat32_mkdir(v , destination , dirname);
		}

		return EXIT_SUCCESS;
	}

	int pfs_fuse_readdir(const char * path , void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){

		uint8_t result;
		struct dirent de;
		struct stat st;

		FatFile * fatFile = pfs_fat32_open(path);
		Volume * v = pfs_state_getVolume();

		if(fatFile == NULL)
			return -ENOENT;

		while( (result = pfs_fat32_readDirectory(&de , fatFile , v)) == 0 ){
	        memset(&st, 0, sizeof(st));
	        st.st_ino = de.d_ino;
			if (de.d_type == DT_DIR) {
				st.st_mode = S_IFDIR;
			} else st.st_mode = S_IFREG;

	        if (filler(buf, de.d_name, &st, 0))
	            break;
		}
		free(fatFile);
		return EXIT_SUCCESS;
	}

	int pfs_fuse_rmdir(const char * path){

		Volume * v = pfs_state_getVolume();
		FatFile * fatFile = pfs_fat32_open(path);
		if (fatFile == NULL || fatFile->shortEntry.DIR_Attr != FAT_32_ATTR_DIRECTORY){
			return -ENOENT;
		}
		if(pfs_fat32_isDirectoryEmpty(v, fatFile)){
			pfs_fat32_rmdir(v,fatFile);
		}
		else
			return -ENOENT;

		return EXIT_SUCCESS;
	}

	int pfs_fuse_getattr(const char *path, struct stat *statbuf){
		Volume * v = pfs_state_getVolume();

		FatFile * file = pfs_fat32_open(path);
		if (file == NULL){
			free(file);
			return -ENOENT;
			//return EXIT_SUCCESS;
		}

		pfs_fat32_utils_fileStat(v , file , statbuf);
		free(file);

		return EXIT_SUCCESS;
	}

	int pfs_fuse_rename(const char * path, const char * newpath){
		FatFile * newFile = pfs_fat32_open(newpath);

		if(newFile != NULL) //Comprobacion de que no exista un archivo con el nuevo nombre
			return -EXIT_FAILURE;

		FatFile * fatFile = pfs_fat32_open(path);
		char dest[14];
		pfs_fat32_utils_getFileNameFromPath(newpath, dest);

		if( strlen(dest) > 13 || fatFile == NULL ) //Comprobacion de que el archivo al cual se le cambia el nombre exista
					return -EXIT_FAILURE;

		Volume * v = pfs_state_getVolume();
		pfs_fat32_rename(v, fatFile, dest);

		return EXIT_SUCCESS;
	}
