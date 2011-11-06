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
	  .getdir = NULL, //getdir aparentemente esta deprecated
	  .mkdir = pfs_fuse_mkdir,
	  .unlink = pfs_fuse_unlink,
	  .rmdir = pfs_fuse_rmdir,
	  .rename = pfs_fuse_rename,
	  .open = pfs_fuse_open,
	  .read = pfs_fuse_read,
	  .write = pfs_fuse_write,
	  .flush = pfs_fuse_flush,
	  .release = pfs_fuse_release,
	  .readdir = pfs_fuse_readdir,
	  .mknod = pfs_fuse_mknod,
	  .truncate = pfs_fuse_truncate,
	};




	void pfs_fuse_launchFuse(int argc , char * argv[] ){
		struct fuse_operations grage_oper;
		fuse_main(argc, argv, &grage_oper);
	}





	int pfs_fuse_mknod(const char *path, mode_t mode, dev_t dev){
		return EXIT_SUCCESS;
	}

	int pfs_fuse_open(const char *path, struct fuse_file_info *fi){

		fi->fh = pfs_fat32_open(path);

		if(fi->fh != NULL)
			return EXIT_SUCCESS;
		else
			return -ENOENT;
	}

	int pfs_fuse_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
		return EXIT_SUCCESS;
	}

	int pfs_fuse_write(const char *path, const char *buf, size_t size, off_t offset,
			struct fuse_file_info *fi){
		return EXIT_SUCCESS;
	}

	int pfs_fuse_flush(const char *path, struct fuse_file_info *fi){
		return EXIT_SUCCESS;
	}

	int pfs_fuse_release(const char *path, struct fuse_file_info *fi){
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
		return EXIT_SUCCESS;
	}

	int pfs_fuse_readdir(const char * path , void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){

		FatFile * fatFile = pfs_fat32_open(path);
		uint8_t result;
		struct dirent * de = NULL;
		Volume * v = pfs_state_getVolume();
		struct stat st;

		if(fatFile == NULL)
			return -ENOENT;

		while( (result = pfs_fat32_readDirectory(de , fatFile , v)) == 0 ){
	        memset(&st, 0, sizeof(st));
	        st.st_ino = de->d_ino;
			if (de->d_type == DT_DIR) {
				st.st_mode = S_IFDIR;
			} else st.st_mode = S_IFREG;

	        if (filler(buf, de->d_name, &st, 0))
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
		return EXIT_SUCCESS;
	}

	int pfs_fuse_rename(const char * path, const char * newpath){
		return EXIT_SUCCESS;
	}
