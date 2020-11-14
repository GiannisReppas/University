#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "BF.h"
#include "HT.h"

int primaryFileDescriptor;

int HashStatistics( char *fileName)
{
	HT_info *hi;
	SHT_info *shi;
	hi = HT_OpenIndex( fileName);
	shi = SHT_OpenSecondaryIndex( fileName);

	if ( hi != NULL)
	{
		/* STAT A: total number of Blocks of the file */
		/* STAT B: lowest, max and average amount of records for the buckets of the file */
		/* STAT C: average amount of blocks for the buckets of the file */
		/* STAT D: number of buckets that have overflow blocks, how much are the overflow blocks for each bucket*/

		// we build an array of integers with all the pointers of the buckets
		int *pArray; pArray = malloc( hi->numBuckets*sizeof(int) ); if ( pArray == NULL) { printf("Error with malloc\n"); return -1; }
		int bucketCounter = 0;
		int totalBlocks = 0;
		int currentBlockPlace;
		void *block; char *mem;
		while ( 1 )
		{
			if ( BF_ReadBlock( hi->fileDesc, totalBlocks, &block) < 0) { printf("Error with BF_ReadBlock()\n"); return -1; }

			currentBlockPlace = 0;
			mem = (char *) block;
			mem += 2*sizeof(char) + strlen( hi->attrName) + sizeof(char) + 2*sizeof(int);
			currentBlockPlace += sizeof(char) + strlen( hi->attrName) + sizeof(char) + 2*sizeof(int);

			while ( 1 )
			{
				pArray[bucketCounter] = * (int *) mem;
				mem += sizeof(int);
				currentBlockPlace += sizeof(int);
				bucketCounter++;

				if ( currentBlockPlace + sizeof(int) >= BLOCK_SIZE) { totalBlocks++; break; }
				if ( bucketCounter == hi->numBuckets) break;
			}

			if ( bucketCounter == hi->numBuckets) break;
		}
		totalBlocks++;

		// for each bucket , do the calculations
		int *recCounter; recCounter = malloc(hi->numBuckets*sizeof(int)); if ( recCounter == NULL) { printf("Error with malloc\n"); return -1; }
		int *blocksCounter; blocksCounter = malloc(hi->numBuckets*sizeof(int)); if ( blocksCounter == NULL) { printf("Error with malloc\n"); return -1; }
		int nextBlock;
		int *temp1; int *temp2;
		int i;
		for ( i = 0; i < hi->numBuckets ; i++)
		{
			nextBlock = pArray[i];
			recCounter[i] = 0;
			blocksCounter[i] = 0;

			if ( nextBlock != -1)
			{
				do
				{
					if ( BF_ReadBlock( hi->fileDesc, nextBlock, &block) < 0) { printf("Error with BF_ReadBlock()\n"); return -1; }
					mem = (char *) block;

					temp1 = (int *) mem; mem += sizeof(int);
					temp2 = (int *) mem;

					recCounter[i] += *temp2;
					blocksCounter[i] += 1;
					totalBlocks++;

					nextBlock = *temp1;
				}while ( nextBlock != -1);
			}
		}

		int minRec,maxRec; float averageRec;
		minRec = recCounter[0]; maxRec = recCounter[0]; averageRec = recCounter[0];
		float averageBlocks;
		averageBlocks = blocksCounter[0];
		for ( i = 1; i < hi->numBuckets ; i++)
		{
			averageRec += recCounter[i];
			if ( recCounter[i] < minRec) minRec = recCounter[i];
			if ( recCounter[i] > maxRec) maxRec = recCounter[i];

			averageBlocks += blocksCounter[i];
		}
		averageRec = averageRec / hi->numBuckets;
		averageBlocks = averageBlocks / hi->numBuckets;

		printf("\tTotal number of Blocks in file %s --> %d\n\n",fileName,totalBlocks);

		printf("\tBucket with less Records has %d Records\n",minRec);
		printf("\tBucket with most Records has %d Records\n",maxRec);
		printf("\tAverage Records for each bucket is %f\n\n",averageRec);

		printf("\tAverage Blocks for each bucket is %f\n\n",averageBlocks);

		int numberOfBucketsWithOverflowBlocks = 0;
		for ( i = 0; i < hi->numBuckets; i++)
		{
			if ( blocksCounter[i] > 1)
			{
				printf("\tBucket %d has %d overflow blocks\n",i,blocksCounter[i]-1);
				numberOfBucketsWithOverflowBlocks++;
			}
			else printf("\tBucket %d has no overflow blocks\n",i);
		}
		printf("\n\t%d out of %ld buckets have overflow blocks\n\n",numberOfBucketsWithOverflowBlocks,hi->numBuckets);

		// free
		free(pArray);
		free(recCounter);
		free(blocksCounter);
	}
	else if ( shi != NULL)
	{
		/* STAT A: total number of Blocks of the file */
		/* STAT B: lowest, max and average amount of records for the buckets of the file */
		/* STAT C: average amount of blocks for the buckets of the file */
		/* STAT D: number of buckets that have overflow blocks, how much are the overflow blocks for each bucket*/

		// we build an array of integers with all the pointers of the buckets
		int *pArray; pArray = malloc( shi->numBuckets*sizeof(int) ); if ( pArray == NULL) { printf("Error with malloc\n"); return -1; }
		int bucketCounter = 0;
		int totalBlocks = 0;
		int currentBlockPlace;
		void *block; char *mem;
		while ( 1 )
		{
			if ( BF_ReadBlock( shi->fileDesc, totalBlocks, &block) < 0) { printf("Error with BF_ReadBlock()\n"); return -1; }

			currentBlockPlace = 0;
			mem = (char *) block;
			mem += ( sizeof(char) + (strlen(shi->fileName) +sizeof(char))*sizeof(char) + (strlen(shi->attrName)+sizeof(char))*sizeof(char) + 2*sizeof(int));
			currentBlockPlace += ( sizeof(char) + (strlen(shi->fileName) +sizeof(char))*sizeof(char) + (strlen(shi->attrName)+sizeof(char))*sizeof(char) + 2*sizeof(int));

			while ( 1 )
			{
				pArray[bucketCounter] = * (int *) mem;
				mem += sizeof(int);
				currentBlockPlace += sizeof(int);
				bucketCounter++;

				if ( currentBlockPlace + sizeof(int) >= BLOCK_SIZE) { totalBlocks++; break; }
				if ( bucketCounter == shi->numBuckets) break;
			}

			if ( bucketCounter == shi->numBuckets) break;
		}
		totalBlocks++;

		// for each bucket , do the calculations
		int *recCounter; recCounter = malloc(shi->numBuckets*sizeof(int)); if ( recCounter == NULL) { printf("Error with malloc\n"); return -1; }
		int *blocksCounter; blocksCounter = malloc(shi->numBuckets*sizeof(int)); if ( blocksCounter == NULL) { printf("Error with malloc\n"); return -1; }
		int nextBlock;
		int *temp1; int *temp2;
		int i;
		for ( i = 0; i < shi->numBuckets ; i++)
		{
			nextBlock = pArray[i];
			recCounter[i] = 0;
			blocksCounter[i] = 0;

			if ( nextBlock != -1)
			{
				do
				{
					if ( BF_ReadBlock( shi->fileDesc, nextBlock, &block) < 0) { printf("Error with BF_ReadBlock()\n"); return -1; }
					mem = (char *) block;

					temp1 = (int *) mem; mem += sizeof(int);
					temp2 = (int *) mem;

					recCounter[i] += *temp2;
					blocksCounter[i] += 1;
					totalBlocks++;

					nextBlock = *temp1;
				}while ( nextBlock != -1);
			}
		}

		int minRec,maxRec; float averageRec;
		minRec = recCounter[0]; maxRec = recCounter[0]; averageRec = recCounter[0];
		float averageBlocks;
		averageBlocks = blocksCounter[0];
		for ( i = 1; i < shi->numBuckets ; i++)
		{
			averageRec += recCounter[i];
			if ( recCounter[i] < minRec) minRec = recCounter[i];
			if ( recCounter[i] > maxRec) maxRec = recCounter[i];

			averageBlocks += blocksCounter[i];
		}
		averageRec = averageRec / shi->numBuckets;
		averageBlocks = averageBlocks / shi->numBuckets;

		printf("\tTotal number of Blocks in file %s --> %d\n\n",fileName,totalBlocks);

		printf("\tBucket with less Records has %d Records\n",minRec);
		printf("\tBucket with most Records has %d Records\n",maxRec);
		printf("\tAverage Records for each bucket is %f\n\n",averageRec);

		printf("\tAverage Blocks for each bucket is %f\n\n",averageBlocks);

		int numberOfBucketsWithOverflowBlocks = 0;
		for ( i = 0; i < shi->numBuckets; i++)
		{
			if ( blocksCounter[i] > 1)
			{
				printf("\tBucket %d has %d overflow blocks\n",i,blocksCounter[i]-1);
				numberOfBucketsWithOverflowBlocks++;
			}
			else printf("\tBucket %d has no overflow blocks\n",i);
		}
		printf("\n\t%d out of %ld buckets have overflow blocks\n\n",numberOfBucketsWithOverflowBlocks,shi->numBuckets);

		// free
		free(pArray);
		free(recCounter);
		free(blocksCounter);
	}

	else
	{
		printf("There is no file with this name\n");
		return -1;
	}

	return 0;
}

int SHT_SecondaryGetAllEntries( SHT_info header_info_sht, HT_info header_info_ht, void *value)
{
	char *sKey;
	sKey = (char*) value;

	// number of bucket in the array of buckets
	int bucket;
	bucket = universalHashingString( header_info_sht.numBuckets, sKey);

	// total number of buckets in the hash table
	int numBuckets = header_info_sht.numBuckets;

	// number of buckets in each block - except the last one
	int bucketsPerBlock = ( BLOCK_SIZE - (strlen(header_info_sht.fileName)*sizeof(char) +sizeof(char)) -(strlen(header_info_sht.attrName)*sizeof(char)+sizeof(char)) - 2*sizeof(int)) / sizeof(int);
	// number of buckets in the last block
	int bucketsInLastBlock = header_info_sht.numBuckets % bucketsPerBlock;

	// number of block for the bucket
	int blockOfBucket = ( bucket / bucketsPerBlock) ;
	if ( ( bucket % bucketsPerBlock == 0) && ( bucket >= bucketsPerBlock) ) blockOfBucket--;
	// place of the bucket in its block
	int numberOfBucketInBlock = (bucket - blockOfBucket*bucketsPerBlock) - 1;

	// we get the block
	void *block;
	if ( BF_ReadBlock( header_info_sht.fileDesc, blockOfBucket, &block) < 0) { printf("Error with BF_ReadBlock()\n"); return -1; }
	char *mem;
	mem = (char *) block;

	// we go to the bucket in the block
	mem += ( sizeof(char) + (strlen(header_info_sht.fileName)*sizeof(char) +sizeof(char)) + (strlen(header_info_sht.attrName)*sizeof(char)+sizeof(char)) + 2*sizeof(int));
	for( int i = 0; i<numberOfBucketInBlock; i++) mem += sizeof(int);
	int *temp;
	temp = (int *) mem;

	int blockCounter = 1; // number of blocks visited
	int recordCounter = 0; // nmber of records printed

	if ( *temp == -1)
	{
		printf("Visited totally %d blocks\n",blockCounter);
		printf("No records with this key\n");
		return -1;
	}

	int newb = *temp;
	if ( BF_ReadBlock( header_info_sht.fileDesc, newb, &block) < 0) { printf("Error with BF_ReadBlock()\n"); return -1; }

	int i;
	int *nextBlock;
	int *curRecordsInBlock;
	int *maxRecordsInBlock;
	SecondaryRecord *rec;
	bool printed = false;

	// start visitting all blocks in the bucket
	while ( 1 )
	{
		blockCounter++;

		mem = (char *) block;

		nextBlock = (int *) mem;
		mem += sizeof(int);

		curRecordsInBlock = (int *) mem;
		mem += sizeof(int);

		maxRecordsInBlock = (int *) mem;
		mem += sizeof(int);

		// for all records in the block
		for (i=0; i< *curRecordsInBlock; i++)
		{
			rec = (SecondaryRecord *) mem;
			if ( ( !strcmp(header_info_sht.attrName,"name") ) && ( !strcmp(rec->record.name,sKey) ) )
			{
				printed = true;
				recordCounter++;
				printf("id --> %d\n",rec->record.id);
				printf("name --> %s\n",rec->record.name);
				printf("surname --> %s\n",rec->record.surname);
				printf("adress --> %s\n",rec->record.address);
				printf("Block_Id --> %d\n",rec->blockId);
			}
			else if ( ( !strcmp(header_info_sht.attrName,"surname") ) && ( !strcmp(rec->record.surname,sKey) ) )
			{
				printed = true;
				recordCounter++;
				printf("id --> %d\n",rec->record.id);
				printf("name --> %s\n",rec->record.name);
				printf("surname --> %s\n",rec->record.surname);
				printf("adress --> %s\n",rec->record.address);
				printf("Block_Id --> %d\n",rec->blockId);
			}
			else if ( ( !strcmp(header_info_sht.attrName,"address") ) && ( !strcmp(rec->record.address,sKey) ) )
			{
				printed = true;
				recordCounter++;
				printf("id --> %d\n",rec->record.id);
				printf("name --> %s\n",rec->record.name);
				printf("surname --> %s\n",rec->record.surname);
				printf("adress --> %s\n",rec->record.address);
				printf("Block_Id --> %d\n",rec->blockId);
			}
			mem += sizeof(SecondaryRecord);
		}

		if ( *nextBlock == -1) // if this is the last block of the bucket
			break;

		// read the next block
		newb = *nextBlock;
		if ( BF_ReadBlock( header_info_sht.fileDesc, newb, &block) < 0) { printf("Error with BF_ReadBlock()\n"); return -1; }
	}

	if ( !printed)
	{
		printf("No records with this key\n");
		printf("Visited totaly %d blocks\n\n",blockCounter);
		return -1;
	}
	else
	{
		printf("Visited totaly %d blocks\n\n",blockCounter);
		return blockCounter;
	}
}

int SHT_SecondaryInsertEntry( SHT_info header_info, SecondaryRecord record)
{
	// number of bucket in the array of buckets
	int bucket;
	if (( !strcmp(header_info.attrName,"name"))) bucket = universalHashingString( header_info.numBuckets, record.record.name);
	else if (( !strcmp(header_info.attrName,"surname")))  bucket = universalHashingString( header_info.numBuckets, record.record.surname);
	else if (( !strcmp(header_info.attrName,"address"))) bucket = universalHashingString( header_info.numBuckets, record.record.address);

	// total number of buckets in the hash table
	int numBuckets = header_info.numBuckets;

	// number of buckets in each block - except the last one
	int bucketsPerBlock = ( BLOCK_SIZE - (strlen(header_info.fileName)+sizeof(char))*sizeof(char) - (strlen(header_info.attrName)+sizeof(char))*sizeof(char) - 2*sizeof(int) ) / sizeof(int);
	// number of buckets in the last block
	int bucketsInLastBlock = header_info.numBuckets % bucketsPerBlock;

	// number of block for the bucket
	int blockOfBucket = ( bucket / bucketsPerBlock) ;
	if ( ( bucket % bucketsPerBlock == 0) && ( bucket >= bucketsPerBlock) ) blockOfBucket--;
	// place of the bucket in its block
	int numberOfBucketInBlock = (bucket - blockOfBucket*bucketsPerBlock) - 1;

	// we get the block of the bucket
	void *block;
	if ( BF_ReadBlock( header_info.fileDesc, blockOfBucket, &block) < 0) { printf("Error2 with BF_ReadBlock()\n"); return -1; }
	char *mem;
	mem = (char *) block;

	// we go to the bucket in the block
	mem += ( sizeof(char) + (strlen(header_info.fileName) +sizeof(char))*sizeof(char) + (strlen(header_info.attrName)+sizeof(char))*sizeof(char) + 2*sizeof(int));
	for( int i = 0; i<numberOfBucketInBlock; i++) mem += sizeof(int);
	int *temp;
	temp = (int *) mem;

	// if the bucket does not point to a block, create that block
	bool check = false;
	if ( *temp == -1)
	{
		check = true;
		if ( BF_AllocateBlock( header_info.fileDesc) < 0) { printf("Error with BF_AllocateBlock()\n"); return -1; }
		*temp = BF_GetBlockCounter( header_info.fileDesc) - 1; if ( *temp < 0) { printf("Error with BF_GetBlockCounter()\n"); return -1; }
	}

	int b = *temp;
	if ( BF_WriteBlock( header_info.fileDesc, blockOfBucket) < 0) { printf("Error with BF_WriteBlock()\n"); return -1; }

	// go to the block that the bucket points to
	if ( BF_ReadBlock( header_info.fileDesc, b, &block) < 0) { printf("Error3 tr with BF_ReadBlock()\n"); return -1; }
	mem = (char *) block;
	if ( check ) // if it was just created, initialize it
	{
		// next block pointer
		temp = (int *) mem;
		*temp = -1;
		mem += sizeof(int);

		// number of records right now in the block
		temp = (int *) mem;
		*temp = 0;
		mem += sizeof(int);

		// max number of records in the block
		temp = (int *) mem;
		*temp = (BLOCK_SIZE - 3*sizeof(int)) / sizeof(SecondaryRecord);
		mem += sizeof(int);

		mem -= 3*sizeof(int);
	}

	// start searching for a block with free space and when u find it, put the record there and return
	int newb;
	int oldb = b;
	while ( 1 )
	{
		// pointer to next block
		temp = (int *) mem;

		// Records currently in this block
		mem += sizeof(int);
		int *curRecordsInBlock;
		curRecordsInBlock = (int *) mem;

		// max Records that this block can take
		mem += sizeof(int);
		int *maxRecordsInBlock;
		maxRecordsInBlock = (int *) mem;

		// if this block has space for another record
		if (( *curRecordsInBlock != *maxRecordsInBlock) && ( *temp == -1))
		{

			// write the new record after the last record
			mem += sizeof(int);
			mem += sizeof(SecondaryRecord)*(*curRecordsInBlock);
			memcpy( mem, &record, sizeof(SecondaryRecord));
			mem -= sizeof(SecondaryRecord)*(*curRecordsInBlock);
			mem -= sizeof(int);

			mem -= sizeof(int);
			mem -= sizeof(int);

			(*curRecordsInBlock)++;

			// if the block became full of records after this insertion, create a new one
			if ( *curRecordsInBlock == *maxRecordsInBlock)
			{

				if ( BF_AllocateBlock( header_info.fileDesc) < 0) { printf("Error with BF_AllocateBlock()\n"); return -1; }

				// change the pointer of the old block
				*temp = BF_GetBlockCounter( header_info.fileDesc) - 1;
				if ( *temp < 0) { printf("Error with BF_GetBlockCounter()\n"); return -1; }
				newb = *temp;
				if ( BF_WriteBlock( header_info.fileDesc, oldb) < 0) { printf("Error with BF_WriteBlock()\n"); return -1; }

				// get the new block, initialize it
				if ( BF_ReadBlock( header_info.fileDesc, newb, &block) < 0) { printf("Error4 with BF_ReadBlock()\n"); return -1; }
				mem = (char *) block;

				temp = (int *) mem;
				*temp = -1;
				mem += sizeof(int);

				temp = (int *) mem;
				*temp = 0;
				mem += sizeof(int);

				temp = (int *) mem;
				*temp = (BLOCK_SIZE - 3*sizeof(int)) / sizeof(SecondaryRecord);
				if ( BF_WriteBlock( header_info.fileDesc, newb) < 0) { printf("Error with BF_WriteBlock()\n"); return -1; }
			}
			else
			{
				if ( BF_WriteBlock( header_info.fileDesc, oldb) < 0) { printf("Error with BF_WriteBlock()\n"); return -1; }
			}

			return 0;
		}

		newb = *temp; // next block
		if ( BF_WriteBlock( header_info.fileDesc, oldb) < 0) { printf("Error with BF_WriteBlock()\n"); return -1; } // write back the old one

		if ( BF_ReadBlock( header_info.fileDesc, newb, &block) < 0) { printf("Error5 with BF_ReadBlock()\n"); return -1; } // read the next block

		oldb = newb;
		mem = (char *) block;
	}

}

int SHT_CloseSecondaryIndex( SHT_info* header_info )
{
	// close the file
	if ( BF_CloseFile( header_info->fileDesc) < 0) { BF_PrintError("Error with BF_OpenFile()\n"); return -1; }

	// free header_info
	free(header_info->attrName);
	free(header_info->fileName);
	free( header_info);

	return 0;
}

SHT_info* SHT_OpenSecondaryIndex(char *sfileName)
{
	int i,j;
	// open the file
	int fd = BF_OpenFile( sfileName);
	if ( fd < 0) { BF_PrintError("Error xxxG7 with BF_OpenFile()\n"); return NULL; }

	// create header info with data from the first block of the hash table
	SHT_info *header_info;
	header_info = malloc(sizeof(SHT_info));
	if ( header_info == NULL) { printf("Error with malloc\n"); exit(1); }

	void *block;
	if ( BF_ReadBlock( fd, 0, &block) < 0) { printf("Error with BF_ReadBlock()\n"); return NULL; }

	int *temp;
	char *mem;
	mem = (char *) block;

	// fileDesc
	header_info->fileDesc = fd;
	// index Type
	if ( *mem != 's') return NULL;
	mem += sizeof(char);
	// fileName
	i=0;
	while (*mem != '\0') { i++; mem += sizeof(char); }
	i++;
	j = i;
	header_info->fileName = malloc( sizeof(char)*i );
	if ( header_info->fileName == NULL) { printf("Error with malloc"); exit(1); }
	i--;
	while ( i != 0) { i--; mem -= sizeof(char); }
	memcpy( header_info->fileName, mem, j*sizeof(char));
	// attrName
	while (*mem != '\0') { mem += sizeof(char); }
	mem += sizeof(char);
	i=0;
	while (*mem != '\0') { i++; mem += sizeof(char); }
	i++;
	j = i;
	header_info->attrName = malloc( sizeof(char)*i );
	if ( header_info->attrName == NULL) { printf("Error with malloc"); exit(1); }
	i--;
	while ( i != 0) { i--; mem -= sizeof(char); }
	memcpy( header_info->attrName, mem, j*sizeof(char));
	// attrLength
	while( *mem != '\0') { mem += sizeof(char); }
	mem += sizeof(char);
	temp = (int *) mem;
	header_info->attrLength = *temp;
	// numBuckets
	mem += sizeof(int);
	temp = (int *) mem;
	header_info->numBuckets = *temp;

	return header_info;
}

int SHT_CreateSecondaryIndex( char *sfileName, char* attrName, int attrLength, int buckets, char* fileName)
{
	/* Create the file */
	if ( BF_CreateFile(sfileName) < 0) { BF_PrintError("Error2 with BF_CreateFile()\n"); return -1; }

	/* Open the file */
	int fd = BF_OpenFile(sfileName);
	if ( fd < 0) { BF_PrintError("Error2 with BF_OpenFile()\n"); return -1;}

	/* Create the hash table (buckets only)*/
	int placedBucketsCounter = 0;

	void *block;
	char *mem;
	int *temp;

	int curBlockPlace;

	do
	{
		// allocate a new block for the hash table
		if ( BF_AllocateBlock( fd) < 0) { BF_PrintError("Error with BF_AllocateBlock()\n"); return -1;}

		// read this block
		if ( BF_ReadBlock( fd, BF_GetBlockCounter( fd)-1, &block) < 0) { BF_PrintError("Error with BF_ReadBlock()\n"); return -1; }
		curBlockPlace = 0; // we are currently at the beggining of the block( the END is BLOCK_SIZE)
		mem = (char *) block; // we use a char pointer for moving in block to make calculations easier

		// first, write the index Type
		*mem = 's';
		mem += sizeof(char);
		curBlockPlace += sizeof(char);

		// then, write the fileName
		memcpy( mem, fileName, strlen(fileName)*sizeof(char) + sizeof(char) );
		mem += (strlen(fileName)*sizeof(char) + sizeof(char));
		curBlockPlace += (strlen(fileName)*sizeof(char) + sizeof(char));

		// then, write the attrName
		memcpy( mem, attrName, strlen(attrName)*sizeof(char) + sizeof(char) );
		mem += (strlen(attrName)*sizeof(char) + sizeof(char));
		curBlockPlace += (strlen(attrName)*sizeof(char) + sizeof(char));

		// then, write the attrLength
		temp = (int *) mem;
		*temp = attrLength;
		mem += sizeof(int);
		curBlockPlace += sizeof(int);

		// then, write the total number of buckets in all the table
		temp = (int *) mem;
		*temp = buckets;
		mem += sizeof(int);
		curBlockPlace += sizeof(int);

		// now, write all buckets in the remaining space of the block
		// currently there are no data blocks, so each bucket has value -1
		// leave this loop only if there is no more space in block OR we finished initializing thebuckets
		do
		{
			temp = (int *) mem;
			*temp = -1;
			mem += sizeof(int);
			curBlockPlace += sizeof(int);

			placedBucketsCounter++;
		} while ( ( placedBucketsCounter != buckets) && ( curBlockPlace + sizeof(int) <= BLOCK_SIZE) );

		// write the block back to the file
		if ( BF_WriteBlock( fd, BF_GetBlockCounter( fd) -1) < 0) { BF_PrintError("Error with BF_WriteBlock()\n"); return -1; }

	// if there are more blocks to put, create another block, otherwise function is over
	}while ( placedBucketsCounter != buckets);

	// synchronize primary and secondary
	//////////////

	// get number of buckets of primary
	int bucketCounter = 0;
	if ( BF_ReadBlock( primaryFileDescriptor, 0, &block) < 0) { BF_PrintError("Error1 with BF_ReadBlock()\n"); return -1; }
	mem = (char *) block;
	int i = 0;
	while ( *mem != '\0') { mem += sizeof(char); i++; }
	mem += sizeof(char);
	i++;
	mem += sizeof(int);
	int numB = * (int *) mem;
	int bucketsPerBlock = ( BLOCK_SIZE - sizeof(char) -i - 2*sizeof(int)) / sizeof(int);
	int blocksOfHash = (numB / bucketsPerBlock) + 1;
	if ( numB % bucketsPerBlock == 0) blocksOfHash--;

	////////
	SHT_info *shi;
	shi = malloc(sizeof(SHT_info));
	shi->fileDesc = fd;
	shi->attrName = malloc( sizeof(char)*(strlen(attrName)+sizeof(char)) ); strcpy(shi->attrName,attrName);
	shi->attrLength = attrLength;
	shi->numBuckets = buckets;
	shi->fileName = malloc( sizeof(char)*(strlen(fileName)+sizeof(char)) ); strcpy(shi->fileName,fileName);

	SecondaryRecord *curA;
	int curR;
	int maxR;
	int j;
	Record rec;
	SecondaryRecord srec;
	for ( i = blocksOfHash ; i<BF_GetBlockCounter( primaryFileDescriptor) ; i++)
	{
		if ( BF_ReadBlock( primaryFileDescriptor, i, &block) < 0) { printf("Error with BF_ReadBlock()\n"); return -1; }
		mem = (char *) block;
		mem += sizeof(int);

		curR = * (int *) mem;
		mem += sizeof(int);
		maxR = * (int *) mem;
		mem += sizeof(int);

		curA = malloc(sizeof(SecondaryRecord)*curR); if ( curA == NULL) { printf("Error with malloc\n"); return -1; }

		for ( j = 0; j < curR ; j++)
		{
			rec = * (Record *) mem;

			srec.record = rec;
			srec.blockId = i;

			curA[j] = srec;

			mem += sizeof(Record);
		}

		if ( BF_WriteBlock( primaryFileDescriptor, i) < 0) { printf("Error with BF_WriteBlock()\n"); return -1;}

		for ( j = 0; j < curR ; j++)
		{
			SHT_SecondaryInsertEntry( *shi, curA[j]);
		}

		free( curA);
	}

	free(shi);
	free(shi->attrName);
	free(shi->fileName);

	// close secondary
	if ( BF_CloseFile( fd) < 0) { BF_PrintError("Error with BF_CloseFile()\n"); return -1; }

	return 0;
}

int universalHashingInt( int M, int x)
{
	// parameters a,b
	int a = 7;
	int b = 8;

	// 1000th prime number
	int p = 7919;

	// universal hashing for integers
	return (((a*x + b) % p ) % M)+1;
}

int universalHashingString( int M, char *str)
{
	int h =0;

	// parameter a
	int a =6;

	// 1000th prime number
	int p = 7919;

	// universal hashing for strings
	int i =0;
	while ( str[i] != '\0')
	{
		h = (h*a + str[i]) % p;
		i++;
	}

	return (h % M)+1;
}

int HT_DeleteEntry( HT_info header_info, void *value)
{
	int *iKey; char *sKey;
	if ( header_info.attrType == 'i' ) iKey = (int *) value;
	else if ( header_info.attrType == 'c' ) sKey = (char*) value;

	// number of bucket in the array of buckets
	int bucket;
	if ( header_info.attrType == 'i' ) bucket = universalHashingInt( header_info.numBuckets, *iKey);
	if ( header_info.attrType == 'c' ) bucket = universalHashingString( header_info.numBuckets, sKey);

	// total number of buckets in the hash table
	int numBuckets = header_info.numBuckets;

	// number of buckets in each block - except the last one
	int bucketsPerBlock = ( BLOCK_SIZE - sizeof(char) -(strlen(header_info.attrName)+sizeof(char)) - 2*sizeof(int)) / sizeof(int);
	// number of buckets in the last block
	int bucketsInLastBlock = header_info.numBuckets % bucketsPerBlock;

	// number of block for the bucket
	int blockOfBucket = ( bucket / bucketsPerBlock) ;
	if ( ( bucket % bucketsPerBlock == 0) && ( bucket >= bucketsPerBlock) ) blockOfBucket--;
	// place of the bucket in its block
	int numberOfBucketInBlock = (bucket - blockOfBucket*bucketsPerBlock) - 1;

	// we get the block
	void *block;
	if ( BF_ReadBlock( header_info.fileDesc, blockOfBucket, &block) < 0) { printf("Error with BF_ReadBlock()\n"); return -1; }
	char *mem;
	mem = (char *) block;

	// we got to the bucket in the block
	mem += 2*sizeof(char) + strlen(header_info.attrName) + sizeof(char) + 2*sizeof(int);
	for( int i = 0; i<numberOfBucketInBlock; i++) mem += sizeof(int);
	int *temp;
	temp = (int *) mem;

	if ( *temp == -1)
	{
		printf("No record with this key exists\n");
		return -1;
	}

	int newb = *temp;
	if ( BF_WriteBlock( header_info.fileDesc, blockOfBucket) < 0) { printf("Error with BF_WriteBlock()\n"); return -1; }
	if ( BF_ReadBlock( header_info.fileDesc, newb, &block) < 0) { printf("Error with BF_ReadBlock()\n"); return -1; }

	int i;
	int *nextBlock;
	int *curRecordsInBlock;
	int *maxRecordsInBlock;
	Record *rec;
	Record *next_rec;
	bool check;
	// start searching for the block in the bucket
	while ( 1 )
	{
		mem = (char *) block;

		nextBlock = (int *) mem;
		mem += sizeof(int);

		curRecordsInBlock = (int *) mem;
		mem += sizeof(int);

		maxRecordsInBlock = (int *) mem;
		mem += sizeof(int);

		check = false;
		// for each record in the block
		for (i=0; i< *curRecordsInBlock; i++)
		{
			rec = (Record *) mem;

			if ( !check)
			{
				if ( ( header_info.attrType == 'i') && ( !strcmp(header_info.attrName,"id") ) )
				{
					if ( *iKey == rec->id) check = true;
				}
				else if ( ( header_info.attrType == 'c') && ( !strcmp(header_info.attrName,"name") ) )
				{
					if ( !strcmp(sKey,rec->name) ) check = true;
				}
				else if ( ( header_info.attrType == 'c') && ( !strcmp(header_info.attrName,"surname") ) )
				{
					if ( !strcmp(sKey,rec->surname) ) check = true;
				}
				else if ( ( header_info.attrType == 'c') && ( !strcmp(header_info.attrName,"adsress") ) )
				{
					if ( !strcmp(sKey,rec->address) ) check = true;
				}
			}
			if ( check)
			{
				if ( i < (*curRecordsInBlock-1) )
				{
					next_rec = (Record *) (mem + sizeof(Record));
					*rec = *next_rec;
				}
			}

			mem += sizeof(Record);
		}

		if ( check) *curRecordsInBlock -= 1;

		if ( BF_WriteBlock( header_info.fileDesc, newb) < 0) { printf("Error with BF_WriteBlock()\n"); return -1; }
		if ( ( *nextBlock == -1) || ( check) ) // if there are no more blocks OR we deleted the entry
			break;

		// next block
		newb = *nextBlock;
		if ( BF_ReadBlock( header_info.fileDesc, newb, &block) < 0) { printf("Error with BF_ReadBlock()\n"); return -1; }
	}

	return 0;
}

int HT_GetAllEntries( HT_info header_info, void *value)
{
	int *iKey; char *sKey;
	if ( header_info.attrType == 'i' ) iKey = (int *) value;
	else if ( header_info.attrType == 'c' ) sKey = (char*) value;

	// number of bucket in the array of buckets
	int bucket;
	if ( header_info.attrType == 'i' ) bucket = universalHashingInt( header_info.numBuckets, *iKey);
	if ( header_info.attrType == 'c' ) bucket = universalHashingString( header_info.numBuckets, sKey);

	// total number of buckets in the hash table
	int numBuckets = header_info.numBuckets;

	// number of buckets in each block - except the last one
	int bucketsPerBlock = ( BLOCK_SIZE - sizeof(char) -(strlen(header_info.attrName)+sizeof(char)) - 2*sizeof(int)) / sizeof(int);
	// number of buckets in the last block
	int bucketsInLastBlock = header_info.numBuckets % bucketsPerBlock;

	// number of block for the bucket
	int blockOfBucket = ( bucket / bucketsPerBlock) ;
	if ( ( bucket % bucketsPerBlock == 0) && ( bucket >= bucketsPerBlock) ) blockOfBucket--;
	// place of the bucket in its block
	int numberOfBucketInBlock = (bucket - blockOfBucket*bucketsPerBlock) - 1;

	// we get the block
	void *block;
	if ( BF_ReadBlock( header_info.fileDesc, blockOfBucket, &block) < 0) { printf("Error with BF_ReadBlock()\n"); return -1; }
	char *mem;
	mem = (char *) block;

	// we go to the bucket in the block
	mem += 2*sizeof(char) + strlen(header_info.attrName) + sizeof(char) + 2*sizeof(int);
	for( int i = 0; i<numberOfBucketInBlock; i++) mem += sizeof(int);
	int *temp;
	temp = (int *) mem;

	int blockCounter = 1; // number of blocks visited
	int recordCounter = 0; // nmber of records printed

	if ( *temp == -1)
	{
		printf("Visited totally %d blocks\n",blockCounter);
		printf("No records with this key\n");
		return -1;
	}

	int newb = *temp;
	if ( BF_WriteBlock( header_info.fileDesc, blockOfBucket) < 0) { printf("Error with BF_WriteBlock()\n"); return -1; }
	if ( BF_ReadBlock( header_info.fileDesc, newb, &block) < 0) { printf("Error with BF_ReadBlock()\n"); return -1; }

	int i;
	int *nextBlock;
	int *curRecordsInBlock;
	int *maxRecordsInBlock;
	Record *rec;
	bool printed = false;

	// start visitting all blocks in the bucket
	while ( 1 )
	{
		blockCounter++;

		mem = (char *) block;

		nextBlock = (int *) mem;
		mem += sizeof(int);

		curRecordsInBlock = (int *) mem;
		mem += sizeof(int);

		maxRecordsInBlock = (int *) mem;
		mem += sizeof(int);

		// for all records in the block
		for (i=0; i< *curRecordsInBlock; i++)
		{
			rec = (Record *) mem;
			if ( header_info.attrType == 'i')
			{
				if ( rec->id == *iKey)
				{
					printed = true;
					recordCounter++;
					printf("id --> %d\n",rec->id);
					printf("name --> %s\n",rec->name);
					printf("surname --> %s\n",rec->surname);
					printf("adress --> %s\n",rec->address);
				}
			}
			else if ( header_info.attrType == 'c')
			{
				if ( ( !strcmp(header_info.attrName,"name") ) && ( !strcmp(rec->name,sKey) ) )
				{
					printed = true;
					recordCounter++;
					printf("id --> %d\n",rec->id);
					printf("name --> %s\n",rec->name);
					printf("surname --> %s\n",rec->surname);
					printf("adress --> %s\n",rec->address);
				}
				else if ( ( !strcmp(header_info.attrName,"surname") ) && ( !strcmp(rec->surname,sKey) ) )
				{
					printed = true;
					recordCounter++;
					printf("id --> %d\n",rec->id);
					printf("name --> %s\n",rec->name);
					printf("surname --> %s\n",rec->surname);
					printf("adress --> %s\n",rec->address);
				}
				else if ( ( !strcmp(header_info.attrName,"adress") ) && ( !strcmp(rec->address,sKey) ) )
				{
					printed = true;
					recordCounter++;
					printf("id --> %d\n",rec->id);
					printf("name --> %s\n",rec->name);
					printf("surname --> %s\n",rec->surname);
					printf("adress --> %s\n",rec->address);
				}
			}

			mem += sizeof(Record);
		}

		if ( BF_WriteBlock( header_info.fileDesc, newb) < 0) { printf("Error with BF_WriteBlock()\n"); return -1; }
		if ( *nextBlock == -1) // if this is the last block of the bucket
			break;

		// read the next block
		newb = *nextBlock;
		if ( BF_ReadBlock( header_info.fileDesc, newb, &block) < 0) { printf("Error with BF_ReadBlock()\n"); return -1; }
	}

	if ( !printed)
	{
		printf("No records with this key\n");
		printf("Visited totaly %d blocks\n\n",blockCounter);
		return -1;
	}
	else
	{
		printf("Visited totaly %d blocks\n\n",blockCounter);
		return recordCounter;
	}
}

int HT_InsertEntry( HT_info header_info, Record record)
{
	// number of bucket in the array of buckets
	int bucket;
	if ( header_info.attrType == 'i') bucket = universalHashingInt( header_info.numBuckets, record.id);
	else if ( ( header_info.attrType == 'c') && ( !strcmp(header_info.attrName,"name") ) ) bucket = universalHashingString( header_info.numBuckets, record.name);
	else if ( ( header_info.attrType == 'c') && ( !strcmp(header_info.attrName,"surname") ) ) bucket = universalHashingString( header_info.numBuckets, record.surname);
	else if ( ( header_info.attrType == 'c') && ( !strcmp(header_info.attrName,"address") ) ) bucket = universalHashingString( header_info.numBuckets, record.address);

	// total number of buckets in the hash table
	int numBuckets = header_info.numBuckets;

	// number of buckets in each block - except the last one
	int bucketsPerBlock = ( BLOCK_SIZE - sizeof(char) -(strlen(header_info.attrName)+sizeof(char)) - 2*sizeof(int)) / sizeof(int);
	// number of buckets in the last block
	int bucketsInLastBlock = header_info.numBuckets % bucketsPerBlock;

	// number of block for the bucket
	int blockOfBucket = ( bucket / bucketsPerBlock) ;
	if ( ( bucket % bucketsPerBlock == 0) && ( bucket >= bucketsPerBlock) ) blockOfBucket--;
	// place of the bucket in its block
	int numberOfBucketInBlock = (bucket - blockOfBucket*bucketsPerBlock) - 1;

	// we get the block of the bucket
	void *block;
	if ( BF_ReadBlock( header_info.fileDesc, blockOfBucket, &block) < 0) { printf("Error yeuths1 with BF_ReadBlock()\n"); return -1; }
	char *mem;
	mem = (char *) block;

	// we go to the bucket in the block
	mem += 2*sizeof(char) + strlen(header_info.attrName) + sizeof(char) + 2*sizeof(int);
	for( int i = 0; i<numberOfBucketInBlock; i++) mem += sizeof(int);
	int *temp;
	temp = (int *) mem;

	// if the bucket does not point to a block, create that block
	bool check = false;
	if ( *temp == -1)
	{
		check = true;
		if ( BF_AllocateBlock( header_info.fileDesc) < 0) { printf("Error with BF_AllocateBlock()\n"); return -1; }
		*temp = BF_GetBlockCounter( header_info.fileDesc) - 1; if ( *temp < 0) { printf("Error with BF_GetBlockCounter()\n"); return -1; }
	}

	int b = *temp;
	if ( BF_WriteBlock( header_info.fileDesc, blockOfBucket) < 0) { printf("Error with BF_WriteBlock()\n"); return -1; }

	// go to the block that the bucket points to
	if ( BF_ReadBlock( header_info.fileDesc, b, &block) < 0) { printf("Error yeyths2 with BF_ReadBlock()\n"); return -1; }
	mem = (char *) block;
	if ( check ) // if it was just created, initialize it
	{
		// next block pointer
		temp = (int *) mem;
		*temp = -1;
		mem += sizeof(int);

		// number of records right now in the block
		temp = (int *) mem;
		*temp = 0;
		mem += sizeof(int);

		// max number of records in the block
		temp = (int *) mem;
		*temp = (BLOCK_SIZE - 3*sizeof(int)) / sizeof(Record);
		mem += sizeof(int);

		mem -= 3*sizeof(int);
	}

	// start searching for a block with free space and when u find it, put the record there and return
	int newb;
	int oldb = b;
	while ( 1 )
	{
		// pointer to next block
		temp = (int *) mem;

		// Records currently in this block
		mem += sizeof(int);
		int *curRecordsInBlock;
		curRecordsInBlock = (int *) mem;

		// max Records that this block can take
		mem += sizeof(int);
		int *maxRecordsInBlock;
		maxRecordsInBlock = (int *) mem;

		// if this block has space for another record
		if (( *curRecordsInBlock != *maxRecordsInBlock) && ( *temp == -1))
		{
			// write the new record after the last record
			mem += sizeof(int);
			mem += sizeof(Record)*(*curRecordsInBlock);
			memcpy( mem, &record, sizeof(Record));
			mem -= sizeof(Record)*(*curRecordsInBlock);
			mem -= sizeof(int);

			mem -= sizeof(int);
			mem -= sizeof(int);

			(*curRecordsInBlock)++;
			// if the block became full of records after this insertion, create a new one
			if ( *curRecordsInBlock == *maxRecordsInBlock)
			{
				if ( BF_AllocateBlock( header_info.fileDesc) < 0) { printf("Error with BF_AllocateBlock()\n"); return -1; }
				// change the pointer of the old block
				*temp = BF_GetBlockCounter( header_info.fileDesc) - 1;
				if ( *temp < 0) { printf("Error with BF_GetBlockCounter()\n"); return -1; }
				newb = *temp;
				if ( BF_WriteBlock( header_info.fileDesc, oldb) < 0) { printf("Error with BF_WriteBlock()\n"); return -1; }

				// get the new block, initialize it
				if ( BF_ReadBlock( header_info.fileDesc, newb, &block) < 0) { printf("Error yeyths3 with BF_ReadBlock()\n"); return -1; }
				mem = (char *) block;

				temp = (int *) mem;
				*temp = -1;
				mem += sizeof(int);

				temp = (int *) mem;
				*temp = 0;
				mem += sizeof(int);

				temp = (int *) mem;
				*temp = (BLOCK_SIZE - 3*sizeof(int)) / sizeof(Record);

				if ( BF_WriteBlock( header_info.fileDesc, newb) < 0) { printf("Error with BF_WriteBlock()\n"); return -1; }
			}
			else
			{
				if ( BF_WriteBlock( header_info.fileDesc, oldb) < 0) { printf("Error with BF_WriteBlock()\n"); return -1; }
			}

			return oldb;
		}
		newb = *temp; // next block
		if ( BF_ReadBlock( header_info.fileDesc, newb, &block) < 0) { printf("Error yeyths4 with BF_ReadBlock()\n"); return -1; } // read the next block
		if ( BF_WriteBlock( header_info.fileDesc, oldb) < 0) { printf("Error with BF_WriteBlock()\n"); return -1; } // write back the old one
		oldb = newb;
		mem = (char *) block;
	}
}

int HT_CloseIndex( HT_info* header_info)
{
	// close the file
	if ( BF_CloseFile( header_info->fileDesc) < 0) { BF_PrintError("Error with BF_OpenFile()\n"); return -1; }

	// free header_info
	free(header_info->attrName);
	free( header_info);

	return 0;
}

HT_info *HT_OpenIndex( char *fileName)
{
	// open the file
	int fd = BF_OpenFile( fileName);
	if ( fd < 0) { BF_PrintError("Error with BF_OpenFile()\n"); return NULL; }

	// create header info with data from the first block of the hash table
	HT_info *header_info;
	header_info = malloc(sizeof(HT_info));
	if ( header_info == NULL) { printf("Error with malloc\n"); exit(1); }

	void *block;
	if ( BF_ReadBlock( fd, 0, &block) < 0) { printf("Error with BF_ReadBlock()\n"); return NULL; }

	int *temp;
	char *mem;
	mem = (char *) block;

	// fileDesc
	header_info->fileDesc = fd;
	// index Type check
	if ( *mem != 'p') return NULL;
	// attrType
	mem += sizeof(char);
	header_info->attrType = *mem;
	// attrName
	mem += sizeof(char);
	int i=0;
	while (*mem != '\0') { i++; mem += sizeof(char); }
	i++;
	int j = i;
	header_info->attrName = malloc( sizeof(char)*i );
	if ( header_info->attrName == NULL) { printf("Error with malloc"); exit(1); }
	i--;
	while ( i != 0) { i--; mem -= sizeof(char); }
	memcpy( header_info->attrName, mem, j*sizeof(char));
	// attrLength
	while( *mem != '\0') { mem += sizeof(char); }
	mem += sizeof(char);
	temp = (int *) mem;
	header_info->attrLength = *temp;
	// numBuckets
	mem += sizeof(int);
	temp = (int *) mem;
	header_info->numBuckets = *temp;

	return header_info;
}

int HT_CreateIndex( char *fileName, char attrType, char* attrName, int attrLength, int buckets)
{
	/* Create the file */
	if ( BF_CreateFile(fileName) < 0) { BF_PrintError("Error with BF_CreateFile()\n"); return -1; }

	/* Open the file */
	int fd = BF_OpenFile(fileName);
	if ( fd < 0) { BF_PrintError("Error with BF_OpenFile()\n"); return -1;}

	primaryFileDescriptor = fd;

	/* Create the hash table (buckets only)*/
	int placedBucketsCounter = 0;

	void *block;
	char *mem;
	int *temp;

	int curBlockPlace;

	do
	{
		// allocate a new block for the hash table
		if ( BF_AllocateBlock( fd) < 0) { BF_PrintError("Error with BF_AllocateBlock()\n"); return -1;}

		// read this block
		if ( BF_ReadBlock( fd, BF_GetBlockCounter( fd)-1, &block) < 0) { BF_PrintError("Error with BF_ReadBlock()\n"); return -1; }
		curBlockPlace = 0; // we are currently at the beggining of the block( the END is BLOCK_SIZE)
		mem = (char *) block; // we use a char pointer for moving in block to make calculations easier

		// then, write the index Type
		*mem = 'p';
		mem += sizeof(char);
		curBlockPlace += sizeof(char);

		// then, write the attrType
		*mem = attrType;
		mem += sizeof(char);
		curBlockPlace += sizeof(char);

		// then, write the attrName
		memcpy( mem, attrName, strlen(attrName) + sizeof(char) );
		mem += strlen(attrName) + sizeof(char);
		curBlockPlace += strlen(attrName) + sizeof(char);

		// then, write the attrLength
		temp = (int *) mem;
		*temp = attrLength;
		mem += sizeof(int);
		curBlockPlace += sizeof(int);

		// then, write the total number of buckets in all the table
		temp = (int *) mem;
		*temp = buckets;
		mem += sizeof(int);
		curBlockPlace += sizeof(int);

		// now, write all buckets in the remaining space of the block
		// currently there are no data blocks, so each bucket has value -1
		// leave this loop only if there is no more space in block OR we finished initializing thebuckets
		do
		{
			temp = (int *) mem;
			*temp = -1;
			mem += sizeof(int);
			curBlockPlace += sizeof(int);

			placedBucketsCounter++;
		} while ( ( placedBucketsCounter != buckets) && ( curBlockPlace + sizeof(int) <= BLOCK_SIZE) );

		// write the block back to the file
		if ( BF_WriteBlock( fd, BF_GetBlockCounter( fd) -1) < 0) { BF_PrintError("Error with BF_WriteBlock()\n"); return -1; }

	// if there are more blocks to put, create another block, otherwise function is over
	}while ( placedBucketsCounter != buckets);

	/* Close the file*/
	if ( BF_CloseFile( fd) < 0) { BF_PrintError("Error with BF_CloseFile()\n"); return -1; }

	return 0;
}
