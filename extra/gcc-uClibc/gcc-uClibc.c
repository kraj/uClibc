
/*
 * Copyright (C) 2000 Manuel Novoa III
 *
 * This is a crude wrapper to use uClibc with gcc.
 * It was originally written to work around ./configure for ext2fs-utils.
 * It certainly can be improved, but it works for me in the normal cases.
 *
 * TODO:
 * Check/modify gcc-specific environment variables?
 */

#ifdef DEBUG
#include <stdio.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "gcc-uClibc.h"

#define UCLIBC_START UCLIBC_DIR"libcrt0.o"
#define UCLIBC_START_G UCLIBC_START
#define UCLIBC_LIB UCLIBC_DIR"libc.a"
#if 1
#define UCLIBC_LIB_G UCLIBC_LIB
#else
#define UCLIBC_LIB_G UCLIBC_DIR"libc.a-debug"
#endif
#define UCLIBC_INC "-I"UCLIBC_DIR"include/"

static char nostdinc[] = "-nostdinc";
static char nostartfiles[] = "-nostartfiles";
static char nodefaultlibs[] = "-nodefaultlibs";
static char nostdlib[] = "-nostdlib";

int main(int argc, char **argv)
{
	int debugging = 0, linking = 1;
	int use_stdinc = 1, use_start = 1, use_stdlib = 1;
	int i, j;
	int source_count;
	char ** gcc_argv;

	source_count = 0;

	for ( i = 1 ; i < argc ; i++ ) {
		if (argv[i][0] == '-') { /* option */
			switch (argv[i][1]) {
				case 'c':
				case 'S':
				case 'E':
				case 'r':
					if (argv[i][2] == 0) linking = 0;
					break;
				case 'g':
					if (argv[i][2] == 0) debugging = 1;
					break;
				case 'n':
					if (strcmp(nostdinc,argv[i]) == 0) {
						use_stdinc = 0;
					} else if (strcmp(nostartfiles,argv[i]) == 0) {
						use_start = 0;
					} else if (strcmp(nodefaultlibs,argv[i]) == 0) {
						use_stdlib = 0;
					} else if (strcmp(nostdlib,argv[i]) == 0) {
						use_start = 0;
						use_stdlib = 0;
					}

			}
		} else {				/* assume it is an existing source file */
			++source_count;
		}
	}

#if 1
	gcc_argv = __builtin_alloca(sizeof(char*) * (argc + 20));
#else
	if (!(gcc_argv = malloc(sizeof(char) * (argc + 20)))) {
		return EXIT_FAILURE;
	}
#endif

	i = 0;
	gcc_argv[i++] = GCC_BIN;
	for ( j = 1 ; j < argc ; j++ ) {
		gcc_argv[i++] = argv[j];
	}
	if (use_stdinc) {
		gcc_argv[i++] = nostdinc;
		gcc_argv[i++] = UCLIBC_INC;
		gcc_argv[i++] = GCC_INCDIR;
	}
	if (linking && source_count) {
		gcc_argv[i++] = "-static";
		if (use_start) {
			if (debugging) {
				gcc_argv[i++] = UCLIBC_START_G;
			} else {
				gcc_argv[i++] = UCLIBC_START;
			}
		}
		if (use_stdlib) {
			gcc_argv[i++] = "-nostdlib";
			if (debugging) {
				gcc_argv[i++] = UCLIBC_LIB_G;
			} else {
				gcc_argv[i++] = UCLIBC_LIB;
			}
			gcc_argv[i++] = GCC_LIB;
		}
	}
	gcc_argv[i++] = NULL;

#ifdef DEBUG
	for ( j = 0 ; gcc_argv[j] ; j++ ) {
		printf("arg[%2i] = %s\n", j, gcc_argv[j]);
	}
	return EXIT_SUCCESS;
#else
	return execvp(GCC_BIN, gcc_argv);
#endif
}
  

  
  
