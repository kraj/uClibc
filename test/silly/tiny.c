#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    write(1,"hello world\n",12);
    _exit(42);
}
