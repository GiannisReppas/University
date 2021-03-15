#include "hash_function.h"

int hash_function( int c, int num)
{
	// parameters a,b
	int a = 7;
	int b = 8;

	// 1000th prime number
	int p = 7919;

	// universal hashing for integers
	return (((a*c + b) % p ) % num);
}
