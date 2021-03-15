#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "types.h"
#include "hash_function.h"
#include "functions_for_error_checking.h"

bool check_if_transaction_is_valid( int *lastTransactionId, char **lastDateTime, int transactionId, char *sender, char *reciever, char *value, char *date, char *time, bucket **senderHashTable, bucket **recieverHashTable, myBucket **myHashTable, treeBucket **treeHashTable, int senderHashTableNumberOfEntries, int recieverHashTableNumberOfEntries)
{
	// we check if the transactionId is valid
	if ( transactionId <= *lastTransactionId) { puts("Error with transaction Id"); return false; }

	// we check if the sender and reciever names exist in bitCoinbalancesFile
	bool check = false;
	int i;
	int index;
	index = hash_function(sender[0], (senderHashTableNumberOfEntries+recieverHashTableNumberOfEntries)/2 );
	myBucket *currentMyBucket;
	currentMyBucket = myHashTable[index];
	while ( currentMyBucket != NULL)
	{
		if ( !strcmp( currentMyBucket->name,sender) )
			check = true;
		if ( !check) currentMyBucket = currentMyBucket->next;
		else break;
	}
	if ( !check) { puts("The name of the sender does not exist in the bitCoinBalancesFile"); return false; }

	check = false;
	index = hash_function(reciever[0], (senderHashTableNumberOfEntries+recieverHashTableNumberOfEntries)/2 );
	currentMyBucket = myHashTable[index];
	while ( currentMyBucket != NULL)
	{
		if ( !strcmp( currentMyBucket->name,reciever) )
			check = true;
		if ( !check) currentMyBucket = currentMyBucket->next;
		else break;
	}
	if ( !check) { puts("The name of the reciever does not exist in the bitCoinBalancesFile"); return false; }

	// we check if the value is a correct number
	if ( atoi(value) <= 0) { puts("Error with value of the transaction"); return false; }

	// we check if date and time are correct
	if ( strlen(date) != 10) { puts("Error with date given"); return false; }
	if ( strlen(time) != 5)	{ puts("Error with time given"); return false; }

	if ( ( time[0] >= 48) && ( time[0] <= 57) && ( time[1] >= 48) && ( time[1] <= 57) && ( time[2] == ':') && ( time[3] >= 48) && ( time[3] <= 57) && ( time[4] >= 48) && ( time[4] <= 57) )
	{
		if ( ( date[0] >= 48) && ( date[0] <= 57) && ( date[1] >= 48) && ( date[1] <= 57) && ( date[2] == '-') )
		{
			if ( ( date[3] >= 48) && ( date[3] <= 57) && ( date[4] >= 48) && ( date[4] <= 57) && ( date[5] == '-') )
			{
				if ( ( date[6] >= 48) && ( date[6] <= 57) && ( date[7] >= 48) && ( date[7] <= 57) && ( date[8] >= 48) && ( date[8] <= 57) && ( date[9] >= 48) && ( date[9] <= 57)) ;
				else { puts("Error with date given"); return false; }
			}
			else { puts("Error with date given"); return false; }
		}
		else { puts("Error with date given"); return false; }
	}
	else { puts("Error with time given"); return false; }

	check = false;
	if ( (*lastDateTime)[6] < date[6]) check = true;
	else if ( (*lastDateTime)[6] == date[6])
	{
		if ( (*lastDateTime)[7] < date[7]) check = true;
		else if ( (*lastDateTime)[7] == date[7])
		{
			if ( (*lastDateTime)[8] < date[8]) check = true;
			else if ( (*lastDateTime)[8] == date[8])
			{
				if ( (*lastDateTime)[9] < date[9]) check = true;
				else if ( (*lastDateTime)[9] == date[9])
				{
					if ( (*lastDateTime)[3] < date[3]) check = true;
					else if ( (*lastDateTime)[3] == date[3])
					{
						if ( (*lastDateTime)[4] < date[4]) check = true;
						else if ( (*lastDateTime)[4] == date[4])
						{
							if ( (*lastDateTime)[0] < date[0]) check = true;
							else if ( (*lastDateTime)[0] == date[0])
							{
								if ( (*lastDateTime)[1] < date[1]) check = true;
								else if ( (*lastDateTime)[1] == date[1])
								{
									if ( (*lastDateTime)[11] < time[0]) check = true;
									else if ( (*lastDateTime)[11] == time[0])
									{
										if ( (*lastDateTime)[12] < time[1]) check = true;
										else if ( (*lastDateTime)[12] == time[1])
										{
											if ( (*lastDateTime)[14] < time[3]) check=true;
											else if ( (*lastDateTime)[14] == time[3])
											{
												if ( (*lastDateTime)[15] < time[4]) check  = true;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if ( !check) { puts("Older date or time given"); return false; }

	return true;
}

bool check_if_name_is_valid( char *name, myBucket **myHashTable, int senderHashTableNumberOfEntries, int recieverHashTableNumberOfEntries)
{
	int index = hash_function( name[0], (senderHashTableNumberOfEntries + recieverHashTableNumberOfEntries) /2);

	myBucket *currentMyBucket;
	currentMyBucket = myHashTable[index];

	while ( currentMyBucket != NULL)
	{
		if ( !strcmp(name,currentMyBucket->name)) return false;

		currentMyBucket = currentMyBucket->next;
	}
	return true;
}

bool check_if_bitCoin_is_valid( int bitCoinId, treeBucket **treeHashTable, int senderHashTableNumberOfEntries, int recieverHashTableNumberOfEntries)
{
	int index = hash_function( bitCoinId, (senderHashTableNumberOfEntries+recieverHashTableNumberOfEntries) /2);

	treeBucket *currentTreeBucket;
	currentTreeBucket = treeHashTable[index];

	while ( currentTreeBucket != NULL)
	{
		if ( bitCoinId == currentTreeBucket->bCId) return false;

		currentTreeBucket = currentTreeBucket->next;
	}
	return true;
}
