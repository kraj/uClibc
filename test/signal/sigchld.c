#include <stdlib.h>
#include <stdio.h>
#include <sys/signal.h>
#include <unistd.h>

void test_handler(int signo) 
{
    fprintf(stderr, "caught signo: %d\n", signo);
}


int main(void) 
{
    int count = 0;
    struct sigaction siga;
    static sigset_t sigset;

    sigfillset(&sigset);

    siga.sa_handler = test_handler;
    siga.sa_mask = sigset;
    siga.sa_flags = 0;

    if (0 != sigaction(SIGCHLD, &siga, (struct sigaction *)NULL)) {
	fprintf(stderr, "ack!");
    }
    printf("give me a SIGCHLD\n");

    for(;;) {
	sleep(10);
	printf("after sleep %d\n", ++count);
    }

    printf("after loop\n");

    return 0;
}

