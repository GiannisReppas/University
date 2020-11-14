#include "sortMergeJoin.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Sort Merge Join /////////////////////////////////
resultBucket* SortMergeJoin( relation *relR, relation *relS,resultBucket* Res)
{
	int i;

	// create new R
	relation relR2;
	relR2.numtuples = relR->numtuples;
	relR2.tuples = malloc(sizeof(tuple)*relR2.numtuples);
	if (relR2.tuples == NULL) { printf("Error with malloc\n"); exit(1); }

	// sort R
	radixSort( relR, &relR2, 0, relR->numtuples-1, 1, 0);

	// create new S
	relation relS2;
	relS2.numtuples = relS->numtuples;
	relS2.tuples = malloc(sizeof(tuple)*relS2.numtuples);
	if (relS2.tuples == NULL) { printf("Error with malloc\n"); exit(1); }

	// sort S
	radixSort( relS, &relS2, 0, relS->numtuples-1, 1, 0);

	// MergeJoin R and S
	Res = MergeJoin(relR,relS,Res);

	// free memory
	free(relR->tuples);
	free(relR2.tuples);
	free(relS->tuples);
	free(relS2.tuples);

	// return the list with the result
	return Res;

}

/////////////////////////////////////////////////////////////////////////////
//////////////////////////// MergeJoin //////////////////////////////////////
resultBucket* MergeJoin(relation* relR,relation* relS,resultBucket* Res)
{
	Res = NULL;
	resultBucket* LastBucket = NULL;
	int ct = 0;
	int ind = 0;

	int tmp = 0;

	int indexR = 0;
	int indexS = 0;

	while(indexR < relR->numtuples && indexS < relS->numtuples)
	{
		if(relR->tuples[indexR].key == relS->tuples[indexS].key) /*Found a match*/
		{
			int ind_tmp = indexS;
			uint64_t r1 = relR->tuples[indexR].key; //Hold the value that matched.
			uint64_t r2 = relS->tuples[indexS].key;

			while(relR->tuples[indexR].key == r1) /*For every duplicate value add it to the list.*/
			{
				while(relS->tuples[ind_tmp].key == r2)
				{
					Res = addToList(Res,relR->tuples[indexR].payload,relS->tuples[ind_tmp].payload,&LastBucket,relR->tuples[indexR].intermediatePayload,relS->tuples[ind_tmp].intermediatePayload);
					ind_tmp++;
				}
				indexR++;
				if(relR->tuples[indexR].key != r1)
				{
					indexS = ind_tmp;
				}
				else
				{
					ind_tmp = indexS;
				}
			}
		}
		else if (relR->tuples[indexR].key < relS->tuples[indexS].key)
		{
			uint64_t r1 = relR->tuples[indexR].key;
			while (r1 == relR->tuples[indexR].key){indexR++;} //Iterate to a different value
		}
		else
		{
			uint64_t r2 = relS->tuples[indexS].key;
			while (r2 == relS->tuples[indexS].key){indexS++;} //Iterate to a different value
		}
	}

	return Res;
}

/////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// add to list ///////////////////////////////////
resultBucket* addToList(resultBucket* Res , uint64_t payload1, uint64_t payload2,resultBucket** Last,uint64_t intpayload1, uint64_t intpayload2)
{
	if (Res == NULL)
	{
		Res = malloc(sizeof(resultBucket));
		Res->next = NULL;
		Res->numOfEntries = 0;
		Res->data[0][0] = payload1;
		Res->data[0][1] = payload2;
		Res->data2[0][0] = intpayload1;
		Res->data2[0][1] = intpayload2;

		Res->numOfEntries++;
		(*Last) = Res;
	}
	else
	{
		if((*Last)->numOfEntries == (BucketSize/(2*sizeof( uint64_t)) - 1 ))
		{
			(*Last)->next = malloc(sizeof(resultBucket));
			(*Last) = (*Last)->next;
			(*Last)->next = NULL;
			(*Last)->numOfEntries = 0;
			(*Last)->data[0][0] = payload1;
			(*Last)->data[0][1] = payload2;
			(*Last)->data2[0][0] = intpayload1;
			(*Last)->data2[0][1] = intpayload2;
			(*Last)->numOfEntries++;
		}
		else
		{
			(*Last)->data[(*Last)->numOfEntries][0] = payload1;
			(*Last)->data[(*Last)->numOfEntries][1] = payload2;
			(*Last)->data2[(*Last)->numOfEntries][0] = intpayload1;
			(*Last)->data2[(*Last)->numOfEntries][1] = intpayload2;
			(*Last)->numOfEntries++;
		}
	}

	return Res;
}

//////////////////////////////////////////////
////////////////// free list /////////////////
void free_list( resultBucket *Res)
{
	if ( Res != NULL)
	{
		resultBucket *current=Res;
		resultBucket *previous=NULL;

		while ( current->next != NULL)
		{
			previous = current;
			current = current->next;

			free(previous);
		}
		free(current);
	}

	return;
}
