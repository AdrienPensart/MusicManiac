#include "wrap.hpp"
#include "MusicFileSystem.hpp"

MusicFileSystem * mfs = 0;

int wrap_getattr(const char *path, struct stat *statbuf) {
    return mfs->Getattr(path, statbuf);
}
int wrap_readlink(const char *path, char *link, size_t size) {
    return mfs->Readlink(path, link, size);
}
int wrap_mknod(const char *path, mode_t mode, dev_t dev) {
    return mfs->Mknod(path, mode, dev);
}
int wrap_mkdir(const char *path, mode_t mode) {
    return mfs->Mkdir(path, mode);
}
int wrap_unlink(const char *path) {
    return mfs->Unlink(path);
}
int wrap_rmdir(const char *path) {
    return mfs->Rmdir(path);
}
int wrap_symlink(const char *path, const char *link) {
    return mfs->Symlink(path, link);
}
int wrap_rename(const char *path, const char *newpath) {
    return mfs->Rename(path, newpath);
}
int wrap_link(const char *path, const char *newpath) {
    return mfs->Link(path, newpath);
}
int wrap_chmod(const char *path, mode_t mode) {
    return mfs->Chmod(path, mode);
}
int wrap_chown(const char *path, uid_t uid, gid_t gid) {
    return mfs->Chown(path, uid, gid);
}
int wrap_truncate(const char *path, off_t newSize) {
    return mfs->Truncate(path, newSize);
}
int wrap_fallocate(const char *path, int mode, off_t offset, off_t length, struct fuse_file_info *fi){
    return mfs->Fallocate(path, mode, offset, length, fi);
}
int wrap_utimens(const char *path, const struct timespec ts[2]){
    return mfs->Utimens(path, ts);
}
int wrap_utime(const char *path, struct utimbuf *ubuf) {
    return mfs->Utime(path, ubuf);
}
int wrap_open(const char *path, struct fuse_file_info *fileInfo) {
    return mfs->Open(path, fileInfo);
}
int wrap_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    return mfs->Read(path, buf, size, offset, fileInfo);
}
int wrap_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    return mfs->Write(path, buf, size, offset, fileInfo);
}
int wrap_statfs(const char *path, struct statvfs *statInfo) {
    return mfs->Statfs(path, statInfo);
}
int wrap_flush(const char *path, struct fuse_file_info *fileInfo) {
    return mfs->Flush(path, fileInfo);
}
int wrap_release(const char *path, struct fuse_file_info *fileInfo) {
    return mfs->Release(path, fileInfo);
}
int wrap_fsync(const char *path, int datasync, struct fuse_file_info *fi) {
    return mfs->Fsync(path, datasync, fi);
}
int wrap_setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
    return mfs->Setxattr(path, name, value, size, flags);
}
int wrap_getxattr(const char *path, const char *name, char *value, size_t size) {
    return mfs->Getxattr(path, name, value, size);
}
int wrap_listxattr(const char *path, char *list, size_t size) {
    return mfs->Listxattr(path, list, size);
}
int wrap_removexattr(const char *path, const char *name) {
    return mfs->Removexattr(path, name);
}
int wrap_opendir(const char *path, struct fuse_file_info *fileInfo) {
    return mfs->Opendir(path, fileInfo);
}
int wrap_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo) {
    return mfs->Readdir(path, buf, filler, offset, fileInfo);
}
int wrap_releasedir(const char *path, struct fuse_file_info *fileInfo) {
    return mfs->Releasedir(path, fileInfo);
}
int wrap_fsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo) {
    return mfs->Fsyncdir(path, datasync, fileInfo);
}
void * wrap_init(struct fuse_conn_info *conn) {
    return mfs->Init(conn);
}
