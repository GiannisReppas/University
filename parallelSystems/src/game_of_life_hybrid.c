#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpi.h"

// rank of master process
#define MASTER    0

// used of defining neighbours in neighbours array
#define UP        0
#define DOWN      1
#define LEFT      2
#define RIGHT     3
#define TOPLEFT   4
#define TOPRIGHT  5
#define DOWNLEFT  6
#define DOWNRIGHT 7

inline int readCommandLine(int, char **argv);												// reads the command the user gave from command line and stores the values into glocal variables
inline void initializeProcessGridDimensions(int *, int *, int *, int **, int **);			// puts in two arrays the dimensions of each process grid (array index means proces rank)
inline void findNeighbours( int *, MPI_Comm, int, int, int *);								// finds the ranks of neighbours of the current process
inline int *initializeGridAndSubgrids( int **, int **, int **, int, int, int, int, int, int, MPI_Comm, MPI_Datatype, MPI_Datatype); // creates and assigns values to main grid U and process subgrid u1
inline void updateInternal( int, int, int *, int *);						// updates the subgrid u1 and prints the results into subgrid u2 (except the elements of the perimeter of u1)
inline void updatePerimeter( int, int, int *, int *);						// updates the subgrid u1 and prints the results into subgrid u2 (updates only the elements of the perimeter u1)
inline int checkGridEquality(int, int, int *, int *);						// checks if subgrid u1 is exactly the same with subgrid u2 and returns 0 or 1
inline void printInFile(int, int, int *, int);								// prints the grid we pass as arguement into results.txt file

int GRID_ROWS;				// number of rows of grid
int GRID_COLUMNS;			// number of columns of grid
int PRINT_EACH_GENERATION;	// enable printing of grid in each generation
int REDUCE_ENABLED;			// enable reduce
int REDUCE;					// reduce will run after REDUCE generations
int GENERATIONS;			// number of generations the program will run (if REDUCE is disabled) 

int main(int argc, char *argv[])
{
	// the starting grid will contain random values
	//srand(time(NULL));

	// read the input the user game from the command line
	if (readCommandLine( argc, argv) == 0)
		return 1;

	// clear the results.txt file
	if (PRINT_EACH_GENERATION == 1)
	{
		FILE *rFile = fopen("results.txt", "w");
		fclose(rFile);
	}

	/*-------------------------------------------------------- FIRST OF ALL, WE CREATE OUR MPI TOPOLOGY --------------------------------------------------------*/

	// read rank of current process and number of all processes
	int processesNum,processRank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &processesNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &processRank);

	// create the "process grid" and our communication group
	MPI_Comm myComm;
	int dims[2], periods[2];
	dims[0] = dims[1] = 0;
	periods[0] = periods[1] = 1;
	MPI_Dims_create(processesNum , 2, dims);
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &myComm);

	// these two arrays store the length of the 2 dimensions for each subgrid (each process has one and only subgrid)
	int *allProcessesRows; int *allProcessesColumns;
	initializeProcessGridDimensions(dims, &processesNum, &processRank, &allProcessesRows, &allProcessesColumns);

	// pass from the master proces, the length of the 2 dimensions of each subgrid to its process
	// the lengths are being stored into variables subgridRows and subgridColumns
	int subgridRows, subgridColumns;
	MPI_Scatter(allProcessesRows,1,MPI_INT,&subgridRows,1,MPI_INT,0,myComm);
	MPI_Scatter(allProcessesColumns,1,MPI_INT,&subgridColumns,1,MPI_INT,0,myComm);

	// we dont need the 2 arrays anymore, free them from master process
	if (processRank == MASTER)
	{
		free(allProcessesRows);
		free(allProcessesColumns);
	}

	// remember that each subgrid contains also another perimeter with the information of the elements of neighbour processes 
	int extendedSubgridRows = subgridRows + 2;
	int extendedSubgridColumns = subgridColumns + 2;

	// Find all 8 neighbours for each process
	int neighbours[8];
	findNeighbours( neighbours, myComm, processRank, processesNum, dims);

	/*----------------------------------------------------- WE CREATE 4 DATATYPES FOR PROCESS COMMUNICATION -----------------------------------------------------*/

	// masterToWorker and workerToMaster are being used to initialize the subgrid of each process
	// columnDatatype and rowDatatype are being used to pass information for the game of life algorithm
	MPI_Datatype masterToWorker, workerToMaster, columnDatatype, rowDatatype;

	// Datatype used by master to send and receive blocks to/from worker processes
	MPI_Type_vector(subgridRows, subgridColumns, GRID_COLUMNS, MPI_INT, &masterToWorker);
	MPI_Type_commit(&masterToWorker);

	// Datatype used by workers to send and receive blocks to/from master process
	MPI_Type_vector(subgridRows, subgridColumns, extendedSubgridColumns, MPI_INT, &workerToMaster);
	MPI_Type_commit(&workerToMaster);

	//datatype to send one column
	MPI_Type_vector(subgridRows, 1, extendedSubgridColumns, MPI_INT, &columnDatatype);
	MPI_Type_commit(&columnDatatype);

	//datatype to send one row
	MPI_Type_vector(1, subgridColumns, extendedSubgridColumns, MPI_INT, &rowDatatype);
	MPI_Type_commit(&rowDatatype);

	/*--------------------------------------------------- CREATE STARTING GRID AND SUBGRID FOR EACH PROCESS ---------------------------------------------------*/

	// allocate memory for u1, u2
	int *u1, *u2, *U;
	int *offsets = initializeGridAndSubgrids( &u1, &u2, &U, extendedSubgridRows, extendedSubgridColumns, processRank, processesNum, subgridRows, subgridColumns, myComm, workerToMaster, masterToWorker);

	/*------------------------------- CREATE SEND AND RECEIVE COMMANDS FOR PROCESS COMMUNICATION (PASSING AND RECEIVEING PERIMETERS) -------------------------------*/

	MPI_Request recvRequests[2][8], sendRequests[2][8];

	// for u1
	MPI_Send_init(u1 + (extendedSubgridRows - 2) * extendedSubgridColumns + 1,	1,		rowDatatype,		neighbours[UP],			0,		myComm,		&sendRequests[0][0]);
	MPI_Send_init(u1 + extendedSubgridColumns + 1,								1,		rowDatatype,		neighbours[DOWN],		0,		myComm,		&sendRequests[0][1]);
	MPI_Send_init(u1 + extendedSubgridColumns + 1,								1,		columnDatatype,		neighbours[RIGHT],		0,		myComm,		&sendRequests[0][2]);
	MPI_Send_init(u1 + 2 * extendedSubgridColumns - 2,							1,		columnDatatype, 	neighbours[LEFT],		0,		myComm,		&sendRequests[0][3]);
	MPI_Send_init(u1 + extendedSubgridColumns + 1,								1,		MPI_INT,			neighbours[TOPLEFT],	0,		myComm,		&sendRequests[0][4]);
	MPI_Send_init(u1 + extendedSubgridColumns * 2 - 2,							1,		MPI_INT,			neighbours[TOPRIGHT],	0,		myComm,		&sendRequests[0][5]);
	MPI_Send_init(u1 + (extendedSubgridRows - 2)*extendedSubgridColumns + 1,	1,		MPI_INT,			neighbours[DOWNLEFT],	0,		myComm,		&sendRequests[0][6]);
	MPI_Send_init(u1 + (extendedSubgridRows - 1)*extendedSubgridColumns - 2,	1,		MPI_INT,			neighbours[DOWNRIGHT],	0,		myComm,		&sendRequests[0][7]);

	MPI_Recv_init(u1 + 1,														1,		rowDatatype,		neighbours[UP],			0,		myComm,		&recvRequests[0][0]);
	MPI_Recv_init(u1 + (extendedSubgridRows - 1) * extendedSubgridColumns + 1,	1,		rowDatatype,		neighbours[DOWN],		0,		myComm,		&recvRequests[0][1]);
	MPI_Recv_init(u1 + 2 * extendedSubgridColumns - 1,							1,		columnDatatype,		neighbours[RIGHT],		0,		myComm,		&recvRequests[0][2]);
	MPI_Recv_init(u1 + extendedSubgridColumns,									1,		columnDatatype,		neighbours[LEFT],		0,		myComm,		&recvRequests[0][3]);
	MPI_Recv_init(u1 + extendedSubgridColumns*extendedSubgridRows - 1,			1,		MPI_INT,			neighbours[DOWNRIGHT],	0,		myComm,		&recvRequests[0][4]);
	MPI_Recv_init(u1 + (extendedSubgridRows - 1)*extendedSubgridColumns,		1,		MPI_INT,			neighbours[DOWNLEFT],	0,		myComm,		&recvRequests[0][5]);
	MPI_Recv_init(u1 + extendedSubgridColumns - 1,								1,		MPI_INT,			neighbours[TOPRIGHT],	0,		myComm,		&recvRequests[0][6]);
	MPI_Recv_init(u1,															1,		MPI_INT,			neighbours[TOPLEFT],	0,		myComm,		&recvRequests[0][7]);

	// for u2
	MPI_Send_init(u2 + (extendedSubgridRows - 2) * extendedSubgridColumns + 1,	1,		rowDatatype,		neighbours[UP],			0,		myComm,		&sendRequests[1][0]);
	MPI_Send_init(u2 + extendedSubgridColumns + 1,								1,		rowDatatype,		neighbours[DOWN],		0,		myComm,		&sendRequests[1][1]);
	MPI_Send_init(u2 + extendedSubgridColumns + 1,								1,		columnDatatype,		neighbours[RIGHT],		0,		myComm,		&sendRequests[1][2]);
	MPI_Send_init(u2 + 2 * extendedSubgridColumns - 2,							1,		columnDatatype, 	neighbours[LEFT],		0,		myComm,		&sendRequests[1][3]);
	MPI_Send_init(u2 + extendedSubgridColumns + 1,								1,		MPI_INT,			neighbours[TOPLEFT],	0,		myComm,		&sendRequests[1][4]);
	MPI_Send_init(u2 + extendedSubgridColumns * 2 - 2,							1,		MPI_INT,			neighbours[TOPRIGHT],	0,		myComm,		&sendRequests[1][5]);
	MPI_Send_init(u2 + (extendedSubgridRows - 2)*extendedSubgridColumns + 1,	1,		MPI_INT,			neighbours[DOWNLEFT],	0,		myComm,		&sendRequests[1][6]);
	MPI_Send_init(u2 + (extendedSubgridRows - 1)*extendedSubgridColumns - 2,	1,		MPI_INT,			neighbours[DOWNRIGHT],	0,		myComm,		&sendRequests[1][7]);

	MPI_Recv_init(u2 + 1,														1,		rowDatatype,		neighbours[UP],			0,		myComm,		&recvRequests[1][0]);
	MPI_Recv_init(u2 + (extendedSubgridRows - 1) * extendedSubgridColumns + 1,	1,		rowDatatype,		neighbours[DOWN],		0,		myComm,		&recvRequests[1][1]);
	MPI_Recv_init(u2 + 2 * extendedSubgridColumns - 1,							1,		columnDatatype,		neighbours[RIGHT],		0,		myComm,		&recvRequests[1][2]);
	MPI_Recv_init(u2 + extendedSubgridColumns,									1,		columnDatatype,		neighbours[LEFT],		0,		myComm,		&recvRequests[1][3]);
	MPI_Recv_init(u2 + extendedSubgridColumns*extendedSubgridRows - 1,			1,		MPI_INT,			neighbours[DOWNRIGHT],	0,		myComm,		&recvRequests[1][4]);
	MPI_Recv_init(u2 + (extendedSubgridRows - 1)*extendedSubgridColumns,		1,		MPI_INT,			neighbours[DOWNLEFT],	0,		myComm,		&recvRequests[1][5]);
	MPI_Recv_init(u2 + extendedSubgridColumns - 1,								1,		MPI_INT,			neighbours[TOPRIGHT],	0,		myComm,		&recvRequests[1][6]);
	MPI_Recv_init(u2,															1,		MPI_INT,			neighbours[TOPLEFT],	0,		myComm,		&recvRequests[1][7]);

	/*---------------------------------------------------------------------- GAME OF LIFE ----------------------------------------------------------------------*/

	// All processes start here
	MPI_Barrier(myComm);

	//if (processRank == MASTER)
	double start, finish, elapsed, wanted;
	start = MPI_Wtime();

	int currentGeneration;	// used as index in game of life basic loop below
	int *temp;				// used for swaping u1 and u2 at the end of a loop
	int reduce,reduction;	// used as flags for the allReduce part
	int swap;				// used for swaping u1 and u2
	int offsetX, offsetY;

	// for GENERATION generations
	swap = 0;
	offsetX = offsets[0]; offsetY = offsets[1];

	#pragma omp parallel
	for (currentGeneration = 1; currentGeneration <= GENERATIONS; currentGeneration++)
	{
		// print current generation at the file, if the user wants to
		if (PRINT_EACH_GENERATION == 1)
		{
			if (processRank != MASTER)
			{
				MPI_Send(&offsetX, 1, MPI_INT, MASTER, 0, myComm);
				MPI_Send(&offsetY, 1, MPI_INT, MASTER, 0, myComm);
				MPI_Send(u1 + extendedSubgridColumns + 1, 1, workerToMaster, MASTER, 0, myComm);
				//offsetX = offsetY = 0;
			}
			else
			{
				// Now wait for results from all worker tasks
				for (int i = 1; i < processesNum; i++)
				{
					MPI_Recv(&offsetX, 1, MPI_INT, i, 0, myComm, MPI_STATUS_IGNORE);
					MPI_Recv(&offsetY, 1, MPI_INT, i, 0, myComm, MPI_STATUS_IGNORE);
					MPI_Recv(U + offsetY * GRID_COLUMNS + offsetX, 1, masterToWorker, i, 0, myComm, MPI_STATUS_IGNORE);
				}

				// Copy master's subgrid to U array
				for (int y = 1; y < extendedSubgridRows-1; y++)
					for (int x = 1; x < extendedSubgridColumns-1; x++)
						*(U + (y-1) * GRID_COLUMNS + (x-1)) = *(u1 + y * extendedSubgridRows + x);

				printInFile(GRID_COLUMNS, GRID_ROWS, U, currentGeneration);
			}
		}

		MPI_Startall(8, &recvRequests[swap][0]);							// Receive perimeter
		updateInternal(extendedSubgridColumns, extendedSubgridRows, u1, u2);	// Update the internal of the subgrid
		MPI_Startall(8, &sendRequests[swap][0]);							// Send perimeter
		MPI_Waitall(8, &recvRequests[swap][0], MPI_STATUSES_IGNORE);		// Wait for all processes to receive perimeter
		updatePerimeter(extendedSubgridColumns, extendedSubgridRows, u1, u2);		// Update the perimeter since you have received
		MPI_Waitall(8, &sendRequests[swap][0], MPI_STATUSES_IGNORE);		// Wait for all processes to send perimeter

		////////////////////////////////////////////////////////////////////////
		////////////////////////// allReduce part //////////////////////////////
		if(currentGeneration % REDUCE == 0 && REDUCE_ENABLED == 1)
		{
			reduce = checkGridEquality(extendedSubgridColumns, extendedSubgridRows, u1, u2);	// check if last 2 generations are equal
			MPI_Allreduce(&reduce, &reduction, 1, MPI_INT, MPI_LOR, myComm);		// get the number of processes that have have different generations

			// if no process has differences between generations in their subgrids, stop game of life
			if(reduction == 0)
				REDUCE_ENABLED = 0;
		}
		////////////////////////////////////////////////////////////////////////

		// swap arrays u1 and u2 (make the new array the starting one for the next generation)
		temp = u1;
		u1 = u2;
		u2 = temp;
		swap = 1 -swap;
	}

	finish = MPI_Wtime();
	elapsed = finish - start;
	MPI_Reduce(&elapsed, &wanted, 1, MPI_DOUBLE, MPI_MAX, 0, myComm);

	// print the last generation (last result), if the user wants to
	if (PRINT_EACH_GENERATION == 1)
	{
		if (processRank != MASTER)
		{
			// Send each process portion of final subgrid results back to master
			MPI_Send(&offsetX, 1, MPI_INT, MASTER, 0, myComm);
			MPI_Send(&offsetY, 1, MPI_INT, MASTER, 0, myComm);
			MPI_Send(u1 + extendedSubgridColumns + 1, 1, workerToMaster, MASTER, 0, myComm);
		}
		else
		{
			// Now wait for results from all worker tasks
			for (int i = 1; i < processesNum; i++)
			{
				MPI_Recv(&offsetX, 1, MPI_INT, i, 0, myComm, MPI_STATUS_IGNORE);
				MPI_Recv(&offsetY, 1, MPI_INT, i, 0, myComm, MPI_STATUS_IGNORE);
				MPI_Recv(U + offsetY * GRID_COLUMNS + offsetX, 1, masterToWorker, i, 0, myComm, MPI_STATUS_IGNORE);
			}

			// Copy master's subgrid to U array
			for (int i = 1; i < extendedSubgridRows-1; i++)
				for (int j = 1; j < extendedSubgridColumns-1; j++)
					*(U + (i-1) * GRID_COLUMNS + (j-1)) = *(u1 + i * extendedSubgridRows + j);

			printInFile(GRID_COLUMNS, GRID_ROWS, U, -1);
		}
	}

	// end of programm
	if (processRank == MASTER)
	{
		printf("\nProgram executed in %f seconds\n\n", wanted);
		free(U);
	}

	free(u1);
	free(u2);
	MPI_Finalize();

	return 0;
}

int readCommandLine(int argc, char *argv[])
{
	if (argc != 13)
	{

		printf("Wrong number arguements at command line\n");
		return 0;
	}

	int nFlag,mFlag,pegFlag,reFlag,rFlag,gFlag,oFlag;
	nFlag = mFlag = pegFlag = reFlag = rFlag = gFlag = oFlag = 0;
	for (int i=0; i < (argc-1); i++)
	{
		if ( strcmp(argv[i],"-n") == 0)
		{
			if (nFlag == 1) {
				nFlag = 0;
				break;
			}
			nFlag = 1;
			GRID_ROWS = atoi(argv[i+1]);
		}
		else if ( strcmp(argv[i],"-m") == 0)
		{
			if (mFlag == 1) {
				mFlag = 0;
				break;
			}
			mFlag = 1;
			GRID_COLUMNS = atoi(argv[i+1]);
		}
		else if ( strcmp(argv[i],"-peg") == 0)
		{
			if (pegFlag == 1) {
				pegFlag = 0;
				break;
			}
			pegFlag = 1;
			if ( atoi(argv[i+1]) == 0)
				PRINT_EACH_GENERATION = 0;
			else
				PRINT_EACH_GENERATION = 1;
		}
		else if ( strcmp(argv[i],"-re") == 0)
		{
			if (reFlag == 1){
				reFlag = 0;
				break;
			}
			reFlag = 1;
			if ( atoi(argv[i+1]) == 0)
				REDUCE_ENABLED = 0;
			else
				REDUCE_ENABLED = 1;
		}
		else if ( strcmp(argv[i],"-r") == 0)
		{
			if (rFlag == 1){
				rFlag = 0;
				break;
			}
			rFlag = 1;
			REDUCE = atoi(argv[i+1]);
		}
		else if ( strcmp(argv[i],"-g") == 0)
		{
			if (gFlag == 1){
				gFlag = 0;
				break;
			}
			gFlag = 1;
			GENERATIONS = atoi(argv[i+1]);
		}
	}

	if ( (nFlag==1) && (mFlag==1) && (pegFlag==1) && (reFlag==1) && (rFlag==1) && (gFlag==1) )
		return 1;
	else
	{
		printf("Wrong arguements at command line\n");
		return 0;
	}
}

void initializeProcessGridDimensions(int dims[2], int *processesNum, int *processRank, int **allProcessesRows, int **allProcessesColumns)
{
	if ( (*processRank) == MASTER)
	{
		*allProcessesRows = (int *) malloc( (*processesNum) * sizeof(int));
		*allProcessesColumns = (int *) malloc( (*processesNum) * sizeof(int));

		for (int i=0; i < (*processesNum); i++) {
			(*allProcessesRows)[i] = GRID_ROWS / dims[0];
			(*allProcessesColumns)[i] = GRID_COLUMNS / dims[1];
		}

		for (int i=0; i < (GRID_ROWS % dims[0]); i++)
			for (int j=0; j < dims[1]; j++)
				(*allProcessesRows)[i*dims[1]+j]++;

		for (int i=0; i < (GRID_COLUMNS % dims[1]); i++)
			for (int j=0; j < dims[0]; j++)
				(*allProcessesColumns)[i+dims[0]*j]++;
	}

	return;
}

void findNeighbours( int neighbours[8], MPI_Comm myComm, int processRank, int processesNum, int dims[2])
{
	// we find up, down, left and right neighbours easily with MPI_Cart_shift
	MPI_Cart_shift(myComm, 0, 1, &neighbours[UP], &neighbours[DOWN]);
	MPI_Cart_shift(myComm, 1, 1, &neighbours[LEFT], &neighbours[RIGHT]);

	// for topleft, topright, downleft and downright neighbours, we use our own algorithm, taking advantage of the way MPI_Dims_create works
	if (processRank == 0) neighbours[TOPLEFT] = processesNum - 1; // topleft of process grid
	else if (processRank < dims[1]) neighbours[TOPLEFT] = (dims[0]-1)*dims[1] + processRank - 1; // first row of process grid
	else if ( (processRank == 0) || ((processRank) % dims[1] == 0) ) neighbours[TOPLEFT] = processRank - 1; // first column of process grid
	else neighbours[TOPLEFT] = processRank - dims[1] - 1; // rest

	if (processRank == (dims[1] - 1)) neighbours[TOPRIGHT] = (dims[0]-1) * dims[1]; // topright element of process grid
	else if (processRank < dims[1]) neighbours[TOPRIGHT] = (dims[0]-1)*dims[1] + processRank + 1; // first row of proces grid
	else if ((processRank+1) % dims[1] == 0) neighbours[TOPRIGHT] = processRank - dims[1]*2 + 1; // last column of process grid
	else neighbours[TOPRIGHT] = processRank - dims[1] + 1; // rest

	if (processRank == ((dims[0]-1)*dims[1])) neighbours[DOWNLEFT] = dims[1] - 1; // downleft element of process grid
	else if (processRank > ((dims[0]-1)*dims[1] - 1)) neighbours[DOWNLEFT] = processRank - (dims[0]-1)*dims[1] - 1; // last row of process grid
	else if ( (processRank == 0) || ((processRank) % dims[1] == 0) ) neighbours[DOWNLEFT] = processRank + dims[1]*2 - 1; // first column of full grid
	else neighbours[DOWNLEFT] = processRank + dims[1] - 1; // rest

	if (processRank == (processesNum-1)) neighbours[DOWNRIGHT] = 0; // downright element of process grid
	else if (processRank > ((dims[0]-1)*dims[1] - 1)) neighbours[DOWNRIGHT] = processRank - (dims[0]-1)*dims[1] + 1; // last row of process grid
	else if ((processRank+1) % dims[1] == 0) neighbours[DOWNRIGHT] = processRank + 1; // last column of process grid
	else neighbours[DOWNRIGHT] = processRank + dims[1] + 1; // rest
}

int *initializeGridAndSubgrids( int **u1, int **u2, int **U, int extendedSubgridRows, int extendedSubgridColumns, int processRank, int processesNum , int subgridRows, int subgridColumns, MPI_Comm myComm, MPI_Datatype workerToMaster, MPI_Datatype masterToWorker)
{
	*u1 = malloc(extendedSubgridColumns * extendedSubgridRows * sizeof (int));
	*u2 = malloc(extendedSubgridColumns * extendedSubgridRows * sizeof (int));

	int offsetX, offsetY; // to pass information from one process to another, we use U[offsetX][offsetY] in a loop and we aso pass the starting offsets for later printing (if needed)
	if (processRank == MASTER)
	{
		// create grid
		*U = malloc(sizeof (int) * GRID_COLUMNS * GRID_ROWS);
		for (int i = 0; i <= GRID_ROWS - 1; i++)
			for (int j = 0; j <= GRID_COLUMNS - 1; j++)
				*( (*U) + i * GRID_COLUMNS + j) = rand() % 2;

		// master process will be a worker too, so we create its subgrid
		for (int i = 1; i < (extendedSubgridRows-1) ; i++)
			for (int j = 1; j < (extendedSubgridColumns-1) ; j++)
				*( (*u1) + i * extendedSubgridColumns + j) = *( (*U) + (i-1) * GRID_COLUMNS + (j-1));

		// send grid info to rest processes
		offsetX = offsetY = 0;
		for (int i = 1; i < processesNum; i++)
		{
			offsetX += subgridColumns;
			if (offsetX == GRID_COLUMNS ) {
				offsetX = 0;
				offsetY += subgridRows;
			}
			MPI_Send(&offsetX, 1, MPI_INT, i, 0, myComm);
			MPI_Send(&offsetY, 1, MPI_INT, i, 0, myComm);
			MPI_Send( (*U) + offsetY * GRID_COLUMNS + offsetX, 1, masterToWorker, i, 0, myComm);
		}
		offsetX = offsetY = 0;
	}
	else
	{
		// Receive grid info from master process
		MPI_Recv(&offsetX, 1, MPI_INT, MASTER, 0, myComm, MPI_STATUS_IGNORE);
		MPI_Recv(&offsetY, 1, MPI_INT, MASTER, 0, myComm, MPI_STATUS_IGNORE);
		MPI_Recv( (*u1) + extendedSubgridColumns + 1, 1, workerToMaster, MASTER, 0, myComm, MPI_STATUS_IGNORE);
	}

	int *toReturn;
	toReturn = malloc(sizeof(int)*2);
	toReturn[0] = offsetX;
	toReturn[1] = offsetY;

	return toReturn;
}

void updateInternal(int columns, int rows, int *u1, int *u2)
{
	// elements inside the perimeter of a subgrid start from element [2][2]
	// first perimeter contains information of neighbouring subgrids/processes (hasnt been initialized the moment this function gets executed)
	// second perimeter contains elements that needs information from the first perimeter (other processes) in order to be initialized

	int c, offset, element, neighboursCount, x, y, i ,j;

	#pragma omp parallel for schedule(static)
	for (x = 2; x < rows - 2; x++)
	{
		for (y = 2; y < columns - 2; y++)
		{
			// find element in subgrid
			offset = x * columns + y;
			element = *(u1 + offset);

			// count neighbours of element
			neighboursCount = 0;
			for (i = -1; i <= +1; i++)
			{
				for (j = -1; j <= +1; j++)
				{
					if (i == 0 && j == 0)
						continue;

					c = *(u1 + offset + (i * columns) + j);

					if (c == 1)
						neighboursCount++;
                }
            }

			// update element of subgrid
			if (neighboursCount < 2)
				*(u2 + offset) = 0;
			else if ((neighboursCount == 2 || neighboursCount == 3) && element == 1)
				*(u2 + offset) = 1;
			else if (neighboursCount > 3 && element == 1)
				*(u2 + offset) = 0;
			else if (neighboursCount == 3 && element == 0)
				*(u2 + offset) = 1;
			else
				*(u2 + offset) = 0;
		}
	}

	return;
}

void updatePerimeter(int columns, int rows, int *u1, int *u2)
{
	int c, offset, element, neighboursCount, x, y, i, j;

	// updating top row of perimeter
	//#pragma omp parallel for schedule(static)
	for (y = 1, x = 1; y <= columns - 2; y++)
	{
		// find element in subgrid
		offset = x * columns + y;
		element = *(u1 + offset);

		// count neighbours of element
		neighboursCount = 0;
		for (i = -1; i <= +1; i++)
		{
			for (j = -1; j <= +1; j++)
			{
				if (i == 0 && j == 0)
					continue;

				c = *(u1 + offset + (i * columns) + j);

				if (c == 1)
					neighboursCount++;
			}
		}

		// update element of subgrid
		if (neighboursCount < 2)
			*(u2 + offset) = 0;
		else if ((neighboursCount == 2 || neighboursCount == 3) && element == 1)
			*(u2 + offset) = 1;
		else if (neighboursCount > 3 && element == 1)
			*(u2 + offset) = 0;
		else if (neighboursCount == 3 && element == 0)
			*(u2 + offset) = 1;
		else
			*(u2 + offset) = 0;
	}

	// updating bottom row of the perimeter
	//#pragma omp parallel for schedule(static)
	for (y = 1, x = rows-2; y <= columns - 2; y++)
	{
		// find element in subgrid
		offset = x * columns + y;
		element = *(u1 + offset);

		// count neighbours of element
		neighboursCount = 0;
		for (i = -1; i <= +1; i++)
		{
			for (j = -1; j <= +1; j++)
			{
				if (i == 0 && j == 0)
                    continue;

				c = *(u1 + offset + (i * columns) + j);

				if (c == 1)
					neighboursCount++;
			}
		}

		// count neighbours of element
		if (neighboursCount < 2)
			*(u2 + offset) = 0;
		else if ((neighboursCount == 2 || neighboursCount == 3) && element == 1)
			*(u2 + offset) = 1;
		else if (neighboursCount > 3 && element == 1)
			*(u2 + offset) = 0;
		else if (neighboursCount == 3 && element == 0)
			*(u2 + offset) = 1;
		else
			*(u2 + offset) = 0;
	}

	// updating left column of the perimeter
	//#pragma omp parallel for schedule(static)
	for (x = 1, y = 1; x <= rows - 2; x++)
	{
		// find element in subgrid
		offset = x * columns + y;
		element = *(u1 + offset);

		// count neighbours of element
		neighboursCount = 0;
		for (i = -1; i <= +1; i++)
		{
			for (j = -1; j <= +1; j++)
			{
				if (i == 0 && j == 0)
					continue;

				c = *(u1 + offset + (i * columns) + j);

				if (c == 1)
					neighboursCount++;
			}
		}

		// update element of subgrid
		if (neighboursCount < 2)
			*(u2 + offset) = 0;
		else if ((neighboursCount == 2 || neighboursCount == 3) && element == 1)
			*(u2 + offset) = 1;
		else if (neighboursCount > 3 && element == 1)
			*(u2 + offset) = 0;
		else if (neighboursCount == 3 && element == 0)
			*(u2 + offset) = 1;
		else
			*(u2 + offset) = 0;
	}

	// updating right column of the perimeter
	//#pragma omp parallel for schedule(static)
	for (x = 1, y = columns-2; x <= rows - 2; x++)
	{
		// find element in subgrid
		offset = x * columns + y;
		element = *(u1 + offset);

		// count neighbours of element
		neighboursCount = 0;
		for (int i = -1; i <= +1; i++)
		{
			for (int j = -1; j <= +1; j++)
			{
				if (i == 0 && j == 0)
					continue;

				c = *(u1 + offset + (i * columns) + j);

				if (c == 1)
					neighboursCount++;
			}
		}

		// update element of subgrid
		if (neighboursCount < 2)
			*(u2 + offset) = 0;
		else if ((neighboursCount == 2 || neighboursCount == 3) && element == 1)
			*(u2 + offset) = 1;
		else if (neighboursCount > 3 && element == 1)
			*(u2 + offset) = 0;
		else if (neighboursCount == 3 && element == 0)
			*(u2 + offset) = 1;
		else
			*(u2 + offset) = 0;
	}

	return;
}

int checkGridEquality(int columns, int rows, int *u1, int *u2)
{
	for (int i = 1; i < rows - 1; i++)
		for (int j = 1; j < columns - 1; j++)
			if (*(u2 + i * columns + j) - *(u1 + i * columns + j) != 0)
				return 1;
	return 0;
}

void printInFile(int columns, int rows, int *u,int currentGeneration)
{
	FILE *rFile = fopen("results.txt", "a");
	if(currentGeneration!=-1)
		fprintf(rFile,"for generation %d:\n",currentGeneration);
	else
		fprintf(rFile,"Final result ->\n");

	for(int i = 0; i < rows ; i++)
	{
		for(int j = 0; j < columns ; j++)
			fprintf(rFile,"%d ", *(u + i*columns + j));
		fprintf(rFile,"\n");
	}
	fprintf(rFile,"\n\n\n\n");
	fclose(rFile);

	return;
}
