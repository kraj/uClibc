/*
 * Copyright (C) 2000 Manuel Novoa III
 *
 * Note: buf is a pointer to the END of the buffer passed.
 * Call like this;
 *     char buf[SIZE], *p;
 *     p = __ultostr(buf + sizeof(buf) - 1, ...)
 *
 * For longs of 32 bits, appropriate buffer sizes are:
 *     base =  2      33  = 32 digits + 1 nul
 *     base = 10      11  = 10 digits + 1 nul
 *     base = 16       9  = 8 hex digits + 1 nul
 */

char *__ultostr(char *buf, unsigned long uval, int base, int uppercase)
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
