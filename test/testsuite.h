/* vi: set sw=4 ts=4: */
/*
 * Some simple macros for use in test applications.
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


void error_msg(int result, int line, const char* file, const char* msg, ...);
void success_msg(int result, const char* msg, ...);
void stop_testing(void)  __attribute__((noreturn));;



static int failures = 0;


#define TEST_STRING_OUTPUT( command, expected_result, message, args...) \
	do { \
		int result=strcmp( command, expected_result); \
		if ( result != 0 ) \
			success_msg( result, message, ## args); \
		else \
			error_msg(result, __LINE__, __FILE__, message, ## args); \
	} while (0)
		

#define TEST_NUMERIC_OUTPUT( command, expected_result, message, args...) \
	do { \
		int result=(command); \
		if ( result == expected_result ) \
			success_msg( result, message, ## args); \
		else \
			error_msg(result, __LINE__, __FILE__, message, ## args); \
	} while (0)
		

#define TEST_SUCCESS(command, message, args...)	TEST_NUMERIC_OUTPUT( command, EXIT_SUCCESS, message, ## args)



void error_msg(int result, int line, const char* file, const char* msg, ...)
{
	va_list p;
	failures++;

	va_start(p, msg);
	printf("FAILED TEST ");
	vprintf(msg, p);
	va_end(p);
	printf(" AT LINE: %d, FILE: %s\n", line, file);
}   

void success_msg(int result, const char* msg, ...)
{
#if 0
	va_list p;
	va_start(p, msg);
	printf("passed test: ");
	vprintf(msg, p);
	va_end(p);
	printf("\n");
#endif
}

void stop_testing(void)
{
    if (0 < failures) {
		printf("Failed %d tests\n", failures);
		exit(EXIT_FAILURE);
	} else {
		printf("All functions tested sucessfully\n");
		exit( EXIT_SUCCESS );
	}
}
