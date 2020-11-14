#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "types.h"
#include "free_memory.h"

void free_memory( bucket ***senderHashTable, bucket ***recieverHashTable, myBucket ***myHashTable, treeBucket ***treeHashTable, int senderHashTableNumberOfEntries, int recieverHashTableNumberOfEntries)
{
	int i;

	// free bukets for sender Hash Table
	for ( i=0; i<senderHashTableNumberOfEntries; i++)
		if ( (*senderHashTable)[i] != NULL)
			free_buckets( (*senderHashTable)[i]);

	// free buckets for reciever Hash Table
	for ( i=0; i<recieverHashTableNumberOfEntries; i++)
		if ( (*recieverHashTable)[i] != NULL)
			free_buckets( (*recieverHashTable)[i]);

	// free buckets for My Hash Table
	for ( i=0; i<( (recieverHashTableNumberOfEntries+senderHashTableNumberOfEntries) /2); i++)
		if ( (*myHashTable)[i] != NULL)
			free_buckets_for_my_structure( (*myHashTable)[i]);

	// free buckets for tree Hash Table
	for ( i=0; i<( (recieverHashTableNumberOfEntries+senderHashTableNumberOfEntries) /2); i++)
		if ( (*treeHashTable)[i] != NULL)
			free_buckets_for_tree_structure( (*treeHashTable)[i]);

	// free all hash tables
	free( *senderHashTable);
	free( *recieverHashTable);
	free( *myHashTable);
	free( *treeHashTable);
}

void free_list( listNode *current)
{
	if ( current->next != NULL) free_list( current->next);
	free(current->info);
	free(current);
}

void free_bitcoin_list( bitCoinNode *current)
{
	if ( current->next != NULL) free_bitcoin_list( current->next);
	free( current);
}

void free_tree( treeNode *current)
{
	// post-order

	if ( current == NULL)
		return;

	free_tree( current->left);
	free_tree( current->right);

	free( current->wId);
	free( current);
}

void free_buckets_for_tree_structure( treeBucket *current)
{
	// go to the last bucket
	if ( current->next != NULL) free_buckets_for_tree_structure( current->next);

	// if the bucket has a list free it
	free_tree( current->thead);

	// free the bucket
	free( current);
}

void free_buckets_for_my_structure( myBucket *current)
{
	// go to the last bucket
	if ( current->next != NULL) free_buckets_for_my_structure( current->next);

	// if the bucket has a list, free it
	free( current->name);
	if ( current->lhead != NULL) free_bitcoin_list( current->lhead);

	// free the bucket
	free( current);

}

void free_buckets( bucket *current)
{
	// go to the last bucket of the bucket list
	if ( current->next != NULL) free_buckets( current->next);

	// for every walletId in the bucket, free its list of transactions, if it exists
	int i;
	for ( i=0; i< current->arraySize ; i++)
	{
		free( current->walletIdArray[i].name);
		if ( current->walletIdArray[i].lhead != NULL)
			free_list( current->walletIdArray[i].lhead);
	}

	// once the list of every walletId is freed, free the bucket
	free( current->walletIdArray);
	free( current);
}
