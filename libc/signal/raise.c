/* Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */

#define kill __kill

#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

#undef raise
int attribute_hidden __raise(int signo)
{
    return kill(__getpid(), signo);
}

/* psm: keep this weak, because the one in libpthread.so could overwrite it */
weak_alias(__raise, raise)
