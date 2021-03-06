#include "MusicFileSystem.hpp"
#include "fs/wrap.hpp"
#include <iostream>
#include <memory>
using namespace std;

#define RETURN_ERRNO(x) (x) == 0 ? 0 : -errno

MusicFileSystem::MusicFileSystem(Collection& _collection) :
    collection(_collection),
    playlist("whatever") {
    std::set<std::string> without { "cutoff", "nosync" };
    playlist.setRating(4);
    playlist.setWithout(without);
}

int MusicFileSystem::run(std::string mountpoint){
    mfs = this;
    const int argc = 4;
    std::unique_ptr<char*[]> argv2 (new char * [argc]);
    argv2[0] = strdup("musicmaniac");
    argv2[1] = strdup(mountpoint.c_str());
    argv2[2] = strdup("-f");
    argv2[3] = strdup("-s");
    int fuse_stat = fuse_main(argc, argv2.get(), &musicfs_oper, NULL);
    cout << "fuse_main returned " << fuse_stat << "\n";
    return fuse_stat;
}

void MusicFileSystem::AbsPath(char dest[PATH_MAX], const char *path) {
    strcpy(dest, collection.getRoot().c_str());
	strncat(dest, path, PATH_MAX);
	//printf("translated path: %s to %s\n", path, dest);
}

int MusicFileSystem::Getattr(const char *path, struct stat *stbuf) {
    char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	printf("getattr(%s)\n", fullPath);
    return RETURN_ERRNO(lstat(fullPath, stbuf));
}

int MusicFileSystem::Readlink(const char *path, char *link, size_t size) {
	printf("readlink(path=%s, link=%s, size=%d)\n", path, link, (int)size);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(readlink(fullPath, link, size));
}

int MusicFileSystem::Mknod(const char *path, mode_t mode, dev_t dev) {
	printf("mknod(path=%s, mode=%d)\n", path, mode);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	
	//handles creating FIFOs, regular files, etc...
	return RETURN_ERRNO(mknod(fullPath, mode, dev));
}

int MusicFileSystem::Mkdir(const char *path, mode_t mode) {
	printf("**mkdir(path=%s, mode=%d)\n", path, (int)mode);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(mkdir(fullPath, mode));
}

int MusicFileSystem::Unlink(const char *path) {
	printf("unlink(path=%s\n)", path);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(unlink(fullPath));
}

int MusicFileSystem::Rmdir(const char *path) {
	printf("rmkdir(path=%s\n)", path);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(rmdir(fullPath));
}

int MusicFileSystem::Symlink(const char *path, const char *link) {
	printf("symlink(path=%s, link=%s)\n", path, link);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(symlink(fullPath, link));
}

int MusicFileSystem::Rename(const char *path, const char *newpath) {
	printf("rename(path=%s, newPath=%s)\n", path, newpath);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(rename(fullPath, newpath));
}

int MusicFileSystem::Link(const char *path, const char *newpath) {
	printf("link(path=%s, newPath=%s)\n", path, newpath);
	char fullPath[PATH_MAX];
	char fullNewPath[PATH_MAX];
	AbsPath(fullPath, path);
	AbsPath(fullNewPath, newpath);
	return RETURN_ERRNO(link(fullPath, fullNewPath));
}

int MusicFileSystem::Chmod(const char *path, mode_t mode) {
	printf("chmod(path=%s, mode=%d)\n", path, mode);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(chmod(fullPath, mode));
}

int MusicFileSystem::Chown(const char *path, uid_t uid, gid_t gid) {
	printf("chown(path=%s, uid=%d, gid=%d)\n", path, (int)uid, (int)gid);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(chown(fullPath, uid, gid));
}

int MusicFileSystem::Truncate(const char *path, off_t newSize) {
	printf("truncate(path=%s, newSize=%d\n", path, (int)newSize);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(truncate(fullPath, newSize));
}

int MusicFileSystem::Utimens(const char *path, const struct timespec ts[2]) {
    int res;
    /* don't use utime/utimes since they follow symlinks */
    res = utimensat(0, path, ts, AT_SYMLINK_NOFOLLOW);
    if (res == -1)
            return -errno;
    return 0;
}

int MusicFileSystem::Fallocate(const char *path, int mode, off_t offset, off_t length, struct fuse_file_info *fi){
    int fd;
    int res;
    (void) fi;
    if (mode)
            return -EOPNOTSUPP;
    fd = open(path, O_WRONLY);
    if (fd == -1)
            return -errno;
    res = -posix_fallocate(fd, offset, length);
    close(fd);
    return res;
}

int MusicFileSystem::Utime(const char *path, struct utimbuf *ubuf) {
	printf("utime(path=%s)\n", path);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(utime(fullPath, ubuf));
}

int MusicFileSystem::Open(const char *path, struct fuse_file_info *fileInfo) {
	printf("open(path=%s)\n", path);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	fileInfo->fh = open(fullPath, fileInfo->flags);
	return 0;
}

int MusicFileSystem::Read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    printf("read(path=%s, size=%d, offset=%d)\n", path, (int)size, (int)offset);
    return pread(fileInfo->fh, buf, size, offset);
}

int MusicFileSystem::Write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
	printf("write(path=%s, size=%d, offset=%d)\n", path, (int)size, (int)offset);
	return RETURN_ERRNO(pwrite(fileInfo->fh, buf, size, offset));
}

int MusicFileSystem::Statfs(const char *path, struct statvfs *statInfo) {
	printf("statfs(path=%s)\n", path);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(statvfs(fullPath, statInfo));
}

int MusicFileSystem::Flush(const char *path, struct fuse_file_info *fileInfo) {
	printf("flush(path=%s)\n", path);
	//noop because we don't maintain our own buffers
	return 0;
}

int MusicFileSystem::Release(const char *path, struct fuse_file_info *fileInfo) {
	printf("release(path=%s)\n", path);
	return 0;
}

int MusicFileSystem::Fsync(const char *path, int datasync, struct fuse_file_info *fi) {
	printf("fsync(path=%s, datasync=%d\n", path, datasync);
	if(datasync) {
		//sync data only
		return RETURN_ERRNO(fdatasync(fi->fh));
	} else {
		//sync data + file metadata
		return RETURN_ERRNO(fsync(fi->fh));
	}
}

int MusicFileSystem::Setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
	printf("setxattr(path=%s, name=%s, value=%s, size=%d, flags=%d\n",
		path, name, value, (int)size, flags);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(lsetxattr(fullPath, name, value, size, flags));
}

int MusicFileSystem::Getxattr(const char *path, const char *name, char *value, size_t size) {
	printf("getxattr(path=%s, name=%s, size=%d\n", path, name, (int)size);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(getxattr(fullPath, name, value, size));
}

int MusicFileSystem::Listxattr(const char *path, char *list, size_t size) {
	printf("listxattr(path=%s, size=%d)\n", path, (int)size);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(llistxattr(fullPath, list, size));
}

int MusicFileSystem::Removexattr(const char *path, const char *name) {
	printf("removexattry(path=%s, name=%s)\n", path, name);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(lremovexattr(fullPath, name));
}

int MusicFileSystem::Opendir(const char *path, struct fuse_file_info *fileInfo) {
	printf("opendir(path=%s)\n", path);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	DIR *dir = opendir(fullPath);
	fileInfo->fh = (uint64_t)dir;
	return NULL == dir ? -errno : 0;
}

int MusicFileSystem::Readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo) {
	printf("readdir(path=%s, offset=%d)\n", path, (int)offset);
	DIR *dir = (DIR*)fileInfo->fh;

	struct dirent *de = readdir(dir);
	if(NULL == de) {
		return -errno;
	} else {
        do {
            boost::filesystem::path fsroot (collection.getRoot());
            boost::filesystem::path fspath (path);
            boost::filesystem::path fsfile (de->d_name);
            boost::filesystem::path folderpath = fsroot / fspath;
            if(de->d_type == DT_REG){
                boost::filesystem::path full_path = folderpath / fsfile;
                auto musics = collection.getMusics();
                auto playlists = collection.getAllPlaylists();

                Musics::const_iterator m;
                if((m = musics.find(full_path.c_str())) != musics.end()){
                    if(!playlist.conform(m->second)){
                        //cout << full_path << " = file does not conform\n";
                        continue;
                    }
                }
                else if(playlists.find(full_path.c_str()) != playlists.end()) {
                    auto p = playlists[full_path.c_str()];
                    if(!p->size()){
                        continue;
                    } else {
                        // if one file in the playlist does not conform, evict
                        bool conform = true;
                        for(auto music : p->getMusics()){
                            if(!playlist.conform(music)){
                                conform = false;
                            }
                        }
                        if(!conform){
                            continue;
                        }
                    }
                }
                else {
                    continue;
                }
            }
            if(filler(buf, de->d_name, NULL, 0) != 0) {
				return -ENOMEM;
			}
		} while(NULL != (de = readdir(dir)));
	}
	return 0;
}

int MusicFileSystem::Releasedir(const char *path, struct fuse_file_info *fileInfo) {
	printf("releasedir(path=%s)\n", path);
	closedir((DIR*)fileInfo->fh);
	return 0;
}

int MusicFileSystem::Fsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo) {
	return 0;
}

void * MusicFileSystem::Init(struct fuse_conn_info *conn) {
    return NULL;
}

int MusicFileSystem::Truncate(const char *path, off_t offset, struct fuse_file_info *fileInfo) {
	printf("truncate(path=%s, offset=%d)\n", path, (int)offset);
	char fullPath[PATH_MAX];
	AbsPath(fullPath, path);
	return RETURN_ERRNO(ftruncate(fileInfo->fh, offset));
}
