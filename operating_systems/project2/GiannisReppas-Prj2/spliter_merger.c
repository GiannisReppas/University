#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>

#include "itoa_plus_nod.h"
#include "types.h"

bool get_info_from_command_line(); // we use this func to get all the info the user gave from command line //returns true if -s was given from the user

int main(int argc, char *argv[])
{
	// we start counting the CPU time for spliter_merger process
	time_t start_t,end_t;
	double total_t;
	start_t = clock();

	// we get the info from command line
	long int r_id;
	int start_height;
	int height;
	char *pattern;
	char *binary_file;
	long int range[2];
	bool s;
	int fd[2];
	s = get_info_from_command_line(argc,argv,&height,&pattern,&binary_file,range,fd,&start_height,&r_id);

	// the pipe that connects spliter-merger with its father
	int p[2];
	if ( pipe(p)==-1) { puts("Error with pipe"); exit(1);}
	dup2(fd[0],p[0]);
	dup2(fd[1],p[1]);

	// we calculate new ranges for the 2 children
	long int range_for_first_child[2];
	long int range_for_second_child[2];
	if ( !s )//without s
	{
		long int temp = range[1] - range[0];
		temp = temp / sizeof(Entry);
		temp = temp / 2;
		temp = temp * sizeof(Entry) + range[0];

		range_for_first_child[0] = range[0];
		range_for_first_child[1] = temp-1;

		range_for_second_child[0] = temp;
		range_for_second_child[1] = range[1];
	}
	else// with s
	{
		range_for_first_child[0] = 2*range[0];
		range_for_first_child[1] = 2*range[0];

		range_for_second_child[0] = 2*range[0] + 1;
		range_for_second_child[1] = 2*range[0] + 1;
	}

	// we create the new children processes
	pid_t first_child, second_child;

	// pipe for the first child
	int p1[2];
	if (pipe(p1)==-1) { puts("Error with pipe"); exit(1);}

	// first children process
	first_child = fork();
	if ( first_child < 0) { puts("Error with fork"); exit(-1); }

	if ( first_child == 0)
	{
		// current height
		int temp_height = height -1;
		char *h;
		h = malloc(sizeof(char)*2);
		if ( h == NULL) { puts("Error with malloc"); exit(1); }
		integer_to_string(temp_height,h);

		// range
		char *r0;
		char *r1;
		r0 = malloc(sizeof(char)*(number_of_digits(range_for_first_child[0])+1));
		if ( r0 == NULL) { puts("Error with malloc"); exit(1); }
		r1 = malloc(sizeof(char)*(number_of_digits(range_for_first_child[1])+1));
		if ( r1 == NULL) { puts("Error with malloc"); exit(1); }
		integer_to_string(range_for_first_child[0],r0);
		integer_to_string(range_for_first_child[1],r1);

		// s
		char skew[2];
		if( !s ) strcpy(skew,"0");
		else strcpy(skew,"1");

		// pipe
		char *f0;
		char *f1;
		f0 = malloc(sizeof(char)*(number_of_digits(p1[0])+1));
		if ( f0 == NULL) { puts("Error with malloc"); exit(1); }
		f1 = malloc(sizeof(char)*(number_of_digits(p1[1])+1));
		if ( f1 == NULL) { puts("Error with malloc"); exit(1); }
		integer_to_string(p1[0],f0);
		integer_to_string(p1[1],f1);

		// height the user gave from command line
		char *sh;
		sh = malloc(sizeof(char)*2);
		if ( sh == NULL) { puts("Error with malloc"); exit(1); }
		integer_to_string(start_height,sh);

		// process id of root
		char *root_id;
		root_id = malloc(sizeof(char)*(number_of_digits(r_id)+1));
		if ( root_id == NULL) { puts("Error with malloc"); exit(1); }
		integer_to_string(r_id,root_id);

		// if height != 1 --> spliter-merger again
		if ( height != 1) execlp("./spliter_merger","./spliter_merger",h,pattern,binary_file,r0,r1,skew,f0,f1,sh,root_id,NULL);
		// else leaf node
		else execlp("./leaf_node","./leaf_node",pattern,binary_file,r0,r1,skew,f0,f1,sh,root_id,NULL);
	}
	else
	{
		// pipe for the second child
		int p2[2];
		if (pipe(p2)==-1) { puts("Error with pipe"); exit(-1); }

		// second child process
		second_child = fork();
		if ( second_child < 0) { puts("Error with fork"); exit(-1); }

		if ( second_child == 0)
		{
			// current height
			int temp_height = height -1;
			char *h;
			h = malloc(sizeof(char)*2);
			if ( h == NULL) { puts("Error with malloc"); exit(1); }
			integer_to_string(temp_height,h);

			// range
			char *r0;
			char *r1;
			r0 = malloc(sizeof(char)*(number_of_digits(range_for_second_child[0])+1));
			if ( r0 == NULL) { puts("Error with malloc"); exit(1); }
			r1 = malloc(sizeof(char)*(number_of_digits(range_for_second_child[1])+1));
			if ( r1 == NULL) { puts("Error with malloc"); exit(1); }
			integer_to_string(range_for_second_child[0],r0);
			integer_to_string(range_for_second_child[1],r1);

			// s
			char skew[2];
			if( !s ) strcpy(skew,"0");
			else strcpy(skew,"1");

			// pipe
			char *f0;
			char *f1;
			f0 = malloc(sizeof(char)*(number_of_digits(p2[0])+1));
			if ( f0 == NULL) { puts("Error with malloc"); exit(1); }
			f1 = malloc(sizeof(char)*(number_of_digits(p2[1])+1));
			if ( f1 == NULL) { puts("Error with malloc"); exit(1); }
			integer_to_string(p2[0],f0);
			integer_to_string(p2[1],f1);

			// height the user gave from command line
			char *sh;
			sh = malloc(sizeof(char)*2);
			if ( sh == NULL) { puts("Error with malloc"); exit(1); }
			integer_to_string(start_height,sh);

			// process id of root
			char *root_id;
			root_id = malloc(sizeof(char)*(number_of_digits(r_id)+1));
			if ( root_id == NULL) { puts("Error with malloc"); exit(1); }
			integer_to_string(r_id,root_id);

			// if height != 1 --> spliter-merger again
			if (height != 1) execlp("./spliter_merger","./spliter_merger",h,pattern,binary_file,r0,r1,skew,f0,f1,sh,root_id,NULL);
			// else leaf node
			else execlp("./leaf_node","./leaf_node",pattern,binary_file,r0,r1,skew,f0,f1,sh,root_id,NULL);
		}
		else
		{
			// we wait the two children of spliter-merger to end first
			wait(NULL);
			wait(NULL);

			// for safer reading and writing with pipes
			close(p[0]);
			close(p1[1]); close(p2[1]);

			int size1; // number of entries in first pipe
			int size2; // number of entries in second pipe
			int size; // total number of entries from both pipes
			read( p1[0], &size1, sizeof(int) );
			read( p2[0], &size2, sizeof(int) );
			size = size1 + size2;
			write( p[1],&size,sizeof(int));

			// we take the entries in the pipes that connect this process with its children
			// and write those entries to the pipe that connects this process with its parent
			Entry temp;
			int i;
			for (i=0;i<size1;i++)
			{
				read(p1[0],&temp,sizeof(Entry));
				write(p[1],&temp,sizeof(Entry));
			}
			for (i=0;i<size2;i++)
			{
				read(p2[0],&temp,sizeof(Entry));
				write(p[1],&temp,sizeof(Entry));
			}

			// same thing for the time of searchers
			read( p1[0], &size1, sizeof(int));
			read( p2[0], &size2, sizeof(int));
			size = size1+size2;
			write( p[1],&size,sizeof(int));
			double time_searcher;
			for (i=0;i<size1;i++)
			{
				read(p1[0],&time_searcher,sizeof(double));
				write(p[1],&time_searcher,sizeof(double));
			}
			for (i=0;i<size2;i++)
			{
				read(p2[0],&time_searcher,sizeof(double));
				write(p[1],&time_searcher,sizeof(double));
			}

			// end of spliter-merger - we calculate its time
			end_t = clock();
			total_t = (end_t - start_t) / (double) CLOCKS_PER_SEC;

			// finally, we write to the pipe that connects this process with its parent, the times of spliter-mergers
			int rsize;
			rsize = read( p1[0], &size1,sizeof(int));
			read( p2[0], &size2, sizeof(int));
			// if current spliter merger has leaf nodes as children
			if ( rsize == 0)
			{
				size = 1;
				write( p[1],&size,sizeof(int));
				write( p[1],&total_t,sizeof(double));
			}
			// if it has spliter-mergers as children
			else
			{
				size = size1 + size2 +1;
				write( p[1],&size,sizeof(int));
				double time_spliter_merger;
				for(i=0;i<size1;i++)
				{
					read(p1[0],&time_spliter_merger,sizeof(double));
					write(p[1],&time_spliter_merger,sizeof(double));
				}
				for(i=0;i<size2;i++)
				{
					read(p2[0],&time_spliter_merger,sizeof(double));
					write(p[1],&time_spliter_merger,sizeof(double));
				}
				write( p[1],&total_t,sizeof(double));
			}
		}
	}
}

bool get_info_from_command_line( int argc, char *argv[], int *h, char **p, char **b_file,long int r[], int fd[], int *sh, long int *r_id)
{
	//height
	*h = atoi(argv[1]);

	//pattern
	*p = malloc( sizeof(char)*(strlen(argv[2])+1) );
	strcpy(*p,argv[2]);

	//binary_file
	*b_file = malloc( sizeof(char)*(strlen(argv[3])+1) );
	strcpy(*b_file,argv[3]);

	//range
	r[0] = atoi(argv[4]);
	r[1] = atoi(argv[5]);

	//pipe
	fd[0] = atoi(argv[7]);
	fd[1] = atoi(argv[8]);

	//start_height
	*sh = atoi(argv[9]);

	// root id
	*r_id = atoi(argv[10]);

	//s
	if ( !strcmp("0",argv[6]) ) return false;
	else if( !strcmp("1",argv[6]) ) return true;
	else { puts("Error"); exit(1);}
}
