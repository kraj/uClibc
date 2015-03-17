#include <stdlib.h>
#include <stdio.h>

int main(void)
{
	int random_number;
	random_number = arc4random() % 65536;
	printf("%d\n", random_number);
	return 0;
}
