#include "sums.h"

uint64_t *calcSums( fileNode *files, iNode *head, char **partC, int partCSize, int *relations)
{
	uint64_t *sums;
	sums = malloc( sizeof(uint64_t)*partCSize);
	if ( sums == NULL) { printf("Error with malloc()\n"); exit(1); }
	uint64_t current = 0;

	iNode *temp;
	char *array; char *column;
	uint64_t i,j,k;
	bool check;

	// for every sum
	for ( i=0; i<partCSize; i++)
	{
		// find array and column
		array = malloc(sizeof(char));
		if ( array == NULL) { printf("Error with malloc()\n"); exit(1); }
		array[0]='\0';
		for( j=0; partC[i][j] != '.'; j++)
		{
			array = realloc( array, strlen(array)*sizeof(char) + 2*sizeof(char));
			if ( array == NULL) { printf("Error with realloc()\n"); exit(1); }
			array[strlen(array)+1] = '\0';
			array[strlen(array)] = partC[i][j];
		}

		column = malloc(sizeof(char));
		if ( column == NULL) { printf("Error with malloc()\n"); exit(1); }
		column[0]='\0';
		for ( j++; partC[i][j] != '\0'; j++)
		{
			column = realloc( column, strlen(column)*sizeof(char) + 2*sizeof(char));
			if ( column == NULL) { printf("Error with malloc()\n"); exit(1); }
			column[strlen(column)+1] = '\0';
			column[strlen(column)] = partC[i][j];
		}

		// calculate sum
		check = false;
		temp = head;
		while ( temp != NULL)
		{
			for ( j=0; j < temp->relationsSize; j++)
			{
				if ( temp->relations[j] == atoi(array))
				{
					sums[current] = 0;
					for ( k=0; k < temp->numRows; k++)
						sums[current] += files[relations[atoi(array)]].array[ atoi(column)*files[relations[atoi(array)]].numRows + temp->info[k][j] ];

					check = true;
					current++;
				}

				if ( check) break;
			}

			if ( check) break;

			temp = temp->next;
		}

		// free memory
		free( array);
		free( column);
	}

	return sums;
}
void insertSums( sumsNode **head, uint64_t **array, uint64_t arraySize)
{
	sumsNode *newNode;
	newNode = malloc( sizeof(sumsNode));
	if (newNode == NULL) { printf("Error with malloc()\n"); exit(1); }

	newNode->array = *array;
	newNode->arraySize = arraySize;
	if ( *array != NULL)
		newNode->isNull = false;
	else
		newNode->isNull = true;
	newNode->next = NULL;

	if ( *head == NULL)
		*head = newNode;
	else
	{
		sumsNode *temp=*head;
		while ( temp->next != NULL)
			temp = temp->next;

		temp->next = newNode;
	}

	return;
}

void printSumsList( sumsNode *head)
{

	FILE *fp;
	fp=fopen("results.txt","a");
	if (fp == NULL) { printf("Error with fopen()"); exit(1); }

	sumsNode *temp=head;
	int i;

	while ( temp != NULL)
	{
		for ( i=0; i < temp->arraySize; i++)
		{
			if ( i != (temp->arraySize-1))
				if ( !temp->isNull)
				{
					printf("%lu ", temp->array[i]);
					fprintf( fp, "%lu ", temp->array[i]);
				}
				else
				{
					printf("NULL ");
					fprintf( fp, "NULL ");
				}
			else
				if ( !temp->isNull)
				{
					printf("%lu",temp->array[i]);
					fprintf( fp, "%lu", temp->array[i]);
				}
				else
				{
					printf("NULL");
					fprintf( fp, "NULL");
				}
		}
		printf("\n");
		fprintf( fp, "\n");

		temp = temp->next;
	}

	fclose(fp);

	return;
}

void freeSumsList( sumsNode **head)
{
	sumsNode *current=*head;
	sumsNode *previous;
	while ( current != NULL)
	{
		free( current->array);

		previous = current;
		current = current->next;
		free(previous);
	}

	return;
}
