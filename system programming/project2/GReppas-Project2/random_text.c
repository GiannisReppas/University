#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main( int argc, char *argv[])
{
	srand(time(NULL));

	int b,i,j = (rand()%(1024*127))+1024;
	for ( i=0; i<j; i++)
	{
		b = (rand() % 35)+97;
		if ( b > 122) b -= 75;
		putchar(b);
	}
	printf("\n");

	return 0;
}
