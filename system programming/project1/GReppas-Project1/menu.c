#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "types.h"
#include "hash_function.h"
#include "functions_for_error_checking.h"
#include "transactionsFile_functions.h"
#include "itoa_plus_nod.h"
#include "menu.h"

void question8recursive( treeNode *current, char *sender)
{
	// pre ordero

	// if a treenode does not have a pointer to a transaction and its left child has then this node and its children are all part of a transaction

	if ( current == NULL)
		return;

	if ( ( current->tran != NULL) && ( !current->duplicated) )
	{
		int i=0;
		while( current->tran->info[i] != ' ')
		{
			putchar(current->tran->info[i]);
			i++;
		}
		putchar(current->tran->info[i]);
		printf("%s",sender);
		char *temp=(current->tran->info+i);
		puts(temp);
	}

	question8recursive( current->left, current->wId);

	question8recursive( current->right, current->wId);
}

bool question8( char *bCId, treeBucket **treeHashTable, int senderHashTableNumberOfEntries, int recieverHashTableNumberOfEntries)
{
	// go to the bitcoin tree in my hash table
	int bitCoinId = atoi(bCId);
	int index = hash_function( bitCoinId, (senderHashTableNumberOfEntries+recieverHashTableNumberOfEntries)/2);

	treeBucket *currentTreeBucket;
	currentTreeBucket = treeHashTable[index];

	while( currentTreeBucket != NULL)
	{
		if ( currentTreeBucket->bCId == bitCoinId)
		{
			treeNode *current;
			current = currentTreeBucket->thead;

			question8recursive( current, NULL);

			return true;
		}
		currentTreeBucket = currentTreeBucket->next;
	}

	return false;
}

// this function returns the unspent amount of a bitCoin
void question7unspent( treeNode *current, int *unspent)
{
	// the tree has a specific structure and the rightest node will always have the unspent amount of the bitcoin

	while ( current->right != NULL)
		current = current->right;

	if ( current->left == NULL) *unspent = current->money;
	else *unspent = 0;
}

// this function counts the transactions in tree
void question7counter( treeNode *current, int *counter)
{
	// visit all nodes in tree with pre order
	// if a node has a pointer to a transaction then it means we have a transaction
	// current->duplicated means the transaction for the current treenode is being described by another treenode

	if ( current == NULL)
		return;

	if ( ( current->tran != NULL) && ( !current->duplicated) )
	{
		*counter += 1;
	}

	question7counter( current->left, counter);

	question7counter( current->right, counter);
}

bool question7( char *bCId, treeBucket **treeHashTable, int senderHashTableNumberOfEntries, int recieverHashTableNumberOfEntries)
{
	// go to the bitcoin tree in tree hash table
	int bitCoinId = atoi(bCId);
	int index = hash_function( bitCoinId, (senderHashTableNumberOfEntries+recieverHashTableNumberOfEntries)/2);

	treeBucket *currentTreeBucket;
	currentTreeBucket = treeHashTable[index];

	while( currentTreeBucket != NULL)
	{
		if ( currentTreeBucket->bCId == bitCoinId)
		{
			treeNode *current;

			current = currentTreeBucket->thead;
			int counter=0;
			question7counter( current, &counter);

			current = currentTreeBucket->thead;
			int unspent = -1;
			question7unspent( current, &unspent);

			printf("%d %d %d\n",bitCoinId,counter,unspent);

			return true;
		}
		currentTreeBucket = currentTreeBucket->next;
	}

	return false;
}

bool question6( char *name, myBucket **myHashTable, int senderHashTableNumberOfEntries, int recieverHashTableNumberOfEntries)
{
	// return money in wallet
	int index = hash_function( name[0], (senderHashTableNumberOfEntries+recieverHashTableNumberOfEntries)/2 );

	myBucket *currentMyBucket;
	currentMyBucket = myHashTable[index];
	while ( currentMyBucket != NULL)
	{
		if ( !strcmp(name, currentMyBucket->name))
		{
			printf("%d\n",currentMyBucket->balance);
			return true;
		}
		currentMyBucket = currentMyBucket->next;
	}

	return false;
}

bool question5withoutArguements( char *name, bucket **senderHashTable, int senderHashTableNumberOfEntries)
{
	// go to the transaction list of the sender
	int index = hash_function( name[0], senderHashTableNumberOfEntries);

	bucket *currentBucket;
	currentBucket = senderHashTable[index];

	int i,j; char *temp; int total = 0;
	while ( currentBucket != NULL)
	{
		for ( i=0; i<currentBucket->arraySize; i++)
		{
			if ( !strcmp(name,currentBucket->walletIdArray[i].name))
			{
				// print them all
				listNode *current=currentBucket->walletIdArray[i].lhead;
				while ( current != NULL)
				{
					i = 0;
					while( current->info[i] != ' ') i++;

					current->info[i] = '\0';
					printf("%s %s",current->info,name);
					current->info[i] = ' ';
					printf("%s\n",current->info + i);

					i++;
					while( current->info[i] != ' ') i++;
					i++;

					j = 0;
					while ( current->info[i] != ' ') { i++; j++; }
					j++;

					temp = malloc(sizeof(char)*j);
					if ( temp == NULL)
					{
						puts("Error with malloc");
						exit(1);
					}

					i--;
					while ( current->info[i] != ' ') i--;

					j=0;
					i++;
					while( current->info[i] != ' ') { temp[j] = current->info[i]; i++; j++; }
					temp[j] = '\0';

					total += atoi(temp);

					free(temp);
					current = current->next;
				}
				printf("Total money from payments: %d\n",total);
				return true;
			}
		}
		currentBucket = currentBucket->next;
	}

	return false;
}

bool question5withArguements( char *name, bucket **senderHashTable, int senderHashTableNumberOfEntries, char *time1, char *date1, char *time2, char *date2)
{
	// go to the transaction list of the sender
	int index = hash_function( name[0], senderHashTableNumberOfEntries);

	bucket *currentBucket;
	currentBucket = senderHashTable[index];

	int i,j; char *temp; int total = 0; bool check; bool checkTime1,checkTime2;
	while ( currentBucket != NULL)
	{
		for ( i=0; i<currentBucket->arraySize; i++)
		{
			if ( !strcmp(name,currentBucket->walletIdArray[i].name))
			{
				listNode *current=currentBucket->walletIdArray[i].lhead;
				while ( current != NULL) // while loop for every transaction of the sender
				{
					check = true;
					checkTime1 = false; checkTime2 = false;
					i = 0;
					while( current->info[i] != ' ') i++;
					i++;
					while( current->info[i] != ' ') i++;
					i++;
					while( current->info[i] != ' ') i++;
					i++;

					// compare with date2
					if ( date2[6] == current->info[i+6])
					{
						if ( date2[7] == current->info[i+7])
						{
							if ( date2[8] == current->info[i+8])
							{
								if ( date2[9] == current->info[i+9])
								{
									if ( date2[3] == current->info[i+3])
									{
										if ( date2[4] == current->info[i+4])
										{
											if ( date2[0] == current->info[i+0])
											{
												if ( date2[1] == current->info[i+1]) checkTime2 = true;
												else if ( date2[1] < current->info[i+1]) check = false;
											}
											else if ( date2[0] < current->info[i+0]) check = false;
										}
										else if ( date2[4] < current->info[i+4]) check = false;
									}
									else if ( date2[3] < current->info[i+3]) check = false;
								}
								else if ( date2[9] < current->info[i+9]) check = false;
							}
							else if ( date2[8] < current->info[i+8]) check = false;
						}
						else if ( date2[7] < current->info[i+7]) check = false;
					}
					else if ( date2[6] < current->info[i+6]) check = false;

					// compare with date1
					if ( date1[6] == current->info[i+6])
					{
						if ( date1[7] == current->info[i+7])
						{
							if ( date1[8] == current->info[i+8])
							{
								if ( date1[9] == current->info[i+9])
								{
									if ( date1[3] == current->info[i+3])
									{
										if ( date1[4] == current->info[i+4])
										{
											if ( date1[0] == current->info[i+0])
											{
												if ( date1[1] == current->info[i+1]) checkTime1 = true;
												else if ( date1[1] > current->info[i+1]) check = false;
											}
											else if ( date1[0] > current->info[i+0]) check = false;
										}
										else if ( date1[4] > current->info[i+4]) check = false;
									}
									else if ( date1[3] > current->info[i+3]) check = false;
								}
								else if ( date1[9] > current->info[i+9]) check = false;
							}
							else if ( date1[8] > current->info[i+8]) check = false;
						}
						else if ( date1[7] > current->info[i+7]) check = false;
					}
					else if ( date1[6] > current->info[i+6]) check = false;

					while( current->info[i] != ' ') i++;
					i++;

					if ( checkTime2)
					{
						// compare with time2
						if ( time2[0] == current->info[i+0])
						{
							if ( time2[1] == current->info[i+1])
							{
								if ( time2[3] == current->info[i+3])
								{
									if ( time2[4] == current->info[i+4]) ;
									else if ( time2[4] < current->info[i+4]) check = false;
								}
								else if ( time2[3] < current->info[i+3]) check = false;
							}
							else if ( time2[1] < current->info[i+1]) check = false;
						}
						else if ( time2[0] < current->info[i+0]) check = false;
					}

					if ( checkTime1)
					{
						// compare with time1
						if ( time1[0] == current->info[i+0])
						{
							if ( time1[1] == current->info[i+1])
							{
								if ( time1[3] == current->info[i+3])
								{
									if ( time1[4] == current->info[i+4]) ;
									else if ( time1[4] > current->info[i+4]) check = false;
								}
								else if ( time1[3] > current->info[i+3]) check = false;
							}
							else if ( time2[1] > current->info[i+1]) check = false;
						}
						else if ( time2[0] > current->info[i+0]) check = false;
					}

					if ( check) // if the date and time of the transaction is between the date and time given
					{
						i = 0;
						while( current->info[i] != ' ') i++;

						current->info[i] = '\0';
						printf("%s %s",current->info,name);
						current->info[i] = ' ';
						printf("%s\n",current->info + i);

						i++;
						while( current->info[i] != ' ') i++;
						i++;

						j = 0;
						while ( current->info[i] != ' ') { i++; j++; }
						j++;

						temp = malloc(sizeof(char)*j);
						if ( temp == NULL)
						{
							puts("Error with malloc");
							exit(1);
						}

						i--;
						while ( current->info[i] != ' ') i--;

						j=0;
						i++;
						while( current->info[i] != ' ') { temp[j] = current->info[i]; i++; j++; }
						temp[j] = '\0';

						total += atoi(temp);

						free(temp);
					}
					current = current->next;
				}
				printf("Total money from payments: %d\n",total);
				return true;
			}
		}
		currentBucket = currentBucket->next;
	}

	return false;
}

bool question4withoutArguements( char *name, bucket **recieverHashTable, int recieverHashTableNumberOfEntries)
{
	// go to the transaction list
	int index = hash_function( name[0], recieverHashTableNumberOfEntries);

	bucket *currentBucket;
	currentBucket = recieverHashTable[index];

	int i,j; char *temp; int total = 0;
	while ( currentBucket != NULL)
	{
		for ( i=0; i<currentBucket->arraySize; i++)
		{
			if ( !strcmp(name,currentBucket->walletIdArray[i].name))
			{
				// print them all
				listNode *current=currentBucket->walletIdArray[i].lhead;
				while ( current != NULL)
				{
					i = 0;
					while( current->info[i] != ' ') i++;
					i++;
					while( current->info[i] != ' ') i++;

					current->info[i] = '\0';
					printf("%s %s",current->info,name);
					current->info[i] = ' ';
					printf("%s\n",current->info + i);

					i++;

					j = 0;
					while ( current->info[i] != ' ') { i++; j++; }
					j++;

					temp = malloc(sizeof(char)*j);
					if ( temp == NULL)
					{
						puts("Error with malloc");
						exit(1);
					}

					i--;
					while ( current->info[i] != ' ') i--;

					j=0;
					i++;
					while( current->info[i] != ' ') { temp[j] = current->info[i]; i++; j++; }
					temp[j] = '\0';

					total += atoi(temp);

					free(temp);
					current = current->next;
				}
				printf("Total money from earnings: %d\n",total);
				return true;
			}
		}
		currentBucket = currentBucket->next;
	}

	return false;
}

bool question4withArguements( char *name, bucket **recieverHashTable, int recieverHashTableNumberOfEntries, char *time1, char *date1, char *time2, char *date2)
{
	// go to the transactions of unser with walletId name
	int index = hash_function( name[0], recieverHashTableNumberOfEntries);

	bucket *currentBucket;
	currentBucket = recieverHashTable[index];

	int i,j; char *temp; int total = 0; bool check; bool checkTime1,checkTime2;
	while ( currentBucket != NULL)
	{
		for ( i=0; i<currentBucket->arraySize; i++)
		{
			if ( !strcmp(name,currentBucket->walletIdArray[i].name))
			{
				listNode *current=currentBucket->walletIdArray[i].lhead;
				// while loop to print the transactions
				while ( current != NULL)
				{
					check = true;
					checkTime1 = false; checkTime2 = false;
					i = 0;
					while( current->info[i] != ' ') i++;
					i++;
					while( current->info[i] != ' ') i++;
					i++;
					while( current->info[i] != ' ') i++;
					i++;

					// compare with date2
					if ( date2[6] == current->info[i+6])
					{
						if ( date2[7] == current->info[i+7])
						{
							if ( date2[8] == current->info[i+8])
							{
								if ( date2[9] == current->info[i+9])
								{
									if ( date2[3] == current->info[i+3])
									{
										if ( date2[4] == current->info[i+4])
										{
											if ( date2[0] == current->info[i+0])
											{
												if ( date2[1] == current->info[i+1]) checkTime2 = true;
												else if ( date2[1] < current->info[i+1]) check = false;
											}
											else if ( date2[0] < current->info[i+0]) check = false;
										}
										else if ( date2[4] < current->info[i+4]) check = false;
									}
									else if ( date2[3] < current->info[i+3]) check = false;
								}
								else if ( date2[9] < current->info[i+9]) check = false;
							}
							else if ( date2[8] < current->info[i+8]) check = false;
						}
						else if ( date2[7] < current->info[i+7]) check = false;
					}
					else if ( date2[6] < current->info[i+6]) check = false;

					// compare with date1
					if ( date1[6] == current->info[i+6])
					{
						if ( date1[7] == current->info[i+7])
						{
							if ( date1[8] == current->info[i+8])
							{
								if ( date1[9] == current->info[i+9])
								{
									if ( date1[3] == current->info[i+3])
									{
										if ( date1[4] == current->info[i+4])
										{
											if ( date1[0] == current->info[i+0])
											{
												if ( date1[1] == current->info[i+1]) checkTime1 = true;
												else if ( date1[1] > current->info[i+1]) check = false;
											}
											else if ( date1[0] > current->info[i+0]) check = false;
										}
										else if ( date1[4] > current->info[i+4]) check = false;
									}
									else if ( date1[3] > current->info[i+3]) check = false;
								}
								else if ( date1[9] > current->info[i+9]) check = false;
							}
							else if ( date1[8] > current->info[i+8]) check = false;
						}
						else if ( date1[7] > current->info[i+7]) check = false;
					}
					else if ( date1[6] > current->info[i+6]) check = false;

					while( current->info[i] != ' ') i++;
					i++;

					if ( checkTime2)
					{
						// compare with time2
						if ( time2[0] == current->info[i+0])
						{
							if ( time2[1] == current->info[i+1])
							{
								if ( time2[3] == current->info[i+3])
								{
									if ( time2[4] == current->info[i+4]) ;
									else if ( time2[4] < current->info[i+4]) check = false;
								}
								else if ( time2[3] < current->info[i+3]) check = false;
							}
							else if ( time2[1] < current->info[i+1]) check = false;
						}
						else if ( time2[0] < current->info[i+0]) check = false;
					}

					if ( checkTime1)
					{
						// compare with time1
						if ( time1[0] == current->info[i+0])
						{
							if ( time1[1] == current->info[i+1])
							{
								if ( time1[3] == current->info[i+3])
								{
									if ( time1[4] == current->info[i+4]) ;
									else if ( time1[4] > current->info[i+4]) check = false;
								}
								else if ( time1[3] > current->info[i+3]) check = false;
							}
							else if ( time2[1] > current->info[i+1]) check = false;
						}
						else if ( time2[0] > current->info[i+0]) check = false;
					}

					if ( check) // if the date of the transaction in between the dates given
					{
						i = 0;
						while( current->info[i] != ' ') i++;

						i++;
						while( current->info[i] != ' ') i++;

						current->info[i] = '\0';
						printf("%s %s",current->info,name);
						current->info[i] = ' ';
						printf("%s\n",current->info + i);

						i++;

						j = 0;
						while ( current->info[i] != ' ') { i++; j++; }
						j++;

						temp = malloc(sizeof(char)*j);
						if ( temp == NULL)
						{
							puts("Error with malloc");
							exit(1);
						}

						i--;
						while ( current->info[i] != ' ') i--;

						j=0;
						i++;
						while( current->info[i] != ' ') { temp[j] = current->info[i]; i++; j++; }
						temp[j] = '\0';

						total += atoi(temp);

						free(temp);
					}
					current = current->next;
				}
				printf("Total money from earnings: %d\n",total);
				return true;
			}
		}
		currentBucket = currentBucket->next;
	}

	return false;
}

bool question1( int transactionId, char *sender, char *reciever, char *ammount, char *date, char *time, bucket **senderHashTable, bucket **recieverHashTable, myBucket **myHashTable, treeBucket **treeHashTable, int senderHashTableNumberOfEntries, int recieverHashTableNumberOfEntries)
{
	// we follow the same steps we did in transactionsFile_functions.c\

	int i;

	// first, we check if the sender has enough money for the transaction
	int index = hash_function( sender[0], (senderHashTableNumberOfEntries+recieverHashTableNumberOfEntries) / 2 );
	myBucket *currentMyBucket;
	currentMyBucket = myHashTable[index];
	while ( currentMyBucket != NULL)
	{
		if ( !strcmp(sender,currentMyBucket->name))
		{
			if ( currentMyBucket->balance >= atoi(ammount)) break;
			else { puts("Not enough money for this transaction in sender's wallet"); return false; }
		}
		currentMyBucket = currentMyBucket->next;
	}

	// do the transaction for senderHashTable
	index = hash_function( sender[0], senderHashTableNumberOfEntries);

	listNode *pointerForTree;
	bucket *currentBucket;
	currentBucket = senderHashTable[index];
	bool check = false;
	while ( currentBucket != NULL)
	{
		for ( i=0; i<currentBucket->arraySize; i++)
		{
			if ( !strcmp(currentBucket->walletIdArray[i].name,sender))
			{
				listNode *current;
				current = currentBucket->walletIdArray[i].lhead;

				char *tId;
				tId = malloc( sizeof(char)*number_of_digits(transactionId));
				if ( tId == NULL) { puts("Error with malloc\n"); exit(1); }
				integer_to_string( transactionId, tId);

				listNode *newNode;
				newNode = malloc(sizeof(listNode));
				if ( newNode == NULL) { printf("Error with malloc\n"); exit(1); }
				newNode->next = NULL;
				newNode->info = malloc( strlen(reciever) + strlen(date) + strlen(time) + strlen(tId) + strlen(ammount) + 5*sizeof(char));
				if ( newNode->info == NULL) { puts("Error with malloc"); exit(1);}
				strcpy(newNode->info,tId); strcat(newNode->info," ");
				strcat(newNode->info,reciever); strcat(newNode->info," ");
				strcat(newNode->info,ammount); strcat(newNode->info," ");
				strcat(newNode->info,date); strcat(newNode->info," ");
				strcat(newNode->info,time);

				pointerForTree = newNode;

				if ( current == NULL)
					currentBucket->walletIdArray[i].lhead = newNode;
				else
				{
					while ( current->next != NULL)
						current = current->next;
					current->next = newNode;
				}

				check = true;
				free(tId);
				break;
			}
		}
		if ( !check) currentBucket = currentBucket->next;
		else break;
	}

	// do the transaction for recieverHashTable
	index = hash_function( reciever[0], recieverHashTableNumberOfEntries);

	currentBucket = recieverHashTable[index];
	check = false;
	while ( currentBucket != NULL)
	{
		for ( i=0; i<currentBucket->arraySize; i++)
		{
			if ( !strcmp(currentBucket->walletIdArray[i].name,reciever))
			{
				listNode *current;
				current = currentBucket->walletIdArray[i].lhead;

				char *tId;
				tId = malloc( sizeof(char)*number_of_digits(transactionId));
				if ( tId == NULL) { puts("Error with malloc\n"); exit(1); }
				integer_to_string( transactionId, tId);

				listNode *newNode;
				newNode = malloc(sizeof(listNode));
				if ( newNode == NULL) { printf("Error with malloc\n"); exit(1); }
				newNode->next = NULL;
				newNode->info = malloc( strlen(sender) + strlen(date) + strlen(time) + strlen(tId) + strlen(ammount) + 5*sizeof(char));
				if ( newNode->info == NULL) { puts("Error with malloc"); exit(1); }
				strcpy(newNode->info,tId); strcat(newNode->info," ");
				strcat(newNode->info,sender); strcat(newNode->info," ");
				strcat(newNode->info,ammount); strcat(newNode->info," ");
				strcat(newNode->info,date); strcat(newNode->info," ");
				strcat(newNode->info,time);

				if ( current == NULL)
					currentBucket->walletIdArray[i].lhead = newNode;
				else
				{
					while ( current->next != NULL)
						current = current->next;
					current->next = newNode;
				}

				check = true;
				free(tId);
				break;
			}
		}
		if ( !check) currentBucket = currentBucket->next;
		else break;
	}

	// do the transaction for myHashTable and for treeHashTable
	int dBClength = 0;
	int *destroyedBitCoins=NULL; int *destroyedBitCoinsValues=NULL; int *destroyedBitCoinsOther=NULL;
	index = hash_function( sender[0], (senderHashTableNumberOfEntries+recieverHashTableNumberOfEntries)/2);

	myBucket *currentMy;
	currentMy = myHashTable[index];
	while ( currentMy != NULL)
	{
		if ( !strcmp(sender,currentMy->name))
		{
			currentMy->balance -= atoi(ammount);
			int tempVal = atoi(ammount);

			bitCoinNode *previousNode;
			bitCoinNode *currentNode;
			currentNode = currentMy->lhead;
			while ( 1 )
			{
				if ( tempVal < currentNode->rest)
				{
					destroyedBitCoins = realloc( destroyedBitCoins, sizeof(int)*(1+dBClength));
					if ( destroyedBitCoins == NULL) { printf("Error with realloc\n"); return false; }
					destroyedBitCoins[dBClength] = currentNode->bitCoinId;
					destroyedBitCoinsValues = realloc( destroyedBitCoinsValues, sizeof(int)*(1+dBClength));
					if ( destroyedBitCoinsValues == NULL) { printf("Error with realloc\n"); return false; }
					destroyedBitCoinsValues[dBClength] = tempVal;
					dBClength++;

					currentNode->rest -= tempVal;

					tempVal = 0;

					destroyedBitCoinsOther = realloc( destroyedBitCoinsOther, sizeof(int)*dBClength);
					if ( destroyedBitCoinsOther == NULL) { printf("Error with malloc\n"); return false; }
					destroyedBitCoinsOther[dBClength - 1] = currentNode->rest;

					break;
				}
				else if ( tempVal == currentNode->rest)
				{
					destroyedBitCoins = realloc( destroyedBitCoins, sizeof(int)*(1+dBClength));
					if ( destroyedBitCoins == NULL) { printf("Error with realloc\n"); return false; }
					destroyedBitCoins[dBClength] = currentNode->bitCoinId;
					destroyedBitCoinsValues = realloc( destroyedBitCoinsValues, sizeof(int)*(1+dBClength));
					if ( destroyedBitCoinsValues == NULL) { printf("Error with realloc\n"); return false; }
					destroyedBitCoinsValues[dBClength] = currentNode->rest;
					dBClength++;

					if ( currentNode == currentMy->lhead)
					{
						currentMy->lhead = currentNode->next;
						free( currentNode);
					}
					else
					{
						previousNode->next = currentNode->next;
						free( currentNode);
					}

					tempVal = 0;

					destroyedBitCoinsOther = realloc( destroyedBitCoinsOther, sizeof(int)*dBClength);
					if ( destroyedBitCoinsOther == NULL) { printf("Error with malloc\n"); return false; }
					destroyedBitCoinsOther[dBClength - 1] = 0;

					break;
				}
				else
				{
					destroyedBitCoins = realloc( destroyedBitCoins, sizeof(int)*(1+dBClength));
					if ( destroyedBitCoins == NULL) { printf("Error with realloc\n"); return false; }
					destroyedBitCoins[dBClength] = currentNode->bitCoinId;
					destroyedBitCoinsValues = realloc( destroyedBitCoinsValues, sizeof(int)*(1+dBClength));
					if ( destroyedBitCoinsValues == NULL) { printf("Error with realloc\n"); return false; }
					destroyedBitCoinsValues[dBClength] = currentNode->rest;
					dBClength++;

					tempVal -= currentNode->rest;

					if ( currentNode == currentMy->lhead)
					{
						currentMy->lhead = currentNode->next;
						free( currentNode);
					}
					else
					{
						previousNode->next = currentNode->next;
						free( currentNode);
					}

					destroyedBitCoinsOther = realloc( destroyedBitCoinsOther, sizeof(int)*dBClength);
					if ( destroyedBitCoinsOther == NULL) { printf("Error with malloc\n"); return false; }
					destroyedBitCoinsOther[dBClength - 1] = 0;
				}
				previousNode = currentNode;
				currentNode = currentNode->next;
			}
			break;
		}
		currentMy = currentMy->next;
	}

	index = hash_function( reciever[0], (senderHashTableNumberOfEntries+recieverHashTableNumberOfEntries)/2);

	currentMy = myHashTable[index];
	while ( currentMy != NULL)
	{
		if ( !strcmp(currentMy->name,reciever) )
		{
			for ( i=0; i<dBClength; i++)
				currentMy->balance += destroyedBitCoinsValues[i];

			bitCoinNode *currentNode;
			currentNode = currentMy->lhead;
			int temp = 0;
			while ( temp <= (dBClength-1) )
			{
				check = false;
				while ( currentNode != NULL)
				{
					if ( destroyedBitCoins[temp] == currentNode->bitCoinId)
					{
						currentNode->rest += destroyedBitCoinsValues[temp];
						check = true;
						break;
					}
					currentNode = currentNode->next;
				}
				if ( !check )
				{
					bitCoinNode *newNode;
					newNode = malloc(sizeof(bitCoinNode));
					if ( newNode == NULL) { printf("Error with malloc\n"); return false; }
					newNode->next = NULL;
					newNode->bitCoinId = destroyedBitCoins[temp];
					newNode->rest = destroyedBitCoinsValues[temp];

					currentNode = currentMy->lhead;
					if ( currentMy->lhead != NULL)
					{
						while ( currentNode->next != NULL)
							currentNode = currentNode->next;

						currentNode->next = newNode;
					}
					else
						currentMy->lhead = newNode;
				}
				temp++;
				currentNode = currentMy->lhead;
			}

			for ( i=0; i<dBClength ; i++)
			{
				index = hash_function( destroyedBitCoins[i], (senderHashTableNumberOfEntries+recieverHashTableNumberOfEntries)/2 );

				treeBucket *currentTreeBucket;
				currentTreeBucket = treeHashTable[index];

				while ( 1 )
				{
					if ( currentTreeBucket->bCId == destroyedBitCoins[i] )
					{
						treeNode *currentTreeNode;
						currentTreeNode = currentTreeBucket->thead;

						int moneyForSender = destroyedBitCoinsOther[i];
						int moneyForReciever = destroyedBitCoinsValues[i];
						bool dup = false;

						put_new_nodes_in_tree( false, currentTreeNode, sender, reciever, &moneyForSender, &moneyForReciever, pointerForTree, &dup);

						break;
					}
					currentTreeBucket = currentTreeBucket->next;
				}
			}

			dBClength = 0;
			break;
		}
		currentMy = currentMy->next;
	}

	free(destroyedBitCoins);
	free(destroyedBitCoinsValues);
	free(destroyedBitCoinsOther);

	return true;
}

bool menu( char **lastDateTime, int *lastTransactionId, bucket **senderHashTable, bucket **recieverHashTable, myBucket **myHashTable, treeBucket **treeHashTable, int senderHashTableNumberOfEntries, int recieverHashTableNumberOfEntries)
{
	bool check;
	char *final;
	final = malloc(sizeof(char));
	if ( final == NULL) { printf("Error with malloc\n"); return false; }
	final[0] = '\0';
	char *input;
	char help[4]=" \n";
	char *str1; char *str2; char *str3; char *str4; char *str5; char *str6;
	time_t currentDateTime;
	char *temp=NULL;
	char *date; char *tim;
	char *anotherFinal;
	FILE *inputfile;
	int c;
	bool leaveRequestTransactions;

	while ( 1 )
	{
		puts("-----------------------------------------------------------------------------------------------------------");
		puts("1. /requestTransaction senderWalletID receiverWalletID amount date time");
		puts("2. /requestTransactions senderWalletID receiverWalletID amount date time; senderWalletID2 receiverWalletID2 amount2 date2 time2; ... senderWalletIDn receiverWalletIDn amountn daten timen; - type /endRequestTransactions to return to menu");
		puts("3. /requestTransactions inputFile - correct structure of inputfile must be all transactions in the same line and with only one space");
		puts("4. /findEarnings walletID [time1] [year1] [time2] [year2]");
		puts("5. /findPayments walletID [time1] [year1] [time2] [year2]");
		puts("6. /walletStatus walletID");
		puts("7. /bitCoinStatus bitCoinID");
		puts("8. /traceCoin bitCoinID");
		puts("9. /exit --> exit");
		puts("-----------------------------------------------------------------------------------------------------------");

		// read input from command line using fgets
		do
		{
			input = malloc(2*sizeof(char));
			if ( input == NULL) { printf("Error with malloc\n"); return false; }
			input = fgets(input,2*sizeof(char),stdin);
			if(input[0] == '\n') break;
			final = realloc( final , (strlen(final)+2)*sizeof(char) );
			strcat( final,input );
			free(input);
		}while(1);
		free(input);

		anotherFinal = malloc(sizeof(char)*(strlen(final)+sizeof(char)));
		if ( anotherFinal == NULL) { puts("Error with malloc\n"); exit(1); }
		strcpy( anotherFinal,final);


		// execute the command the user gave, if there is an error, print it
		str1 = strtok(final,help);

		if ( str1 != NULL)
		{
			if ( !strcmp(str1,"/requestTransaction"))
			{
				str1 = strtok( NULL,help);
				if( str1 == NULL ) puts("\n\n Wrong menu input \n\n");
				else
				{
					str2 = str1;
					str1 = strtok( NULL,help);
					if( str1 == NULL ) puts("\n\n Wrong menu input \n\n");
					else
					{
						str3 = str1;
						str1 = strtok(NULL,help);
						if( str1 == NULL ) puts("\n\n Wrong menu input \n\n");
						else
						{
							str4 = str1;
							str1 = strtok(NULL,help);
							if( str1 == NULL ) // if the user didn't give a date or time
							{
								// add the current date and time
								currentDateTime = time(NULL);
								if ( currentDateTime != -1)
								{
									temp = malloc(sizeof(char)*26);
									if ( temp == NULL) { puts("Error with malloc\n"); exit(1); }
									strcpy(temp,asctime(gmtime(&currentDateTime)));
									date = malloc(sizeof(char)*11);
									if ( date == NULL) { puts("Error with malloc\n"); exit(1); }
									tim = malloc(sizeof(char)*6);
									if (tim == NULL) { puts("Error with malloc\n"); exit(1); }

									date[0] = temp[8]; date[1] = temp[9]; date[2] = '-'; if ( date[0] == ' ') date[0] = '0';
									if ( ( temp[4] == 'J') && ( temp[5] == 'a') ) { date[3] = '0'; date[4] = '1'; date[5] = '-';}
									else if ( temp[4] == 'F') { date[3] = '0'; date[4] = '2'; date[5] = '-';}
									else if ( ( temp[4] == 'M') && ( temp[5] == 'a') && ( temp[6] == 'r') ) { date[3] = '0'; date[4] = '3'; date[5] = '-';}
									else if ( ( temp[4] == 'A') && ( temp[5] == 'p') ) { date[3] = '0'; date[4] = '4'; date[5] = '-';}
									else if ( ( temp[4] == 'M') && ( temp[5] == 'a') && ( temp[6] == 'y') ) { date[3] = '0'; date[4] = '5'; date[5] = '-';}
									else if ( ( temp[4] == 'J') && ( temp[5] == 'u') && ( temp[6] == 'n') ) { date[3] = '0'; date[4] = '6'; date[5] = '-';}
									else if ( ( temp[4] == 'J') && ( temp[5] == 'u') && ( temp[6] == 'l') ) { date[3] = '0'; date[4] = '7'; date[5] = '-';}
									else if ( ( temp[4] == 'A') && ( temp[5] == 'u') ) { date[3] = '0'; date[4] = '8'; date[5] = '-';}
									else if ( temp[4] == 'S') { date[3] = '0'; date[4] = '9'; date[5] = '-';}
									else if ( temp[4] == 'O') { date[3] = '1'; date[4] = '0'; date[5] = '-';}
									else if ( temp[4] == 'N') { date[3] = '1'; date[4] = '1'; date[5] = '-';}
									else if ( temp[4] == 'D') { date[3] = '1'; date[4] = '2'; date[5] = '-';}
									date[6] = temp[20]; date[7] = temp[21]; date[8] = temp[22]; date[9] = temp[23];
									date[10] = '\0';

									tim[0] = temp[11]; tim[1] = temp[12]; tim[2] = temp[13]; tim[3] = temp[14]; tim[4] = temp[15]; tim[5] = '\0';

									free(temp);
								}

								// check if he gave a correct input
								check = check_if_transaction_is_valid( lastTransactionId, lastDateTime, (*lastTransactionId)+1, str2, str3, str4, date, tim, senderHashTable, recieverHashTable, myHashTable, treeHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries);

								if ( check) // if the input is correct, do the transaction
								{
									free( *lastDateTime);
									*lastDateTime = malloc(sizeof(char)*17);
									if ( *lastDateTime == NULL) { printf("Error with malloc\n"); return false; }
									strcpy( *lastDateTime, date);
									strcat( *lastDateTime, " ");
									strcat( *lastDateTime, tim);
									*lastTransactionId += 1;

									check = question1( *lastTransactionId, str2, str3, str4, date, tim, senderHashTable, recieverHashTable, myHashTable, treeHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries);

									if ( check) puts("/requestTransaction command successfull");
								}

								free(date);
								free(tim);
							}
							else
							{
								str5 = str1;
								str1 = strtok(NULL,help);
								if( str1 == NULL) puts("\n\n Wrong menu input \n\n");
								else
								{
									str6 = str1;
									str1 = strtok(NULL,help);
									if ( str1 != NULL) puts("\n\n Wrong menu input \n\n");
									else // if the user gave date or time
									{
										// check if he gave a correct input
										check = check_if_transaction_is_valid( lastTransactionId, lastDateTime, (*lastTransactionId)+1, str2, str3, str4, str5, str6, senderHashTable, recieverHashTable, myHashTable, treeHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries);

										if ( check) // if the input is correct, do the transaction
										{
											free( *lastDateTime);
											*lastDateTime = malloc(sizeof(char)*17);
											if ( *lastDateTime == NULL) { printf("Error with malloc\n"); return false; }
											strcpy( *lastDateTime, str5);
											strcat( *lastDateTime, " ");
											strcat( *lastDateTime, str6);
											*lastTransactionId += 1;

											check = question1( *lastTransactionId, str2, str3, str4, str5, str6, senderHashTable, recieverHashTable, myHashTable, treeHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries);

											if ( check) puts("/requestTransaction command successfull");
										}
									}
								}
							}
						}
					}
				}
			}
			else if ( !strcmp(str1,"/requestTransactions") )
			{
				// first check if the command was called for an inputfile
				int i;
				bool check = true;
				bool firstTime = true;

				i = 1;
				while ( (str1 = strtok(NULL,help)) != NULL)
				{
					if ( i == 1) str2 = str1;
					else if ( i == 2) { firstTime = false; str3 = str1; }
					else if ( i == 3) str4 = str1;
					else if ( i == 4) str5 = str1;
					else if ( i == 5) str6 = str1;
					i++;
					if ( i == 6)
					{
						if ( str6[strlen(str6) -1 ] != ';')
						{
							check = false;
							puts(" Each Transaction should end with ';' right after the time-error");
							break;
						}
						i = 1;
					}
				}

				// if it was called for an input file
				if ( (check == true) && ( i != 1) && ( i != 4) ) // if /requestTransactions was called for an inputfile
				{
					if ( firstTime )
					{
						inputfile = fopen( str2, "r");
						if ( inputfile == NULL)
						{
							puts("Error with inputfile given");
						}
						else
						{
							// read inputfile
							int length;
							while ( 1 )
							{
								i = 1;
								// str2
								str2 = malloc(sizeof(char));
								if ( str2 == NULL)
								{
									puts("Error with malloc\n");
									exit(1);
								}
								str2[0] = '\0';

								do
								{
									c = fgetc( inputfile);

									if ( ( c == ' ') || ( c == EOF) ) break;

									length = strlen(str2);

									str2 = realloc( str2, length + 2*sizeof(char));
									if ( str2 == NULL)
									{
										puts("Error with malloc");
										exit(1);
									}
									str2[length] = c;
									str2[length+1] = '\0';

								}while ( 1 );
								if ( c == EOF) break;

								i++;
								//str3
								str3 = malloc(sizeof(char));
								if ( str3 == NULL)
								{
									puts("Error with malloc\n");
									exit(1);
								}
								str3[0] = '\0';
								do
								{
									c = fgetc( inputfile);

									if ( ( c == ' ') || ( c == EOF) ) break;

									length = strlen(str3);

									str3 = realloc( str3, length + 2*sizeof(char));
									if ( str3 == NULL)
									{
										puts("Error with malloc");
										exit(1);
									}
									str3[length] = c;
									str3[length+1] = '\0';
								}while ( 1 );
								if ( c == EOF) break;

								i++;
								//str4
								str4 = malloc(sizeof(char));
								if ( str4 == NULL)
								{
									puts("Error with malloc\n");
									exit(1);
								}
								str4[0] = '\0';
								do
								{
									c = fgetc( inputfile);

									if ( ( c == ' ') || ( c == EOF) ) break;

									length = strlen(str4);

									str4 = realloc( str4, length + 2*sizeof(char));
									if ( str4 == NULL)
									{
										puts("Error with malloc");
										exit(1);
									}
									str4[length] = c;
									str4[length+1] = '\0';
								}while ( 1 );
								if ( c == EOF) break;

								i++;
								//str5
								str5 = malloc(sizeof(char));
								if ( str5 == NULL)
								{
									puts("Error with malloc\n");
									exit(1);
								}
								str5[0] = '\0';
								do
								{
									c = fgetc( inputfile);

									if ( ( c == ' ') || ( c == EOF) ) break;

									length = strlen(str5);

									str5 = realloc( str5, length + 2*sizeof(char));
									if ( str5 == NULL)
									{
										puts("Error with malloc");
										exit(1);
									}
									str5[length] = c;
									str5[length+1] = '\0';
								}while ( 1 );
								if ( c == EOF) break;

								i++;
								//str6
								str6 = malloc(sizeof(char));
								if ( str6 == NULL)
								{
									puts("Error with malloc\n");
									exit(1);
								}
								str6[0] = '\0';
								do
								{
									c = fgetc( inputfile);

									if ( ( c == '\n') || ( c == EOF) ) break;

									length  = strlen(str6);

									str6 = realloc( str6, length + 2*sizeof(char));
									if ( str6 == NULL)
									{
										puts("Error with malloc");
										exit(1);
									}
									str6[length] = c;
									str6[length+1] = '\0';
								}while ( 1 );

								if ( str6[strlen(str6)-1] == ';') { str6[strlen(str6)-1] = '\0'; check = true; }
								else check = false;
								i++;
								//check now
								if ( check)
								{
									check = check_if_transaction_is_valid( lastTransactionId, lastDateTime, (*lastTransactionId)+1, str2, str3, str4, str5, str6, senderHashTable, recieverHashTable, myHashTable, treeHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries);

									if ( check)
									{
										free( *lastDateTime);
										*lastDateTime = malloc(sizeof(char)*17);
										if ( *lastDateTime == NULL) { printf("Error with malloc\n"); return false; }
										strcpy( *lastDateTime, str5);
										strcat( *lastDateTime, " ");
										strcat( *lastDateTime, str6);
										*lastTransactionId += 1;

										check = question1( *lastTransactionId, str2, str3, str4, str5, str6, senderHashTable, recieverHashTable, myHashTable, treeHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries);

										if ( check) puts("/requestTransaction command successfull");
									}
									else { puts("Error with inputfile"); break; }
								}
								else { puts("Wrong structure of inputfile"); break; }

								free(str2); str2=NULL; free(str3); free(str4); free(str5); free(str6);
								if ( c == EOF) break;
							}

							if ( i == 1) { if ( strcmp(str2,"") ) puts("Last transaction of inputfile had an error and was not executed1"); free(str2);}
							else if ( i == 2) { free(str2); free(str3); puts("Last transaction of inputfile had an error and was not executed2"); }
							else if ( i == 3) { free(str2); free(str3); free(str4); puts("Last transaction of inputfile had an error and was not executed3"); }
							else if ( i == 4) { free(str2); free(str3); free(str4); free(str5); puts("Last transaction of inputfile had an error and was not executed4"); }
							else if ( i == 5) { free(str2); free(str3); free(str4); free(str5); free(str6); puts("Last transaction of inputfile had an error and was not executed5"); }

							fclose(inputfile);
						}

						check = false;
					}
					else
					{
						puts("\n\n Wrong menu input \n\n");
						check = false;
					}
				}

				// if the command was called without date and time
				if ( ( check) && ( i == 4) )
				{
					if ( str4[strlen(str4) -1] != ';')
					{
						check = false;
						puts(" Each Transaction should end with ';' right after the amount of money of the transaction");
					}
					else
					{
						i = 1;
					}
				}
				if ( check) // /requestTransactions NOT for an inputfile
				{
					str1 = strtok(anotherFinal,help);

					leaveRequestTransactions = false;

					while ( 1 ) // start reading transactions from command line, until the user types /endRequestTransactions
					{
						// while loop for 1 transaction
						while ( (str1 = strtok(NULL,help)) != NULL)
						{
							if ( ( i == 1) && ( !strcmp(str1,"/endRequestTransactions")) ) { leaveRequestTransactions = true; break;}

							if ( i == 1) str2 = str1;
							else if ( i == 2) str3 = str1;
							else if ( i == 3) str4 = str1;
							else if ( i == 4) str5 = str1;
							else if ( i == 5) { str6 = str1; str6[strlen(str6)-1] = '\0'; }
							i++;
							if ( i == 6)
							{
								check = check_if_transaction_is_valid( lastTransactionId, lastDateTime, (*lastTransactionId)+1, str2, str3, str4, str5, str6, senderHashTable, recieverHashTable, myHashTable, treeHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries);

								if ( check)
								{
									free( *lastDateTime);
									*lastDateTime = malloc(sizeof(char)*17);
									if ( *lastDateTime == NULL) { printf("Error with malloc\n"); return false; }
									strcpy( *lastDateTime, str5);
									strcat( *lastDateTime, " ");
									strcat( *lastDateTime, str6);
									*lastTransactionId += 1;

									check = question1( *lastTransactionId, str2, str3, str4, str5, str6, senderHashTable, recieverHashTable, myHashTable, treeHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries);

									if ( check) puts("/requestTransaction command successfull");
								}

								i = 1;
							}
						}
						if ( leaveRequestTransactions) break;
						if ( ( i != 1) && ( i != 4) )
						{
							puts("\n\n Wrong input for /requestTransactions command\n\n");
							i = 1;
						}
						else if ( i == 4) // if there was not given a date and time, do the transaction for current date and time
						{
							// add the current date and time
							currentDateTime = time(NULL);
							if ( currentDateTime != -1)
							{
								temp = malloc(sizeof(char)*26);
								if ( temp == NULL) { puts("Error with malloc\n"); exit(1); }
								strcpy(temp,asctime(gmtime(&currentDateTime)));
								date = malloc(sizeof(char)*11);
								if ( date == NULL) { puts("Error with malloc\n"); exit(1); }
								tim = malloc(sizeof(char)*6);
								if (tim == NULL) { puts("Error with malloc\n"); exit(1); }

								date[0] = temp[8]; date[1] = temp[9]; date[2] = '-'; if ( date[0] == ' ') date[0] = '0';
								if ( ( temp[4] == 'J') && ( temp[5] == 'a') ) { date[3] = '0'; date[4] = '1'; date[5] = '-';}
								else if ( temp[4] == 'F') { date[3] = '0'; date[4] = '2'; date[5] = '-';}
								else if ( ( temp[4] == 'M') && ( temp[5] == 'a') && ( temp[6] == 'r') ) { date[3] = '0'; date[4] = '3'; date[5] = '-';}
								else if ( ( temp[4] == 'A') && ( temp[5] == 'p') ) { date[3] = '0'; date[4] = '4'; date[5] = '-';}
								else if ( ( temp[4] == 'M') && ( temp[5] == 'a') && ( temp[6] == 'y') ) { date[3] = '0'; date[4] = '5'; date[5] = '-';}
								else if ( ( temp[4] == 'J') && ( temp[5] == 'u') && ( temp[6] == 'n') ) { date[3] = '0'; date[4] = '6'; date[5] = '-';}
								else if ( ( temp[4] == 'J') && ( temp[5] == 'u') && ( temp[6] == 'l') ) { date[3] = '0'; date[4] = '7'; date[5] = '-';}
								else if ( ( temp[4] == 'A') && ( temp[5] == 'u') ) { date[3] = '0'; date[4] = '8'; date[5] = '-';}
								else if ( temp[4] == 'S') { date[3] = '0'; date[4] = '9'; date[5] = '-';}
								else if ( temp[4] == 'O') { date[3] = '1'; date[4] = '0'; date[5] = '-';}
								else if ( temp[4] == 'N') { date[3] = '1'; date[4] = '1'; date[5] = '-';}
								else if ( temp[4] == 'D') { date[3] = '1'; date[4] = '2'; date[5] = '-';}
								date[6] = temp[20]; date[7] = temp[21]; date[8] = temp[22]; date[9] = temp[23];
								date[10] = '\0';

								tim[0] = temp[11]; tim[1] = temp[12]; tim[2] = temp[13]; tim[3] = temp[14]; tim[4] = temp[15]; tim[5] = '\0';

								free(temp);
							}

							// check if the user gave a correct input
							check = check_if_transaction_is_valid( lastTransactionId, lastDateTime, (*lastTransactionId)+1, str2, str3, str4, date, tim, senderHashTable, recieverHashTable, myHashTable, treeHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries);

							if ( check) // if the input is correct, do the transaction
							{
								free( *lastDateTime);
								*lastDateTime = malloc(sizeof(char)*17);
								if ( *lastDateTime == NULL) { printf("Error with malloc\n"); return false; }
								strcpy( *lastDateTime, date);
								strcat( *lastDateTime, " ");
								strcat( *lastDateTime, tim);
								*lastTransactionId += 1;

								check = question1( *lastTransactionId, str2, str3, str4, date, tim, senderHashTable, recieverHashTable, myHashTable, treeHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries);

								if ( check) puts("/requestTransaction command successfull");
							}

							free(date);
							free(tim);

							i = 1;
						}

						// read again input from command line
						free(final);
						free(anotherFinal);
						final = malloc(1*sizeof(char));
						final[0] = '\0';

						do
						{
							input = malloc(2*sizeof(char));
							if ( input == NULL) { printf("Error with malloc\n"); return false; }
							input = fgets(input,2*sizeof(char),stdin);
							if(input[0] == '\n') break;
							final = realloc( final , (strlen(final)+2)*sizeof(char) );
							strcat( final,input );
							free(input);
						}while(1);
						free(input);

						anotherFinal = malloc(strlen(final) + 3*sizeof(char));
						if ( anotherFinal == NULL) { puts("Error with malloc"); exit(1); }
						anotherFinal[0] = 'c';
						anotherFinal[1] = ' ';
						anotherFinal[2] = '\0';
						strcat(anotherFinal,final);

						str1 = strtok(anotherFinal,help);

					}
				}
			}
			else if ( !strcmp(str1,"/findEarnings"))
			{
				str1 = strtok( NULL,help);
				if ( str1 != NULL)
				{
					str2 = str1;
					str1 = strtok( NULL,help);
					if ( str1 != NULL)
					{
						str3 = str1;
						str1 = strtok( NULL,help);
						if ( str1 != NULL)
						{
							str4 = str1;
							str1 = strtok( NULL,help);
							if ( str1 != NULL)
							{
								str5 = str1;
								str1 = strtok( NULL,help);
								if ( str1 != NULL)
								{
									str6 = str1;
									str1 = strtok( NULL,help);
									if ( str1 == NULL) // if /findEarnigns was called for a specific date and time
									{
										check = true;
										// check if str3 and str5 ( time1 and time2) are valid
										if ( strlen(str3) != 5) check = false;
										if ( strlen(str5) != 5) check = false;
										if ( str3[2] != ':') check = false;
										if ( str5[2] != ':') check = false;
										if ( ( str3[0] < 48) || ( str3[0] > 57) ) check = false;
										if ( ( str5[0] < 48) || ( str5[0] > 57) ) check = false;
										if ( ( str3[1] < 48) || ( str3[1] > 57) ) check = false;
										if ( ( str5[1] < 48) || ( str5[1] > 57) ) check = false;
										if ( ( str3[3] < 48) || ( str3[3] > 57) ) check = false;
										if ( ( str5[3] < 48) || ( str5[3] > 57) ) check = false;
										if ( ( str3[4] < 48) || ( str3[4] > 57) ) check = false;
										if ( ( str5[4] < 48) || ( str5[4] > 57) ) check = false;

										// check if str4 and str6 ( date1 and date2) are valid
										if ( strlen(str4) != 10) check = false;
										if ( strlen(str6) != 10) check = false;
										if ( str4[2] != '-') check = false;
										if ( str6[2] != '-') check = false;
										if ( str4[5] != '-') check = false;
										if ( str6[5] != '-') check = false;
										if ( ( str4[0] < 48) || ( str4[0] > 57) ) check = false;
										if ( ( str6[0] < 48) || ( str6[0] > 57) ) check = false;
										if ( ( str4[1] < 48) || ( str4[1] > 57) ) check = false;
										if ( ( str6[1] < 48) || ( str6[1] > 57) ) check = false;
										if ( ( str4[3] < 48) || ( str4[3] > 57) ) check = false;
										if ( ( str6[3] < 48) || ( str6[3] > 57) ) check = false;
										if ( ( str4[4] < 48) || ( str4[4] > 57) ) check = false;
										if ( ( str6[4] < 48) || ( str6[4] > 57) ) check = false;
										if ( ( str4[6] < 48) || ( str4[6] > 57) ) check = false;
										if ( ( str6[6] < 48) || ( str6[6] > 57) ) check = false;
										if ( ( str4[7] < 48) || ( str4[7] > 57) ) check = false;
										if ( ( str6[7] < 48) || ( str6[7] > 57) ) check = false;
										if ( ( str4[8] < 48) || ( str4[8] > 57) ) check = false;
										if ( ( str6[8] < 48) || ( str6[8] > 57) ) check = false;
										if ( ( str4[9] < 48) || ( str4[9] > 57) ) check = false;
										if ( ( str6[9] < 48) || ( str6[9] > 57) ) check = false;

										if (check)
										{
											check = question4withArguements( str2, recieverHashTable, recieverHashTableNumberOfEntries, str3, str4, str5, str6);

											if ( check) puts("/findEarnings command successfull");
											else puts("Error with walletId given");
										}
										else puts("Error with times and dates given");
									}
									else puts("\n\n Wrong menu input \n\n");
								}
								else puts("\n\n Wrong menu input \n\n");
							}
							else puts("\n\n Wrong menu input \n\n");
						}
						else puts("\n\n Wrong menu input \n\n");
					}
					else // if /findEarnings was not called for a specific time space
					{
						check = question4withoutArguements( str2, recieverHashTable, recieverHashTableNumberOfEntries);

						if ( check) puts("/findEarnings command successfull");
						else puts("Error with walletId given");
					}
				}
				else puts("\n\n Wrong menu input \n\n");
			}
			else if ( !strcmp(str1,"/findPayments"))
			{
				str1 = strtok( NULL,help);
				if ( str1 != NULL)
				{
					str2 = str1;
					str1 = strtok( NULL,help);
					if ( str1 != NULL)
					{
						str3 = str1;
						str1 = strtok( NULL,help);
						if ( str1 != NULL)
						{
							str4 = str1;
							str1 = strtok( NULL,help);
							if ( str1 != NULL)
							{
								str5 = str1;
								str1 = strtok( NULL,help);
								if ( str1 != NULL)
								{
									str6 = str1;
									str1 = strtok( NULL,help);
									if ( str1 == NULL) // if /findPayments was called for a specific date and time
									{
										check = true;
										// check if str3 and str5 ( time1 and time2) are valid
										if ( strlen(str3) != 5) check = false;
										if ( strlen(str5) != 5) check = false;
										if ( str3[2] != ':') check = false;
										if ( str5[2] != ':') check = false;
										if ( ( str3[0] < 48) || ( str3[0] > 57) ) check = false;
										if ( ( str5[0] < 48) || ( str5[0] > 57) ) check = false;
										if ( ( str3[1] < 48) || ( str3[1] > 57) ) check = false;
										if ( ( str5[1] < 48) || ( str5[1] > 57) ) check = false;
										if ( ( str3[3] < 48) || ( str3[3] > 57) ) check = false;
										if ( ( str5[3] < 48) || ( str5[3] > 57) ) check = false;
										if ( ( str3[4] < 48) || ( str3[4] > 57) ) check = false;
										if ( ( str5[4] < 48) || ( str5[4] > 57) ) check = false;

										// check if str4 and str6 ( date1 and date2) are valid
										if ( strlen(str4) != 10) check = false;
										if ( strlen(str6) != 10) check = false;
										if ( str4[2] != '-') check = false;
										if ( str6[2] != '-') check = false;
										if ( str4[5] != '-') check = false;
										if ( str6[5] != '-') check = false;
										if ( ( str4[0] < 48) || ( str4[0] > 57) ) check = false;
										if ( ( str6[0] < 48) || ( str6[0] > 57) ) check = false;
										if ( ( str4[1] < 48) || ( str4[1] > 57) ) check = false;
										if ( ( str6[1] < 48) || ( str6[1] > 57) ) check = false;
										if ( ( str4[3] < 48) || ( str4[3] > 57) ) check = false;
										if ( ( str6[3] < 48) || ( str6[3] > 57) ) check = false;
										if ( ( str4[4] < 48) || ( str4[4] > 57) ) check = false;
										if ( ( str6[4] < 48) || ( str6[4] > 57) ) check = false;
										if ( ( str4[6] < 48) || ( str4[6] > 57) ) check = false;
										if ( ( str6[6] < 48) || ( str6[6] > 57) ) check = false;
										if ( ( str4[7] < 48) || ( str4[7] > 57) ) check = false;
										if ( ( str6[7] < 48) || ( str6[7] > 57) ) check = false;
										if ( ( str4[8] < 48) || ( str4[8] > 57) ) check = false;
										if ( ( str6[8] < 48) || ( str6[8] > 57) ) check = false;
										if ( ( str4[9] < 48) || ( str4[9] > 57) ) check = false;
										if ( ( str6[9] < 48) || ( str6[9] > 57) ) check = false;

										if (check)
										{
											check = question5withArguements( str2, senderHashTable, senderHashTableNumberOfEntries, str3, str4, str5, str6);

											if ( check) puts("/findPayments command successfull");
											else puts("Error with walletId given");
										}
										else puts("Error with times and dates given");
									}
									else puts("\n\n Wrong menu input \n\n");
								}
								else puts("\n\n Wrong menu input \n\n");
							}
							else puts("\n\n Wrong menu input \n\n");
						}
						else puts("\n\n Wrong menu input \n\n");
					}
					else // if /findPayments was NOT called for a specific date and time
					{
						check = question5withoutArguements( str2, senderHashTable, senderHashTableNumberOfEntries);

						if ( check) puts("/findPayments command successfull");
						else puts("Error with walletId given");
					}
				}
				else puts("\n\n Wrong menu input \n\n");
			}
			else if ( !strcmp(str1,"/walletStatus"))
			{
				str1 = strtok( NULL,help);
				if( str1 != NULL)
				{
					str2 = str1;
					str1 = strtok( NULL,help);
					if ( str1 == NULL)
					{
						check = question6( str2, myHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries);

						if ( check) puts("/walletStatus command successfull");
						else puts("Error with walletId given");
					}
					else puts("\n\n Wrong menu input \n\n");
				}
				else puts("\n\n Wrong menu input \n\n");
			}
			else if ( !strcmp(str1,"/bitCoinStatus"))
			{
				str1 = strtok( NULL,help);
				if ( str1 != NULL)
				{
					str2 = str1;
					str1 = strtok( NULL,help);
					if ( str1 == NULL)
					{
						check = question7( str2, treeHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries);

						if ( check) puts("/bitCoinStatus command successfull");
						else puts("Error with bitCoinId given");
					}
					else puts("\n\n Wrong menu input \n\n");
				}
				else puts("\n\n Wrong menu input \n\n");
			}
			else if ( !strcmp(str1,"/traceCoin"))
			{
				str1 = strtok( NULL,help);
				if ( str1 != NULL)
				{
					str2 = str1;
					str1 = strtok( NULL,help);
					if ( str1 == NULL)
					{
						check = question8( str2, treeHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries);

						if ( check) puts("/traceCoin command successfull");
						else puts("Error with bitCoinId given");
					}
					else puts("\n\n Wrong menu input \n\n");
				}
				else puts("\n\n Wrong menu input \n\n");
			}
			else if ( !strcmp(str1,"/exit"))
			{
				str1 = strtok( NULL, help);
				if( str1 == NULL )
				{
					puts("exit program");
					break;
				}
				else puts("\n\n Wrong menu input \n\n");
			}
			else
			{
				puts("\n\n Wrong menu input \n\n");
			}
		}

		free(final);
		free(anotherFinal);
		final = malloc(1*sizeof(char));
		final[0] = '\0';
	}
	free(final);
	free(anotherFinal);

	return true;
}
