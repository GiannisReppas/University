// ./smj.exe file1 file2
// file1 -> file with array number 1
// file2 -> file with array number 2
// -t at the end possible -> optinal parameter for testing

#include "types.h"
#include "quicksort.h"
#include "radixsort.h"
#include "sortMergeJoin.h"

int COUNTING = 0 ;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////// functions ////////////////////////////////////////////////////////////

/** reads files and saves in memory column by column the 2 arrays that will be joined - returns false in case of an error **/
bool create_relation( FILE *fp, relation *rel);

/** prints the rows of the 2 arrays that will be joined **/
void printData(resultBucket*);

/** free list **/
void free_list( resultBucket*);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// main ////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	if ( (argc != 3) && (argc != 4) ) { printf("Error with input from command line - must have eactly 3 arguements\n"); exit(1); }

	bool testing = false;
	if ( argc == 4)
	{
		if ( strcmp(argv[3],"-t")) { printf("Error with 4th arguement\n"); exit(1); }
		else testing = true;
	}

	// open files
	FILE *fp1;
	fp1 = fopen(argv[1],"r");
	if (fp1 == NULL) { printf("Error opening file1\n"); exit(1); }
	FILE *fp2;
	fp2 = fopen(argv[2],"r");
	if (fp2 == NULL) { printf("Error opening file2\n"); exit(1); }

	// create the 2 arrays
	relation relation1; relation1.tuples = NULL; relation1.numtuples = 0;
	if ( !create_relation( fp1, &relation1) ) { printf("Error with function create_array()\n"); exit(1); }
	relation relation2; relation2.tuples = NULL; relation2.numtuples = 0;
	if ( !create_relation( fp2, &relation2) ) { printf("Error with function create_array()\n"); exit(1); }

	// close files
	fclose(fp1);
	fclose(fp2);

	// sort merge join for the 2 relations
	resultBucket* Res;
	Res = SortMergeJoin( &relation1, &relation2, Res, testing);

	printf("FINAL PRINT \n\n\n\n\n\n");
	printData( Res);
	printf("\n\n\n");
	printf("COUNTED A TOTAL OF : %d VALUES.\n",COUNTING);

	// free list
	free_list( Res);

	return 0;
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

/////////////////////////////////////////////////////////////////
///////////////////////// create relation ///////////////////////
bool create_relation( FILE *fp, relation *rel)
{
	// to store a character of a file
	int c;

	// to store a number of the file in string mode
	char *str; str = malloc(sizeof(char)); if (str == NULL) { printf("Error with malloc\n"); return false; }
	str[0] = '\0';
	int length;
	char *ptr;

	// to store a number of a file
	uint64_t current;

	do
	{
		// read a character from the file
		c = fgetc(fp);

		if ( c == ',') // first number of line ended, it was a key
		{
			current = strtoull(str,&ptr,10);

			free(str);
			str = malloc(sizeof(char)); str[0] = '\0';
			if (str == NULL) { printf("Error with malloc\n"); return false; }
			str[0] = '\0';

			rel->tuples = realloc( rel->tuples, rel->numtuples*sizeof(tuple) + sizeof(tuple));
			if (rel->tuples == NULL) { printf("Error with realloc\n"); return false; }
			rel->tuples[rel->numtuples].key = current;

			rel->numtuples++;
		}
		else if ( c == '\n') // end of line, last number was a payload
		{
			current = strtoull(str,&ptr,10);

			free(str);
			str = malloc(sizeof(char)); str[0] = '\0';
			if (str == NULL) { printf("Error with malloc\n"); return false; }
			str[0] = '\0';

			rel->tuples[rel->numtuples-1].payload = current;
		}
		else if ( c == EOF ) // reached the end of file
		{
			break;
		}
		else // reading a number digit
		{
			length = strlen(str);
			str = realloc(str, sizeof(char)*length + 2*sizeof(char));
			if (str == NULL) { printf("Error with realloc()\n"); return false; }
			str[length+1] = '\0';
			str[length] = c;
		}

	}while( 1 );
	free(str);

	return true;
}

void printData(resultBucket* Res)
{
	resultBucket* current = Res;
	FILE *fpointer;
	fpointer=fopen("results.txt","w"); /*Opens a file to export data*/
	if (fpointer == NULL ){ printf("Failed to create file! \n");exit(1); }

	while(current != NULL)
	{
		for (int i = 0; i < current->numOfEntries; i++)
		{
			fprintf(fpointer,"%" PRIu64 ",%" PRIu64 "\n",current->data[i][0],current->data[i][1]);
			//printf("%" PRIu64 ",%" PRIu64 "\n",current->data[i][0],current->data[i][1]);
			COUNTING ++ ;// = COUNTING + current->numOfEntries;
			//break;
		}
		current = current->next;
	}
	fclose(fpointer);
}
