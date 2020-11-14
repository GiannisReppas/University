#include "read_files.h"

int read_files( fileNode **files, FILE *fp)
{
	*files = NULL;
	int filesSize = 0;

	char *input = NULL;
	size_t len = 0;

	int fd;

	uint64_t temp;
	int returnValue;
	uint64_t i;
	uint64_t current1,current2;

	// each loop goes for every line at stdin
	while ( 1 )
	{
		if ( getline( &input, &len, fp) == -1)
			break;

		input[strlen(input)-1] = '\0';

		// open file
		fd = open( input, O_RDONLY, 0666);
		if ( fd < 0)
		{
			// display error message
			printf("Error with filename given\n");
		}
		else
		{
			// allocate memory for new table
			*files = (fileNode *) realloc( *files, (filesSize+1)*sizeof(fileNode) );
			if ( *files == NULL) { printf("Error with realloc()\n"); exit(1); }
			filesSize++;

			// rows
			returnValue = read( fd, &temp, sizeof(uint64_t));
			if ( returnValue < 0) { printf("Error with read()\n"); exit(1); }
			(*files)[filesSize-1].numRows = temp;

			//columns
			returnValue = read( fd, &temp, sizeof(uint64_t));
			if ( returnValue < 0) { printf("Error with read()\n"); exit(1); }
			(*files)[filesSize-1].numColumns = temp;

			// data
			(*files)[filesSize-1].array = (uint64_t *) malloc( (*files)[filesSize-1].numColumns * (*files)[filesSize-1].numRows * sizeof(uint64_t)  );
			if ( (*files)[filesSize-1].array == NULL) { printf("Error with malloc()\n"); exit(1); }

			i=0;
			while ( (returnValue = read( fd, &temp, sizeof(uint64_t)) > 0) )
			{
				(*files)[filesSize-1].array[i] = temp;

				i++;
			}
			if ( returnValue < 0) { printf("Error with read()\n"); exit(1); }

			// close file
			close(fd);
		}
		free(input);
		input=NULL; len=0;
	}

	return filesSize;
}
