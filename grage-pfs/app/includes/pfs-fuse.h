/*
 * pfs-fuse.h
 *
 *  Created on: 15/09/2011
 *      Author: Fernando
 */

#ifndef PFS_FUSE_H_
#define PFS_FUSE_H_

	int pfs_fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi);
	int pfs_fuse_open(const char *path, struct fuse_file_info *fi);
	int pfs_fuse_read(const char *path, char *buf,
			size_t size, off_t offset, struct fuse_file_info *fi);
	int pfs_fuse_write(const char *path, const char *buf, size_t size, off_t offset,
			struct fuse_file_info *fi);
	int pfs_fuse_flush(const char *path, struct fuse_file_info *fi);
	int pfs_fuse_release(const char *path, struct fuse_file_info *fi);
	int pfs_fuse_truncate(const char *path, off_t newsize);
	int pfs_fuse_unlink(const char * path);
	int pfs_fuse_mkdir(const char *path, mode_t mode);
	int pfs_fuse_readdir(const char * path , void *buf, fuse_fill_dir_t filler,
			off_t offset, struct fuse_file_info *fi);
	int pfs_fuse_rmdir(const char * path);
	int pfs_fuse_getattr(const char *path, struct stat *statbuf);
	int pfs_fuse_rename(const char * path, const char * newpath);

#endif /* PFS_FUSE_H_ */
