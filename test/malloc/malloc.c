
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define N_PTRS 1000
#define N_ALLOCS 10000
#define MAX_SIZE 0x10000

#define random_size()	(random()%MAX_SIZE)
#define random_ptr()	(random()%N_PTRS)

int main(int argc,char *argv[])
{
	void **ptrs;
	int i,j;
	int size;

	srandom(0x19730929);

	ptrs = malloc(N_PTRS*sizeof(void *));

	for(i=0;i<N_PTRS;i++){
		ptrs[i]=malloc(random_size());
	}
	for(i=0;i<N_ALLOCS;i++){
		j=random_ptr();
		free(ptrs[j]);

		size=random_size();
		ptrs[j]=malloc(size);
		if(!ptrs[j]){
			printf("malloc failed! %d\n",i);
		}
		memset(ptrs[j],0,size);
	}
	for(i=0;i<N_PTRS;i++){
		free(ptrs[i]);
	}

	return 0;
}

