#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    write(1,"hello world\n",12);
    exit(42);
}
