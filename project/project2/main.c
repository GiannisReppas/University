#include "types.h"
#include "quicksort.h"
#include "radixsort.h"
#include "sortMergeJoin.h"
// ./main.exe file1 file2

#include "read_files.h"
#include "read_queries.h"
#include "execute_query.h"
#include "sums.h"
#include "updates.h"

/* frees memory */
void free_memory( fileNode**, int);

/* reads arguements the user gave from the command line */
void readCommandLine( int, char**, FILE**, FILE**);

int main( int argc, char *argv[])
{
	// take info from command line
	FILE *fpFiles, *fpQueries;
	readCommandLine( argc, argv, &fpFiles, &fpQueries);

	// take info from files and store them in memory
	fileNode *files;
	int filesSize;
	filesSize = read_files( &files, fpFiles);

	// read queries
	read_queries( files, filesSize, fpQueries);

	// free memory
	free_memory( &files, filesSize);
	fclose( fpFiles);
	fclose( fpQueries);

	return 0;
}

void readCommandLine( int argc, char **argv, FILE **fpFiles, FILE **fpQueries)
{
	if ( argc != 3) { printf("Error with arguements from command line\n"); exit(1); }

	*fpFiles = fopen(argv[1], "r");
	if ( fpFiles == NULL) { printf("Error openning the files file\n"); exit(1); }

	*fpQueries = fopen( argv[2], "r");
	if ( fpQueries == NULL) { printf("Error openning the queries file\n"); exit(1); }

	return;
}

// frees all fileNodes
void free_memory( fileNode **files,int filesSize)
{
	uint64_t i;
	for ( i=0; i<filesSize; i++)
		free( (*files)[i].array);

	free(*files);

	return;
}
