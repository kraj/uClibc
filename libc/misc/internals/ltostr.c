/*
 * Copyright (C) 2000 Manuel Novoa III
 *
 * Note: buf is a pointer to the END of the buffer passed.
 * Call like this:
 *     char buf[SIZE], *p;
 *     p = __ltostr(buf + sizeof(buf) - 1, ...)
 *
 * For longs of 32 bits, appropriate buffer sizes are:
 *     base =  2      34  = 1 (possible -) sign + 32 digits + 1 nul
 *     base = 10      12  = 1 (possible -) sign + 10 digits + 1 nul
 *     base = 16      10  = 1 (possible -) sign + 8 hex digits + 1 nul
 */

extern char *__ultostr(char *buf, unsigned long uval, int base, int uppercase);

char *__ltostr(char *buf, long val, int base, int uppercase)
{
	unsigned long uval;
	char *pos;
    int negative;

	negative = 0;
    if (val < 0) {
		negative = 1;
		uval = ((unsigned long)(-(1+val))) + 1;
    } else {
		uval = val;
	}


    pos = __ultostr(buf, uval, base, uppercase);

    if (pos && negative) {
		*--pos = '-';
    }

    return pos;
}
