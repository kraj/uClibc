
#include <sys/stat.h>
#include <stdio.h>


int main(int argc,char *argv[])
{
	struct stat s;
	int ret;

	ret = stat("/",&s);

	if(ret<0){
		perror("stat");
		exit(1);
	}

	/* The casts are because glibc thinks it's cool */
	printf("device    : 0x%x\n",(unsigned int)s.st_dev);
	printf("inode     : %d\n",(int)s.st_ino);
	printf("mode      : 0x%x\n",s.st_mode);
	printf("nlink     : %d\n",s.st_nlink);
	printf("uid       : %d\n",s.st_uid);
	printf("gid       : %d\n",s.st_gid);
	printf("rdev      : 0x%x\n",(unsigned int)s.st_rdev);
	printf("size      : %ld\n",s.st_size);
	printf("blksize   : %ld\n",s.st_blksize);
	printf("blocks    : %ld\n",s.st_blocks);
	printf("atime     : %ld\n",s.st_atime);
	printf("mtime     : %ld\n",s.st_mtime);
	printf("ctime     : %ld\n",s.st_ctime);

	exit(0);
}

