
#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>


jmp_buf jb;
int tries=0;

int main(int argc,char *argv[])
{
	int ret;

	printf("calling setjmp, should return with 0\n");

	ret = setjmp(jb);
	
	printf("setjmp returned %d\n",ret);

	if(!ret){
		if(tries++>4){
			printf("Hmmm... in loop, must be broken.\n");
			return 0;
		}
		printf("now calling longjmp, setjmp should return with 1\n");

		longjmp(jb,1);

		printf("returned from longjmp, must be broken\n");
		return 0;
	}

	return 0;
}

