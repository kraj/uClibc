#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <unistd.h>


void test_handler(int signo) 
{
    printf("caught signo: %d\n", signo);
    fflush(stdout);
}


int main(void) 
{
    pid_t mypid;
    int count = 0;
    struct sigaction siga;
    static sigset_t sigset;

    mypid = getpid();

    if (fork() == 0) {
	int i;

	for (i=0; i < 3; i++) {
	    sleep(2);
	    kill(mypid, SIGCHLD);
	}
	exit(EXIT_SUCCESS);
    }

    sigfillset(&sigset);

    siga.sa_handler = test_handler;
    siga.sa_mask = sigset;
    siga.sa_flags = 0;

    if (sigaction(SIGCHLD, &siga, (struct sigaction *)NULL) != 0) {
	fprintf(stderr, "sigaction choked: %s!", strerror(errno));
	exit(EXIT_FAILURE);
    }
    printf("waiting for a SIGCHLD\n");
    fflush(stdout);

    for(;;) {
	sleep(10);
	if (waitpid(-1, NULL, WNOHANG | WUNTRACED) > 0)
	    break;
	printf("after sleep %d\n", ++count);
	fflush(stdout);
    }

    printf("after loop\n");

    return 0;
}

