#include "execute_query.h"

uint64_t* execute_query( fileNode *files, int filesSize, char **partA, int partASize, char **partB, int partBSize, char **partC, int partCSize)
{
	iNode *head=NULL;

	// from partA, create an array with all the relations that take part in the query
	int *relations;
	relations = malloc( partASize*sizeof(int));
	if ( relations == NULL) { printf("Error with malloc()\n"); exit(1); }

	int i;
	for ( i=0; i<partASize; i++)
		relations[i] = atoi(partA[i]);

	// execute each predicate of partB
	bool filter;
	char *array1; char *column1;
	char *array2; char *column2;
	char symbol;
	char *number;
	int j,k,previousI;
	bool sameArray;
	int position1,position2;

	resultBucket *Res;
	relation relR;
	relation relS;

	uint64_t *filterArray;
	uint64_t filterArraySize;

	for ( j=0; j<partBSize; j++) // for every predicate
	{
		// find what kind of predicate it is ( filter or join )
		for ( i=(strlen(partB[j])-1); i>0; i--)
		{
			if ( partB[j][i] == '.')
			{
				filter = false;
				break;
			}
			else if ( (partB[j][i] == '=') || (partB[j][i] == '>') || (partB[j][i] == '<') )
			{
				filter = true;
				break;
			}
		}

		// based on its kind, do your actions
		if ( filter)
		{
			// extract info from the predicate
			// array1, column1, symbol, number
			previousI = 0;
			for ( i=0; i<strlen(partB[j])+1; i++)
			{
				if ( partB[j][i] == '.')
				{
					array1 = malloc( (i-previousI+1)*sizeof(char));
					if ( array1 == NULL) { printf("Error with malloc()\n"); exit(1); }
					for ( k=previousI; k<i; k++)
						array1[k-previousI] = partB[j][k];
					array1[k-previousI] = '\0';

					previousI = i+1;
				}
				else if ( (partB[j][i] == '=') || (partB[j][i] == '<') || (partB[j][i] == '>') )
				{
					column1 = malloc( (i-previousI+1)*sizeof(char));
					if ( column1 == NULL) { printf("Error with malloc()\n"); exit(1); }
					for ( k=previousI; k<i; k++)
						column1[k-previousI] = partB[j][k];
					column1[k-previousI] = '\0';

					symbol = partB[j][i];

					previousI = i+1;
				}
				else if ( partB[j][i] == '\0')
				{
					number = malloc( (i-previousI+1)*sizeof(char));
					if ( number == NULL) { printf("Error with malloc()\n"); exit(1); }
					for ( k=previousI; k<i; k++)
						number[k-previousI] = partB[j][k];
					number[k-previousI] = '\0';
				}
			}

			position1 = existsInIntermediate(array1,head);

			if (position1 == -1) //The relation does not exist in the intermediate data structure
			{
				filterArray = NULL;
				filterArraySize = 0;

				for (i=0; i < files[relations[atoi(array1)]].numRows; i++)
				{
					if ( symbol == '<')
					{
						if ( files[relations[atoi(array1)]].array[ atoi(column1)*files[relations[atoi(array1)]].numRows + i ] < atoi(number))
						{
							filterArraySize++;
							filterArray = realloc( filterArray, filterArraySize*sizeof(uint64_t));
							if ( filterArray == NULL) { printf("Error with malloc()\n"); exit(1); }
							filterArray[filterArraySize-1] = i;
						}
					}
					else if( symbol == '=')
					{
						if ( files[relations[atoi(array1)]].array[ atoi(column1)*files[relations[atoi(array1)]].numRows + i] == atoi(number))
						{
							filterArraySize++;
							filterArray = realloc( filterArray, filterArraySize*sizeof(uint64_t));
							if ( filterArray == NULL) { printf("Error with malloc()\n"); exit(1); }
							filterArray[filterArraySize-1]=i;
						}
					}
					else if ( symbol == '>')
					{
						if ( files[relations[atoi(array1)]].array[ atoi(column1)*files[relations[atoi(array1)]].numRows + i] > atoi(number))
						{
							filterArraySize++;
							filterArray = realloc( filterArray, filterArraySize*sizeof(uint64_t));
							if ( filterArray == NULL) { printf("Error with malloc()\n"); exit(1); }
							filterArray[filterArraySize-1]=i;
						}
					}
				}
				if ( filterArraySize == 0)
				{
					free( array1); free( column1); free( number);
					free( relations);
					freeIntermediate( &head);
					return NULL;
				}

				intermediateInsertNewFilter( &head, filterArray, filterArraySize, array1);

				free(filterArray);
			}
			else //The relation exists in the intermediate data structure
			{
				if ( !updateNumericFilter( files, &head, array1, column1, symbol, atoi(number), position1, relations))
				{
					free( array1); free( column1); free( number);
					freeIntermediate(&head);
					return NULL;
				}
			}

			free(array1);
			free(column1);
			free(number);
		}
		else
		{
			// extract info from the predicate
			// array1, column1, array2, column2
			previousI=0;
			for ( i=0; i<strlen(partB[j])+1; i++)
			{
				if ( (partB[j][i] == '.') && (previousI == 0) )
				{
					array1 = malloc( (i-previousI+1)*sizeof(char));
					if ( array1 == NULL) { printf("Error with malloc()\n"); exit(1); }
					for ( k=previousI; k<i; k++)
						array1[k-previousI] = partB[j][k];
					array1[k-previousI] = '\0';

					previousI = i+1;
				}
				else if ( partB[j][i] == '=')
				{
					column1 = malloc( (i-previousI+1)*sizeof(char));
					if ( column1 == NULL) { printf("Error with malloc()\n"); exit(1); }
					for ( k=previousI; k<i; k++)
						column1[k-previousI] = partB[j][k];
					column1[k-previousI] = '\0';

					previousI = i+1;
				}
				else if ( (partB[j][i] == '.') && (previousI != 0) )
				{
					array2 = malloc( (i-previousI+1)*sizeof(char));
					if ( array2 == NULL) { printf("Error with malloc()\n"); exit(1); }
					for ( k=previousI; k<i; k++)
						array2[k-previousI] = partB[j][k];
					array2[k-previousI] = '\0';

					previousI = i+1;
				}
				else if ( partB[j][i] == '\0')
				{
					column2 = malloc( (i-previousI+1)*sizeof(char));
					if ( column2 == NULL) { printf("Error with malloc()\n"); exit(1); }
					for ( k=previousI; k<i; k++)
						column2[k-previousI] = partB[j][k];
					column2[k-previousI] = '\0';

					previousI = i+1;
				}
			}

			// find if array1==array2
			// and if array1 and array2 exist in the predicate
			if ( !strcmp(array1,array2))
				sameArray = true;
			position1 = existsInIntermediate( array1, head);
			position2 = existsInIntermediate( array2, head);

			if ( !sameArray)
			{
				// both arrays don't exist in intermeditate results
				// insert new Node in intermediate data structure
				if ( (position1==-1) && (position2==-1) )
				{
					relR.tuples = malloc( files[relations[atoi(array1)]].numRows*sizeof(tuple));
					if ( relR.tuples == NULL) { printf("Error with malloc()\n"); exit(1); }
					for ( i=0; i<files[relations[atoi(array1)]].numRows; i++)
					{
						relR.tuples[i].key = files[relations[atoi(array1)]].array[ atoi(column1)*files[relations[atoi(array1)]].numRows + i ];
						relR.tuples[i].payload = i;
						relR.tuples[i].intermediatePayload = 0;
					}
					relR.numtuples = files[relations[atoi(array1)]].numRows;

					relS.tuples = malloc( files[relations[atoi(array2)]].numRows*sizeof(tuple));
					if ( relS.tuples == NULL) { printf("Error with malloc()\n"); exit(1); }
					for ( i=0; i<files[relations[atoi(array2)]].numRows; i++)
					{
						relS.tuples[i].key = files[relations[atoi(array2)]].array[ atoi(column2)*files[relations[atoi(array2)]].numRows + i ];
						relS.tuples[i].payload = i;
						relS.tuples[i].intermediatePayload = 0;
					}
					relS.numtuples = files[relations[atoi(array2)]].numRows;

					Res = SortMergeJoin( &relR, &relS, Res);
					if ( Res == NULL)
					{
						free( array1); free( column1); free( array2); free( column2);
						free( relations);
						freeIntermediate( &head);
						return NULL;
					}

					intermediateInsertNewJoin( &head, Res, atoi(array1), atoi(array2));

					free_list( Res);
				}
				// 1 array exists in intermediate results, the other one does not
				// add the new one in the intermediate results
				else if ( (position1!=-1) && (position2==-1) )
				{
					createRelationFromIntermediate( files, head, &relR, array1, column1, relations, position1);

					relS.tuples = malloc( files[relations[atoi(array2)]].numRows*sizeof(tuple));
					if ( relS.tuples == NULL) { printf("Error with malloc()\n"); exit(1); }
					for ( i=0; i<files[relations[atoi(array2)]].numRows; i++)
					{
						relS.tuples[i].key = files[relations[atoi(array2)]].array[ atoi(column2)*files[relations[atoi(array2)]].numRows + i ];
						relS.tuples[i].payload = i;
						relS.tuples[i].intermediatePayload = 0;
					}
					relS.numtuples = files[relations[atoi(array2)]].numRows;

					Res = SortMergeJoin( &relR, &relS, Res);
					if ( Res == NULL)
					{
						free( array1); free( column1); free( array2); free( column2);
						free( relations);
						freeIntermediate( &head);
						return NULL;
					}

					updateIntermediateOneNode( &head, Res, array1, array2, relations, position1, 1);

					free_list( Res);
				}
				// 1 array exists in intermediate results, the other one does not
				// add the new one in the intermediate results
				else if ( (position1==-1) && (position2!=-1) )
				{
					relR.tuples = malloc( files[relations[atoi(array1)]].numRows*sizeof(tuple));
					if ( relR.tuples == NULL) { printf("Error with malloc()\n"); exit(1); }
					for ( i=0; i<files[relations[atoi(array1)]].numRows; i++)
					{
						relR.tuples[i].key = files[relations[atoi(array1)]].array[ atoi(column1)*files[relations[atoi(array1)]].numRows + i ];
						relR.tuples[i].payload = i;
						relR.tuples[i].intermediatePayload = 0;
					}
					relR.numtuples = files[relations[atoi(array1)]].numRows;

					createRelationFromIntermediate( files, head, &relS, array2, column2, relations, position2);

					Res = SortMergeJoin( &relR, &relS, Res);
					if ( Res == NULL)
					{
						free( array1); free( column1); free( array2); free( column2);
						free( relations);
						freeIntermediate( &head);
						return NULL;
					}

					updateIntermediateOneNode( &head, Res, array1, array2, relations, position2, 0);

					free_list( Res);
				}
				// both arrays exist in intermidiate results
				else if ( (position1!=-1) && (position2!=-1) )
				{
					// if they are in the same node, do filter
					if ( position1 == position2)
					{
						if ( !updateSameNodeFilter( files, &head, array1, array2, column1, column2, position1, relations))
						{
							free( array1); free( column1); free( array2); free( column2);
							free( relations);
							freeIntermediate( &head);
							return NULL;
						}
					}
					// if they are in different node, do sortMergeJoin
					else
					{
						createRelationFromIntermediate( files, head, &relR, array1, column1, relations, position1);

						createRelationFromIntermediate( files, head, &relS, array2, column2, relations, position2);

						Res = SortMergeJoin( &relR, &relS, Res);
						if ( Res == NULL)
						{
							free( array1); free( column1); free( array2); free( column2);
							free( relations);
							freeIntermediate( &head);
							return NULL;
						}

						updateIntermediateTwoNodes( &head, Res, array1, array2, relations, position1, position2 );

						free_list( Res);
					}
				}
			}
			else // join for the same array ( not join actually, just filter)
			{
				if ( position1 == -1)
				{
					filterArray = NULL;
					filterArraySize = 0;

					for ( i=0; i < files[relations[atoi(array1)]].numRows; i++)
					{
						if ( files[relations[atoi(array1)]].array[ atoi(column1)*files[relations[atoi(array1)]].numRows + i] == files[relations[atoi(array1)]].array[ atoi(column2)*files[relations[atoi(array1)]].numRows + i] )
						{
							filterArraySize++;
							filterArray = realloc( filterArray, filterArraySize*sizeof(uint64_t));
							if ( filterArray == NULL) { printf("Error with malloc()\n"); exit(1); }
							filterArray[filterArraySize-1]=i;
						}
					}
					if ( filterArraySize == 0)
					{
						free( array1); free( column1); free( array2); free( column2);
						free( relations);
						freeIntermediate( &head);
						return NULL;
					}

					intermediateInsertNewFilter( &head, filterArray, filterArraySize, array1);

					free(filterArray);
				}
				else
				{
					if ( !updateSameArrayFilter( files, &head, array1, column1, column2, position1, relations) )
					{
						free(array1); free(column1); free(array2); free(column2);
						free( relations);
						freeIntermediate( &head);
						return NULL;
					}
				}
			}
			/*******************************************/

			// free memory
			free(array1);
			free(column1);
			free(array2);
			free(column2);
		}
	}

	// part 3 - sums
	uint64_t *sums;
	sums = calcSums( files, head, partC, partCSize, relations);

	// free memory
	free(relations);
	freeIntermediate( &head);

	return sums;
}

int existsInIntermediate( char *array, iNode *head)
{
	int position=0;
	iNode *temp=head;
	int i;

	while ( temp != NULL)
	{
		for ( i=0; i< temp->relationsSize; i++)
		{
			if ( temp->relations[i] == atoi(array))
				return position;
		}

		temp = temp->next;
		position += 1;
	}

	return -1;
}

void createRelationFromIntermediate( fileNode *files, iNode *head, relation *rel, char *array, char *column, int *relations, int position)
{
	// find node
	int i=0;
	iNode *temp=head;
	while ( i != position)
	{
		temp=temp->next;
		i++;
	}

	// find column (array with rowIds)
	int j=0;
	while ( temp->relations[j] != atoi(array) )
		j++;

	// create relation
	rel->tuples = malloc( temp->numRows*sizeof(tuple));
	if ( rel->tuples == NULL) { printf("Error with malloc()\n"); exit(1); }
	for ( i=0; i < temp->numRows; i++)
	{
		rel->tuples[i].payload = temp->info[i][j];
		rel->tuples[i].key = files[relations[atoi(array)]].array[ atoi(column)*files[relations[atoi(array)]].numRows + rel->tuples[i].payload ];
		rel->tuples[i].intermediatePayload = i;
	}
	rel->numtuples = temp->numRows;

	return;
}

void freeIntermediate( iNode **head)
{
	iNode *current;
	iNode *previous;

	current = *head;
	uint64_t i;

	while( current != NULL)
	{
		free( current->relations);

		for ( i=0; i< current->numRows; i++)
			free( current->info[i]);
		free( current->info);

		previous = current;
		current = current->next;
		free( previous);
	}

	return;
}

