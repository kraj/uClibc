/* copied from rsync */

#include <sys/types.h>
#include <stdarg.h>
void foo(const char *format, ...) {
	va_list ap;
	int len;
	char buf[5];

	va_start(ap, format);
	len = vsnprintf(0, 0, format, ap);
	va_end(ap);
	if (len != 5) exit(1);

	if (snprintf(buf, 3, "hello") != 5 || strcmp(buf, "he") != 0) exit(1);

	exit(0);
}
int main() { foo("hello"); }
