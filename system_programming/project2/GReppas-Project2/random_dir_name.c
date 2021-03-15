#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main( int argc, char *argv[])
{
	srand(time(NULL));

	int a = (rand() % 8)+1;
	char str[a+1];

	int i,b;
	for ( i=0; i<a; i++)
	{
		b = (rand() % 35)+97;
		if ( b > 122) b -= 75;
		str[i]=b;
	}
	str[i]='\0';
	printf("%s\n",str);

	return 0;
}
