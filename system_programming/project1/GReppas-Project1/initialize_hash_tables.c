#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "types.h"
#include "initialize_hash_tables.h"

bool initialize_hash_tables( bucket ***senderHashTable, bucket ***recieverHashTable, myBucket ***myHashTable, treeBucket ***treeHashTable, int senderHashTableNumberOfEntries, int recieverHashTableNumberOfEntries)
{
	// allocate all memory space needed for each hash table

	int i;

	*senderHashTable = malloc(sizeof(bucket *)*senderHashTableNumberOfEntries);
	if ( *senderHashTable == NULL) { printf("Error with malloc\n"); return false; }
	for ( i = 0; i<senderHashTableNumberOfEntries; i++)
		(*senderHashTable)[i] = NULL;

	*recieverHashTable = malloc(sizeof(bucket *)*recieverHashTableNumberOfEntries);
	if ( *recieverHashTable == NULL) { printf("Error with malloc\n"); return false; }
	for ( i = 0; i<recieverHashTableNumberOfEntries; i++)
		(*recieverHashTable)[i] = NULL;

	*myHashTable = malloc(sizeof(myBucket *)*( (recieverHashTableNumberOfEntries+senderHashTableNumberOfEntries)/ 2));
	if ( *myHashTable == NULL) { printf("Error with malloc\n"); return false; }
	for ( i = 0; i< ( (recieverHashTableNumberOfEntries+senderHashTableNumberOfEntries)/ 2); i++)
		(*myHashTable)[i] = NULL;

	*treeHashTable = malloc(sizeof(treeBucket *)*( (recieverHashTableNumberOfEntries+senderHashTableNumberOfEntries)/ 2));
	if ( *treeHashTable == NULL) { printf("Error with malloc\n"); return false; }
	for ( i = 0; i< ( (recieverHashTableNumberOfEntries+senderHashTableNumberOfEntries)/ 2); i++)
		(*treeHashTable)[i] = NULL;

	return true;
}
