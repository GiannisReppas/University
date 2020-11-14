#include "sortMergeJoin.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Sort Merge Join /////////////////////////////////
resultBucket* SortMergeJoin( relation *relR, relation *relS,resultBucket* Res, bool testing)
{
	if ( testing)
	{
		int i;

		// create new R
		relation relR2;
		relR2.numtuples = relR->numtuples;
		relR2.tuples = malloc(sizeof(tuple)*relR2.numtuples);
		if (relR2.tuples == NULL) { printf("Error with malloc\n"); exit(1); }

		// create new R for testing and quicksort it
		relation relRtesting;
		relRtesting.numtuples = relR->numtuples;
		relRtesting.tuples = malloc(sizeof(tuple)*relRtesting.numtuples);
		if (relRtesting.tuples == NULL) { printf("Error with malloc\n"); exit(1); }
		for ( i=0; i<relRtesting.numtuples; i++) relRtesting.tuples[i] = relR->tuples[i];

		quickSort( &relRtesting, 0, relRtesting.numtuples-1);

		// sort R
		radixSort( relR, &relR2, 0, relR->numtuples-1, 1, 0);

		// test if R was sorted correctly
		FILE *fp; fp=fopen("testResults.txt","w"); if (fp == NULL) { printf("Error with fopen\n"); exit(1); }
		bool check = true;
		for ( i=0; i< relR->numtuples ; i++)
		{
			if ( relR->tuples[i].key != relRtesting.tuples[i].key)
			{
				check = false;
				break;
			}
		}
		if (check) fprintf(fp,"R was sorted correctly\n");
		else fprintf(fp,"R was NOT sorted correctly\n");

		// create new S
		relation relS2;
		relS2.numtuples = relS->numtuples;
		relS2.tuples = malloc(sizeof(tuple)*relS2.numtuples);
		if (relS2.tuples == NULL) { printf("Error with malloc\n"); exit(1); }

		// create new S for testing and quicksort it
                relation relStesting;
                relStesting.numtuples = relS->numtuples;
                relStesting.tuples = malloc(sizeof(tuple)*relStesting.numtuples);
                if (relStesting.tuples == NULL) { printf("Error with malloc\n"); exit(1); }
                for ( i=0; i<relStesting.numtuples; i++) relStesting.tuples[i] = relS->tuples[i];

                quickSort( &relStesting, 0, relStesting.numtuples-1);

		// sort S
		radixSort( relS, &relS2, 0, relS->numtuples-1, 1, 0);

		// test if R was sorted correctly
		check = true;
		for ( i=0; i< relS->numtuples ; i++)
		{
			if ( relS->tuples[i].key != relStesting.tuples[i].key)
			{
				check = false;
				break;
			}
		}
		if (check) fprintf(fp,"S was sorted correctly\n");
		else fprintf(fp,"S was NOT sorted correctly\n");
		fclose(fp);

		// MergeJoin R and S
		Res = MergeJoin(relR,relS,Res);

		// free memory
		free(relR->tuples);
		free(relR2.tuples);
		free(relS->tuples);
		free(relS2.tuples);
		free(relRtesting.tuples);
		free(relStesting.tuples);

		// return the list with the result
		return Res;
	}
	else
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
}

/////////////////////////////////////////////////////////////////////////////
//////////////////////////// MergeJoin //////////////////////////////////////
resultBucket* MergeJoin(relation* relR,relation* relS,resultBucket* Res)
{
	Res = NULL;
	resultBucket* LastBucket = NULL;
	int ct = 0;
	int ind = 0;
	bool flag= false;
	bool Dupl1 = true;
	bool Dupl2 = true;
	int tmp = 0;

	// for (int i = 0; i < relR->numtuples - 1; i++) /*Check if relR has any duplicates*/
	// {
	// 	if (relR->tuples[i].key == relR->tuples[i + 1].key)
	// 	{
	// 		Dupl1 = true;
	// 		break;
	// 	}
	// }
	// for (int i = 0; i < relS->numtuples - 1; i++) /*Check if relS has any duplicates*/
	// {
	// 	if (relS->tuples[i].key == relS->tuples[i + 1].key)
	// 	{
	// 		Dupl2 = true;
	// 		break;
	// 	}
	// }

	if (Dupl1 == true && Dupl2 == true ) /*Both arrays contain duplicates*/
	{
		printf("Join will be executed with method #1.\n");
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
						//ct++;
						//printf("%d\n",ct);
						Res = addToList(Res,relR->tuples[indexR].payload,relS->tuples[ind_tmp].payload,&LastBucket);
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




		// for (int i = 0; i < relR->numtuples; i++) /*For every distinct element in relR*/
		// {
		// 	for (int j = ind; j < relS->numtuples; j++) /*For every distinct element in relS*/
		// 	{
		// 		if (relR->tuples[i].key == relS->tuples[j].key)
		// 		{
		// 			if(flag ==  false){ind = j;flag = true;}
		// 			ct++;
		// 			printf("%d\n",ct);
		// 			Res = addToList(Res,relR->tuples[i].payload,relS->tuples[j].payload,&LastBucket); /*Adds to the list*/
		// 		}
		// 		else if (relR->tuples[i].key < relS->tuples[j].key)
		// 		{
		// 			break;
		// 		}
		// 		else
		// 		{
		// 			ind ++;
		// 		}	
		// 	}
		// 	flag = false;
		// }
		printf("\n\n\n\n");
		//printData(Res);
	}
	// else if (Dupl1 == true && Dupl2 == false) /*Only relS contains duplicates*/
	// {
	// 	printf("Join will be executed with method #2.\n");
	// 	int index1 = 0;
	// 	int index2 = 0;
	// 	while (true)
	// 	{
	// 		if (index1 >= relS->numtuples || index2 >= relR->numtuples)
	// 		{
	// 			break;
	// 		}
	// 		if (relR->tuples[index1].key== relS->tuples[index2].key)
	// 		{
	// 			//printf("[%llu] - [%llu]\n", relR->tuples[index1].payload, relS->tuples[index2].payload);
	// 			Res = addToList(Res,relR->tuples[index1].payload,relS->tuples[index2].payload);/*Adds to the list*/
	// 			index1++;
	// 		}
	// 		else if (relR->tuples[index1].key < relS->tuples[index2].key)
	// 		{
	// 			index1++;
	// 		}
	// 		else
	// 		{
	// 			index2++;
	// 		}
	// 	}
	// }
	// else if (Dupl1 == false && Dupl2 == true) /*Only the relR contains duplicates*/
	// {
	// 	printf("Join will be executed with method #3.\n");

	// 	int index1 = 0;
	// 	int index2 = 0;
	// 	while (true)
	// 	{
	// 		if (index1 >= relS->numtuples || index2 >= relR->numtuples)
	// 		{
	// 			break;
	// 		}
	// 		if (relR->tuples[index1].key == relS->tuples[index2].key)
	// 		{
	// 			//printf("[%llu] - [%llu]\n", relR->tuples[index1].payload, relS->tuples[index2].payload);
	// 			Res = addToList(Res,relR->tuples[index1].payload,relS->tuples[index2].payload);/*Adds to the list*/
	// 			index2++;
	// 		}
	// 		else if (relR->tuples[index1].key < relS->tuples[index2].key)
	// 		{
	// 			index1++;
	// 		}
	// 		else
	// 		{
	// 			index2++;
	// 		}
	// 	}
	// }
	// else if (Dupl1 == false && Dupl2 == false) /*No duplicates in any of the arrays */
	// {
	// 	printf("Join will be executed with method #4.\n");

	// 	int index1 = 0;
	// 	int index2 = 0;
	// 	while (true)
	// 	{
	// 		if (index1 >= relS->numtuples || index2 >= relR->numtuples)
	// 		{
	// 			break;
	// 		}
	// 		if (relR->tuples[index1].key == relS->tuples[index2].key)
	// 		{
	// 			//printf("[%llu] - [%llu]\n", relR->tuples[index1].key, relS->tuples[index2].key);
	// 			Res = addToList(Res,relR->tuples[index1].payload,relS->tuples[index2].payload);/*Adds to the list*/
	// 			index1++;
	// 			index2++;
	// 		}
	// 		else if (relR->tuples[index1].key< relS->tuples[index2].key)
	// 		{
	// 			index1++;
	// 		}
	// 		else
	// 		{
	// 			index2++;
	// 		}
	// 	}
	// }

	return Res;
}

/////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// add to list ///////////////////////////////////
resultBucket* addToList(resultBucket* Res , uint64_t payload1, uint64_t payload2,resultBucket** Last)
{
	if (Res == NULL)
	{
		Res = malloc(sizeof(resultBucket));
		Res->next = NULL;
		Res->numOfEntries = 0;
		Res->data[0][0] = payload1;
		Res->data[0][1] = payload2;
		Res->numOfEntries++;
		(*Last) = Res;
	}
	else
	{
		// resultBucket* current = Res;
		// while(current->next != NULL)
		// {
		// 	current = current->next;
		// }
		if((*Last)->numOfEntries == (BucketSize/(2*sizeof( uint64_t)) - 1 ))
		{ 
			(*Last)->next = malloc(sizeof(resultBucket));
			(*Last) = (*Last)->next;
			(*Last)->next = NULL;
			(*Last)->numOfEntries = 0;
			(*Last)->data[0][0] = payload1;
			(*Last)->data[0][1] = payload2;
			(*Last)->numOfEntries++;
		}
		else
		{
			(*Last)->data[(*Last)->numOfEntries][0] = payload1;
			(*Last)->data[(*Last)->numOfEntries][1] = payload2;
			(*Last)->numOfEntries++;
		}
	}

	return Res;
}
