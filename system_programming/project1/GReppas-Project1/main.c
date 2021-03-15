#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "types.h"
#include "functions_for_error_checking.h"
#include "hash_function.h"
#include "bitCoinBalancesFile_functions.h"
#include "transactionsFile_functions.h"
#include "free_memory.h"
#include "itoa_plus_nod.h"
#include "print_hash_tables.h"
#include "initialize_hash_tables.h"
#include "take_info_from_command_line.h"
#include "menu.h"

/////////////////////////////////////////////////////

int main( int argc, char *argv[])
{
	// get info from command line
	int bitCoinValue;
	int senderHashTableNumberOfEntries;
	int recieverHashTableNumberOfEntries;
	int bucketSize;
	FILE *bCBF_fp;
	FILE *tF_fp;

	if ( !take_info_from_command_line( argc, argv, &bitCoinValue, &senderHashTableNumberOfEntries, &recieverHashTableNumberOfEntries, &bucketSize, &bCBF_fp, &tF_fp) ) return -1;

	// Initialize the 4 hash tables
	bucket **senderHashTable;
	bucket **recieverHashTable;
	myBucket **myHashTable;
	treeBucket **treeHashTable;

	if ( !initialize_hash_tables( &senderHashTable, &recieverHashTable, &myHashTable, &treeHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries) ) return -1;
puts("hi1");
	// create the structure for the bitcoins and put every name in the sender and reciever hash table
	if ( !create_my_structure_and_tree_structure_and_put_names_in_hash_tables( senderHashTable, recieverHashTable, myHashTable, treeHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries, bCBF_fp, bucketSize, bitCoinValue) ) return -1;
puts("hi2");
	//read all transactions from transactionFile and put them in the hash Tables
	char *lastDateTime=NULL;
	int lastTransactionId=0;

	if ( !put_transactions_in_sender_and_reciever_hash_table( senderHashTable, recieverHashTable, myHashTable, treeHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries, tF_fp, bucketSize, &lastDateTime, &lastTransactionId) ) return -1;

	// print menu and let user choose
	if ( !menu( &lastDateTime, &lastTransactionId, senderHashTable, recieverHashTable, myHashTable, treeHashTable, senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries)) return -1;

	// print all hash tables
	//printAll( senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries, senderHashTable, recieverHashTable, myHashTable, treeHashTable);

	// free memory
	free( lastDateTime);
	free_memory( &senderHashTable, &recieverHashTable, &myHashTable, &treeHashTable ,senderHashTableNumberOfEntries, recieverHashTableNumberOfEntries);

	// close files
	fclose(bCBF_fp);
	fclose(tF_fp);

	return 0;
}
