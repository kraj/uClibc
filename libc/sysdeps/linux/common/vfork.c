/* Trivial implementation for arches that lack vfork */
#include <unistd.h>
#include <sys/types.h>

extern __pid_t __fork (void) __THROW attribute_hidden;

pid_t attribute_hidden __vfork(void)
{
    return __fork();
}
strong_alias(__vfork,vfork)
