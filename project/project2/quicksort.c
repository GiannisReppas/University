#include "quicksort.h"

//////////////////////////////////////////////
//////////// quicksort functions /////////////
void swap( uint64_t *a, uint64_t *b)
{
	uint64_t t = *a;
	*a = *b;
	*b = t;
}

int partition ( relation *rel, int start, int end)
{
	uint64_t x = rel->tuples[end].key;
	int i = (start - 1);

	int j;
	for ( j=start; j<=(end-1); j++)
	{
	        if ( rel->tuples[j].key < x)
		{
			i++;
			swap( &(rel->tuples[i].key), &(rel->tuples[j].key));
			swap( &(rel->tuples[i].payload), &(rel->tuples[j].payload));
			swap( &(rel->tuples[i].intermediatePayload), &(rel->tuples[j].intermediatePayload));
		}
	}

	swap( &(rel->tuples[i+1].key), &(rel->tuples[end].key));
	swap( &(rel->tuples[i+1].payload), &(rel->tuples[end].payload));
	swap( &(rel->tuples[i+1].intermediatePayload), &(rel->tuples[end].intermediatePayload));

	return (i + 1);
}

void quickSort( relation *rel, int start, int end)
{
	if (start < end)
	{
		int q = partition( rel, start, end);

		quickSort( rel, start, q-1);
		quickSort( rel, q+1, end);
	}

	return;
}
