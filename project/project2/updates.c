#include "updates.h"

void updateIntermediateTwoNodes( iNode **head, resultBucket *Res, char *array1, char *array2, int *relations, int position1, int position2)
{
	// array small1

	// find Node1
	int i=0;
	iNode *temp=*head;
	while ( i != position1)
	{
		i++;
		temp = temp->next;
	}

	// newNumRows of small1
	uint64_t newNumRows=0;
	resultBucket *tempRes = Res;
	while ( tempRes != NULL)
	{
		newNumRows += tempRes->numOfEntries;
		tempRes = tempRes->next;
	}

	// allocate small1
	uint64_t **small1;
	small1 = malloc( newNumRows*sizeof(uint64_t*));
	if ( small1 == NULL) { printf("Error with malloc()\n"); exit(1); }
	for ( i=0; i < newNumRows; i++)
	{
		small1[i] = malloc( temp->relationsSize*sizeof(uint64_t));
		if ( small1[i] == NULL) { printf("Error with malloc()\n"); exit(1); }
	}

	// initialize small1
	tempRes=Res;
	uint64_t currentRow=0,currentColumn;
	i=0;
	while ( tempRes != NULL)
	{
		for ( currentColumn=0; currentColumn < temp->relationsSize; currentColumn++)
			small1[currentRow][currentColumn] = temp->info[ tempRes->data2[i][0] ][currentColumn];

		currentRow++;

		i++;
		if ( i == tempRes->numOfEntries)
		{
			i=0;
			tempRes = tempRes->next;
		}
	}

	// array small2

	// find Node2
	i=0;
	temp=*head;
	while ( i != position2)
	{
		i++;
		temp = temp->next;
	}

	// newNumRows of small1 = newNumRows of small2

	// allocate small2
	uint64_t **small2;
	small2 = malloc( newNumRows*sizeof(uint64_t*));
	if ( small2 == NULL) { printf("Error with malloc()\n"); exit(1); }
	for ( i=0; i < newNumRows; i++)
	{
		small2[i] = malloc( temp->relationsSize*sizeof(uint64_t));
		if ( small2[i] == NULL) { printf("Error with malloc()\n"); exit(1); }
	}

	// initialize small2
	tempRes=Res;
	currentRow=0;
	i=0;
	while ( tempRes != NULL)
	{
		for ( currentColumn=0; currentColumn < temp->relationsSize; currentColumn++)
			small2[currentRow][currentColumn] = temp->info[ tempRes->data2[i][1] ][currentColumn];

		currentRow++;

		i++;
		if ( i == tempRes->numOfEntries)
		{
			i=0;
			tempRes = tempRes->next;
		}
	}

	// initialize new node
	iNode *newNode;
	newNode = malloc( sizeof(iNode));
	if ( newNode == NULL) { printf("Error with malloc()\n"); exit(1); }

	// newNode relationsSize and relations
	newNode->relationsSize = 0;
	i=0;
	temp = *head;
	while ( i != position1)
	{
		i++;
		temp = temp->next;
	}
	newNode->relationsSize += temp->relationsSize;
	newNode->relations = malloc( newNode->relationsSize*sizeof(uint64_t));
	if ( newNode->relations == NULL) { printf("Error with malloc()\n"); exit(1); }
	for ( i=0; i < newNode->relationsSize; i++)
		newNode->relations[i] = temp->relations[i];
	uint64_t j = newNode->relationsSize;

	i=0;
	temp = *head;
	while ( i != position2)
	{
		i++;
		temp = temp->next;
	}
	newNode->relationsSize += temp->relationsSize;
	newNode->relations = realloc( newNode->relations, newNode->relationsSize*sizeof(uint64_t));
	if ( newNode->relations == NULL) { printf("Error with realloc()\n"); exit(1); }
	for ( i=j; i < newNode->relationsSize; i++)
		newNode->relations[i] = temp->relations[i-j];

	// newNode next
	newNode->next = NULL;

	// newNode numRows
	newNode->numRows = newNumRows;

	// newNode info allocate
	newNode->info = malloc( newNode->numRows*sizeof(uint64_t*));
	if ( newNode->info == NULL) { printf("Error with malloc()\n"); exit(1); }
	for ( i=0; i < newNode->numRows; i++)
	{
		newNode->info[i] = malloc( newNode->relationsSize*sizeof(uint64_t));
		if ( newNode->info[i] == NULL) { printf("Error with malloc()\n"); exit(1); }
	}

	// newNode info initialize for small1
	int k;
	for ( i=0; i < newNode->numRows; i++)
		for ( k=0; k < j; k++)
			newNode->info[i][k] = small1[i][k];

	// newNode info initialize for small2
	for ( i=0; i < newNode->numRows; i++)
		for ( k=j; k < newNode->relationsSize; k++)
			newNode->info[i][k] = small2[i][k-j];

	// free small1, small2
	for ( i=0; i < newNumRows; i++)
		free(small1[i]);
	free(small1);

	for ( i=0; i < newNumRows; i++)
		free(small2[i]);
	free(small2);

	// free Node1 and Node 2
	iNode *temp1,*temp2;
	i=0;
	temp1=*head;
	while ( i != position1)
	{
		i++;
		temp1 = temp1->next;
	}
	i=0;
	temp2=*head;
	while ( i != position2)
	{
		i++;
		temp2 = temp2->next;
	}

	iNode *previous;

	if ( temp1 == *head)
		*head = temp1->next;
	else
	{
		previous=*head;
		while ( previous->next != temp1)
			previous = previous->next;
		previous->next = temp1->next;
	}
	free( temp1->relations);
	for ( i=0; i < temp1->numRows; i++)
		free(temp1->info[i]);
	free( temp1->info);
	free( temp1);

	if ( temp2 == *head)
		*head = temp2->next;
	else
	{
		previous=*head;
		while ( previous->next != temp2)
			previous = previous->next;
		previous->next = temp2->next;
	}
	free( temp2->relations);
	for ( i=0; i < temp2->numRows; i++)
		free( temp2->info[i]);
	free( temp2->info);
	free( temp2);

	// insert NewNode
	if ( *head == NULL)
		*head = newNode;
	else
	{
		temp = *head;
		while ( temp->next != NULL)
			temp = temp->next;
		temp->next = newNode;
	}

	return;
}

bool updateSameNodeFilter( fileNode *files, iNode **head, char *array1, char *array2, char *column1, char *column2, int position, int *relations)
{
	int i=0;
	iNode *temp = *head;
	while ( i != position)
	{
		i++;
		temp = temp->next;
	}

	int j1,j2,j;
	for ( j1=0; j1 < temp->relationsSize; j1++)
		if ( temp->relations[j1] == atoi(array1))
			break;

	for ( j2=0; j2 < temp->relationsSize; j2++)
		if ( temp->relations[j2] == atoi(array2))
			break;

	uint64_t **newInfo;
	uint64_t newNumRows=0;
	for ( i=0; i < temp->numRows; i++)
	{
		if ( files[relations[atoi(array1)]].array[ atoi(column1)*files[relations[atoi(array1)]].numRows + temp->info[i][j1] ] == files[relations[atoi(array2)]].array[ atoi(column2)*files[relations[atoi(array2)]].numRows + temp->info[i][j2] ] )
		{
			if ( newNumRows != 0)
			{
				newNumRows++;
				newInfo = realloc( newInfo, newNumRows*sizeof(uint64_t*));
				if (newInfo==NULL) { printf("Error with realloc()\n"); exit(1); }
				newInfo[newNumRows-1] = malloc( temp->relationsSize*sizeof(uint64_t));
				if (newInfo[newNumRows-1]==NULL) { printf("Error with malloc()\n"); exit(1); }
				for ( j=0; j < temp->relationsSize; j++)
					newInfo[newNumRows-1][j] = temp->info[i][j];
			}
			else
			{
				newNumRows++;
				newInfo = malloc( newNumRows*sizeof(uint64_t*));
				if (newInfo==NULL) { printf("Error with malloc()\n"); exit(1); }
				newInfo[newNumRows-1] = malloc( temp->relationsSize*sizeof(uint64_t));
				if (newInfo[newNumRows-1]==NULL) { printf("Error with malloc()\n"); exit(1); }
				for ( j=0; j < temp->relationsSize; j++)
					newInfo[newNumRows-1][j] = temp->info[i][j];
			}
		}
	}
	if ( newNumRows == 0)
		return false;

	for ( i=0; i < temp->numRows; i++)
		free( temp->info[i]);
	free( temp->info);

	temp->info = malloc( newNumRows*sizeof(uint64_t*));
	if ( temp->info == NULL) { printf("Error with malloc()\n"); exit(1); }
	for ( i=0; i < newNumRows; i++)
	{
		temp->info[i] = malloc( temp->relationsSize*sizeof(uint64_t));
		if ( temp->info[i] == NULL) { printf("Error with malloc()\n"); exit(1); }
	}
	temp->numRows = newNumRows;

	for ( i=0; i < newNumRows; i++)
		for ( j=0; j < temp->relationsSize; j++)
			temp->info[i][j] = newInfo[i][j];

	for ( i=0; i < temp->numRows; i++)
		free( newInfo[i]);
	free( newInfo);

	return true;
}

bool updateSameArrayFilter( fileNode *files, iNode **head, char *array, char *column1, char *column2, int position, int *relations)
{
	int i=0,j;
	iNode *temp=*head;
	while ( i != position)
	{
		i++;
		temp = temp->next;
	}

	uint64_t *filterArray=NULL;
	uint64_t filterArraySize=0;
	for ( i=0; i < temp->numRows; i++)
	{
		if ( files[relations[atoi(array)]].array[ atoi(column1)*files[relations[atoi(array)]].numRows + temp->info[i][0] ] == files[relations[atoi(array)]].array[ atoi(column2)*files[relations[atoi(array)]].numRows + temp->info[i][0] ] )
		{
			filterArraySize++;
			filterArray = realloc( filterArray, filterArraySize*sizeof(uint64_t));
			if ( filterArray == NULL) { printf("Error with malloc()\n"); exit(1); }

			filterArray[filterArraySize-1] = temp->info[i][0];
		}
	}

	if ( filterArraySize != 0)
	{
		for ( i=0; i < temp->numRows; i++)
			free( temp->info[i]);
		free(temp->info);

		temp->numRows = filterArraySize;
		temp->info = malloc( filterArraySize*sizeof(uint64_t));
		if ( temp->info == NULL) { printf("Error with malloc()\n"); exit(1); }
		for ( i=0; i < temp->numRows; i++ )
		{
			temp->info[i] = malloc( temp->relationsSize*sizeof(uint64_t));
			if ( temp->info[i] == NULL) { printf("Error with malloc()\n"); exit(1); }
		}
		for ( i=0; i < temp->numRows; i++)
			temp->info[i][0] = filterArray[i];

		free(filterArray);

		return true;
	}
	else
		return false;
}

bool updateNumericFilter( fileNode *files, iNode **head, char *array, char *column1, char symbol, int convertedNumber, int position, int *relations)
{
	// find Node
	uint64_t i=0,j;
	iNode *temp=*head;
	while ( i != position)
	{
		i++;
		temp = temp->next;
	}

	// put the results in a temp array (filterArray)
	uint64_t *filterArray=NULL;
	uint64_t filterArraySize=0;
	for ( i=0; i < temp->numRows; i++)
	{
		if ( symbol == '<')
		{
			if ( files[relations[atoi(array)]].array[ atoi(column1)*files[relations[atoi(array)]].numRows + temp->info[i][0] ] < convertedNumber)
			{
				filterArraySize++;
				filterArray = realloc( filterArray, filterArraySize*sizeof(uint64_t));
				if ( filterArray == NULL) { printf("Error with malloc()\n"); exit(1); }

				filterArray[filterArraySize-1] = temp->info[i][0];
			}
		}
		else if ( symbol == '=')
		{
			if ( files[relations[atoi(array)]].array[ atoi(column1)*files[relations[atoi(array)]].numRows + temp->info[i][0] ] == convertedNumber)
			{
				filterArraySize++;
				filterArray = realloc( filterArray, filterArraySize*sizeof(uint64_t));
				if ( filterArray == NULL) { printf("Error with malloc()\n"); exit(1); }

				filterArray[filterArraySize-1] = temp->info[i][0];
			}
		}
		else if ( symbol == '>')
		{
			if ( files[relations[atoi(array)]].array[ atoi(column1)*files[relations[atoi(array)]].numRows + temp->info[i][0] ] > convertedNumber)
			{
				filterArraySize++;
				filterArray = realloc( filterArray, filterArraySize*sizeof(uint64_t));
				if ( filterArray == NULL) { printf("Error with malloc()\n"); exit(1); }

				filterArray[filterArraySize-1] = temp->info[i][0];
			}
		}
	}
	if ( filterArraySize == 0)
		return false;

	// free temp->info
	for ( i=0; i < temp->numRows; i++)
		free( temp->info[i]);
	free(temp->info);

	// pass filter Array to info
	temp->numRows = filterArraySize;
	temp->info = malloc( filterArraySize*sizeof(uint64_t));
	if ( temp->info == NULL) { printf("Error with malloc()\n"); exit(1); }
	for ( i=0; i < temp->numRows; i++ )
	{
		temp->info[i] = malloc( temp->relationsSize*sizeof(uint64_t));
		if ( temp->info[i] == NULL) { printf("Error with malloc()\n"); exit(1); }
	}
	for ( i=0; i < temp->numRows; i++)
		temp->info[i][0] = filterArray[i];

	// free filter array
	free(filterArray);

	return true;
}


void intermediateInsertNewFilter( iNode **head, uint64_t *filterArray, uint64_t filterArraySize, char *array)
{
	int i;

	// allocate
	iNode *newNode = malloc(sizeof(iNode));
	if ( newNode == NULL) { printf("Error with malloc()\n"); exit(1); }

	// initialize
	newNode->next = NULL;
	newNode->numRows = filterArraySize;
	newNode->relationsSize = 1;
	newNode->relations=malloc(sizeof(uint64_t));
	if ( newNode->relations == NULL) { printf("Error with malloc()\n"); exit(1); }
	newNode->relations[0] = atoi(array);
	newNode->info = malloc(newNode->numRows*sizeof(uint64_t));
	if ( newNode->info == NULL) { printf("Error with malloc()\n"); exit(1); }
	for ( i=0; i < newNode->numRows; i++)
	{
		newNode->info[i] = malloc(newNode->relationsSize*sizeof(uint64_t));
		if ( newNode->info[i] == NULL) { printf("Error with malloc()\n"); exit(1); }
	}
	for ( i=0; i < newNode->numRows; i++)
		newNode->info[i][0] = filterArray[i];

	// insert
	if ( *head == NULL)
		*head = newNode;
	else
	{
		iNode *temp=*head;
		while ( temp->next != NULL)
			temp = temp->next;
		temp->next = newNode;
	}

	return;
}

void intermediateInsertNewJoin( iNode **head, resultBucket *Res, int relA, int relB)
{
	// new node
	uint64_t i,j;
	iNode *toInsert;
	toInsert = malloc(sizeof(iNode));
	if ( toInsert == NULL) { printf("Error with malloc()\n"); exit(1); }

	// next
	toInsert->next = NULL;

	// relations, relationsSize
	toInsert->relationsSize = 2;
	toInsert->relations = malloc( toInsert->relationsSize*sizeof(uint64_t));
	if ( toInsert->relations == NULL) { printf("Error with malloc()\n"); exit(1); }
	toInsert->relations[0] = relA;
	toInsert->relations[1] = relB;

	// numRows
	toInsert->numRows = 0;
	resultBucket *temp = Res;
	while ( temp != NULL)
	{
		toInsert->numRows += temp->numOfEntries;
		temp = temp->next;
	}

	// info
	toInsert->info = malloc( toInsert->numRows * sizeof(uint64_t));
	if ( toInsert->info == NULL) { printf("Error with malloc()\n"); exit(1); }
	for ( i=0; i< toInsert->numRows; i++)
		toInsert->info[i] = malloc( toInsert->relationsSize * sizeof(uint64_t));
	i=0; j=0;
	temp = Res;
	while ( temp != NULL)
	{
		toInsert->info[j][0] = temp->data[i][0];
		toInsert->info[j][1] = temp->data[i][1];

		j++;
		i++;
		if ( i== temp->numOfEntries)
		{
			i = 0;
			temp = temp->next;
		}
	}

	// add new node to list
	if ( *head == NULL)
		*head = toInsert;
	else
	{
		iNode *current;
		current = *head;

		while ( current->next != NULL)
			current = current->next;

		current->next = toInsert;
	}

	return;
}

void updateIntermediateOneNode( iNode **head, resultBucket *Res, char *array1, char *array2, int *relations, int position, int nonExisting)
{
	// find the node you want from the intermediate data structure
	uint64_t i=0;
	iNode *tempInode=*head;
	while ( i != position)
	{
		tempInode = tempInode->next;
		i++;
	}

	// update relationsSize and relations
	tempInode->relationsSize++;
	tempInode->relations = realloc( tempInode->relations, tempInode->relationsSize*sizeof(uint64_t));
	if ( tempInode->relations == NULL) { printf("Error with realloc()\n"); exit(1); }
	if ( nonExisting == 0)
		tempInode->relations[tempInode->relationsSize-1] = atoi(array1);
	else
		tempInode->relations[tempInode->relationsSize-1] = atoi(array2);

	// store the info and numRows in a temp array (oldinfo , oldNumRows)
	uint64_t **oldInfo;
	oldInfo = malloc( tempInode->numRows*sizeof(uint64_t*));
	if ( oldInfo == NULL) { printf("Error with malloc()\n"); exit(1); }
	for ( i=0; i < tempInode->numRows; i++)
	{
		oldInfo[i] = malloc( (tempInode->relationsSize-1)*sizeof(uint64_t));
		if ( oldInfo[i] == NULL) { printf("Error with malloc()\n"); exit(1); }
	}
	uint64_t j;
	for ( i=0; i < tempInode->numRows; i++)
		for ( j=0; j < (tempInode->relationsSize-1); j++)
			oldInfo[i][j] = tempInode->info[i][j];
	uint64_t oldNumRows = tempInode->numRows;

	// free info
	for ( i=0; i< tempInode->numRows; i++)
		free( tempInode->info[i]);
	free( tempInode->info);

	// update numRows
	tempInode->numRows = 0;
	resultBucket *temp=Res;
	while ( temp != NULL)
	{
		tempInode->numRows += temp->numOfEntries;
		temp = temp->next;
	}

	// update info (malloc only)
	tempInode->info = malloc( tempInode->numRows*sizeof(uint64_t*));
	if ( tempInode->info == NULL) { printf("Error with malloc()\n"); exit(1); }
	for ( i=0; i < tempInode->numRows; i++)
	{
		tempInode->info[i] = malloc( tempInode->relationsSize*sizeof(uint64_t));
		if ( tempInode->info[i] == NULL) { printf("Error with malloc()\n"); exit(1); }
	}

	// update info (data)
	temp=Res;
	uint64_t currentRow=0,currentColumn;
	int existing;

	if ( nonExisting == 0) existing = 1;
	else existing = 0;

	i=0;
	while ( temp != NULL)
	{
		for ( currentColumn=0; currentColumn < (tempInode->relationsSize-1); currentColumn++)
			tempInode->info[currentRow][currentColumn] = oldInfo[ temp->data2[i][existing]  ][currentColumn];
		tempInode->info[currentRow][currentColumn] = temp->data[i][nonExisting];

		currentRow++;

		i++;
		if ( i == temp->numOfEntries)
		{
			i=0;
			temp = temp->next;
		}
	}

	// free oldInfo
	for ( i=0; i< oldNumRows; i++)
		free(oldInfo[i]);
	free(oldInfo);

	return;
}
