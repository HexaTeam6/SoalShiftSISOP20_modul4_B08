#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdlib.h>
  
void logs(int level, char* cmd, char* desc1, char* temp){
    char data[1000];
    char lvl[100];
    char desc2 [100];
    FILE * fPtr;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    sprintf(lvl, level? "WARNING" : "INFO");
    if(strcmp(temp,"")) sprintf(desc2, "::%s", temp);
    
    fPtr = fopen("/home/hexa/fs.log", "a");
    sprintf(data, "%s::%d%d%d-%d:%d:%d::%s::%s%s\n", lvl, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, cmd, desc1, desc2);
    
    if(fPtr == NULL)
    {
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }
    fputs(data, fPtr);
    fclose(fPtr);
}

static const char *dirpath = "/home/hexa/Documents";

static int xmp_getattr(const char *path, struct stat *stbuf) {
    int res;
    char fullpath[1000];
    sprintf(fullpath, "%s%s", dirpath, path);
    res = lstat(fullpath, stbuf);
    if (res == -1) return -errno;
    return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
	
    int fd;
	int res;
    char fullpath[1000];
    if (!strcmp(fullpath, "/"))
    {
        path = dirpath;
        strcpy(fullpath, path);
    }
    else
    {
        sprintf(fullpath, "%s%s", dirpath, path);
    }

	(void) fi;
	fd = open(fullpath, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
    logs(0, "WRITE", path, "");
	return res;
}

static int xmp_truncate(const char *path, off_t size){

	int res;
    char fullpath[1000];
    sprintf(fullpath, "%s%s", dirpath, path);
	res = truncate(fullpath, size);
	if (res == -1)
		return -errno;
	return 0;
}
  

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){

    char fpath[1000];

    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    }

    else sprintf(fpath, "%s%s",dirpath,path);


    int res = 0;
    DIR *dp;
    struct dirent *de;

    (void) offset;
    (void) fi;

    dp = opendir(fpath);

    if (dp == NULL) 
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;

        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        res = (filler(buf,de->d_name, &st, 0));
        if(res!=0) 
            break;
    }

    closedir(dp);
    return 0;
}

  

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){

    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);

    int res = 0;
    int fd = 0 ;

    // logs(0, "READ", fpath, "");

    (void) fi;
    fd = open(fpath, O_RDONLY);
    if (fd == -1) 
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1) 
        res = -errno;

    close(fd);
    return res;
}

static int xmp_mkdir(const char *path, mode_t mode){
	int res;

    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);

	res = mkdir(fpath, mode);
	if (res == -1)
		return -errno;

    logs(0, "MKDIR", path, "");
	return 0;
}

static int xmp_unlink(const char *path){
	int res;

    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);

	res = unlink(fpath);
	if (res == -1)
		return -errno;

    logs(1, "UNLINK", path, "");
	return 0;
}

static int xmp_rmdir(const char *path){
	int res;

    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);

	res = rmdir(fpath);
	if (res == -1)
		return -errno;

    logs(1, "RMDIR", path, "");
	return 0;
}

static int xmp_rename(const char *from, const char *to){
	int res;

    char ffrom[1000];
    char fto[1000];
    if(strcmp(from,"/") == 0){
        from=dirpath;
        to=dirpath;
        sprintf(ffrom, "%s",from);
        sprintf(fto, "%s",to);
    }
    else {
        sprintf(ffrom, "%s%s",dirpath,from);
        sprintf(fto, "%s%s",dirpath,to);
    }

	res = rename(ffrom, fto);
	if (res == -1)
		return -errno;

    logs(0, "RENAME", from, to);
	return 0;
}

static int xmp_chmod(const char *path, mode_t mode){
	int res;

    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);

	res = chmod(fpath, mode);
	if (res == -1)
		return -errno;

    logs(0, "CHMOD", path, "");
	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid){
	int res;

    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);

	res = lchown(fpath, uid, gid);
	if (res == -1)
		return -errno;

    logs(0, "CHOWN", path, "");
	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi){
	int res;

    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);

	res = open(fpath, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
    // logs(0, "OPEN", path, "");
	return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi) {

    (void) fi;

    int res;

    char fpath[1000];
    if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);

    res = creat(fpath, mode);
    if(res == -1)
	return -errno;

    close(res);

    logs(0, "CREATE", path, "");
    return 0;
}

static struct fuse_operations xmp_oper = {

    .getattr    = xmp_getattr,
    .readdir    = xmp_readdir,
    .read       = xmp_read,
    .truncate   = xmp_truncate,
    .write      = xmp_write,
    .mkdir      = xmp_mkdir,
    .unlink     = xmp_unlink,
    .rmdir      = xmp_rmdir,
    .rename     = xmp_rename,
    .chmod      = xmp_chmod,
    .chown      = xmp_chown,
    .open       = xmp_open,
    .create     = xmp_create,
};

  

int  main(int  argc, char *argv[]){
    
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}