#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>

#include "types.h"
#include "itoa_plus_nod.h"

bool command_line_check(); // checks if the user gave correct arguements at command line

int init_shared_memory(); // initializes the shared memory segment
void add_data_to_string(); // adds data from configfile to given string

void execute_vessel(); // executes vessel
void execute_portmaster(); // executes port-master
void execute_monitor(); // executes monitor
void vessel_generator(); // creates a random vessel

void write_v();

int main(int argc, char *argv[])
{
	/* If myport was executed before, we delete old results */
	pid_t oldResults;
	oldResults = fork();
	if ( oldResults == 0) execlp("rm","rm","-f","results.txt",NULL);

	srand(time(NULL));

	/* We check if the user gave correct arguements at command line */
	if ( !command_line_check(argc,argv) ) return 1;

	/* Create semaphore */
	sem_t *sem1 = sem_open("sem1", O_CREAT|O_EXCL , S_IRUSR | S_IWUSR, 0);
	if ( sem1 == SEM_FAILED) { printf("At myport: Error with sem open %d\n",errno); exit(1); }

	/* Create and write to shared memeory the data of configuration file */
	int id;
	FILE *fp;
	fp = fopen( argv[2],"r");
	if ( fp==NULL) { printf("At myport: Error opening file\n"); return -1; }
	id = init_shared_memory( fp);
	fclose(fp);

	/* Run monitor, port-master, vessels */
	pid_t pid1, pid2, pid3;
	pid1 = fork();
	if ( pid1 == 0) // monitor
	{
		pid2 = fork();
		if ( pid2 == 0) // port-master
		{

			pid3 = fork();
			if ( pid3 == 0) // vessels
			{
				write_v(id); // write the id of the mother of vessels at shared memory
				char *temp_type; char *temp_postype; char *temp_parkperiod; char * temp_mantime;

				pid_t v;
				pid_t arrayOfV[NUMBER_OF_VESSELS];

				int i;
				for ( i=0; i<NUMBER_OF_VESSELS ; i++)
				{
					v = fork();
					if ( v == 0)
					{
						vessel_generator( argv, &temp_type, &temp_postype, &temp_parkperiod, &temp_mantime);
						execute_vessel( temp_type, temp_postype, temp_parkperiod, temp_mantime, id);
					}
					arrayOfV[i] = v;
					sleep(1);
					usleep(5);
				}
				// wait for all vessels
				for ( i=0; i<NUMBER_OF_VESSELS ; i++) waitpid( arrayOfV[i], NULL, 0);

				sem_close( sem1);
				exit(0);
			}
			execute_portmaster( id);

		}

		execute_monitor( id);
	}

	/* When monitor ends then the port closes and the segment is removed */
	waitpid( pid1, NULL, 0);

	/* Remove segment */
	int err;
	err = shmctl( id, IPC_RMID, 0);
	if ( err == -1) { printf("At myport: Error removing the shared memory segment\n"); return 1; }

	/* Delete semaphore */
	sem_close( sem1);
	sem_unlink("sem1");

	printf("\nEnd of myport\n");

	return 0;
}

void write_v( int id)
{
	//attach
	char *mem;
	mem = (char *)shmat( id, (void *) 0, 0);
	if ( *(int *) mem == -1) perror("Attachment.");

	char *temp;
	temp = mem;

	//go to v id
	mem += sizeof(int);
	int nop = *(int *) mem;
	mem += sizeof(int);
	for ( int i; i<nop; i++) mem += sizeof(Vessel);
	mem += sizeof(int);
	mem += sizeof(float);
	mem += 3*sizeof(int);
	mem += 3*sizeof(float);
	mem += 5*sizeof(char);
	mem += 3*sizeof(int);
	int *vid;
	vid = (int *) mem;
	*vid = getpid();

	//back to start
	mem -= 3*sizeof(int);
	mem -= 5*sizeof(char);
	mem -= 3*sizeof(float);
	mem -= 3*sizeof(int);
	mem -= sizeof(float);
	mem -= sizeof(int);
	for ( int i; i<nop; i++) mem -= sizeof(Vessel);
	mem -= sizeof(int);
	mem -= sizeof(int);

	//detach
	int err;
	err  = shmdt((void *) mem);
	if ( err==-1) perror("Detachment.");
}

void vessel_generator( char **argv, char **temp_type, char **temp_postype, char **temp_parkperiod, char **temp_mantime)
{
	// get types again from configfile
	FILE *fp;
	fp = fopen( argv[2], "r");
	char *small;
	char *medium;
	char *large;
	int i = 1;
	int c;
	do // Each loop is a line
	{
		// The last word of the line is the word we need
		// So, we ignore the rest words
		do
		{
			c = getc(fp);
		}while ( c != ' ');

		if ( i == 1) add_data_to_string( &small, &fp);
		else if ( i == 2) add_data_to_string( &medium, &fp);
		else if ( i == 3) add_data_to_string( &large, &fp);

		i++;
	} while( i <= 3);
	fclose( fp);

	// random type ( small, medium, large)
	int ttype = rand() % 3;
	if ( ttype == 0)
	{
		*temp_type = malloc( sizeof(char)*(strlen(small)+1) );
		if ( *temp_type == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
		strcpy( *temp_type , small );
	}
	else if ( ttype == 1)
	{
		*temp_type = malloc( sizeof(char)*(strlen(medium)+1) );
		if ( *temp_type == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
		strcpy( *temp_type , medium );
	}
	else
	{
		*temp_type = malloc( sizeof(char)*(strlen(large)+1) );
		if ( *temp_type == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
		strcpy( *temp_type , large );
	}
	free(small); free(medium); free(large);

	// random wish choice to have other place types
	int tpostype = rand() % 2;
	*temp_postype = malloc( sizeof(char)*2 );
	if ( *temp_postype == NULL ) { printf("At myport: Error with malloc\n"); exit(1); }
	if ( tpostype == 0) strcpy( *temp_postype , "0");
	else strcpy( *temp_postype , "1");

	// random parkperiod
	int tparkperiod = rand() % ( TIME_PORT_FUNCTIONS / 4);
	if ( tparkperiod == 0) tparkperiod++;
	*temp_parkperiod = malloc( sizeof(char)*(number_of_digits(tparkperiod)+1) );
	if ( *temp_parkperiod == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
	integer_to_string( tparkperiod, *temp_parkperiod);

	// random mantime
	int tmantime = rand() % ( TIME_PORT_FUNCTIONS / 4);
	if ( tmantime == 0) tmantime++;
	*temp_mantime = malloc( sizeof(char)*(number_of_digits(tmantime)+1) );
	if ( *temp_mantime == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
	integer_to_string( tmantime, *temp_mantime);
}

void execute_monitor( int id)
{
	// times
	time_t t = TIME;
	char *time;
	time = malloc( sizeof(char)*(number_of_digits(t)+1) );
	if ( time == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
	integer_to_string(t,time);

	//stattimes
	time_t statt = STATTIMES;
	char *stattimes;
	stattimes = malloc( sizeof(char)*(number_of_digits(statt)+1) );
	if ( stattimes == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
	integer_to_string(statt,stattimes);

	// shmid
	char *shmid;
	shmid = malloc(sizeof(char)*(number_of_digits(id)+1));
	if ( shmid == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
	integer_to_string(id,shmid);

	// monitor
	execlp("./monitor","./monitor","-d",time,"-t",stattimes,"-s",shmid,NULL);
}

void execute_portmaster( int id)
{
	// shmid
	char *shmid;
	shmid = malloc(sizeof(char)*(number_of_digits(id)+1));
	if ( shmid == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
	integer_to_string( id, shmid);

	// port-master
	execlp("./port-master","./port-master","-s",shmid,NULL);
}

void execute_vessel( char *temp_type, char *temp_postype, char *temp_parkperiod, char *temp_mantime, int id)
{
	// type;
	char *type;
	type = malloc( sizeof(char)*(strlen(temp_type)+1) );
	if ( type == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
	strcpy( type, temp_type);

	// postype
	char *postype;
	postype = malloc( sizeof(char)*(strlen(temp_postype)+1) );
	if ( postype == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
	strcpy( postype, temp_postype);

	// parkperiod
	char *parkperiod;
	parkperiod = malloc( sizeof(char)*(strlen(temp_parkperiod)+1) );
	if ( parkperiod == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
	strcpy( parkperiod, temp_parkperiod);

	// mantime
	char *mantime;
	mantime = malloc( sizeof(char)*(strlen(temp_mantime)+1) );
	if ( mantime == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
	strcpy( mantime, temp_mantime);

	// shmid;
	char *shmid;
	shmid = malloc(sizeof(char)*(number_of_digits(id)+1));
	if ( shmid == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
	integer_to_string( id, shmid);

	// free
	free( temp_type); free( temp_postype); free( temp_parkperiod); free( temp_mantime);

	// vessel
	execlp("./vessel","./vessel","-t",type,"-u",postype,"-p",parkperiod,"-m",mantime,"-s",shmid,NULL);
}

void add_data_to_string( char **str, FILE **fp)
{
	int c;
	int size;
	*str = malloc( sizeof(char) );
	if ( *str == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
	size = 1;
	do
	{
		c = fgetc( *fp);
		if ( c == '\n') break;
		*str = realloc( *str, sizeof(char)*(size+1) );
		if ( *str == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
		(*str)[size-1] = c;
		(*str)[size] = '\0';
		size++;
	} while ( 1 );
}

int init_shared_memory( FILE *fp)
{
	/* First, we read the info from configfile */
	int i = 1;
	int size;
	int c;
	char *types1;
	char *types2;
	char *types3;
	char *capacity1;
	char *capacity2;
	char *capacity3;
	char *cost1;
	char *cost2;
	char *cost3;
	do // Each loop is a line
	{
		// The last word of the line is the word we need
		// So, we ignore the rest words
		do
		{
			c = getc(fp);
		}while ( c != ' ');

		if ( i == 1) add_data_to_string( &types1, &fp);
		else if ( i == 2) add_data_to_string( &types2, &fp);
		else if ( i == 3) add_data_to_string( &types3, &fp);
		else if ( i == 4) add_data_to_string( &capacity1, &fp);
		else if ( i == 5) add_data_to_string( &capacity2, &fp);
		else if ( i == 6) add_data_to_string( &capacity3, &fp);
		else if ( i == 7) add_data_to_string( &cost1, &fp);
		else if ( i == 8) add_data_to_string( &cost2, &fp);
		else if ( i == 9) add_data_to_string( &cost3, &fp);

		i++;
	} while( i <= 9);

	if ( strlen(types1) > 1) { printf("Types must have length of 1 character\n"); exit(1); }
	if ( strlen(types2) > 1) { printf("Types must have length of 1 character\n"); exit(1); }
	if ( strlen(types3) > 1) { printf("Type must have length of 1 character\n"); exit(1); }

	// c1, c2, c3 --> capacities
	// co1, co2, co3 --> costs
	int c1 = atoi(capacity1);
	int c2 = atoi(capacity2);
	int c3 = atoi(capacity3);
	int co1 = atoi(cost1);
	int co2 = atoi(cost2);
	int co3 = atoi(cost3);

	/* We create 3 arrays dynamically and these arrays will be asigned to the segment */
	// small --> array for small places for ships in the harbor
	// medium --> array for medium places for ships in the harbor
	// large --> array for large places for ships int he harbor

	Vessel *small;
	small = malloc( c1*sizeof(Vessel) );
	if ( small == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
	for (i=0 ; i<c1 ; i++)
	{
		strcpy( small[i].placeType, types1);
		small[i].cost = co1;
		small[i].status = false;
	}
	Vessel *medium;
	medium = malloc( c2*sizeof(Vessel) );
	if ( medium == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
	for (i=0 ; i<c2 ; i++)
	{
		strcpy( medium[i].placeType, types2);
		medium[i].cost = co2;
		medium[i].status = false;
	}
	Vessel *large;
	large = malloc( c3*sizeof(Vessel) );
	if ( large == NULL) { printf("At myport: Error with malloc\n"); exit(1); }
	for (i=0 ; i<c3 ; i++)
	{
		strcpy( large[i].placeType, types3);
		large[i].cost = co3;
		large[i].status = false;
	}

	// The info in the shared segment will have the following structure:
	// monitor time - number of total places of ships in the port - places in the port - total money  port collected - average money port collected for each ship -
	// - total wait time for each ship category - average wait time for each ship category - seamaphore name - port_in_use_value -
	// - port master process id- length of semaphore queue

	/* Make shared memory shegment*/
	int id;
	id = shmget (IPC_PRIVATE, sizeof(int) + sizeof(int) + (c1+c2+c3)*sizeof(Vessel) + sizeof(int) + sizeof(float) + 3*sizeof(int) + 3*sizeof(float) + 5*sizeof(char) + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int), 0666);
	if ( id == -1) { printf("At myport: Error creating shared memory shegment\n"); return 1; }

	/* Attach the segment */
	char *mem;
	mem = shmat (id, (void *) 0, 0);
	if ( *(int *) mem == -1) { printf("At myport: Error attaching the shared memory segment\n"); return 1; }

	/* Initialize the segment */
	// monitor time
	int *temp0;
	temp0 = (int *) mem;
	*temp0 = 0;

	// total places of ships in the port
	mem += sizeof(int);
	int *temp1;
	temp1 = (int *) mem;
	*temp1 = c1+c2+c3;

	// initialize each place of ship in the port
	mem += sizeof(int);
	Vessel *temp2;
	temp2 = (Vessel *) mem;
	for (i=0 ; i<(c1+c2+c3) ; i++)
	{
		if ( i < c1 ) temp2[i] = small[i];
		else if ( i < c1+c2) temp2[i] = medium[i-c1];
		else temp2[i] = large[i-(c1+c2)];
	}

	// total money port collected
	for (i=0 ; i<(c1+c2+c3) ; i++) mem += sizeof(Vessel);
	int *temp3;
	temp3 = (int *) mem;
	*temp3 = 0;

	// average money the port collected
	mem += sizeof(int);
	float *temp4;
	temp4 = (float *) mem;
	*temp4 = 0;

	// total wait time for each category
	mem += sizeof(float);
	int *temp5;
	temp5 = (int *) mem;
	*temp5 = 0;

	mem += sizeof(int);
	int *temp6;
	temp6 = (int *) mem;
	*temp6 = 0;

	mem += sizeof(int);
	int *temp7;
	temp7 = (int *) mem;
	*temp7 = 0;

	// average wait time for each category
	mem += sizeof(int);
	float *temp8;
	temp8 = (float *) mem;
	*temp8 = 0;

	mem += sizeof(float);
	float *temp9;
	temp9 = (float *) mem;
	*temp9 = 0;

	mem += sizeof(float);
	float *temp10;
	temp10 = (float *) mem;
	*temp10 = 0;

	// semaphore name ("sem1")
	mem += sizeof(float);
	*mem = 's';
	mem += sizeof(char);
	*mem = 'e';
	mem += sizeof(char);
	*mem = 'm';
	mem += sizeof(char);
	*mem = '1';
	mem += sizeof(char);
	*mem = '\0';

	// port_in_use_value
	mem += sizeof(char);
	int *temp11;
	temp11 = (int *) mem;
	*temp11 = 0;

	// port-master id
	mem += sizeof(int);
	int *temp12;
	temp12 = (int *) mem;
	*temp12 = 0;

	// length of semaphore queue
	mem += sizeof(int);
	int *temp13;
	temp13 = (int *) mem;
	*temp13 = 0;

	// v id
	mem += sizeof(int);
	int *temp14;
	temp14 = (int *) mem;
	*temp14 = 0;

	// back to start (no need to do this right now but this what we will do when we will need to detach the segment)
	// in this part the segment will be removed and not detached
	mem -= sizeof(int);
	mem -= sizeof(int);
	mem -= sizeof(int);
	mem -= 5*sizeof(char);
	mem -= 3*sizeof(float);
	mem -= 3*sizeof(int);
	mem -= sizeof(float);
	mem -= sizeof(int);
	for (i=0 ; i<(c1+c2+c3) ; i++) mem -= sizeof(Vessel);
	mem -= sizeof(int);
	mem -= sizeof(int);

	int err;
	err = shmdt( (void *) mem);
	if ( err == -1) perror("Detachment");

	/* Free all the memory that was used to initialize the segment */
	free(small);
	free(medium);
	free(large);

	free(types1);
	free(types2);
	free(types3);
	free(capacity1);
	free(capacity2);
	free(capacity3);
	free(cost1);
	free(cost2);
	free(cost3);

	return id;
}

bool command_line_check( int argc, char **argv)
{
	// the command the user gave must be:
	// ./myport -l fileName
	if ( argc != 3) { printf("At myport: Wrong number of arguments at command line\n"); return false; }

	if ( strcmp(argv[1],"-l") ) { printf("At myport: The 1st arguement at command line must be -l\n"); return false; }

	FILE *fp;
	fp=fopen(argv[2],"r");
	if ( fp == NULL) { printf("At myport: Error with file given as 2nd arguement at command line\n"); return false; }
	fclose(fp);

	return true;
}
