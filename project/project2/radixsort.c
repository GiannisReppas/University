#include "radixsort.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Radix Sort ////////////////////////////////
void radixSort( relation *rel1, relation *rel2, int start, int end, int numByte, int currentArray)
{
	/* if the array that is to be sorted is greater than 64KB, call radixsort again
	   , otherwise call quicksort for the part [start,end] */
	if ( (((end-start)*sizeof(tuple)+1) > (1024*64)) && (numByte < 9) )
	{
		// create hist (2^8 places)
		int *hist;
		hist = malloc(sizeof(int)*(2*2*2*2*2*2*2*2));
		if (hist == NULL) { printf("Error with malloc\n"); exit(1); }

		// create psum
		int *psum;
		psum = malloc(sizeof(int)*(2*2*2*2*2*2*2*2));
		if (psum == NULL) { printf("Error with malloc\n"); exit(1); }

		/* initialize hist (all elements = 0) */
		int i;
		for ( i=0; i<(2*2*2*2*2*2*2*2); i++)
		{
			hist[i] = 0;
		}

		/* for each number in [start,end] part,
		   , find its numByte byte
		   and depending on numByte value, increase hist[byte] by one
		   , where byte -> value of numByte byte */
		uint64_t byte;
		for ( i=start; i<=end; i++)
		{
			byte = rel1->tuples[i].key >> (8*8 - numByte*8);
			byte = byte << 7*8;
			byte = byte >> 7*8;

			hist[byte] += 1;
		}

		/* initialize psum */
		int sum=start;
		for ( i=0; i< (2*2*2*2*2*2*2*2); i++)
		{
			psum[i] = sum;

			sum += hist[i];
		}

		/* for each element in rel1
		   , find its byte and based on its byte and the info for this byte on psum
		   , put it on the correct position on rel2 */
		for ( i=start; i<=end; i++)
		{
			byte = rel1->tuples[i].key >> (8*8 - numByte*8);
			byte = byte << 7*8;
			byte = byte >> 7*8;

			rel2->tuples[ psum[byte] ] = rel1->tuples[i];
			psum[byte] += 1;
		}

		/* reinitialize psum, we will need it for the recursive calls of readix sort */
		sum=start;
		for ( i=0; i< (2*2*2*2*2*2*2*2); i++)
		{
			psum[i] = sum;

			sum += hist[i];
		}

		/* for every element in psum, try to call radix sort.
		   Radix sort will be called only if the next element of the current element is different
		   , otherwise it means there is no group to sort for the category of psum[i] byte. */
		if ( currentArray == 0) currentArray = 1;
		else currentArray = 0;
		int j;
		for ( i=0; i< (2*2*2*2*2*2*2*2); i++)
		{
			if ( i != (2*2*2*2*2*2*2*2) - 1)
			{
				if ( psum[i] != psum[i+1])
				{
					radixSort( rel2, rel1, psum[i], psum[i+1] - 1, numByte+1, currentArray);
				}
			}
			else
			{
				if ( psum[i] <= end)
				{
					radixSort( rel2, rel1, psum[i], end, numByte+1, currentArray);
				}
			}
		}

		free(hist);
		free(psum);
	}
	else
	{
		quickSort( rel1, start, end);

		/* if currentArray == 1, then it means that the results got sorted in the new array (R') but we want our results in R, so we copy them */
		if ( currentArray == 1)
		{
			int i;
			for ( i=start; i<=end; i++)
			{
				rel2->tuples[i] = rel1->tuples[i];
			}
		}
	}

	return;
}
