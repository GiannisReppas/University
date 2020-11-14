#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdbool.h>
#include <semaphore.h>
#include <fcntl.h>

#include "itoa_plus_nod.h"
#include "types.h"

bool command_line_check(); // checks if the user gave correct arguements at coomand line
void take_arguements_from_command_line();

int main( int argc, char *argv[])
{
	/* we check if the user gave correct arguements at command line */
	if ( !command_line_check(argc, argv) ) return 1;

	/* we take the info from the command line */
	time_t time;
	time_t stattimes;
	int id;
	take_arguements_from_command_line( argc, argv, &time, &stattimes, &id);

	/* Attach memory segment */
	char *mem;
	mem = (char *)shmat( id, (void *) 0, 0);
	if ( *(int *) mem == -1) perror("Attachment.");

	/* we get the first two data: monitor time + number of ship places */
	// *monitorTime --> monitor time
	int *monitorTime;
	monitorTime = (int *) mem;

	// *numberOfShipPlaces --> number of ship places
	mem += sizeof(int);
	int *numberOfShipPlaces;
	numberOfShipPlaces = (int *) mem;

	// back to start
	mem -= sizeof(int);

	Vessel *shipPlaces; Vessel *tempArray;
	int *totalMoneyPortCollected; float *averageMoneyPortCollected;
	int *totalWaitTimeS, *totalWaitTimeM, *totalWaitTimeL;
	float *averageWaitTimeS, *averageWaitTimeM, *averageWaitTimeL;

	int j,counter;
	while ( *monitorTime <= TIME_PORT_FUNCTIONS )
	{
		if ( ( *monitorTime == 0) || ( *monitorTime % time == 0) || ( *monitorTime % stattimes == 0) )
		{
			// monitor time
			monitorTime = (int *) mem;

			// number of ship places
			mem += sizeof(int);

			// shipPlaces --> array of ship places , tempArray --> array of ship places that have ships
			mem += sizeof(int);
			tempArray = NULL;
			counter = 0;
			shipPlaces = (Vessel *) mem;
			for ( j = 0 ; j<*numberOfShipPlaces ; j++)
			{
				if ( shipPlaces[j].status )
				{
					if ( counter == 0) tempArray = malloc(sizeof(Vessel));
					else tempArray = realloc( tempArray, sizeof(Vessel) + counter*sizeof(Vessel) );
					if ( tempArray == NULL) { printf("Error with malloc\n"); exit(1); }
					tempArray[counter] = shipPlaces[j];
					counter++;
				}
			}
			if ( ( *monitorTime == 0) || ( *monitorTime % time == 0) )
			{
				printf("\nFor monitor time --> %d\n",*monitorTime);
				printf("In use %d out of %d ship places:\n",counter,*numberOfShipPlaces);
				for ( j=0 ; j<counter; j++)
				{
					printf("Ship %d:\n",j+1);
					printf("Id of ship --> %d\t",tempArray[j].vesselId);
					printf("Moment ship starts waiting to enter the port--> %ld\t",tempArray[j].startWaitTime);
					printf("Moment ship enters the port --> %ld\t",tempArray[j].arriveTime);
					printf("Place type of ship --> %s\t",tempArray[j].placeType);
					printf("Type of ship --> %s\t",tempArray[j].shipType);
					printf("Cost of place of ship --> %d\n",tempArray[j].cost);
				}
			}
			if ( tempArray != NULL) free(tempArray);

			// *totalMoneyPortCollected --> total money port collected
			for (j=0; j<*numberOfShipPlaces ; j++) mem += sizeof(Vessel);
			totalMoneyPortCollected = (int *) mem;

			// *averageMoneyPortCollected --> average money that port collected
			mem += sizeof(int);
			averageMoneyPortCollected = (float *) mem;

			// *totalWaitTimeS, *totalWaitTimeM, *totalWaitTimeL --> total wait time for each ship category
			mem += sizeof(float);
			totalWaitTimeS = (int *) mem;

			mem += sizeof(int);
			totalWaitTimeM = (int *) mem;

			mem += sizeof(int);
			totalWaitTimeL = (int *) mem;

			// *averageWaitTimeS, *averageWaitTimeM, *averageWaitTimeL --> average wait time for each ship category
			mem += sizeof(int);
			averageWaitTimeS = (float *) mem;

			mem += sizeof(float);
			averageWaitTimeM = (float *) mem;

			mem += sizeof(float);
			averageWaitTimeL = (float *) mem;

			if ( ( *monitorTime == 0) || ( *monitorTime % stattimes == 0) )
			{
				printf("\nFor port time --> %d\n",*monitorTime);
				printf("Total money that port collected --> %d\n",*totalMoneyPortCollected);
				printf("Average money that port collects from each ship --> %f\n",*averageMoneyPortCollected);
				printf("Total wait_time for small ships --> %d\n",*totalWaitTimeS);
				printf("Total wait_time for medium ships --> %d\n",*totalWaitTimeM);
				printf("Total wait_time for large ships --> %d\n",*totalWaitTimeL);
				printf("Average wait_time for small ships --> %f\n",*averageWaitTimeS);
				printf("Average wait_time for medium ships --> %f\n",*averageWaitTimeM);
				printf("Average wait time for large ships --> %f\n",*averageWaitTimeL);
				printf("\n");
			}

			// back to start
			mem -= 2*sizeof(float);
			mem -= 3*sizeof(int);
			mem -= sizeof(float);
			mem -= sizeof(int);
			for (j=0; j<*numberOfShipPlaces ; j++) mem -= sizeof(Vessel);
			mem -= sizeof(int);
			mem -= sizeof(int);
		}
		sleep(1);
		*monitorTime += 1;
	}

	int i = 0;
	// take port-master id
	mem += sizeof(int);
	mem += sizeof(int);
	for (j=0; j<*numberOfShipPlaces ; j++) mem += sizeof(Vessel);
	mem += sizeof(int);
	mem += sizeof(float);
	mem += 3*sizeof(int);
	mem += 3*sizeof(float);
	while ( *(char *) mem != '\0')
	{
		i++;
		mem += sizeof(char);
	}
	mem += sizeof(char);
	mem += sizeof(int);

	pid_t pid = *( int *) mem;

	// back to start
	mem -= sizeof(int);
	mem -= sizeof(char);
	while ( i != 0)
	{
		i--;
		mem -= sizeof(char);
	}
	mem -= 3*sizeof(float);
	mem -= 3*sizeof(int);
	mem -= sizeof(float);
	mem -= sizeof(int);
	for (j=0; j<*numberOfShipPlaces ; j++) mem -= sizeof(Vessel);
	mem -= sizeof(int);
	mem -= sizeof(int);

	/* Wait for port-master */
	waitpid( pid, NULL, 0);

	/* Detach segment */
	int err;
	err  = shmdt((void *) mem);
	if ( err==-1) perror("Detachment.");

	/* End of monitor */
	puts("\n\n\nEnd of monitor\n\n\n");
	return 0;
}

void take_arguements_from_command_line( int argc, char **argv, time_t *time, time_t *stattimes, int *id)
{
	int i;
	for ( i=0; i<argc ; i++)
	{
		if ( !strcmp(argv[i],"-d") )
		{
			*time = atoi(argv[i+1]);
			if ( *time <= 0) { printf("At monitor: TIME can't be <= 0, converted to 5\n"); *time = 5;}
		}
		if ( !strcmp(argv[i],"-t") )
		{
			*stattimes = atoi(argv[i+1]);
			if ( *stattimes <= 0) { printf("At monitor: STATTIMES can't be <= 0, converted to 15\n"); *stattimes = 15; }
		}
		if ( !strcmp(argv[i],"-s") ) *id = atoi(argv[i+1]);
	}
}

bool command_line_check(int argc, char **argv)
{
	if ( argc != 7) { printf("At monitor: Wrong number of arguements given at command line\n"); return false; }

	if (  (!strcmp(argv[1],"-d"))  ||  (!strcmp(argv[1],"-t"))  ||  (!strcmp(argv[1],"-s"))  )
	{
		if (  ((!strcmp(argv[3],"-d"))  ||  (!strcmp(argv[3],"-t"))  ||  (!strcmp(argv[3],"-s")))  &&  (strcmp(argv[3],argv[1]))  )
		{
			if (  ((!strcmp(argv[5],"-d"))  ||  (!strcmp(argv[5],"-t"))  ||  (!strcmp(argv[5],"-s")))  &&  (strcmp(argv[5],argv[1]))  &&  (strcmp(argv[5],argv[3]))  ) ;
			else { printf("At monitor: The 6th arguement at command line is wrong\n"); return false; }
		}
		else { printf("At monitor: The 4th arguement at command line is wrong\n"); return false; }
	}
	else { printf("At monitor: The 2nd arguement at command line is wrong\n"); return false; }

	return true;
}
