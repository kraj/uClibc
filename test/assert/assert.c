/* vi: set sw=4 ts=4: */
/*
 * Test application for functions defined in ctype.h
 *
 * Copyright (C) 2000 by Lineo, inc.  Written by Erik Andersen
 * <andersen@lineo.com>, <andersee@debian.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


int main( int argc, char **argv)
{

    printf( "Testing functions defined in assert.h\n");

    printf( "Testing \"assert(0==0)\"\n");
	assert(0==0);

    printf( "Testing \"assert(0==1)\" with NDEBUG disabled\n");
#undef  NDEBUG
	assert(0==1);

#define  NDEBUG
    printf( "Testing \"assert(0==1)\" with NDEBUG enabled\n");
#undef  NDEBUG
	assert(0==1);

    printf( "Finished testing assert.h\n");

	exit(0);
}
