#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "types.h"
#include "hash_function.h"
#include "transactionsFile_functions.h"

bool put_transactions_in_sender_and_reciever_hash_table( bucket **senderHashTable, bucket **recieverHashTable, myBucket **myHashTable, treeBucket **treeHashTable, int senderHashTableNumberOfEntries, int recieverHashTableNumberOfEntries, FILE *tF_fp, int bucketSize, char **lastDateTime, int *lastTransactionId)
{
	int i;
	int c;
	char *str;
	int strLength;
	int counter = 0;
	int index;
	bool check;
	char *restSender=NULL;
	char *restReciever=NULL;
	char *sender=NULL;
	char *reciever=NULL;
	bool transactionBanned;
	int value;
	int *destroyedBitCoins=NULL;
	int dBClength = 0;
	int *destroyedBitCoinsValues=NULL;
	int *destroyedBitCoinsOther=NULL;
	listNode *pointerForTree;

	while ( 1 ) // for each word in transactionFile
	{
		if ( counter == 0)
			transactionBanned = false;

		str = malloc(sizeof(char));
		if ( str == NULL) { printf("Error with malloc\n"); return false; }
		str[0] = '\0';

		do
		{
			c = fgetc( tF_fp);
			if ( (c==' ') || (c=='\n') || (c==EOF) || (c=='\t')) break;

			strLength = strlen(str);

			str = realloc(str,strLength+(sizeof(char)*2));
			if ( str == NULL) { printf("Error with ralloc\n"); return false; }

			str[strLength] = c;
			str[strLength+1] = '\0';
		}while ( 1 );

		// str --> word in transactionsFile

		if ( c == '\t') { puts("No tabs allowed in transactionsFile"); exit(1); }
		if ( c == EOF) break;
		if ( c == '\n') counter = 0;
		if ( c == ' ') counter++;

		if ( ( !strcmp(str,"")) || ( !strcmp(str," ")) )
		{
			puts("Continous spaces in transactionsFile1 - error");
			exit(1);
		}

		if ( !transactionBanned) // if we found that the sender has no money in wallet for the transaction, then dont do anything, otherwise do the transaction
		{

			if ( counter == 1)
			{
				free(restSender);
				restSender = malloc( strlen(str) + 2*sizeof(char));
	                        if ( restSender == NULL) { printf("Eror with malloc\n"); return false; }
	                        strcpy( restSender,str);
	                        strcat( restSender," ");

				if ( atoi(str) <= *lastTransactionId )
				{
					puts("Error with transactionId - transactioIds must be given from smaller to bigger");
					exit(1);
				}

				*lastTransactionId = atoi(str);
			}
			else if ( counter == 2) // if str is the walletId of sender
			{
				free(sender);
				sender = malloc( strlen(str) + sizeof(char));
				if ( sender == NULL) { printf("Error with malloc\n"); return false; }
				strcpy(sender,str);
			}
			else if ( ( counter == 3) || ( counter == 4) || ( counter == 5) || (counter == 0) )
			{
				restSender = realloc( restSender, strlen(restSender)+strlen(str)+sizeof(char));
				if ( restSender == NULL) { printf("Error with realloc\n"); return false; }
				strcat( restSender,str);
				if ( counter != 0) // if we still read info for the transaction
				{

					restSender = realloc( restSender, strlen(restSender)+sizeof(char)*2);
					strcat( restSender," ");

					if ( counter == 4) // if we read the money the sender wants to give, check he really have them
					{
						value = atoi(str);

						index = hash_function( sender[0], (senderHashTableNumberOfEntries+recieverHashTableNumberOfEntries)/2 );

						myBucket *current;
						current = myHashTable[index];
						while ( current != NULL)
						{
							if ( !strcmp(sender,current->name) )
							{
								if ( current->balance < value)
								{
									transactionBanned = true;
									break;
								}
							}
							current = current->next;
						}
					}
					if ( counter == 5) // if we read the date, then we have aa new latest Date
					{
						if ( *lastDateTime != NULL) free( *lastDateTime);
						*lastDateTime = malloc( sizeof(char)*17);
						if ( *lastDateTime == NULL) { printf("Error with malloc\n"); return false; }
						strcpy( *lastDateTime, str);
					}

				}
				else // if we read all the info for the transaction
				{
					strcat( *lastDateTime, " ");
					strcat( *lastDateTime, str);

					index = hash_function( sender[0], senderHashTableNumberOfEntries);

					bucket *current;
					current = senderHashTable[index];

					// insert info of transaction in sender hash table
					bool check=false;
					while ( 1 )
					{
						for ( i = 0; i< current->arraySize; i++)
						{
							if ( !strcmp(sender,current->walletIdArray[i].name) )
							{
								listNode *newNode;
								newNode = malloc(sizeof(listNode));
								if ( newNode == NULL) { printf("Error with malloc\n"); return false; }

								newNode->next = NULL;
								newNode->info = malloc(strlen(restSender)+sizeof(char));
								strcpy( newNode->info,restSender);

								pointerForTree = newNode;

								listNode *currentListNode;
								currentListNode = current->walletIdArray[i].lhead;

								if ( current->walletIdArray[i].lhead == NULL)
								{
									current->walletIdArray[i].lhead = newNode;
								}
								else
								{
									while ( currentListNode->next != NULL)
										currentListNode = currentListNode->next;
									currentListNode->next = newNode;
								}
								check = true;
								break;
							}
						}
						if( check == true)
							break;
						current = current->next;
					}

					// remove the money from wallet of sender and put info about the money in 3 deynamic arrays
					index = hash_function( sender[0], (senderHashTableNumberOfEntries+recieverHashTableNumberOfEntries)/2 );

					myBucket *currentMy;
					currentMy = myHashTable[index];
					while ( 1 )
					{
						if ( !strcmp(currentMy->name,sender) )
						{
							currentMy->balance -= value;
							int tempVal = value;

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
				}
			}

			// same work with sender hash table, but for the reciever this time
			if ( counter == 1)
			{
				free(restReciever);
				restReciever = malloc( strlen(str) + 2*sizeof(char));
	                        if ( restReciever == NULL) { printf("Eror with malloc\n"); return false; }
	                        strcpy( restReciever,str);
	                        strcat( restReciever," ");
			}
			else if ( counter == 3) // if str is the walletId of the reciever
			{
				free(reciever);
				reciever = malloc( strlen(str) + sizeof(char));
				if ( reciever == NULL) { printf("Error with malloc\n"); return false; }
				strcpy(reciever,str);

				if ( !strcmp(sender,reciever)) { puts("There can't be a transaction with same sender and reciever"); exit(1); }
			}
			else if ( ( counter == 2) || ( counter == 4) || ( counter == 5) || (counter == 0) )
			{
				restReciever = realloc( restReciever, strlen(restReciever)+strlen(str)+sizeof(char));
				if ( restReciever == NULL) { printf("Error with realloc\n"); return false; }
				strcat( restReciever, str);
				if ( counter != 0) // if we still read info for the transaction
				{
					restReciever = realloc( restReciever, strlen(restReciever)+sizeof(char)*2);
					strcat( restReciever," ");
				}
				else // if we read all the info for the transaction
				{
					index = hash_function( reciever[0], recieverHashTableNumberOfEntries);

					bucket *current;
					current = recieverHashTable[index];

					// insert info of transaction in reciever hash table
					bool check=false;
					while ( 1 )
					{
						for ( i = 0; i< current->arraySize; i++)
						{
							if ( !strcmp(reciever, current->walletIdArray[i].name) )
							{
								listNode *newNode;
								newNode = malloc(sizeof(listNode));
								if ( newNode == NULL) { printf("Error with malloc\n"); return false; }

								newNode->next = NULL;
								newNode->info = malloc(strlen(restReciever)+sizeof(char));
								strcpy( newNode->info,restReciever);

								listNode *currentListNode;
								currentListNode = current->walletIdArray[i].lhead;

								if ( current->walletIdArray[i].lhead == NULL)
								{
									current->walletIdArray[i].lhead = newNode;
								}
								else
								{
									while ( currentListNode->next != NULL)
										currentListNode = currentListNode->next;
									currentListNode->next = newNode;
								}
								check = true;
								break;
							}
						}
						if( check == true)
							break;
						current = current->next;
					}

					// put the money you took from sender's wallet to reciever's wallet
					index = hash_function( reciever[0], (senderHashTableNumberOfEntries+recieverHashTableNumberOfEntries)/2 );

					myBucket *currentMy;
					currentMy = myHashTable[index];
					while ( 1 )
					{
						if ( !strcmp( reciever, currentMy->name) )
						{
							int i;
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

							// for each bitcoin of the transaction that just happened, update its tree
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
				}
			}

		}

		free(str);
	}
	free(restSender);
	free(restReciever);
	free(str);
	free(sender);
	free(reciever);
	free(destroyedBitCoins);
	free(destroyedBitCoinsValues);
	free(destroyedBitCoinsOther);

	return true;
}

bool put_new_nodes_in_tree( bool check, treeNode *currentTreeNode, char *sender, char *reciever, int *moneyForSender, int *moneyForReciever, listNode *pointerForTransaction, bool *dup)
{
	// post order

	if ( check)
		return check;

	if ( currentTreeNode == NULL)
		return check;
	check = put_new_nodes_in_tree( check, currentTreeNode->left, sender, reciever, moneyForSender, moneyForReciever, pointerForTransaction, dup);
	check = put_new_nodes_in_tree( check, currentTreeNode->right, sender, reciever, moneyForSender, moneyForReciever, pointerForTransaction, dup);

	// if the node is a leaf AND is the leaf we want( walletId) AND the update is not done yet, then do the update
	if ( ( currentTreeNode->left == NULL) && ( currentTreeNode->right == NULL) && ( !strcmp(sender,currentTreeNode->wId)) && ( !check) )
	{
		treeNode *newNodeSender=NULL;
		if ( *moneyForSender > 0)
		{
			newNodeSender = malloc( sizeof(treeNode));
			if ( newNodeSender == NULL) { printf("Error with malloc\n"); exit(1); }
			newNodeSender->wId = malloc( strlen(sender) + sizeof(char));
			if ( newNodeSender->wId == NULL) { printf("Error with malloc\n"); exit(1); }
			strcpy(newNodeSender->wId,sender);
			newNodeSender->money = *moneyForSender;
			newNodeSender->tran = NULL;
			newNodeSender->right = NULL;
			newNodeSender->left = NULL;
			newNodeSender->duplicated = false;

			currentTreeNode->right = newNodeSender;
		}

		treeNode *newNodeReciever;
		newNodeReciever = malloc( sizeof(treeNode));
		if ( newNodeReciever == NULL) { printf("Error with malloc\n"); exit(1); }
		newNodeReciever->wId = malloc( strlen(reciever) + sizeof(char));
		if ( newNodeReciever->wId == NULL) { printf("Error with malloc\n"); exit(1); }
		strcpy(newNodeReciever->wId,reciever);
		newNodeReciever->money = *moneyForReciever;
		newNodeReciever->tran = pointerForTransaction;
		newNodeReciever->right = NULL;
		newNodeReciever->left = NULL;
		newNodeReciever->duplicated = *dup;

		currentTreeNode->left = newNodeReciever;

		// check if the update needs more than one insertion
		if ( currentTreeNode->money == *moneyForSender + *moneyForReciever)
		{
			check = true;
		}
		else
		{
			currentTreeNode->left->money = currentTreeNode->money;
			if ( newNodeSender != NULL)
			{
				free(newNodeSender->wId);
				free(newNodeSender);
				currentTreeNode->right = NULL;
			}
			*moneyForReciever -= currentTreeNode->money;
			*dup = true;
		}
	}

	return check;
}
