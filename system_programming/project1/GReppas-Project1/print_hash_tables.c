#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "types.h"
#include "print_hash_tables.h"

void printGivenLevel( treeNode *root, int level)
{
	if ( root == NULL)
	{
		return;
	}
	if ( level == 1)
	{
		printf("|%s%d|   ",root->wId,root->money);
	}
	else if ( level > 1)
	{
		printGivenLevel( root->left, level-1);
		printGivenLevel( root->right, level-1);
	}
}

int height( treeNode *node)
{
	if ( node == NULL)
		return 0;
	else
	{
		int lheight = height( node->left);
		int rheight = height( node->right);

		if (lheight > rheight)
			return lheight+1;
		else
			return rheight+1;
	}
}

void printLevelOrder( treeNode *root)
{
	int h = height(root);
	int i;
	for ( i=1 ; i<=h ; i++)
	{
		printGivenLevel(root,i);
	}
}

void printAll( int senderHashTableNumberOfEntries, int recieverHashTableNumberOfEntries, bucket **senderHashTable, bucket **recieverHashTable, myBucket **myHashTable, treeBucket **treeHashTable)
{
	int i,j;
	bucket *currentBucket;
	listNode *currentListNode;
	myBucket *currentMyBucket;
	bitCoinNode *currentBitCoinNode;
	treeBucket *currentTreeBucket;
	treeNode *currentTreeNode;

	puts("-----------------------------Sender Hash Table-------------------------------------");
	for ( i=0; i<senderHashTableNumberOfEntries; i++)
	{
		printf("For Bucket %d:\n",i);
		currentBucket = senderHashTable[i];
		while ( currentBucket != NULL)
		{
			for ( j = 0 ; j<currentBucket->arraySize ; j++)
			{
				printf("\t%s\n",currentBucket->walletIdArray[j].name);
				printf("\t\tTransactions for %s:\n",currentBucket->walletIdArray[j].name);
				currentListNode = currentBucket->walletIdArray[j].lhead;
				while ( currentListNode != NULL)
				{
					printf("\t\t%s\n" , currentListNode->info);
					currentListNode = currentListNode->next;
				}
			}
			currentBucket = currentBucket->next;
		}
	}
	puts("---------------------------------------------------------------------------------\n");


	puts("-----------------------------Reciever Hash Table-----------------------------------");
	for ( i=0; i<recieverHashTableNumberOfEntries; i++)
	{
		printf("For Bucket %d:\n",i);
		currentBucket = recieverHashTable[i];
		while ( currentBucket != NULL)
		{
			for ( j = 0 ; j<currentBucket->arraySize ; j++)
			{
				printf("\t%s\n",currentBucket->walletIdArray[j].name);
				printf("\t\tTransactions for %s:\n",currentBucket->walletIdArray[j].name);
				currentListNode = currentBucket->walletIdArray[j].lhead;
				while ( currentListNode != NULL)
				{
					printf("\t\t%s\n", currentListNode->info);
					currentListNode = currentListNode->next;
				}
			}
			currentBucket = currentBucket->next;
		}
	}
	puts("---------------------------------------------------------------------------------\n");


	puts("-----------------------------My Hash Table-----------------------------------------");
	for ( i=0; i<((senderHashTableNumberOfEntries+recieverHashTableNumberOfEntries)/2) ; i++)
	{
		printf("For myBucket %d:\n",i);
		currentMyBucket = myHashTable[i];
		while ( currentMyBucket != NULL)
		{
			printf("\t%s with balance %d\n",currentMyBucket->name,currentMyBucket->balance);
			printf("\t\tBitCoins for %s:\n",currentMyBucket->name);
			currentBitCoinNode = currentMyBucket->lhead;
			while ( currentBitCoinNode != NULL)
			{
				printf("\t\tBitCoinId --> %d ||||||||||||| rest --> %d\n",currentBitCoinNode->bitCoinId,currentBitCoinNode->rest);
				currentBitCoinNode = currentBitCoinNode->next;
			}
			currentMyBucket = currentMyBucket->next;
		}
	}
	puts("---------------------------------------------------------------------------------\n");


	puts("-----------------------------Tree Hash Table-----------------------------------------");
	for ( i=0; i<((senderHashTableNumberOfEntries+recieverHashTableNumberOfEntries)/2) ; i++)
	{
		printf("For treeBucket %d:\n",i);
		currentTreeBucket = treeHashTable[i];
		while ( currentTreeBucket != NULL)
		{
			printf("\t BitcoinId: %d\n",currentTreeBucket->bCId);
			currentTreeNode = currentTreeBucket->thead;
			if ( ( currentTreeNode->left != NULL) || ( currentTreeNode->right != NULL) )
			{
				printLevelOrder(currentTreeNode);
			}
			currentTreeBucket = currentTreeBucket->next;
			puts("");
		}
	}
	puts("-----------------------------------------------------------------------------------");
}
