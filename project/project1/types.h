#ifndef TYPES
#define TYPES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

static const int  BucketSize = 1024*1024; /*The bucket size will be 1MB.*/

/////////////////////////////////////////////////////
/////////////////// data types //////////////////////

/** Type definition for a tuple */
typedef struct tuple
{
	uint64_t key;
	uint64_t payload;
}tuple;

/** Type definition for a relation */
typedef struct relation
{
	tuple *tuples;
	uint64_t numtuples;
}relation;

typedef struct resultBucket
{
	uint64_t data[65536][2]; /*Since the bucket size is 1MB every bucket can hold 65536 * 2 uint64_t numbers*/
	struct resultBucket *next; /* SET next = NULL*/
	int numOfEntries; /* SET numOfEntries = 0;*/
}resultBucket;

#endif
