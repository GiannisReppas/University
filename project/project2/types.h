#ifndef TYPES
#define TYPES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static const int  BucketSize = 1024*1024; /*The bucket size will be 1MB.*/

/////////////////////////////////////////////////////
/////////////////// data types //////////////////////

/** Type definition for a tuple */
typedef struct tuple
{
	uint64_t key;
	uint64_t payload;
	uint64_t intermediatePayload;
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
	uint64_t data2[65536][2]; /*Holds the intermediate payloads */
}resultBucket;

///////////////////////////

typedef struct fileNode
{
	uint64_t numRows;
	uint64_t numColumns;
	uint64_t *array;
}fileNode;

typedef struct intermediateNode
{
	uint64_t numRows;
	uint64_t *relations;
	uint64_t relationsSize;
	uint64_t **info;
	struct intermediateNode *next;
}iNode;

typedef struct sumsNode
{
	uint64_t *array;
	uint64_t arraySize;
	bool isNull;
	struct sumsNode *next;
}sumsNode;

#endif
