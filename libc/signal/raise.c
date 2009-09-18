/* Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>



int __raise (int signo)  attribute_hidden;
int __raise(int signo)
{
    return kill(getpid(), signo);
}
weak_alias(__raise,raise)
libc_hidden_def(raise)
