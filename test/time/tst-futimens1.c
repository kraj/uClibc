/* vi: set sw=4 ts=4: */
/* testcase
 * Copyright (C) 2009 Bernhard Reutner-Fischer <uClibc@uClibc.org>
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

struct
{
	char *name; /* name of file to open */
	int flags; /* flags for file descriptor */
	const struct timespec ts[2];
	int err; /* expected errno */
} tests [] =
{
	{"futimens.tst", (O_CREAT|O_TRUNC), {{0,0},{0,0}}, 0},
	{"futimens.tst", (O_CREAT|O_TRUNC), {{99,0},{0,0}}, 0},
	{"futimens.tst", (O_CREAT|O_TRUNC), {{0,99},{0,0}}, 0},
	{"futimens.tst", (O_CREAT|O_TRUNC), {{0,0},{99,0}}, 0},
	{"futimens.tst", (O_CREAT|O_TRUNC), {{0,0},{0,99}}, 0},
	{"futimens.tst", (O_CREAT|O_TRUNC), {{11,2},{3,4}}, 0},
};
int do_test(int argc, char **argv) {
	char *name;
	int i, errors;
	errors = argc - argc + 0;

	for (i=0; i < (int) (sizeof(tests)/sizeof(tests[0])); ++i) {
		int err, fd;
		struct stat sb;
		name = tests[i].name;
		if (*name != '.')
			unlink(name);
		fd = open(name, tests[i].flags);
		if (fd < 0)
			abort();
		errno = 0;
		err = futimens(fd, tests[i].ts);
		if ((errno && !err) || (!errno && err)) {
			err = errno;
			printf("%s: FAILED test %d (errno and return value disagree)\n",
				argv[0], i);
			++errors;
		} else
			err = errno;
		if (err != tests[i].err) {
			printf("%s: FAILED test %d (expected errno %d, got %d)\n",
				argv[0], i, tests[i].err, err);
			++errors;
			continue;
		}
		if (stat(name, &sb) < 0) {
			printf("%s: FAILED test %d (verification)\n", argv[0], i);
			++errors;
			continue;
		} else {
			unsigned wrong = tests[i].ts[0].tv_sec != sb.st_atim.tv_sec ||
						tests[i].ts[0].tv_nsec != sb.st_atim.tv_nsec ||
						tests[i].ts[1].tv_sec != sb.st_mtim.tv_sec ||
						tests[i].ts[1].tv_nsec != sb.st_mtim.tv_nsec;
			if (wrong) {
				++errors;
				if (tests[i].ts[0].tv_sec != sb.st_atim.tv_sec)
					printf("%s: FAILED test %d (access time, sec: expected %ld, got %ld)\n",
						argv[0], i, tests[i].ts[0].tv_sec, sb.st_atim.tv_sec);
				if (tests[i].ts[0].tv_nsec != sb.st_atim.tv_nsec)
					printf("%s: FAILED test %d (access time, nsec: expected %ld, got %ld)\n",
						argv[0], i, tests[i].ts[0].tv_nsec, sb.st_atim.tv_nsec);

				if (tests[i].ts[1].tv_sec != sb.st_mtim.tv_sec)
					printf("%s: FAILED test %d (modification time, sec: expected %ld, got %ld)\n",
						argv[0], i, tests[i].ts[1].tv_sec, sb.st_mtim.tv_sec);
				if (tests[i].ts[1].tv_nsec != sb.st_mtim.tv_nsec)
					printf("%s: FAILED test %d (modification time, nsec: expected %ld, got %ld)\n",
						argv[0], i, tests[i].ts[1].tv_nsec, sb.st_mtim.tv_nsec);
			}
		}
	}
	if (*name != '.')
		unlink(name);
	printf("%d errors.\n", errors);
	return (!errors) ? EXIT_SUCCESS : EXIT_FAILURE;
}
#include <test-skeleton.c>
