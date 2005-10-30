/*
 * Distributed under the terms of the GNU Lesser General Public License
 * $Header: $
 *
 * This is a modified version of Hiroaki Etoh's stack smashing routines
 * implemented for glibc.
 *
 * The following people have contributed input to this code.
 * Ned Ludd - <solar[@]gentoo.org>
 * Alexander Gabert - <pappy[@]gentoo.org>
 * The PaX Team - <pageexec[@]freemail.hu>
 * Peter S. Mazinger - <ps.m[@]gmx.net>
 * Yoann Vandoorselaere - <yoann[@]prelude-ids.org>
 * Robert Connolly - <robert[@]linuxfromscratch.org>
 * Cory Visi <cory[@]visi.name>
 * Mike Frysinger <vapier[@]gentoo.org>
 */

#include <string.h>
#include <unistd.h>
#include <sys/syslog.h>

#include <ssp-internal.h>

static __always_inline void block_signals(void)
{
	struct sigaction sa;
	sigset_t mask;

	sigfillset(&mask);

	sigdelset(&mask, SSP_SIGTYPE);	/* Block all signal handlers */
	SIGPROCMASK(SIG_BLOCK, &mask, NULL);	/* except SSP_SIGTYPE */

	/* Make the default handler associated with the signal handler */
	memset(&sa, 0, sizeof(struct sigaction));
	sigfillset(&sa.sa_mask);	/* Block all signals */
	sa.sa_flags = 0;
	sa.sa_handler = SIG_DFL;
	SIGACTION(SSP_SIGTYPE, &sa, NULL);
}

static __always_inline void ssp_write(int fd, const char *msg1, const char *msg2, const char *msg3)
{
	WRITE(fd, msg1, strlen(msg1));
	WRITE(fd, msg2, strlen(msg2));
	WRITE(fd, msg3, strlen(msg3));
	WRITE(fd, "()\n", 3);
	openlog("ssp", LOG_CONS | LOG_PID, LOG_USER);
	syslog(LOG_INFO, "%s%s%s()", msg1, msg2, msg3);
	closelog();
}

static __always_inline void terminate(void)
{
	(void) KILL(GETPID(), SSP_SIGTYPE);
	EXIT(127);
}

void __attribute__ ((noreturn)) __stack_smash_handler(char func[], int damaged __attribute__ ((unused)));
void __attribute__ ((noreturn)) __stack_smash_handler(char func[], int damaged)
{
	extern char *__progname;
	static const char message[] = ": stack smashing attack in function ";

	block_signals();

	ssp_write(STDERR_FILENO, __progname, message, func);

	/* The loop is added only to keep gcc happy. */
	while(1)
		terminate();
}

void __attribute__ ((noreturn)) __stack_chk_fail(void)
{
	extern char *__progname;
	static const char msg1[] = "stack smashing detected: ";
	static const char msg3[] = " terminated";

	block_signals();

	ssp_write(STDERR_FILENO, msg1, __progname, msg3);

	/* The loop is added only to keep gcc happy. */
	while(1)
		terminate();
}

#if 0
void __attribute__ ((noreturn)) __chk_fail(void)
{
	extern char *__progname;
	static const char msg1[] = "buffer overflow detected: ";
	static const char msg3[] = " terminated";

	block_signals();

	ssp_write(STDERR_FILENO, msg1, __progname, msg3);

	/* The loop is added only to keep gcc happy. */
	while(1)
		terminate();
}
#endif
