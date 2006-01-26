/* Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */

#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>


libc_hidden_proto(getpid)
libc_hidden_proto(kill)

int __raise (int signo)  attribute_hidden;
int __raise(int signo)
{
    return kill(getpid(), signo);
}
libc_hidden_proto(raise)
weak_alias(__raise,raise)
libc_hidden_def(raise)
