
/* The mmap test is useful, since syscalls with 6 arguments
 * (as mmap) are done differently on various architectures.
 */

#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>


int main(int argc,char *argv)
{
	void *ptr;


	ptr = mmap(NULL, 4096, PROT_READ|PROT_WRITE,
		MAP_PRIVATE|MAP_ANONYMOUS,
		0, 0);

	if(ptr==MAP_FAILED){
		perror("mmap");
		exit(1);
	}else{
		printf("mmap returned %p\n",ptr);
		exit(0);
	}
}

