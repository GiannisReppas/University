#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>

#include "types.h"

bool command_line_check();
void get_info_from_command_line();

// gives the number of the first available place
int give_port_place();

// returns 0 for small, 1 for medium, and 2 for large ship
int get_type_of_ship();

// functions that calculate the info that monitor will print
int calc_total_cost();
float calc_average_cost();
int calc_wait_time_small();
int calc_wait_time_medium();
int calc_wait_time_large();
float calc_average_time_small();
float calc_average_time_medium();
float calc_average_time_large();

// checks if the monitor time is greater than the time monitor functions and if it is, vessel is termineted
void check();

int main( int argc, char *argv[] )
{
	/* We check if the arguements at command line are correct*/
	if ( !command_line_check(argc, argv) ) return false;

	/* we take the info from command line*/
	char *type;
	int postype;
	int parkperiod;
	int mantime;
	int id;
	get_info_from_command_line( argc, argv, &type, &postype, &parkperiod, &mantime, &id);

	/* Attach memory segment*/
	char *mem;
	mem = (char *)shmat( id, (void *) 0, 0);
	if ( *(int *) mem == -1) perror("Attachment.");

	/* we create the vessel of this process*/
	Vessel currentVessel;
	currentVessel.vesselId = getpid();
	currentVessel.startWaitTime = time(0);
	currentVessel.leaveTime = 0;
	strcpy(currentVessel.shipType,type);
	currentVessel.status = true;

	/* we get data from shared memory */
	int i;

	// port time
	int *portTime;
	portTime = (int *) mem;

	// number of all ship places
	int *numberOfAllShipPlaces;
	mem += sizeof(int);
	numberOfAllShipPlaces = (int *) mem;

	// ship places
	Vessel *shipPlaces;
	mem += sizeof(int);
	shipPlaces = (Vessel *) mem;

	// total money port collected
	for (i=0 ; i<*numberOfAllShipPlaces ; i++) mem += sizeof(Vessel);
	int *moneyPortCollected;
	moneyPortCollected = (int *) mem;

	// average money port collected
	mem += sizeof(int);
	float *averageMoneyPortCollected;
	averageMoneyPortCollected = (float *) mem;

	// total wait times
	mem += sizeof(float);
	int *totalWaitTimeForSmallShips;
	totalWaitTimeForSmallShips = (int *) mem;

	mem += sizeof(int);
	int *totalWaitTimeForMediumShips;
	totalWaitTimeForMediumShips = (int *) mem;

	mem += sizeof(int);
	int *totalWaitTimeForLargeShips;
	totalWaitTimeForLargeShips = (int *) mem;

	// average wait times
	mem += sizeof(int);
	float *averageWaitTimeForSmallShips;
	averageWaitTimeForSmallShips = (float *) mem;

	mem += sizeof(float);
	float *averageWaitTimeForMediumShips;
	averageWaitTimeForMediumShips = (float *) mem;

	mem += sizeof(float);
	float *averageWaitTimeForLargeShips;
	averageWaitTimeForLargeShips = (float *) mem;

	// semaphore
	mem += sizeof(float);
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
	sem_t *sem1 = sem_open( sem_name1 , O_EXCL); // semaphore pointer
	free( sem_name1);

	// use_port value
	int *use_port;
	mem += sizeof(char);
	use_port = (int *) mem;

	// port-master id
	mem += sizeof(int);
	int pm_id = * (int *) mem;

	// length of queue that is created after sem_wait
	mem += sizeof(int);
	int *semQueueLength;
	semQueueLength = (int *) mem;

	/////////////////////////////////////////////////////
	/////////////////////////////////////////////////////
	/////////////////////////////////////////////////////
	// get permision to enter the port from port-master
	check( portTime, mem, j1, numberOfAllShipPlaces, sem1, &type);
	*semQueueLength += 1;
	sem_wait( sem1);
	*semQueueLength -= 1;
	check( portTime, mem, j1, numberOfAllShipPlaces, sem1, &type);

	currentVessel.arriveTime = time(0);

	int tempCost;
	char temp[2];
	// get first available place in the port
	if (postype == 0)
	{
		for ( i=0 ; i<*numberOfAllShipPlaces ; i++ )
		{
			if ( (!shipPlaces[i].status) && (!strcmp(shipPlaces[i].placeType,currentVessel.shipType)) )
			{
				break;
			}
		}
	}
	else
	{
		i = give_port_place( currentVessel, shipPlaces, *numberOfAllShipPlaces);
	}

	// if there is an available place in the port
	if ( i != *numberOfAllShipPlaces)
	{
		// go to place
		sleep(mantime);

		strcpy(temp,shipPlaces[i].placeType);
		tempCost = shipPlaces[i].cost;
		shipPlaces[i] = currentVessel;

		strcpy(shipPlaces[i].placeType,temp);
		shipPlaces[i].cost = tempCost*parkperiod ;

		// port-available for another ship
		// stay in place
		*use_port = 0;
		sleep(parkperiod);

		// leave the port
		check( portTime, mem, j1, numberOfAllShipPlaces, sem1, &type);
		*semQueueLength += 1;
		sem_wait(sem1);
		*semQueueLength -= 1;
		check( portTime, mem, j1, numberOfAllShipPlaces, sem1, &type);

		sleep(mantime);

		shipPlaces[i].leaveTime = time(0);
		shipPlaces[i].status = false;

		/* Update results.txt*/
		FILE *fp;
		fp = fopen("results.txt","a+");
		if ( fp == NULL) { printf("\nAt vessel: Error with fopen\n");}

		fprintf(fp,"\nThis ship arrived at port  at: %ld\n",shipPlaces[i].arriveTime);
		fprintf(fp,"Its id was: %d\n",shipPlaces[i].vesselId);
		fprintf(fp,"Its type was: %s\n",shipPlaces[i].shipType);
		fprintf(fp,"Its place type was: %s\n",shipPlaces[i].placeType);
		fprintf(fp,"It waited to learn its place in the port for: %ld\n",shipPlaces[i].arriveTime - shipPlaces[i].startWaitTime);
		fprintf(fp,"Staying there costed %d\n",shipPlaces[i].cost);
		fprintf(fp,"Left at:%ld\n",shipPlaces[i].leaveTime);
		fclose(fp);

		/* Update the results monitor will print */

		fp = fopen("results.txt","r");
		if ( fp == NULL) { printf("\nAt vessel: Error with fopen\n"); exit(1); }

		int numberOfShipsThatLeft = 1;
		int c1 = 'c'; int c2 = 'c';
		do
		{
			c1 = fgetc(fp); if ( c1 == EOF) break;
			if ( ( c1 == '\n') && ( c2 == '\n') ) numberOfShipsThatLeft++;

			c2 = fgetc(fp); if ( c2 == EOF) break;
			if ( ( c1 == '\n') && ( c2 == '\n') ) numberOfShipsThatLeft++;
		} while ( 1 );
		rewind(fp);

		int newTotalCost = calc_total_cost( fp); rewind( fp);
		float newAverageCost = calc_average_cost( fp, numberOfShipsThatLeft); rewind( fp);

		int newWaitTimeS = calc_wait_time_small( shipPlaces, *numberOfAllShipPlaces, &numberOfShipsThatLeft, fp); rewind(fp);
		int newWaitTimeM = calc_wait_time_medium( shipPlaces, *numberOfAllShipPlaces, &numberOfShipsThatLeft, fp); rewind(fp);
		int newWaitTimeL = calc_wait_time_large( shipPlaces, *numberOfAllShipPlaces, &numberOfShipsThatLeft, fp); rewind(fp);
		float newAverageTimeS = calc_average_time_small( shipPlaces, *numberOfAllShipPlaces, &numberOfShipsThatLeft, fp); rewind(fp);
		float newAverageTimeM = calc_average_time_medium( shipPlaces, *numberOfAllShipPlaces, &numberOfShipsThatLeft, fp); rewind(fp);
		float newAverageTimeL = calc_average_time_large( shipPlaces, *numberOfAllShipPlaces, &numberOfShipsThatLeft, fp); rewind(fp);

		*moneyPortCollected = newTotalCost;
		*averageMoneyPortCollected = newAverageCost;

		*totalWaitTimeForSmallShips = newWaitTimeS;
		*totalWaitTimeForMediumShips = newWaitTimeM;
		*totalWaitTimeForLargeShips = newWaitTimeL;
		*averageWaitTimeForSmallShips = newAverageTimeS;
		*averageWaitTimeForMediumShips = newAverageTimeM;
		*averageWaitTimeForLargeShips = newAverageTimeL;

		fclose(fp);

		*use_port = 0;

	}
	else
	{
		printf("\n\n\nNo space to enter for ship with id: %d\n\n\n", getpid() );
	}

	////////////////////////////////////////////////
	////////////////////////////////////////////////
	////////////////////////////////////////////////

	// back to start
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
	for (i=0 ; i<*numberOfAllShipPlaces ; i++) mem -= sizeof(Vessel);
	mem -= sizeof(int);
	mem -= sizeof(int);

	/* Close the semaphore we opened before */
	sem_close( sem1);

	/* Detach segment */
	int err;
	err  = shmdt((void *) mem);
	if ( err==-1) perror("Detachment.");

	/* End of vessel */
	free(type);
	return 0;
}

void check( int *portTime, char *mem, int j1, int *numberOfAllShipPlaces, sem_t *sem1, char **type)
{
	if ( *portTime > TIME_PORT_FUNCTIONS)
	{
		printf("\n\n\nShip with id: %d didn't finish\n\n\n",getpid());

		// back to start
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
		int i;
		for (i=0 ; i<*numberOfAllShipPlaces ; i++) mem -= sizeof(Vessel);
		mem -= sizeof(int);
		mem -= sizeof(int);

		/* Detach segment */
		int err;
		err  = shmdt((void *) mem);
		if ( err==-1) perror("Detachment.");

		/* Close the semaphore we opened before */
		sem_close( sem1);

		/* End of vessel */
		free(*type);
		exit(0);
	}
}

int calc_total_cost( FILE *fp)
{
	int temp;
	int total = 0;
	int c;
	int lineCounter = 1;
	int columnCounter = 1;
	char *number;
	number = malloc( sizeof(char));
	if ( number == NULL) { printf("At vessel: Error with malloc\n"); exit(1); }
	number[0] = '\0';

	// for each ship in results.txt, add cost to total
	do
	{
		c = fgetc(fp);
		columnCounter++;
		if ( c == '\n')
		{
			if ( lineCounter == 8) lineCounter = 1;
			else lineCounter++;
			columnCounter = 1;
			if ( number[0] != '\0')
			{
				total += atoi( number);
				free(number);
				number = malloc( sizeof(char));
				if ( number == NULL) { printf("At vessel: Error with malloc\n"); exit(1); }
				number[0] = '\0';
			}
		}
		else if ( ( lineCounter == 7) && ( columnCounter > 21 ) )
		{
			temp = strlen(number);
			number = realloc( number, sizeof(char) + temp + sizeof(char) );
			if ( number == NULL) { printf("At vessel: Error with malloc\n"); exit(1); }
			number[temp] = c;
			number[temp+1] = '\0';
		}
	} while ( c != EOF);

	free( number);
	return total;
}

float calc_average_cost( FILE *fp, int NoStL)
{
	int total = calc_total_cost( fp);
	float number = NoStL;

	return total / number;
}

int calc_wait_time_small( Vessel *array, int nov, int *nostl, FILE *fp)
{
	*nostl = 0;

	int temp;
	int total = 0;
	int c;
	int lineCounter = 1;
	int columnCounter = 1;
	char *number;
	number = malloc( sizeof(char));
	if ( number == NULL) { printf("At vessel: Error with malloc\n"); exit(1); }
	number[0] = '\0';
	int numberOfShip;
	char *shipType;
	shipType = malloc( sizeof(char));
	if ( shipType == NULL) { printf("At vessel: Error with malloc\n"); exit(1); }
	shipType[0] = '\0';
	int i;
	int j;

	char *small;
	char *medium;
	char *large;

	// for each small ship in results.txt, add wait time to total
	do
	{
		c = fgetc(fp);
		columnCounter++;
		if ( c == '\n')
		{
			if ( lineCounter == 8) lineCounter = 1;
			else lineCounter++;
			columnCounter = 1;
			if ( number[0] != '\0')
			{
				*nostl += 1;
				total += atoi( number);
				free(number);
				number = malloc( sizeof(char));
				if ( number == NULL) { printf("At vessel: Error with malloc\n"); exit(1); }
				number[0] = '\0';
			}
			if (shipType[0] != '\0')
			{
				free(shipType);
				shipType = malloc( sizeof(char));
				if ( shipType == NULL) { printf("At vessel: Error with malloc\n"); exit(1); }
				shipType[0] = '\0';
			}
		}
		else if ( ( lineCounter == 4) && ( columnCounter > 15) )
		{
			temp = strlen(number);
			shipType = realloc( shipType, sizeof(char) + temp + sizeof(char) );
			if ( shipType == NULL) { printf("Error with malloc\n"); exit(1); }
			shipType[temp] = c;
			shipType[temp+1] = '\0';

			small = malloc( sizeof(char) * (strlen(array[0].placeType)+1) );
			if (small == NULL) { printf("At vessel: Error with malloc\n"); exit(1);}
			strcpy(small,array[0].placeType);

			j = 0;
			for (i=1 ; i<nov ; i++)
			{
				if ( strcmp(array[i].placeType,array[i-1].placeType) )
				{
					if ( j == 0 )
					{
						medium = malloc( sizeof(char) * (strlen(array[i].placeType)+1) );
						if ( medium == NULL ) { printf("At vessel: Error with malloc\n"); exit(1);}
						strcpy( medium, array[i].placeType);

						j++;
					}
					else if ( j == 1)
					{
						large = malloc( sizeof(char) * (strlen(array[i].placeType)+1) );
						if ( large == NULL ) { printf("At vessel: Error with malloc\n"); exit(1); }
						strcpy( large, array[i].placeType);

						j++;
					}
				}
				if (j == 2) break;
			}

			if ( !strcmp(small,shipType) ) numberOfShip = 0;
			else if ( !strcmp(medium,shipType) ) numberOfShip = 1;
			else numberOfShip = 2;

			free(small);
			free(medium);
			free(large);
		}
		else if ( ( lineCounter == 6) && ( columnCounter > 47 ) && ( numberOfShip == 0) )
		{
			temp = strlen(number);
			number = realloc( number, sizeof(char) + temp + sizeof(char) );
			if ( number == NULL) { printf("Error with malloc\n"); exit(1); }
			number[temp] = c;
			number[temp+1] = '\0';
		}
	} while ( c != EOF);

	free(number);
	free(shipType);

	return total;
}

int calc_wait_time_medium( Vessel *array, int nov, int *nostl, FILE *fp)
{
	*nostl = 0;

	int temp;
	int total = 0;
	int c;
	int lineCounter = 1;
	int columnCounter = 1;
	char *number;
	number = malloc( sizeof(char));
	if ( number == NULL) { printf("At vessel: Error with malloc\n"); exit(1); }
	number[0] = '\0';
	int numberOfShip;
	char *shipType;
	shipType = malloc( sizeof(char));
	if ( shipType == NULL) { printf("At vessel: Error with malloc\n"); exit(1); }
	shipType[0] = '\0';
	int i;
	int j;

	char *small; small = NULL;
	char *medium; medium = NULL;
	char *large; large = NULL;

	// for each medium ship in results.txt, add wait time to total
	do
	{
		c = fgetc(fp);
		columnCounter++;
		if ( c == '\n')
		{
			if ( lineCounter == 8) lineCounter = 1;
			else lineCounter++;
			columnCounter = 1;
			if ( number[0] != '\0')
			{
				*nostl += 1;

				total += atoi( number);
				free(number);
				number = malloc( sizeof(char));
				if ( number == NULL) { printf("At vessel: Error with malloc\n"); exit(1); }
				number[0] = '\0';
			}
			if (shipType[0] != '\0')
			{
				free(shipType);
				shipType = malloc( sizeof(char));
				if ( shipType == NULL) { printf("At vessel: Error with malloc\n"); exit(1); }
				shipType[0] = '\0';
			}
		}
		else if ( ( lineCounter == 4) && ( columnCounter > 15) )
		{
			temp = strlen(number);
			shipType = realloc( shipType, sizeof(char) + temp + sizeof(char) );
			if ( shipType == NULL) { printf("Error with malloc\n"); exit(1); }
			shipType[temp] = c;
			shipType[temp+1] = '\0';

			small = malloc( sizeof(char) * (strlen(array[0].placeType)+1) );
			if (small == NULL) { printf("At vessel: Error with malloc\n"); exit(1);}
			strcpy(small,array[0].placeType);

			j = 0;
			for (i=1 ; i<nov ; i++)
			{
				if ( strcmp(array[i].placeType,array[i-1].placeType) )
				{
					if ( j == 0 )
					{
						medium = malloc( sizeof(char) * (strlen(array[i].placeType)+1) );
						if ( medium == NULL ) { printf("At vessel: Error with malloc\n"); exit(1);}
						strcpy( medium, array[i].placeType);

						j++;
					}
					else if ( j == 1)
					{
						large = malloc( sizeof(char) * (strlen(array[i].placeType)+1) );
						if ( large == NULL ) { printf("At vessel: Error with malloc\n"); exit(1); }
						strcpy( large, array[i].placeType);

						j++;
					}
				}
				if (j == 2) break;
			}

			if ( !strcmp(small,shipType) ) numberOfShip = 0;
			else if ( !strcmp(medium,shipType) ) numberOfShip = 1;
			else numberOfShip = 2;

			free(small);
			free(medium);
			free(large);
		}
		else if ( ( lineCounter == 6) && ( columnCounter > 47 ) && ( numberOfShip == 1) )
		{
			temp = strlen(number);
			number = realloc( number, sizeof(char) + temp + sizeof(char) );
			if ( number == NULL) { printf("Error with malloc\n"); exit(1); }
			number[temp] = c;
			number[temp+1] = '\0';
		}
	} while ( c != EOF);

	free(number);
	free(shipType);

	return total;
}

int calc_wait_time_large( Vessel *array, int nov, int *nostl, FILE *fp)
{
	*nostl = 0;

	int temp;
	int total = 0;
	int c;
	int lineCounter = 1;
	int columnCounter = 1;
	char *number;
	number = malloc( sizeof(char));
	if ( number == NULL) { printf("At vessel: Error with malloc\n"); exit(1); }
	number[0] = '\0';
	int numberOfShip;
	char *shipType;
	shipType = malloc( sizeof(char));
	if ( shipType == NULL) { printf("At vessel: Error with malloc\n"); exit(1); }
	shipType[0] = '\0';
	int i;
	int j;

	char *small; small = NULL;
	char *medium; medium = NULL;
	char *large; large = NULL;

	// for each large ship in results.txt, add wait time in total
	do
	{
		c = fgetc(fp);
		columnCounter++;
		if ( c == '\n')
		{
			if ( lineCounter == 8) lineCounter = 1;
			else lineCounter++;
			columnCounter = 1;
			if ( number[0] != '\0')
			{
				*nostl += 1;

				total += atoi( number);
				free(number);
				number = malloc( sizeof(char));
				if ( number == NULL) { printf("At vessel: Error with malloc\n"); exit(1); }
				number[0] = '\0';
			}
			if (shipType[0] != '\0')
			{
				free(shipType);
				shipType = malloc( sizeof(char));
				if ( shipType == NULL) { printf("At vessel: Error with malloc\n"); exit(1); }
				shipType[0] = '\0';
			}
		}
		else if ( ( lineCounter == 4) && ( columnCounter > 15) )
		{
			temp = strlen(number);
			shipType = realloc( shipType, sizeof(char) + temp + sizeof(char) );
			if ( shipType == NULL) { printf("Error with malloc\n"); exit(1); }
			shipType[temp] = c;
			shipType[temp+1] = '\0';

			small = malloc( sizeof(char) * (strlen(array[0].placeType)+1) );
			if (small == NULL) { printf("At vessel: Error with malloc\n"); exit(1);}
			strcpy(small,array[0].placeType);

			j = 0;
			for (i=1 ; i<nov ; i++)
			{
				if ( strcmp(array[i].placeType,array[i-1].placeType) )
				{
					if ( j == 0 )
					{
						medium = malloc( sizeof(char) * (strlen(array[i].placeType)+1) );
						if ( medium == NULL ) { printf("At vessel: Error with malloc\n"); exit(1);}
						strcpy( medium, array[i].placeType);

						j++;
					}
					else if ( j == 1)
					{
						large = malloc( sizeof(char) * (strlen(array[i].placeType)+1) );
						if ( large == NULL ) { printf("At vessel: Error with malloc\n"); exit(1); }
						strcpy( large, array[i].placeType);

						j++;
					}
				}
				if (j == 2) break;
			}

			if ( !strcmp(small,shipType) ) numberOfShip = 0;
			else if ( !strcmp(medium,shipType) ) numberOfShip = 1;
			else numberOfShip = 2;

			free(small);
			free(medium);
			free(large);
		}
		else if ( ( lineCounter == 6) && ( columnCounter > 47 ) && ( numberOfShip == 2) )
		{
			temp = strlen(number);
			number = realloc( number, sizeof(char) + temp + sizeof(char) );
			if ( number == NULL) { printf("Error with malloc\n"); exit(1); }
			number[temp] = c;
			number[temp+1] = '\0';
		}
	} while ( c != EOF);

	free(number);
	free(shipType);

	return total;
}

float calc_average_time_small( Vessel *array, int nov, int *nostl, FILE *fp)
{
	int total;
	total = calc_wait_time_small( array, nov, nostl, fp);
	if ( *nostl == 0) return 0;
	float number = *nostl;
	number = total / number;

	return number;
}

float calc_average_time_medium( Vessel *array, int nov, int *nostl, FILE *fp)
{
	int total;
	total = calc_wait_time_medium( array, nov, nostl, fp);
	if ( *nostl == 0) return 0;
	float number = *nostl;
	number = total / number;

	return number;
}

float calc_average_time_large( Vessel *array, int nov, int *nostl, FILE *fp)
{
	int total;
	total = calc_wait_time_large( array, nov, nostl, fp);
	if ( *nostl == 0) return 0;
	float number = *nostl;
	number = total / number;

	return number;
}


int get_type_of_ship( Vessel *array, int p, int nov)
{
	char *small;
	char *medium;
	char *large;

	small = malloc( sizeof(char) * (strlen(array[0].placeType)+1) );
	if (small == NULL) { printf("At vessel: Error with malloc\n"); exit(1);}
	strcpy(small,array[0].placeType);

	int i;
	int j = 0;
	for ( i=1 ; i<nov ; i++)
	{
		if ( strcmp(array[i].placeType,array[i-1].placeType) )
		{
			if ( j == 0 )
			{
				medium = malloc( sizeof(char) * (strlen(array[i].placeType)+1) );
				if ( medium == NULL ) { printf("At vessel: Error with malloc\n"); exit(1);}
				strcpy( medium, array[i].placeType);

				j++;
			}
			else if ( j == 1)
			{
				large = malloc( sizeof(char) * (strlen(array[i].placeType)+1) );
				if ( large == NULL ) { printf("At vessel: Error with malloc\n"); exit(1); }
				strcpy( large, array[i].placeType);

				j++;
			}
		}
		if (j == 2) break;
	}

	if ( !strcmp(small,array[p].placeType) )
	{
		free(small);
		free(medium);
		free(large);
		return 0;
	}
	else if ( !strcmp(medium,array[p].placeType) )
	{
		free(small);
		free(medium);
		free(large);
		return 1;
	}
	else
	{
		free(small);
		free(medium);
		free(large);
		return 2;
	}
}

int give_port_place( Vessel current, Vessel *array, int nov)
{
	char *small;
	char *medium;
	char *large;

	small = malloc( sizeof(char) * (strlen(array[0].placeType)+1) );
	if (small == NULL) { printf("At vessel: Error with malloc\n"); exit(1);}
	strcpy(small,array[0].placeType);

	int i;
	int j = 0;
	for ( i=1 ; i<nov ; i++)
	{
		if ( strcmp(array[i].placeType,array[i-1].placeType) )
		{
			if ( j == 0 )
			{
				medium = malloc( sizeof(char) * (strlen(array[i].placeType)+1) );
				if ( medium == NULL ) { printf("At vessel: Error with malloc\n"); exit(1);}
				strcpy( medium, array[i].placeType);

				j++;
			}
			else if ( j == 1)
			{
				large = malloc( sizeof(char) * (strlen(array[i].placeType)+1) );
				if ( large == NULL ) { printf("At vessel: Error with malloc\n"); exit(1); }
				strcpy( large, array[i].placeType);

				j++;
			}
		}
		if (j == 2) break;
	}

	if ( !strcmp(small,current.shipType) )
	{
		for( i=0 ; i<nov ; i++)
		{
			if ( !array[i].status)
			{
				break;
			}
		}
	}
	else if ( !strcmp(medium,current.shipType) )
	{
		for( i=0 ; i<nov ; i++)
		{
			if ( (!array[i].status) && ( (!strcmp(medium,array[i].placeType)) || (!strcmp(large,array[i].placeType)) )  )
			{
				break;
			}
		}
	}
	else if ( !strcmp(large,current.shipType))
	{
		for( i=0 ; i<nov ; i++)
		{
			if ( (!array[i].status) && (!strcmp(large,array[i].placeType)) )
			{
				break;
			}
		}
	}

	free(small);
	free(medium);
	free(large);

	return i;
}

void get_info_from_command_line( int argc, char **argv, char **type, int *postype, int *parkperiod, int *mantime, int *id)
{
	int i;
	for ( i=0; i<argc ; i++)
	{
		if ( !strcmp(argv[i],"-t") ) { *type = malloc(sizeof(char)*(strlen(argv[i+1])+1)); if ( type==NULL) {printf("Error with malloc\n"); exit(1);} strcpy(*type,argv[i+1]); }
		if ( !strcmp(argv[i],"-u") ) { *postype = atoi(argv[i+1]); if ( (*postype != 0) && (*postype != 1) ) *postype = 0; }
		if ( !strcmp(argv[i],"-p") ) *parkperiod = atoi(argv[i+1]);
		if ( !strcmp(argv[i],"-m") ) *mantime = atoi(argv[i+1]);
		if ( !strcmp(argv[i],"-s") ) *id = atoi(argv[i+1]);
	}
}

bool command_line_check( int argc, char **argv)
{
	if ( argc != 11) { printf("Wrong number of arguements\n"); exit(1); }

	if ( (strcmp(argv[1],argv[3])) && (strcmp(argv[1],argv[5])) && (strcmp(argv[1],argv[7])) && (strcmp(argv[1],argv[9])) && (strcmp(argv[3],argv[5])) && (strcmp(argv[3],argv[7])) && (strcmp(argv[3],argv[9])) && (strcmp(argv[5],argv[7])) && (strcmp(argv[5],argv[9])) && (strcmp(argv[7],argv[9])) )
	{
		if ( (!strcmp(argv[1],"-t")) || (!strcmp(argv[1],"-u")) || (!strcmp(argv[1],"-p")) || (!strcmp(argv[1],"-m")) || (!strcmp(argv[1],"-s")) )
		{
			if ( (!strcmp(argv[3],"-t")) || (!strcmp(argv[3],"-u")) || (!strcmp(argv[3],"-p")) || (!strcmp(argv[3],"-m")) || (!strcmp(argv[3],"-s")) )
			{
				if ( (!strcmp(argv[5],"-t")) || (!strcmp(argv[5],"-u")) || (!strcmp(argv[5],"-p")) || (!strcmp(argv[5],"-m")) || (!strcmp(argv[5],"-s")) )
				{
					if ( (!strcmp(argv[7],"-t")) || (!strcmp(argv[7],"-u")) || (!strcmp(argv[7],"-p")) || (!strcmp(argv[7],"-m")) || (!strcmp(argv[7],"-s")) )
					{
						if ( (!strcmp(argv[9],"-t")) || (!strcmp(argv[9],"-u")) || (!strcmp(argv[9],"-p")) || (!strcmp(argv[9],"-m")) || (!strcmp(argv[9],"-s")) ) ;
						else { printf("5th flag is wrong\n"); return false; }
					}
					else { printf("4th flag is wrong\n"); return false; }
				}
				else { printf("3rd flag is wrong\n"); return false; }
			}
			else { printf("2nd flag is wrong\n"); return false; }
		}
		else { printf("1st flag is wrong\n"); return false; }
	}
	else { printf("Flags must be unique\n"); return false; }

	return true;
}
