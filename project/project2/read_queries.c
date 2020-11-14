#include "read_queries.h"

void read_queries( fileNode *files, int filesSize, FILE *fp)
{
	int i;

	char *input=NULL;
	size_t len=0;

	char **parts;
	int partsSize;

	char **partA;	int partASize;
	char **partB;	int partBSize;
	char **partC;	int partCSize;

	uint64_t *current;
	sumsNode *head=NULL;

	// each loop goes for every query
	while( 1 )
	{
		if ( getline( &input, &len, fp) == -1)
			break;

		input[strlen(input)-1] = '\0';

		if ( !strcmp(input,"F") )
		{
			printSumsList( head);

			freeSumsList( &head);
			head=NULL;
		}
		else if ( !strcmp(input,"exit") )
		{
			printf("Program terminated\n");
			break;
		}
		else
		{
			// take info from query
			partsSize = split( input, '|', &parts);

			partASize = split( parts[0], ' ', &partA);
			partBSize = split( parts[1], '&', &partB);
			partCSize = split( parts[2], ' ', &partC);

			// swap the position of predicates in partB
			// putting filters in the beginning will speed up the predicates execution
			swap_predicates( &partB, partBSize);

			// execute query and write results in sums list
			current = execute_query( files, filesSize, partA, partASize, partB, partBSize, partC, partCSize);
			insertSums( &head, &current, partCSize);

			// free arrays
			for ( i=0; i<partsSize; i++)
				free(parts[i]);
			free(parts);

			for ( i=0; i<partASize; i++)
				free(partA[i]);
			free(partA);

			for ( i=0; i<partBSize; i++)
				free(partB[i]);
			free(partB);

			for ( i=0; i<partCSize; i++)
				free(partC[i]);
			free(partC);
		}
		free(input);
		input = NULL; len = 0;
	}
	free(input);

	return;
}

int split( char *input, char toBreak, char ***array)
{
	int arraySize=0;
	int i,j,previousI=0;

	// for every character in input
	for ( i=0; i< strlen(input)+1; i++)
	{
		// if you found toBreak or '\0', add a string to the array
		// the string you add starts right after from the last toBreak you found(or input[0]) and ends at teh current toBreak(or '\0')
		if ( (input[i] == toBreak) || ( input[i] == '\0') )
		{
			if ( arraySize != 0)
			{
				*array = (char **) realloc( *array, (arraySize+1)*sizeof(char*));
				if ( *array == NULL) { printf("Error with realloc()\n"); exit(1); }
			}
			else
			{
				*array = (char **) malloc( (arraySize+1)*sizeof(char *));
				if ( *array == NULL) { printf("Error with malloc()\n"); exit(1); }
			}
			arraySize++;

			(*array)[arraySize-1] = malloc( (i-previousI+1)*sizeof(char) );
			if ( (*array)[arraySize-1] == NULL) { puts("Error with malloc()\n"); exit(1); }
			for ( j=previousI; j<i; j++)
				(*array)[arraySize-1][j-previousI] = input[j];
			(*array)[arraySize-1][j-previousI] = '\0';

			previousI = j+1;
		}
	}

	return arraySize;
}

void swap_predicates( char ***predicates, int size)
{
	int firstJoin=-1;
	int i,j,k,previousK,l;
	bool filter;
	char *tempA; char *tempB;
	char *array1; char *array2;

	// for each predicate
	for ( i=0; i<size; i++)
	{
		// extract info from predicate ( isFilter and store position if its the first join)
		for ( j=strlen((*predicates)[i]); j>0; j--)
		{
			if ( (*predicates)[i][j] == '.')
			{
				// find array1 and array2
				previousK = 0;
				for ( k=0; k < strlen( (*predicates)[i]); k++)
				{
					if ( ((*predicates)[i][k] == '.') && ( previousK == 0) )
					{
						array1 = malloc( (k-previousK+1)*sizeof(char));
						if ( array1 == NULL) { printf("Error with malloc()\n"); exit(1); }
						for ( l=0; l < (k-previousK); l++)
							array1[l] = (*predicates)[i][l+previousK];
						array1[l] = '\0';
					}
					else if ( ((*predicates)[i][k] == '.') && (previousK != 0) )
					{
						array2 = malloc( (k-previousK+1)*sizeof(char));
						if ( array2 == NULL) { printf("Error with malloc()\n"); exit(1); }
						for ( l=0; l < (k-previousK); l++)
							array2[l] = (*predicates)[i][l+previousK];
						array2[l] = '\0';
					}

					if ( (*predicates)[i][k] == '=')
						previousK = k+1;
				}

				if ( !strcmp(array1,array2))
					filter = true;
				else
				{
					filter = false;
					if (firstJoin==-1)
						firstJoin=i;
				}

				free( array1); free(array2);
				break;
			}
			else if ( ((*predicates)[i][j]=='>') || ((*predicates)[i][j]=='<') || ((*predicates)[i][j]=='=') )
			{
				filter = true;
				break;
			}
		}

		// swap (*predicates)[i] and (*predicates)[firstJoin]
		if ( (filter) && (firstJoin!=-1) )
		{
			tempA = malloc( strlen( (*predicates)[firstJoin])*sizeof(char) + sizeof(char));
			if ( tempA == NULL) { printf("Error with malloc()\n"); exit(1); }
			strcpy( tempA, (*predicates)[firstJoin]);
			tempB = malloc( strlen( (*predicates)[i])*sizeof(char) + sizeof(char));
			if ( tempB == NULL) { printf("Error with malloc()\n"); exit(1); }
			strcpy( tempB, (*predicates)[i]);

			free( (*predicates)[firstJoin]);
			free( (*predicates)[i]);

			(*predicates)[firstJoin] = malloc( strlen(tempB)*sizeof(char) + sizeof(char));
			if ( (*predicates)[firstJoin] == NULL) { printf("Error with malloc()\n"); exit(1); }
			strcpy( (*predicates)[firstJoin], tempB);
			(*predicates)[i] = malloc( strlen(tempA)*sizeof(char) + sizeof(char));
			if ( (*predicates)[i] == NULL) { printf("Error with malloc()\n"); exit(1); }
			strcpy( (*predicates)[i], tempA);

			firstJoin = -1;
			i=0;

			free( tempA);
			free( tempB);
		}
	}

	return;
}
