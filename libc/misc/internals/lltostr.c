/*
 * Copyright (C) 2000 Manuel Novoa III
 *
 * Note: buf is a pointer to the END of the buffer passed.
 * Call like this:
 *     char buf[SIZE], *p;
 *     p = __lltostr(buf + sizeof(buf) - 1, ...)
 * For long longs of 64 bits, appropriate buffer sizes are:
 *     base =  2      66  = 1 (possible -) sign + 64 digits + 1 nul
 *     base = 10      21  = 1 (possible -) sign + 19 digits + 1 nul
 *     base = 16      18  = 1 (possible -) sign + 16 hex digits + 1 nul
 */

extern char *__ulltostr(char *buf, unsigned long long uval, int base, 
						int uppercase);

char *__lltostr(char *buf, long long val, int base, int uppercase)
{
	unsigned long long uval;
	char *pos;
    int negative;

	negative = 0;
    if (val < 0) {
		negative = 1;
		uval = ((unsigned long long)(-(1+val))) + 1;
    } else {
		uval = val;
	}


    pos = __ulltostr(buf, uval, base, uppercase);

    if (pos && negative) {
		*--pos = '-';
    }

    return pos;
}
