#include "wrap.hpp"
#include "MusicFileSystem.hpp"

int wrap_getattr(const char *path, struct stat *statbuf) {
    return MusicFileSystem::instance().Getattr(path, statbuf);
}
int wrap_readlink(const char *path, char *link, size_t size) {
    return MusicFileSystem::instance().Readlink(path, link, size);
}
int wrap_mknod(const char *path, mode_t mode, dev_t dev) {
    return MusicFileSystem::instance().Mknod(path, mode, dev);
}
int wrap_mkdir(const char *path, mode_t mode) {
    return MusicFileSystem::instance().Mkdir(path, mode);
}
int wrap_unlink(const char *path) {
    return MusicFileSystem::instance().Unlink(path);
}
int wrap_rmdir(const char *path) {
    return MusicFileSystem::instance().Rmdir(path);
}
int wrap_symlink(const char *path, const char *link) {
    return MusicFileSystem::instance().Symlink(path, link);
}
int wrap_rename(const char *path, const char *newpath) {
    return MusicFileSystem::instance().Rename(path, newpath);
}
int wrap_link(const char *path, const char *newpath) {
    return MusicFileSystem::instance().Link(path, newpath);
}
int wrap_chmod(const char *path, mode_t mode) {
    return MusicFileSystem::instance().Chmod(path, mode);
}
int wrap_chown(const char *path, uid_t uid, gid_t gid) {
    return MusicFileSystem::instance().Chown(path, uid, gid);
}
int wrap_truncate(const char *path, off_t newSize) {
    return MusicFileSystem::instance().Truncate(path, newSize);
}
int wrap_fallocate(const char *path, int mode, off_t offset, off_t length, struct fuse_file_info *fi){
    return MusicFileSystem::instance().Fallocate(path, mode, offset, length, fi);
}
int wrap_utimens(const char *path, const struct timespec ts[2]){
    return MusicFileSystem::instance().Utimens(path, ts);
}
int wrap_utime(const char *path, struct utimbuf *ubuf) {
    return MusicFileSystem::instance().Utime(path, ubuf);
}
int wrap_open(const char *path, struct fuse_file_info *fileInfo) {
    return MusicFileSystem::instance().Open(path, fileInfo);
}
int wrap_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    return MusicFileSystem::instance().Read(path, buf, size, offset, fileInfo);
}
int wrap_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    return MusicFileSystem::instance().Write(path, buf, size, offset, fileInfo);
}
int wrap_statfs(const char *path, struct statvfs *statInfo) {
    return MusicFileSystem::instance().Statfs(path, statInfo);
}
int wrap_flush(const char *path, struct fuse_file_info *fileInfo) {
    return MusicFileSystem::instance().Flush(path, fileInfo);
}
int wrap_release(const char *path, struct fuse_file_info *fileInfo) {
    return MusicFileSystem::instance().Release(path, fileInfo);
}
int wrap_fsync(const char *path, int datasync, struct fuse_file_info *fi) {
    return MusicFileSystem::instance().Fsync(path, datasync, fi);
}
int wrap_setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
    return MusicFileSystem::instance().Setxattr(path, name, value, size, flags);
}
int wrap_getxattr(const char *path, const char *name, char *value, size_t size) {
    return MusicFileSystem::instance().Getxattr(path, name, value, size);
}
int wrap_listxattr(const char *path, char *list, size_t size) {
    return MusicFileSystem::instance().Listxattr(path, list, size);
}
int wrap_removexattr(const char *path, const char *name) {
    return MusicFileSystem::instance().Removexattr(path, name);
}
int wrap_opendir(const char *path, struct fuse_file_info *fileInfo) {
    return MusicFileSystem::instance().Opendir(path, fileInfo);
}
int wrap_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo) {
    return MusicFileSystem::instance().Readdir(path, buf, filler, offset, fileInfo);
}
int wrap_releasedir(const char *path, struct fuse_file_info *fileInfo) {
    return MusicFileSystem::instance().Releasedir(path, fileInfo);
}
int wrap_fsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo) {
    return MusicFileSystem::instance().Fsyncdir(path, datasync, fileInfo);
}
void * wrap_init(struct fuse_conn_info *conn) {
    return MusicFileSystem::instance().Init(conn);
}
