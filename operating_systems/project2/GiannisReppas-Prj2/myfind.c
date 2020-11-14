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
#include <signal.h>

#include "itoa_plus_nod.h"
#include "types.h"

// counter for the signals that the searchers send to process root
int total_sigusr2;

void sig_handler(int signo)
{
	total_sigusr2++;
}

bool get_info_from_command_line(); // we use this func to get all the info the user gave from command line //returns true if -s was given from the user
bool command_line_check(); // checks if the user's command was correct //returns true for success

int main(int argc,char *argv[])
{
	// we start counting the CPU time for root process
	time_t start_t, end_t;
	double total_t;
	start_t = clock();

	//we check if the user called the executable program correctly
	if ( !command_line_check(argc,argv) ) return -1;

	// we initialize the signal counter
	total_sigusr2 = 0;

	// we set the signal handler (and other helpful stuff) using sigaction
	struct sigaction sa;
	sa.sa_handler = sig_handler;
	sa.sa_flags = SA_RESTART;
	sigfillset(&sa.sa_mask);
	if ( sigaction(SIGUSR2,&sa,NULL) == -1) { puts("Error with sigaction") ; exit(1); }

	// we get the info the user gave from command line
	char *height; // height
	char *pattern; // pattern
	char *binary_file; // name of binary file
	bool s = get_info_from_command_line( argc, argv, &height, &pattern, &binary_file); // s==true --> user gave -s at command line

	// r_id --> root process id
	pid_t r_id;
	r_id = getpid();

	//this pipe connects root with first spliter-merger
	int p[2];
	if ( pipe(p) == -1) { puts("Error with pipe"); return -1;}

	// we create new process
	pid_t pid;
	pid = fork();
	if ( pid == 0 )
	{
		// first spliter-merger

		// we initialize the arguements that we will pass to the spliter-merger executable
		long int range[2];
		FILE *file_pointer;
		file_pointer = fopen ( binary_file,"rb+" );
		fseek(file_pointer,0,SEEK_END);
		range[0] = 0;
		range[1] = ftell(file_pointer);
		fclose(file_pointer);

		// now, we have to turn all these arguements into strings

		// s
		char *skew;
		skew = malloc(sizeof(char)*2);
		if ( skew == NULL) { puts("Error with malloc"); exit(1); }
		if (s) strcpy(skew,"1"); // 1 --> true
		else strcpy(skew,"0"); // 0 --> false

		//range
		char *r0;
		char *r1;
		r0 = malloc(sizeof(char)*(number_of_digits(range[0])+1));
		if ( r0 == NULL) { puts("Error with malloc"); exit(1); }
		r1 = malloc(sizeof(char)*(number_of_digits(range[1])+1));
		if ( r1 == NULL) { puts("Error with malloc"); exit(1); }
		integer_to_string(range[0],r0);
		integer_to_string(range[1],r1);

		//pipe
		char *p0;
		char *p1;
		p0 = malloc(sizeof(char)*(number_of_digits(p[0])+1));
		if ( p0 == NULL) { puts("Error with malloc"); exit(1); }
		p1 = malloc(sizeof(char)*(number_of_digits(p[1])+1));
		if ( p1 == NULL) { puts("Error with malloc"); exit(1); }
		integer_to_string(p[0],p0);
		integer_to_string(p[1],p1);

		// root process id
		long int temp = r_id;
		char *root_id;
		root_id = malloc(sizeof(char)*(number_of_digits(temp)+1));
		if ( root_id == NULL) { puts("Error with malloc"); exit(1); }
		integer_to_string(temp,root_id);

		// command for first spliter merger
		execlp("./spliter_merger","./spliter_merger",height,pattern,binary_file,r0,r1,skew,p0,p1,height,root_id,NULL);
	}
	else
	{
		// root

		// this part will continue execution only if child process has ended
		wait(NULL);

		// we open the file in which we will write the output
		FILE *new_fp;
		new_fp = fopen("results.txt","w");

		// we close p[1] for safer reading from the pipe
		close(p[1]);

		// we read the number of entries we will read from the pipe
		int size;
		read(p[0],&size,sizeof(int));

		// read an entry from the pipe size times
		Entry temp;
		int i;
		for (i=0;i<size;i++)
		{
			// we take an entry from the pipe
			read(p[0],&temp,sizeof(Entry));
			// we write the entry we read from the pipe to the file
			fprintf(new_fp,"%ld %s %s %s %d %s %s %f\n",temp.customer_id, temp.first_name, temp.last_name, temp.street_living, temp.number_of_street_living, temp.city_living, temp.postal_sector, temp.salary);
		}
		// no more writing to the file
		fclose(new_fp);

		// next, we read the number of searchers
		read(p[0],&size,sizeof(int));
		double total = 0; // this variable will be used to store the sum of all searchers 
		double *time_searchers; // array for every searcher
		time_searchers = malloc(sizeof(double)*size);
		if (time_searchers == NULL) { puts("Error with malloc"); exit(1); }
		for (i=0;i<size;i++)
		{
			read(p[0],&time_searchers[i],sizeof(double));
			total+=time_searchers[i];
		}
		// min max algorithm for time_searchers
		double min=time_searchers[0];
		double max=time_searchers[0];
		for(i=0;i<size;i++)
		{
			if( time_searchers[i] < min) min = time_searchers[i];
			if( time_searchers[i] > max) max = time_searchers[i];
		}
		puts("-----");
		printf("Smallest searcher's time-->%lf\n",min);
		printf("Biggest searcher's time-->%lf\n",max);
		printf("Average time of a searcher-->%lf\n",total/size);
		free(time_searchers);

		// next, we read the number of spliter_mergers
		read(p[0],&size,sizeof(int));
		total = 0;
		double *time_spliter_mergers;
		time_spliter_mergers = malloc(sizeof(double)*size);
		if (time_spliter_mergers == NULL) { puts("Error with malloc"); exit(1); }
		for(i=0;i<size;i++)
		{
			read(p[0],&time_spliter_mergers[i],sizeof(double));
			total+=time_spliter_mergers[i];
		}
		min = time_spliter_mergers[0];
		max = time_spliter_mergers[0];
		for(i=0;i<size;i++)
		{
			if( time_spliter_mergers[i] < min) min = time_spliter_mergers[i];
			if( time_spliter_mergers[i] > max) max = time_spliter_mergers[i];
		}
		puts("-----");
		printf("Smallest spliter-merger's time-->%lf\n",min);
		printf("Biggest spliter-merger's time-->%lf\n",max);
		printf("Average time of a spliter-merger-->%lf\n",total/size);
		puts("-----");
		free(time_spliter_mergers);

		pid = fork();
		if ( pid < 0) { puts("Error with fork"); exit(1);}

		if ( pid == 0)
		{
			// sort
			puts("Results:");
			free(height);
			free(pattern);
			free(binary_file);
			execlp("sort","sort","results.txt",NULL);
		}
		else
		{
			// initial

			// results of sort will be printed first
			wait(NULL);

			free(height);
			free(pattern);
			free(binary_file);

			// time of root
			end_t = clock();
			total_t = ( end_t - start_t ) / (double) CLOCKS_PER_SEC;
			puts("-----");
			printf("Total root time(turnaround time)-->%lf\n",total_t);
			puts("-----");

			// we print the number of signals that root took from searchers
			printf("Total SIGUSR2 recieved: %d\n",total_sigusr2);
			puts("-----");

			return 0;
		}
	}
}

bool get_info_from_command_line( int argc, char *argv[], char **h, char **p, char **b_file)
{
	int i;
	//search for height
	for(i=0;i<argc;i++)
	{
		if( !strcmp("-h",argv[i]) )
		{
			*h = malloc( sizeof(char)*(strlen(argv[i+1])+1));
			strcpy(*h,argv[i+1]);
			break;
		}
	}

	//search for pattern
	for(i=0;i<argc;i++)
	{
		if( !strcmp("-p",argv[i]) )
		{
			*p = malloc( sizeof(char)*(strlen(argv[i+1])+1) );
			strcpy(*p,argv[i+1]);
			break;
		}
	}

	//search for binary file
	for(i=0;i<argc;i++)
	{
		if( !strcmp("-d",argv[i]) )
		{
			*b_file = malloc( sizeof(char)*(strlen(argv[i+1])+1) );
			strcpy(*b_file,argv[i+1]);
			break;
		}
	}

	//search for -s
	for(i=0;i<argc;i++)
	{
		if( !strcmp("-s",argv[i]) )
		{
			return true;
		}
	}
	return false;
}

bool command_line_check(int argc, char **argv)
{
	// we check if the number of arguements the user gave is correct
	if ( (argc != 8)&&(argc!=7) ) { puts("Wrong number of arguements at command line"); return false; }

	//we check if the arguements the user gave are the correct ones
	char *banned; // banned is a string that will have all the arguements that exist in the command line and start with -
	banned = malloc(sizeof(char)*1);
	if (banned == NULL) { puts("Error with malloc"); return false; }
	banned[0] = '\0'; // for the time being we havent read any of them so its empty

	int j,i=1;
	int banned_length;
	char temp;
	//for every arguemnt
	while( i<argc )
	{
		if ( (!strcmp(argv[i],"-h")) || (!strcmp(argv[i],"-d")) || (!strcmp(argv[i],"-p")) || (!strcmp(argv[i],"-s")) )
		{
			// we check if the arguement appears twice at command line
			temp = argv[i][1];
			banned_length = strlen(banned);
			for(j=0;j<banned_length;j++)
				if( banned[j] == temp) { puts("Wrong arguements at command line"); free(banned); return false; }

			// we add it at banned beq we just read it
			banned_length = strlen(banned);
			banned = realloc( banned , sizeof(char) + (banned_length+sizeof(char)) );
			if ( banned == NULL ) { puts("Error with malloc"); return false; }
			banned[banned_length] = temp;
			banned[banned_length+1] = '\0';

			// Height of tree must be between 1 and 5
			if ( temp == 'h' )
			{
				int height;
				height = atoi(argv[i+1]);
				if ( ( height < 1) || (height > 5) ) { puts("Height of tree must be between 1 and 5"); free(banned); return false; }
			}
			// We check if the binary file the user gave exists
			else if ( temp == 'd' )
			{
				FILE *file_pointer;
				file_pointer  = fopen(argv[i+1],"rb");
				if ( file_pointer == NULL ) { puts("Problem with binary file"); free(banned) ; return false; }
				fclose(file_pointer);
			}

			// -s takes no extra arguements
			if ( temp == 's') i++;
			else i+=2;
		}
		else { puts("Wrong arguements at command line"); free(banned); return false; }
	}
	// so if the user gave the command ./myfind -h 4 -s -d Datafile -p pa then the final banned string (in this code line) will be hsdp
	free(banned);
	return true;
}
