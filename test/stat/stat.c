#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

void print_struct_stat(char *msg, struct stat *s)
{
    printf("%s\n", msg);
    /* The casts are because glibc thinks it's cool */
    printf("device    : 0x%x\n",(unsigned int)s->st_dev);
    printf("inode     : %d\n",(int)s->st_ino);
    printf("mode      : 0x%x\n",s->st_mode);
    printf("nlink     : %d\n",s->st_nlink);
    printf("uid       : %d\n",s->st_uid);
    printf("gid       : %d\n",s->st_gid);
    printf("rdev      : 0x%x\n",(unsigned int)s->st_rdev);
    printf("size      : %ld\n",s->st_size);
    printf("blksize   : %ld\n",s->st_blksize);
    printf("blocks    : %ld\n",s->st_blocks);
    printf("atime     : %ld\n",s->st_atime);
    printf("mtime     : %ld\n",s->st_mtime);
    printf("ctime     : %ld\n",s->st_ctime);
}

int main(int argc,char **argv)
{
    int fd, ret;
    char *file;
    struct stat s;

    if (argc < 2) {
	fprintf(stderr, "Usage: stat FILE\n");
	exit(1);
    }
    file = argv[1];

    memset(&s, 0, sizeof(struct stat));
    ret = stat(file, &s);
    if(ret<0){
	perror("stat");
	exit(1);
    }
    print_struct_stat("\nTesting stat:", &s);

    memset(&s, 0, sizeof(struct stat));
    ret = lstat(file, &s);
    if(ret<0){
	perror("lstat");
	exit(1);
    }
    print_struct_stat("\nTesting lstat:", &s);


    fd = open(file, O_RDONLY);
    if(fd<0){
	perror("open");
	exit(1);
    }
    memset(&s, 0, sizeof(struct stat));
    ret = fstat(fd,&s);
    if(ret<0){
	perror("fstat");
	exit(1);
    }
    print_struct_stat("\nTesting fstat:", &s);

    exit(0);
}

