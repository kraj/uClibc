/* +++Date last modified: 05-Jul-1997 */

/*
**  ssort()  --  Fast, small, qsort()-compatible Shell sort
**
**  by Ray Gardner,  public domain   5/90
*/

/*
 * Manuel Novoa III       Dec 2000
 *
 * There were several problems with the qsort code contained in uClibc.
 * It assumed sizeof(int) was 2 and sizeof(long) was 4.  It then had three
 * seperate quiicksort routines based on the width of the data passed: 2, 4,
 * or anything else <= 128.  If the width was > 128, it returned -1 (although
 * qsort should not return a value) and did no sorting.  On i386 with
 * -Os -fomit-frame-pointer -ffunction-sections, the text segment of qsort.o
 * was 1358 bytes, with an additional 4 bytes in bss.
 *
 * I decided to completely replace the existing code with a small
 * implementation of a shell sort.  It is a drop-in replacement for the
 * standard qsort and, with the same gcc flags as above, the text segment
 * size on i386 is only 183 bytes.
 *
 * Grabbed original file rg_ssort.c from snippets.org.
 * Modified original code to avoid possible overflow in wgap calculation.
 * Modified wgap calculation in loop and eliminated variables gap and wnel.
 */


#include <stdlib.h>
#include <assert.h>

void qsort (void  *base,
            size_t nel,
            size_t width,
            int (*comp)(const void *, const void *))
{
	size_t wgap, i, j, k;
	char *a, *b, tmp;

	/* Note: still conceivable that nel * width could overflow! */
	assert(width > 0);

	if (nel > 1) {
		for (wgap = 0; ++wgap < (nel-1)/3 ; wgap *= 3) {}
		wgap *= width;
		nel *= width;			/* convert nel to 'wnel' */
		do {
			for (i = wgap; i < nel; i += width) {
				for (j = i - wgap; ;j -= wgap) {
					a = j + ((char *)base);
					b = a + wgap;
					if ( (*comp)(a, b) <= 0 ) {
						break;
					}
					k = width;
					do {
						tmp = *a;
						*a++ = *b;
						*b++ = tmp;
					} while ( --k );
					if (j < wgap) {
						break;
					}
				}
			}
			wgap = (wgap - width)/3;
		} while (wgap);
	}
}
