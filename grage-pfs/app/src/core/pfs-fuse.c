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
	  .getattr = pfs_fuse_getattr,
	  .mkdir = pfs_fuse_mkdir,
	  .unlink = pfs_fuse_unlink,
	  .rmdir = pfs_fuse_rmdir,
	  .rename = pfs_fuse_rename,			//Pendiente
	  .open = pfs_fuse_open,
	  .read = pfs_fuse_read,
	  .write = pfs_fuse_write,				//Pendiente
	  .flush = pfs_fuse_flush,				//Pendiente
	  .release = pfs_fuse_release,
	  .readdir = pfs_fuse_readdir,
	  .mknod = pfs_fuse_mknod,
	  .truncate = pfs_fuse_truncate,		//Pendiente
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
		if (destination-> dirType == 0 )
			pfs_fat32_mknod(v , destination , filename);
		if ( destination->shortEntry.DIR_Attr != FAT_32_ATTR_DIRECTORY ) {//Intenta crear un archivo adentro de otro
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

		uint8_t result = pfs_fat32_utils_seek(volume , file , offset , filesize);
		if ( result == EXIT_FAILURE )
			return -ESPIPE;

		uint32_t read = pfs_fat32_read(volume , file , buf , size);

		return read;
	}

	int pfs_fuse_write(const char *path, const char *buf, size_t size, off_t offset,
			struct fuse_file_info *fi){
		return EXIT_SUCCESS;
	}

	int pfs_fuse_flush(const char *path, struct fuse_file_info *fi){
		return EXIT_SUCCESS;
	}

	int pfs_fuse_release(const char *path, struct fuse_file_info *fi){

		FatFile * file = (FatFile *)fi->fh;
		commons_misc_doFreeNull((void**)file);

		return EXIT_SUCCESS;
	}

	int pfs_fuse_truncate(const char *path, off_t newsize){
		return EXIT_SUCCESS;
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
			return -ENOENT;
		}

		pfs_fat32_utils_fileStat(v , file , statbuf);

		return EXIT_SUCCESS;
	}

	int pfs_fuse_rename(const char * path, const char * newpath){
		return EXIT_SUCCESS;
	}

