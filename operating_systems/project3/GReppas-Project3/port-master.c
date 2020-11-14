#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>

#include "types.h"
#include "itoa_plus_nod.h"

bool command_line_check();
void get_data_from_command_line();

int main( int argc, char *argv[])
{
	/* We check if the user gave the correct arguements at command line */
	if ( !command_line_check(argc,argv) ) return 1;

	/* We get the info the user gave at command line */
	int id;
	get_data_from_command_line( argv, &id);

	/* Attach memory segment */
	char *mem;
	mem = (char *)shmat( id, (void *) 0, 0);
	if ( *(int *) mem == -1) perror("Attachment.");

	/* We get data from shared memory */
	int i;

	// monitor time
	int *monitorTime;
	monitorTime = (int *) mem;

	// number of  all ship places
	int *numberOfShipPlaces;
	mem += sizeof(int);
	numberOfShipPlaces = (int *) mem;

	// move on
	mem += sizeof(int);
	for (i=0 ; i<*numberOfShipPlaces ; i++) mem += sizeof(Vessel);
	mem += sizeof(int);
	mem += sizeof(float);
	mem += 3*sizeof(int);
	mem += 3*sizeof(float);

	// semaphore
	char *sem_name1;
	int j1=1;
	while ( *mem != '\0' )
	{
		j1++;
		mem += sizeof(char);
	}
	sem_name1 = malloc( sizeof(char)*j1 );
	if ( sem_name1 == NULL) { printf("Error with malloc\n"); exit(1); }
	while ( j1 != 1)
	{
		j1--;
		mem -= sizeof(char);
	}
	while ( *mem != '\0' )
	{
		sem_name1[j1-1] = *mem;
		j1++;
		mem += sizeof(char);
	}
	sem_name1[j1-1] = *mem;
	sem_t *sem1 = sem_open( sem_name1 , O_EXCL); // semaphore name
	free( sem_name1);

	// use_port_value
	mem += sizeof(char);
	int *usePortValue;
	usePortValue = (int *) mem;

	// port-master id
	mem += sizeof(int);
	int *pm_id;
	pm_id = (int *) mem;
	*pm_id = getpid();

	// length of queue
	mem += sizeof(int);
	int *semQueueLength;
	semQueueLength = (int *) mem;

	// v id
	mem += sizeof(int);
	pid_t v_id = *(int *) mem;

	/////////////////////////////
	while ( ( *monitorTime <= TIME_PORT_FUNCTIONS) )
	{
		if ( ( *usePortValue == 0) )
		{
			sem_post( sem1);
			*usePortValue = 1;
		}
	}

	if ( *semQueueLength != 0)
	{
		for ( i=0; i<*semQueueLength; 1)
		{
			sem_post( sem1);
		}
	}
	waitpid( v_id, NULL, 0);
	//////////////////////////////

	// back to start
	mem -= sizeof(int);
	mem -= sizeof(int);
	mem -= sizeof(int);
	mem -= sizeof(char);
	while ( j1 != 1)
	{
		j1--;
		mem -= sizeof(char);
	}
	mem -= 3*sizeof(float);
	mem -= 3*sizeof(int);
	mem -= sizeof(float);
	mem -= sizeof(int);
	for (i=0 ; i<*numberOfShipPlaces ; i++) mem -= sizeof(Vessel);
	mem -= sizeof(int);
	mem -= sizeof(int);

	/* Close the semaphore we opened before */
	sem_close( sem1);

	/* Detach segment */
	int err;
	err  = shmdt((void *) mem);
	if ( err==-1) perror("Detachment.");

	/* End of port-master */
	puts("\n\n\nEnd of port-master\n\n\n");
	return 0;
}

void get_data_from_command_line( char **argv, int *id)
{
	*id = atoi( argv[2]);
}

bool command_line_check( int argc, char *argv[])
{
	if ( argc != 3) { printf("Wrong number of arguements at command line"); return false; }

	if ( !strcmp(argv[1],"-s") ) ;
	else { printf(" Wrong 2nd arguement\n"); return false; }

	return true;
}
