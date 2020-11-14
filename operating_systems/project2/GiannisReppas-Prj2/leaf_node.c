#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <math.h>
#include <time.h>
#include <signal.h>

#include "itoa_plus_nod.h"
#include "types.h"

bool get_info_from_command_line(); // we use this func to get all the info the user gave from command line //returns true if -s was given from the user

int main(int argc , char *argv[])
{
	// we start counting the CPU time for searcher process
	time_t start_t, end_t;
	double total_t;
	start_t = clock();

	// we get the info from command line
	long int r_id;
	int start_height;
	char *pattern;
	char *binary_file;
	long int range[2];
	bool s;
	int fd[2];
	s = get_info_from_command_line(argc,argv,&pattern,&binary_file,range,fd,&start_height,&r_id);

	FILE *file_pointer;
	// if the user gave -s as an option, then the range is calculated here, we use a different algorithm
	// otherwise, the range has already been calculated at spliter mergers
	if (s)
	{
		// k are the total entries of the file
		// each leaf_node has a special number, given from command line, temp
		// top = pow((double)2,(double)start_height);
		// for (i=1;i<=top;i++) total+=i;
		// entries for this search node --> k*temp/ total

		range[0] = range[0] + 1;
		range[1] = range[1] + 1;
		long int temp = range[0];

		int i,j,total=0;
		double top = pow((double)2,(double)start_height);
		for (i=1;i<=top;i++) total+=i;

		file_pointer = fopen(binary_file,"rb+");
		fseek(file_pointer,0,SEEK_END);
		long int end_of_file=ftell(file_pointer);
		fclose(file_pointer);
		int k = end_of_file / sizeof(Entry);

		long int current=0;
		for (i=1;i<temp;i++)
		{
			for(j=0;j< k*i/total;j++)
			{
				current+=sizeof(Entry);
			}
		}
		range[0] = current;
		for (j=0;j<k*temp/total;j++)
		{
			current+=sizeof(Entry);
		}
		range[1] = current;
		if ( temp == (int)top) range[1] = end_of_file;
	}

	// pipe that connects leaf_node with its parent
	int p[2];
	if ( pipe(p)==-1) { puts("Error with pipe"); exit(1);}
	dup2(fd[0],p[0]);
	dup2(fd[1],p[1]);

	// we get the entries that match pattern and put them temporarilly in an array
	// we count them and write their counter in the pipe
	// then, we put them one by one in the pipe, then free the array 

	file_pointer = fopen(binary_file,"rb+");
	fseek(file_pointer,range[0],SEEK_SET);

	bool check;
	Entry temp;
	Entry *array;
	int size = 0;
	array = NULL;
	close(p[0]);

	char *str;
	int t;
	while ( ftell(file_pointer) < range[1] )
	{
		// we take an entry
		check = false;
		fread(&temp,sizeof(Entry),1,file_pointer);

		// search
		str = malloc( (number_of_digits(temp.customer_id)+1)*sizeof(char));
		integer_to_string(temp.customer_id,str);
		if ( strstr(str,pattern) != NULL) check = true;
		free(str);
		if ( strstr(temp.first_name,pattern) != NULL ) check = true;
		if ( strstr(temp.last_name,pattern) != NULL ) check = true;
		if ( strstr(temp.street_living,pattern) != NULL ) check = true;
		str = malloc( (number_of_digits(temp.number_of_street_living)+1)*sizeof(char));
		integer_to_string(temp.number_of_street_living,str);
		if( strstr(str,pattern) != NULL) check = true;
		free(str);
		if ( strstr(temp.city_living,pattern) != NULL ) check = true;
		if ( strstr(temp.postal_sector,pattern) != NULL ) check = true;
		t = temp.salary;
		str = malloc( (number_of_digits(t)+1)*sizeof(char));
		integer_to_string(t,str);
		if ( strstr(str,pattern) != NULL) check = true;
		free(str);

		// if search matches, we put it in the array and increase the counter
		if ( check )
		{
			if(array != NULL)
			{
				array = realloc( array , (size+1)*sizeof(Entry) );
				array[size] = temp;
				size++;
			}
			else
			{
				array = malloc( sizeof(Entry) );
				array[size] = temp;
				size++;
			}
		}
	}

	write( p[1], &size, sizeof(int) );
	int i;
	for( i=0; i<size ; i++)
	{
		temp = array[i];
		write( p[1], &temp, sizeof(Entry) );
	}

	free(array);
	fclose(file_pointer);

	// end of leaf_node
	size =1;
	end_t = clock();
	total_t = (end_t - start_t) / (double) CLOCKS_PER_SEC;

	// w write its time too in the pipe
	write( p[1], &size, sizeof(int));
	write( p[1], &total_t, sizeof(double));

	// we send the singal to root
	if( kill(r_id,SIGUSR2) ) { puts("Error with kill"); exit(1);}
	return 0;
}

bool get_info_from_command_line( int argc, char *argv[], char **p, char **b_file,long int r[], int fd[], int *h, long int *r_id)
{
	//pattern
	*p = malloc( sizeof(char)*(strlen(argv[2])+1) );
	strcpy(*p,argv[1]);

	//binary_file
	*b_file = malloc( sizeof(char)*(strlen(argv[3])+1) );
	strcpy(*b_file,argv[2]);

	//range
	r[0] = atoi(argv[3]);
	r[1] = atoi(argv[4]);

	//pipe
	fd[0] = atoi(argv[6]);
	fd[1] = atoi(argv[7]);

	//start_height
	*h = atoi(argv[8]);

	// root id
	*r_id = atoi(argv[9]);

	//s
	if ( !strcmp("0",argv[5]) ) return false;
	else if( !strcmp("1",argv[5]) ) return true;
	else { puts("Error"); exit(1);}
}
