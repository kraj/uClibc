/*
 * Copyright (C) 2000 Manuel Novoa III
 *
 * Note: buf is a pointer to the END of the buffer passed.
 * Call like this:
 *     char buf[SIZE], *p;
 *     p = __ulltostr(buf + sizeof(buf) - 1, ...)
 *
 * For long longs of 64 bits, appropriate buffer sizes are:
 *     base =  2      65  = 64 digits + 1 nul
 *     base = 10      20  = 19 digits + 1 nul
 *     base = 16      17  = 16 hex digits + 1 nul
 */

char *__ulltostr(char *buf, unsigned long long uval, int base, int uppercase)
{
    int digit;

    if ((base < 2) || (base > 36)) {
		return 0;
    }

    *buf = '\0';

    do {
		digit = uval % base;
		uval /= base;

		/* note: slightly slower but generates less code */
		*--buf = '0' + digit;
		if (digit > 9) {
			*buf = (uppercase ? 'A' : 'a') + digit - 10;
		}
    } while (uval);

    return buf;
}
