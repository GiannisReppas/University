#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "types.h"
#include "functions_for_error_checking.h"
#include "hash_function.h"
#include "bitCoinBalancesFile_functions.h"

bool create_my_structure_and_tree_structure_and_put_names_in_hash_tables( bucket **senderHashTable, bucket **recieverHashTable, myBucket **myHashTable, treeBucket **treeHashTable, int senderHashTableNumberOfEntries, int recieverHashTableNumberOfEntries, FILE *bCBF_fp, int bucketSize, int bitCoinValue)
{
	int c;
	char *str;
	int strLength;
	int counter = 0;
	int index;
	bool check;
	int bitCoinId;
	char *wId = NULL;

	while ( 1 ) // for each word in bitCoinBalancesFile
	{
		str = malloc(sizeof(char));
		if ( str == NULL) { printf("Error with malloc\n"); return false; }
		str[0] = '\0';

		do
		{
			c = fgetc( bCBF_fp);

			if ( (c==' ') || (c=='\n') || (c==EOF) || (c=='\t')) break;
			strLength = strlen(str);

			str = realloc(str,strLength+(sizeof(char)*2));
			if ( str == NULL) { printf("Error with ralloc\n"); return false; }

			str[strLength] = c;
			str[strLength+1] = '\0';
		}while ( 1 );

		// str --> word in bitCoinBalancesFile

		if ( c == '\t') { puts("No tabs allowed in bitCoinBalancesFile-this is forbidden-programm terminated"); exit(1); }
		if ( c == EOF) break;
		if ( ( c == '\n') && ( counter == 0) ) counter = 1;
		else if ( c == '\n') counter=0;
		if ( c == ' ') counter++;

		if ( !strcmp("",str))
		{
			puts("Continous white spaces in bitCoinBalancesFile-this is forbidden-programm terminated");
			exit(1);
		}

		// check if the name or the bitCoin read was all already given
		if ( counter == 1)
		{
			if ( !check_if_name_is_valid( str, myHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries) )
			{
				printf("name %s already given twice in bitCoinBalancesFile-error\n",str);
				exit(1);
			}
		}
		else
		{
			if ( !check_if_bitCoin_is_valid( atoi(str), treeHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries) )
			{
				printf("bitCoin %s already given twice in bitCoinBalancesFile-error\n",str);
				exit(1);
			}
		}

		// insert the name or the bitcoin in the hash tables
		if ( counter == 1) // counter == 1 means we have a name (walletId)
		{
			// my hash table name insertion
			free(wId);
			wId = malloc(sizeof(char) + strlen(str) );
			if ( wId == NULL) { printf("Error with malloc\n"); return false; }
			strcpy(wId,str);

			index = hash_function( str[0], (recieverHashTableNumberOfEntries+senderHashTableNumberOfEntries)/2);

			myBucket *newBucket;
			newBucket = malloc(sizeof(myBucket));
			if ( newBucket == NULL) { printf("Error with malloc\n"); return false; }
			newBucket->next = NULL;
			newBucket->balance = 0;
			newBucket->lhead = NULL;

			newBucket->name = malloc(strlen(str)+sizeof(char));
			if ( newBucket->name == NULL) { printf("Error with malloc\n"); return false; }
			strcpy( newBucket->name, str);

			if ( myHashTable[index] == NULL )
				myHashTable[index] = newBucket;
			else
			{
				myBucket *currentBucket;
				currentBucket = myHashTable[index];
				while ( currentBucket->next != NULL)
					currentBucket = currentBucket->next;

				currentBucket->next = newBucket;
			}

			// sender and receiver hash table name insertion
			if ( !put_name_in_sender_hash_table( senderHashTable, senderHashTableNumberOfEntries, str, bucketSize) ) return false;
			if ( !put_name_in_reciever_hash_table( recieverHashTable, recieverHashTableNumberOfEntries, str, bucketSize) ) return false;
		}
		else // counter != 1 means we have a bitcoin
		{
			// myHash table insertion
			bitCoinId = atoi(str);
			if ( bitCoinId <= 0) { puts("Error with bitCoinBalancesFile"); exit(1); }

			index = hash_function( wId[0], (recieverHashTableNumberOfEntries+senderHashTableNumberOfEntries)/2);

			myBucket *currentBucket;
			currentBucket = myHashTable[index];
			while( 1 )
			{
				if ( !strcmp(wId,currentBucket->name) )
				{

					currentBucket->balance += bitCoinValue;

					bitCoinNode *newNode;
					newNode = malloc(sizeof(bitCoinNode));
					if ( newNode == NULL) { printf("Error with malloc\n"); return false; }
					newNode->bitCoinId = bitCoinId;
					newNode->rest = bitCoinValue;
					newNode->next = NULL;

					if ( currentBucket->lhead == NULL)
						currentBucket->lhead = newNode;
					else
					{
						bitCoinNode *currentNode;
						currentNode = currentBucket->lhead;

						while ( currentNode->next != NULL)
						{
							currentNode = currentNode->next;
						}
						currentNode->next = newNode;
					}
					break;
				}
				currentBucket = currentBucket->next;
			}

			// tree hash table insertion
			index = hash_function( bitCoinId, (recieverHashTableNumberOfEntries+senderHashTableNumberOfEntries)/2);

			treeBucket *newTreeBucket;
			newTreeBucket = malloc( sizeof(treeBucket));
			if ( newTreeBucket == NULL) { printf("Error with malloc\n"); return false; }
			newTreeBucket->bCId = bitCoinId;
			newTreeBucket->next = NULL;
			treeNode *newTreeNode;
			newTreeNode = malloc( sizeof(treeNode));
			if ( newTreeNode == NULL) { printf("Error with malloc\n"); return false; }
			newTreeNode->money = bitCoinValue;
			newTreeNode->right = NULL;
			newTreeNode->left = NULL;
			newTreeNode->tran = NULL;
			newTreeNode->wId = malloc( strlen(wId) + sizeof(char));
			if ( newTreeNode->wId == NULL) { printf("Error with malloc\n"); return false; }
			strcpy( newTreeNode->wId, wId);
			newTreeBucket->thead = newTreeNode;

			treeBucket *currentTreeBucket;
			currentTreeBucket = treeHashTable[index];
			if ( treeHashTable[index] == NULL)
				treeHashTable[index] = newTreeBucket;
			else
			{
				while ( currentTreeBucket->next != NULL)
					currentTreeBucket = currentTreeBucket->next;
				currentTreeBucket->next = newTreeBucket;
			}
		}

		if ( c == '\n') counter = 0;

		free(str);
	}
	free(wId);
	free(str);

	return true;
}

bool put_name_in_reciever_hash_table( bucket **recieverHashTable, int recieverHashTableNumberOfEntries, char *str, int bucketSize)
{
	int index = hash_function( str[0], recieverHashTableNumberOfEntries);

	// if there are no buckets
	if ( recieverHashTable[index]== NULL )
	{
		bucket *newBucket;
		newBucket = malloc(sizeof(bucket));
		if ( newBucket == NULL) { printf("Error with malloc\n"); return false; }
		newBucket->next = NULL;
		newBucket->arraySize = 1;
		newBucket->remainingSpace = bucketSize;

		newBucket->walletIdArray = malloc(sizeof(walletId));
		if ( newBucket->walletIdArray == NULL) { printf("Error with malloc\n"); return false; }

		newBucket->walletIdArray[0];
		newBucket->walletIdArray[0].name = malloc(strlen(str)+sizeof(char));
		if ( newBucket->walletIdArray[0].name == NULL) { printf("Error with malloc\n"); return false; }
		strcpy( newBucket->walletIdArray[0].name, str);
		newBucket->walletIdArray[0].lhead = NULL;

		recieverHashTable[index] = newBucket;
	}
	// if there already are buckets
	else
	{
		bucket *currentBucket;
		currentBucket = recieverHashTable[index];
		while ( currentBucket->next != NULL)
		currentBucket = currentBucket->next;
		if ( strlen(str)+sizeof(char)+sizeof(walletId) < currentBucket->remainingSpace) // if the bucket has space for another walletId
		{
			currentBucket->arraySize++;
			currentBucket->remainingSpace -= strlen(str)+sizeof(char)+sizeof(walletId);
			currentBucket->walletIdArray = realloc( currentBucket->walletIdArray, sizeof(walletId)*currentBucket->arraySize);
			if( currentBucket->walletIdArray == NULL) { printf("Error with realloc\n"); return false; }

			currentBucket->walletIdArray[currentBucket->arraySize-1].name = malloc(strlen(str)+sizeof(char));
			if ( currentBucket->walletIdArray[currentBucket->arraySize-1].name == NULL) { printf("Error with malloc\n"); return false; }
			strcpy( currentBucket->walletIdArray[currentBucket->arraySize-1].name, str);
			currentBucket->walletIdArray[currentBucket->arraySize-1].lhead = NULL;
		}
		else // if there is no more space for another walletId, then create a new bucket
		{
			bucket *newBucket;
			newBucket = malloc(sizeof(bucket));
			if ( newBucket == NULL) { printf("Error with malloc\n"); return false; }

			newBucket->next = NULL;
			newBucket->arraySize = 1;
			newBucket->remainingSpace = bucketSize;
			newBucket->walletIdArray = malloc(sizeof(walletId));
			if ( newBucket->walletIdArray == NULL) { printf("Error with malloc\n"); return false; }

			newBucket->walletIdArray[0].name = malloc(strlen(str)+sizeof(char));
			if ( newBucket->walletIdArray[0].name == NULL) { printf("Error with malloc\n"); return false; }
			strcpy( newBucket->walletIdArray[0].name, str);
			newBucket->walletIdArray[0].lhead = NULL;

			currentBucket->next = newBucket;
		}
	}

	return true;
}

bool put_name_in_sender_hash_table( bucket **senderHashTable, int senderHashTableNumberOfEntries, char *str, int bucketSize)
{
	int index = hash_function( str[0], senderHashTableNumberOfEntries);

	// if there are no buckets
	if ( senderHashTable[index]== NULL )
	{
		bucket *newBucket;
		newBucket = malloc(sizeof(bucket));
		if ( newBucket == NULL) { printf("Error with malloc\n"); return false; }
		newBucket->next = NULL;
		newBucket->arraySize = 1;
		newBucket->remainingSpace = bucketSize;

		newBucket->walletIdArray = malloc(sizeof(walletId));
		if ( newBucket->walletIdArray == NULL) { printf("Error with malloc\n"); return false; }

		newBucket->walletIdArray[0];
		newBucket->walletIdArray[0].name = malloc(strlen(str)+sizeof(char));
		if ( newBucket->walletIdArray[0].name == NULL) { printf("Error with malloc\n"); return false; }
		strcpy( newBucket->walletIdArray[0].name, str);
		newBucket->walletIdArray[0].lhead = NULL;

		senderHashTable[index] = newBucket;
	}
	// if there already are buckets
	else
	{
		bucket *currentBucket;
		currentBucket = senderHashTable[index];
		while ( currentBucket->next != NULL)
		currentBucket = currentBucket->next;
		if ( strlen(str)+sizeof(char)+sizeof(walletId) < currentBucket->remainingSpace) // if the bucket has space for another walletId
		{
			currentBucket->arraySize++;
			currentBucket->remainingSpace -= strlen(str)+sizeof(char)+sizeof(walletId);
			currentBucket->walletIdArray = realloc( currentBucket->walletIdArray, sizeof(walletId)*currentBucket->arraySize);
			if( currentBucket->walletIdArray == NULL) { printf("Error with realloc\n"); return false; }

			currentBucket->walletIdArray[currentBucket->arraySize-1].name = malloc(strlen(str)+sizeof(char));
			if ( currentBucket->walletIdArray[currentBucket->arraySize-1].name == NULL) { printf("Error with malloc\n"); return false; }
			strcpy( currentBucket->walletIdArray[currentBucket->arraySize-1].name, str);
			currentBucket->walletIdArray[currentBucket->arraySize-1].lhead = NULL;
		}
		else // if there is no more space for another walletId, then create a new bucket
		{
			bucket *newBucket;
			newBucket = malloc(sizeof(bucket));
			if ( newBucket == NULL) { printf("Error with malloc\n"); return false; }

			newBucket->next = NULL;
			newBucket->arraySize = 1;
			newBucket->remainingSpace = bucketSize;
			newBucket->walletIdArray = malloc(sizeof(walletId));
			if ( newBucket->walletIdArray == NULL) { printf("Error with malloc\n"); return false; }

			newBucket->walletIdArray[0].name = malloc(strlen(str)+sizeof(char));
			if ( newBucket->walletIdArray[0].name == NULL) { printf("Error with malloc\n"); return false; }
			strcpy( newBucket->walletIdArray[0].name, str);
			newBucket->walletIdArray[0].lhead = NULL;

			currentBucket->next = newBucket;
		}
	}

	return true;
}
