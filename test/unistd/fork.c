/* vi: set sw=4 ts=4: */
/*
 * fork test for uClibc
 *
 * Copyright (C) 1999,2000 by Lineo, inc.
 * Written by Erik Andersen <andersen@lineo.com>, <andersee@debian.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) 
{
	pid_t pid;
	int status, wpid;

	if ((pid = fork()) == 0) {
		printf("The child process sleeps 5 seconds...\n");
		sleep(5);
		printf("Child exiting.\n");
		exit(-1);
	}

	printf("Parent: waiting for the child to die.\n");
	while (1) {
		wpid = wait(&status);
		if (wpid > 0 && wpid != pid) {
			continue;
		}
		if (wpid == pid)
			break;
	}

	printf("Child process exited.\nGoodbye.\n");
	return EXIT_SUCCESS;
}

/*
Local Variables:
c-file-style: "linux"
c-basic-offset: 4
tab-width: 4
End:
*/
