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
#include <stdarg.h>
#include <ctype.h>
#include "../testsuite.h"

#define TRUE	0

int main( int argc, char **argv)
{
	int i, c;


    printf( "Testing functions defined in ctype.h\n");


	/* isalnum() */
	{
		int buffer[]={ '1', '4', 'a', 'z', 'A', 'Z', '5', -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( isalnum(c)==TRUE, 
					"isalnum(%c)", c);
		}
	}
	{
		int buffer[]={  2, 128, 254, '\n', -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( isalnum(c)!=TRUE, 
					"!isalnum(%d)", c);
		}
	}



	/* isalpha() */
	{
		int buffer[]={ 'a', 'z', 'A', 'Z', -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( isalpha(c)==TRUE, 
					"isalpha(%c)", c);
		}
	}
	{
		int buffer[]={  2, 63, 128, 254, '\n', -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( isalpha(c)!=TRUE, 
					"!isalpha(%d)", c);
		}
	}



	/* isascii() */
	{
		int buffer[]={ 'a', 'z', 'A', 'Z', '\n', -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( isascii(c)==TRUE, 
					"isascii(%d)", c);
		}
	}
	{
		int buffer[]={  128, 254, -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( isascii(c)!=TRUE, 
					"!isascii(%d)", c);
		}
	}


	/* iscntrl() */
	{
		int buffer[]={ 0x7F, 6, '\t', '\n', 0x7F, -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( iscntrl(c)==TRUE, 
					"iscntrl(%d)", c);
		}
	}
	{
		int buffer[]={  63, 128, 254, -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( iscntrl(c)!=TRUE, 
					"!iscntrl(%d)", c);
		}
	}


	/* isdigit() */
	{
		int buffer[]={ '1', '5', '7', '9', -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( isdigit(c)==TRUE, 
					"isdigit(%c)", c);
		}
	}
	{
		int buffer[]={  2, 'a', 'z', 'A', 'Z', 63, 128, 254, '\n', -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( isdigit(c)!=TRUE, 
					"!isdigit(%d)", c);
		}
	}



	/* isgraph() */
	{
		int buffer[]={ ')', '~', '9', -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( isgraph(c)==TRUE, 
					"isgraph(%d)", c);
		}
	}
	{
		int buffer[]={ 9, ' ', '\t', '\n', 200, 0x7F, -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( isgraph(c)!=TRUE, 
					"!isgraph(%d)", c);
		}
	}


	/* islower() */
	{
		int buffer[]={ 'a', 'g', 'z', -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( islower(c)==TRUE, 
					"islower(%c)", c);
		}
	}
	{
		int buffer[]={ 9, 'A', 'Z', 128, 254, ' ', '\t', '\n', 0x7F, -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( islower(c)!=TRUE, 
					"!islower(%d)", c);
		}
	}


	/* isprint() */
	{
		int buffer[]={ ' ', ')', '~', '9', -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( isprint(c)==TRUE, 
					"isprint(%c)", c);
		}
	}
	{
		int buffer[]={ '\b', '\t', '\n', 9, 128, 254, 200, 0x7F, -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( isprint(c)!=TRUE, 
					"!isprint(%d)", c);
		}
	}


	/* ispunct() */
	{
		int buffer[]={ '.', '#', '@', ';', -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( ispunct(c)==TRUE, 
					"ispunct(%c)", c);
		}
	}
	{
		int buffer[]={  2, 'a', 'Z', '1', 128, 254, '\n', -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( ispunct(c)!=TRUE, 
					"!ispunct(%d)", c);
		}
	}


	/* isspace() */
	{
		int buffer[]={ ' ', '\t', '\r', '\v', '\n', -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( isspace(c)==TRUE, 
					"isspace(%d)", c);
		}
	}
	{
		int buffer[]={  2, 'a', 'Z', '1', 128, 254, -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( isspace(c)!=TRUE, 
					"!isspace(%d)", c);
		}
	}


	/* isupper() */
	{
		int buffer[]={ 'A', 'G', 'Z', -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( isupper(c)==TRUE, 
					"isupper(%c)", c);
		}
	}
	{
		int buffer[]={  2, 'a', 'z', '1', 128, 254, -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( isupper(c)!=TRUE, 
					"!isupper(%d)", c);
		}
	}



	/* isxdigit() */
	{
		int buffer[]={ 'f', 'A', '1', '8', -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( isxdigit(c)==TRUE, 
					"isxdigit(%c)", c);
		}
	}
	{
		int buffer[]={  2, 'g', 'G', 'x', '\n', -1};
		for(i=0; buffer[i]!=-1; i++) {
			c = buffer[i];
			TEST_SUCCESS( isxdigit(c)!=TRUE, 
					"!isxdigit(%d)", c);
		}
	}


	/* tolower() */
	c='A';
	TEST_NUMERIC_OUTPUT( tolower(c), 'a', "tolower(%c)=%c", c, tolower(c));
	c='a';
	TEST_NUMERIC_OUTPUT( tolower(c), 'a', "tolower(%c)=%c", c, tolower(c));
	c='#';
	TEST_NUMERIC_OUTPUT( tolower(c), c, "tolower(%c)=%c", c, tolower(c));

	/* toupper() */
	c='a';
	TEST_NUMERIC_OUTPUT( toupper(c), 'A', "toupper(%c)=%c", c, toupper(c));
	c='A';
	TEST_NUMERIC_OUTPUT( toupper(c), 'A', "toupper(%c)=%c", c, toupper(c));
	c='#';
	TEST_NUMERIC_OUTPUT( toupper(c), c, "toupper(%c)=%c", c, toupper(c));


    printf( "Finished testing ctype.h\n");

	stop_testing();
}
