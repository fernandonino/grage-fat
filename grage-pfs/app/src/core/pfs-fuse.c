/*
 * pfs-fuse.c
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */


#include <stdlib.h>
#include <stdio.h>

#include <fuse.h>

#include <pfs-fuse.h>

	int pfs_fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi){
		return EXIT_SUCCESS;
	}

	int pfs_fuse_open(const char *path, struct fuse_file_info *fi){
		return EXIT_SUCCESS;
	}

	int pfs_fuse_read(const char *path, char *buf,
			size_t size, off_t offset,
			struct fuse_file_info *fi){
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
		return EXIT_SUCCESS;
	}

	int pfs_fuse_mkdir(const char *path, mode_t mode){
		return EXIT_SUCCESS;
	}

	int pfs_fuse_readdir(const char * path , void *buf, fuse_fill_dir_t filler,
			off_t offset,
			struct fuse_file_info *fi){
		return EXIT_SUCCESS;
	}

	int pfs_fuse_rmdir(const char * path){
		return EXIT_SUCCESS;
	}

	int pfs_fuse_getattr(const char *path, struct stat *statbuf){
		return EXIT_SUCCESS;
	}

	int pfs_fuse_rename(const char * path, const char * newpath){
		return EXIT_SUCCESS;
	}
