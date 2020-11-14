#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#define GENERATIONS 300

double get_time()
{
	struct timeval t;
	gettimeofday( &t, NULL);
	double now = t.tv_sec + t.tv_usec/1000000.0;
	return now;
}

void command_line_check( int argc, char **argv, int *rows, int *columns)
{
	if (argc != 3) { printf("Number of arguements must be exactly 3\n"); exit(1); }

	*rows = atoi(argv[1]);
	if ( !((*rows)>0) ) { printf("Number of rows must be greater than zero\n"); exit(1); }

	*columns = atoi( argv[2]);
	if ( !((*columns)>0) ) { printf("Number of columns must be greater than zero\n"); exit(1); }

	return;
}

void create_grid( bool ***grid, int rows, int columns)
{
	// allocate memory
	int i,j;
	*grid = malloc(rows * sizeof(bool *));
	if (*grid == NULL) { printf("Error with malloc()\n"); exit(1); }
	for( i=0; i < rows; i++)
	{
		(*grid)[i] = malloc(columns * sizeof(bool));
		if ( (*grid)[i] == NULL) { printf("Error with malloc();\n"); exit(1); }
	}

	// initialize array
	for (i=0; i < rows; i++)
		for (j=0; j  < columns; j++)
			(*grid)[i][j] = rand() % 2;

	return;
}

int findNeighbors(bool **grid, int i, int j, int rows, int columns)
{
	int neighborsNum = 0;

	// points inside of the perimeter
	if ( (i > 0) && (j > 0) && (i < (rows-2)) && (j < (columns-2)) )
	{
		if ( grid[i-1][j-1] ) neighborsNum++;
		if ( grid[i-1][j] ) neighborsNum++;
		if ( grid[i-1][j+1] ) neighborsNum++;
		if ( grid[i][j-1] ) neighborsNum++;
		if ( grid[i][j+1] ) neighborsNum++;
		if ( grid[i+1][j-1] ) neighborsNum++;
		if ( grid[i+1][j] ) neighborsNum++;
		if ( grid[i+1][j+1] ) neighborsNum++;
	}

	// points in the perimeter but not corners
	else if ( (i == 0) && (j < (columns-1)) && (j > 0) )
	{
		if ( grid[rows-1][j-1] ) neighborsNum++;
		if ( grid[rows-1][j] ) neighborsNum++;
		if ( grid[rows-1][j+1] ) neighborsNum++;
		if ( grid[i][j-1] ) neighborsNum++;
		if ( grid[i][j+1] ) neighborsNum++;
		if ( grid[i+1][j-1] ) neighborsNum++;
		if ( grid[i+1][j] ) neighborsNum++;
		if ( grid[i+1][j+1] ) neighborsNum++;
	}
	else if ( (j == (columns-1)) && (i < (rows-1)) && (i > 0) )
	{
		if ( grid[i-1][j-1] ) neighborsNum++;
		if ( grid[i-1][j] ) neighborsNum++;
		if ( grid[i-1][0] ) neighborsNum++;
		if ( grid[i][j-1] ) neighborsNum++;
		if ( grid[i][0] ) neighborsNum++;
		if ( grid[i+1][j-1] ) neighborsNum++;
		if ( grid[i+1][j] ) neighborsNum++;
		if ( grid[i+1][0] ) neighborsNum++;
	}
	else if ( (i == (rows-1)) && (j < (columns-1)) && (j > 0) )
	{
		if ( grid[i-1][j-1] ) neighborsNum++;
		if ( grid[i-1][j] ) neighborsNum++;
		if ( grid[i-1][j+1] ) neighborsNum++;
		if ( grid[i][j-1] ) neighborsNum++;
		if ( grid[i][j+1] ) neighborsNum++;
		if ( grid[0][j-1] ) neighborsNum++;
		if ( grid[0][j] ) neighborsNum++;
		if ( grid[0][j+1] ) neighborsNum++;
	}
	else if ( (j == 0) && (i < (rows-1)) && (i > 0) )
	{
		if ( grid[i-1][columns-1] ) neighborsNum++;
		if ( grid[i-1][j] ) neighborsNum++;
		if ( grid[i-1][j+1] ) neighborsNum++;
		if ( grid[i][columns-1] ) neighborsNum++;
		if ( grid[i][j+1] ) neighborsNum++;
		if ( grid[i+1][columns-1] ) neighborsNum++;
		if ( grid[i+1][j] ) neighborsNum++;
		if ( grid[i+1][j+1] ) neighborsNum++;
	}

	// corners of the perimeter
	else if ( (i == 0) && (j == 0) )
	{
		if ( grid[rows-1][columns-1] ) neighborsNum++;
		if ( grid[rows-1][0] ) neighborsNum++;
		if ( grid[rows-1][1] ) neighborsNum++;
		if ( grid[0][columns-1] ) neighborsNum++;
		if ( grid[0][1] ) neighborsNum++;
		if ( grid[1][columns-1] ) neighborsNum++;
		if ( grid[1][0] ) neighborsNum++;
		if ( grid[1][1] ) neighborsNum++;
	}
	else if ( (i == 0) && (j == (columns-1)) )
	{
		if ( grid[rows-1][columns-2] ) neighborsNum++;
		if ( grid[rows-1][j] ) neighborsNum++;
		if ( grid[rows-1][0] ) neighborsNum++;
		if ( grid[0][columns-2] ) neighborsNum++;
		if ( grid[0][0] ) neighborsNum++;
		if ( grid[1][columns-2] ) neighborsNum++;
		if ( grid[1][columns-1] ) neighborsNum++;
		if ( grid[1][0] ) neighborsNum++;
	}
	else if ( (i == (rows-1)) && (j == (columns-1)) )
	{
		if ( grid[rows-2][columns-2] ) neighborsNum++;
		if ( grid[rows-2][j] ) neighborsNum++;
		if ( grid[rows-2][0] ) neighborsNum++;
		if ( grid[rows-1][columns-2] ) neighborsNum++;
		if ( grid[rows-1][0] ) neighborsNum++;
		if ( grid[0][columns-2] ) neighborsNum++;
		if ( grid[0][columns-1] ) neighborsNum++;
		if ( grid[0][0] ) neighborsNum++;
	}
	else if ( (i == (rows-1)) && (j == 0) )
	{
		if ( grid[rows-2][columns-1] ) neighborsNum++;
 		if ( grid[rows-2][0] ) neighborsNum++;
 		if ( grid[rows-2][1] ) neighborsNum++;
 		if ( grid[rows-1][columns-1] ) neighborsNum++;
 		if ( grid[rows-1][1] ) neighborsNum++;
 		if ( grid[0][columns-1] ) neighborsNum++;
 		if ( grid[0][0] ) neighborsNum++;
 		if ( grid[0][1] ) neighborsNum++;
	}

	return neighborsNum;
}

void next_generation( bool ***grid, int rows, int columns)
{
	bool **old_grid;
	old_grid = malloc(rows * sizeof(bool*));
	if (old_grid == NULL) { printf("Error with malloc()\n"); exit(1); }
	int i,j;
	for (i=0; i < rows; i++)
	{
		old_grid[i] = malloc(columns * sizeof(bool));
		if ( old_grid[i] == NULL) { printf("Error with malloc()\n"); exit(1); }
	}

	for ( i=0; i < rows; i++)
		for ( j=0; j < columns; j++)
			old_grid[i][j] = (*grid)[i][j];

	for ( i=0; i < rows; i++)
	{
		for ( j=0; j < columns; j++)
		{
			int neighborsNum = findNeighbors(old_grid,i,j,rows,columns);

			if (old_grid[i][j] == false)
			{
				if (neighborsNum == 3)
					(*grid)[i][j] = true;
				else
					(*grid)[i][j] = false;
			}
			else
			{
				if (neighborsNum == 0 || neighborsNum == 1)
					(*grid)[i][j] = false;
				else if (neighborsNum == 2 || neighborsNum == 3)
					(*grid)[i][j] = true;
				else
					(*grid)[i][j] = false;
			}
		}
	}

	for (i=0; i < rows; i++)
		free(old_grid[i]);
	free(old_grid);
}

void game_of_life( int rows, int columns)
{
	// create grid
	bool **grid;
	create_grid(&grid, rows, columns);

	// game of life
	int i;
	for (i=0; i < GENERATIONS; i++)
		next_generation( &grid, rows, columns);

	// free memory
	for ( i=0; i < rows; i++)
		free(grid[i]);
	free(grid);

	return;
}

int main( int argc, char *argv[])
{
	srand(time(NULL));

	int rows,columns;
	command_line_check( argc, argv, &rows, &columns);

	double start = get_time(NULL);
	game_of_life( rows, columns);
	double end = get_time(NULL);

	printf("Program executed in -> %e\n",end-start);

	return 0;
}
