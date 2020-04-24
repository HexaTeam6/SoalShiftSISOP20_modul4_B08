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
#define SEGMENT 1024 //approximate target size of small file

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

void getPath(char *path, char *files, char temp[]){
    char string[1000] = "";
    strcpy(string, path);
    char * token = strtok(string, "/");
    while(strcmp(token, files)) {
        sprintf( temp, "%s/%s", temp, token);
        token = strtok(NULL, "/");
    }
}

char *getName(char* myStr) {
    char *retStr;
    char *lastExt;
    if (myStr == NULL) return NULL;
    if ((retStr = malloc (strlen (myStr) + 1)) == NULL) return NULL;
    strcpy (retStr, myStr);
    lastExt = strrchr (retStr, '.');
    if (lastExt != NULL)
        *lastExt = '\0';
    return retStr;
}

void combineFile(char *path){

    char temp[1000]="";
    char *files = rindex(path, '/');
    char *ext = rindex(files+1, '.');
    ext++;
    char *name = getName(files+1);
    char newFile[1000]="";
    getPath(path, files+1, temp);

    sprintf(newFile, "%s/%s", temp, name);
    // printf("%s\n", newFile);

    // if(strstr(path, newFile) != NULL && strcmp(path, newFile)){
    //     char partFile[1000]="";
    //     sprintf(partFile, "%s", strstr(path, newFile));
    //     printf("%s\n", partFile); 
    // }

    int count=4;
    while(1){
        char partFile[1000]="";
        sprintf(partFile, "%s.%d", newFile, count);
        printf("%s\n",partFile);
        if(strstr(path, partFile) == NULL) break;
        count++;
        remove(partFile);
        sleep(2);
    }
        

    // FILE *largeFile = fopen(newFile, "w"); 
    // int count=0;
    // while(strstr(path, newFile) != NULL && strcmp(path, newFile)){
    //     char partFile[1000]="";
    //     sprintf(partFile, "%s", strstr(path, newFile));
    //     printf("%s\n", partFile);

    //     FILE *smallFile = fopen(partFile, "r");
    //     char c;  
    //     while ((c = fgetc(smallFile)) != EOF) 
    //         fputc(c, largeFile);
    //     fclose(smallFile);
    //     remove(partFile);
    // }
    // fclose(largeFile);
    
    // // Open file to store the result 
    // FILE *fp6 = fopen("file3.py", "w"); 
    // char c;  
    
    // // Copy contents of first file to file3.txt 
    // while ((c = fgetc(fp1)) != EOF) 
    //     fputc(c, fp6); 
    
    // // Copy contents of second file to file3.txt 
    // while ((c = fgetc(fp2)) != EOF) 
    //     fputc(c, fp6); 

    //  // Copy contents of second file to file3.txt 
    // while ((c = fgetc(fp3)) != EOF) 
    //     fputc(c, fp6); 
    
    // while ((c = fgetc(fp4)) != EOF) 
    //     fputc(c, fp6); 
    
    // while ((c = fgetc(fp5)) != EOF) 
    //     fputc(c, fp6); 
    
    // fclose(fp1); 
    // fclose(fp2); 
    // fclose(fp3); 
    // fclose(fp4); 
    // fclose(fp5); 
    // fclose(fp6);
}

long file_size(char *name){
    FILE *fp = fopen(name, "rb"); //must be binary read to get bytes

    long size=-1;
    if(fp)
    {
        fseek (fp, 0, SEEK_END);
        size = ftell(fp)+1;
        fclose(fp);
    }
    return size;
}

void splitFile(char *path){
    int segments=0, i, accum;
    FILE *fp1, *fp2;
    
    // char *files = rindex(path, '/');
    // char *ext = rindex(files+1, '.');
    // ext++;
    // char *name = getName(files+1);
    // printf("%s\n", files+1);
    // getPath(path, files+1);

    char filename[255];//base name for small files.
    sprintf(filename, "%s", path);
    char largeFileName[255];//change to your path
    sprintf(largeFileName, "%s", path);
    
    char smallFileName[255];
    char line[1080];
    long sizeFile = file_size(largeFileName);
    segments = sizeFile/SEGMENT + 1;//ensure end of file
    
    if(sizeFile<=1080) return;

    fp1 = fopen(largeFileName, "r");
    if(fp1)
    {
        for(i=0;i<segments;i++)
        {
            accum = 0;
            sprintf(smallFileName, "%s.%d", filename, i);
            fp2 = fopen(smallFileName, "w");
            if(fp2)
            {
                while(fgets(line, 1080, fp1) && accum <= SEGMENT)
                {
                    accum += strlen(line);//track size of growing file
                    fputs(line, fp2);
                }
                fclose(fp2);
            }
        }
        fclose(fp1);
    }
    remove(path);
}

static const char *dirpath = "/home/hexa/Documents/testad";

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
    // logs(0, "WRITE", path, "");
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

    if(strstr(fpath, "encv2_") != NULL && strstr(fpath, ".Trash") == NULL) {
        splitFile(fpath);
        printf("splitfile\n");
    }else if(strstr(fpath, ".Trash") == NULL){
        combineFile(fpath);
        printf("combinrfile\n");
    }

    int res = 0;
    int fd = 0 ;

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

    // logs(0, "MKDIR", path, "");
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

    // logs(1, "RMDIR", path, "");
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

    // logs(0, "RENAME", from, to);
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

    // logs(0, "CREATE", path, "");
    return 0;
}

static struct fuse_operations xmp_oper = {

    .getattr    = xmp_getattr,
    .readdir    = xmp_readdir,
    .read       = xmp_read,
    .truncate   = xmp_truncate,
    .write      = xmp_write,
    .mkdir      = xmp_mkdir,
    .rmdir      = xmp_rmdir,
    .rename     = xmp_rename,
    .create     = xmp_create,
};

  

int  main(int  argc, char *argv[]){
    
    umask(0);

    return fuse_main(argc, argv, &xmp_oper, NULL);
}

