#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <poll.h>

#include "itoa_plus_nod.h"

#define SECONDS 5

// functions
bool take_info_from_command_line( int argc, char **argv, int *id, char **common_dir, char **input_dir, char **mirror_dir, int *buffer_size, char **log_file); // checks if command line has correct arguements and initializes them
bool write_file_to_common_dir( int id, char *common_dir, char *input_dir, char *log_file); // goes to common_dir and creates a file with id name and process id into it
bool execute_mirror_client( int id, char *common_dir, char *input_dir, int buffer_size, char *mirror_dir, char *log_file); // executes main part of mirror client, calls writer.exe and reader.exe
bool write_files_in_fifo( DIR *dr, char *str, int fd); // takes a folder and puts all its files and subdirectories in the given fifo
bool end_mirror_client( char *mirror_dir, int id, char *common_dir); // deletes all files and directories needed
bool delete_all_folder_files( DIR *dr, char *str); // takes a folder pointer and name and deletes all its files and subdirectories
bool log_file_first( char *log_file); // write at log file that a mirror client started its execution

// global variables
bool end_check = false; // global boolean variable that turns true when mirror client receives a SIGINT or a SIGQUIT from the user
bool sigusr1_check; // global boolean variable that turns true when mirror client receives a SIGUSR1 from reader
bool sigusr2_check; // global boolean variable that turns true when mirror client receives a SIGUSR2 from reader
int sigusr2_counter; // global integer variable that gets increased every time mirror client receives a SIGUSR2 from reader. However, it turns to 0 every time we find a new id to connect with the one of mirror client

// signal handler
void handler( int signum)
{
	if ( (signum==SIGINT) || (signum==SIGQUIT) )
	{
		end_check = true;
	}
	else if (signum==SIGUSR1)
	{
		sigusr1_check=true;
	}
	else if (signum==SIGUSR2)
	{
		sigusr2_counter++;
		sigusr2_check=true;
	}
}

// main
int main( int argc, char *argv[])
{
	// set signal handler for some signals
	signal( SIGINT, handler); signal( SIGQUIT, handler); signal( SIGALRM, handler); signal( SIGUSR1, handler); signal( SIGUSR2, handler);

	int id;
	char *common_dir;
	char *input_dir;
	int buffer_size;
	char *mirror_dir;
	char *log_file;

	// take info from command line and print error if something went wrong
	if ( !take_info_from_command_line( argc, argv, &id, &common_dir, &input_dir, &mirror_dir, &buffer_size, &log_file) ) return 1;

	// write at log file that a mirror client started its execution
	if ( !log_file_first(log_file) ) return 1;

	// write a file with .id extension to commondir
	if ( !write_file_to_common_dir(id, common_dir, input_dir, log_file) ) return 1;

	// main part of mirror client - most of its work
	if ( !execute_mirror_client( id, common_dir, input_dir, buffer_size, mirror_dir, log_file) ) return 1;

	// the end - delete mirror_dir and the id file in common dir
	if ( !end_mirror_client( mirror_dir, id, common_dir) ) return 1;
puts("");
	return 0;
}

bool execute_mirror_client( int id, char *common_dir, char *input_dir, int buffer_size, char *mirror_dir, char *log_file)
{
	// visited is an array with the ids that have been read in common_dir
	int *visited=NULL;
	int visitedLength = 0;
	int currentId;
	char *IdNumber;
	int pid_1, pid_2;
	bool check;
	int i=0, j, m;
	char *pathname;
	FILE *fp;
	char *char_id;
	while ( 1 )
	{
		// after SECONDS seconds pass
		alarm(SECONDS); pause();
		if ( end_check ) break; // SIGINT or SIGQUIT means end of prog

		// for every file or directory we search in common_dir, id files have a number
		// this number will be stored in IdNumber
		// we initialize IdNumber
		IdNumber = malloc(sizeof(char));
		if ( IdNumber == NULL ) { puts("Error with malloc"); return false; }
		IdNumber[0] = '\0';
		int IdNumberLength = 1;

		// for every file or directory in common_dir
		struct dirent *de;
		DIR *dr = opendir(common_dir);
		if ( dr == NULL) { puts("Error with opendir"); return false; }
		while ( (de = readdir(dr)) != NULL)
		{
			if ( !strcmp(".",de->d_name) || !strcmp("..",de->d_name) ) continue;

			// IdNumber
			m = 0;
			while( de->d_name[m] != '.')
			{
				IdNumber = realloc(IdNumber,++IdNumberLength);
				if ( IdNumber == NULL) { puts("Error with realloc"); return false;}
				IdNumber[m] = de->d_name[m];
				IdNumber[m+1] = '\0';

				m++;
			}
			currentId = atoi(IdNumber);

			// check is a boollean variable that is false if the current id file is non visited
			// initialize check
			check = false;
			for ( j=0; j<visitedLength; j++)
			{
				if ( visited[j] == currentId)
				{
					check = true;
					break;
				}
			}
			if ( ( currentId == 0) || (currentId == id) ) check = true;

			if ( !check) // if the id file is a new one
			{
				puts("New id found");

				sigusr1_check = false;
				sigusr2_check = true;
				sigusr2_counter = 0;

				// add it to visited id files
				visited = realloc( visited, ++visitedLength);
				if ( visited == NULL) { puts("Error with realloc"); return false;}
				visited[visitedLength-1]=currentId;

				while( (sigusr2_check) && (sigusr2_counter<3) ) // this is a loop that will run maximum 3 times, if theres an error with reader that sent a SIGUSR2 signal
				{
					sigusr2_check = false;
					if (sigusr2_counter != 0) printf("\nTrying again to create - current id --> %d\n",id);

					// algorithm - first two children to make the fifos - father waits
					pid_1 = fork();
					if ( pid_1 != 0) // father
					{
						pid_2 = fork();
						if ( pid_2 != 0) // father
						{
							wait(NULL); wait(NULL);

							if ( sigusr1_check == true) { printf("Been waiting for 30 seconds and pipe id%d_to_id%d is still empty - pipe deleted\n",currentId,id); break; }
						}
						else // child2 (reader fifo)
						{
							char *bs; bs = malloc( number_of_digits(buffer_size) + sizeof(char));
							if ( bs == NULL) { puts("Error with malloc"); return false; }
							integer_to_string( buffer_size, bs);

							char *char_id; char_id = malloc( number_of_digits(id) + sizeof(char));
							if ( char_id == NULL) { puts("Error with malloc"); return false; }
							integer_to_string( id, char_id);

							execlp("./reader","./reader", IdNumber, char_id, common_dir, input_dir, mirror_dir, bs, log_file, NULL);
							puts("Error executing reader"); exit(1);
						}
					}
					else // child1 (writer fifo)
					{
						if ( sigusr2_counter == 0)
						{
							char *bs; bs = malloc( number_of_digits(buffer_size) + sizeof(char));
							if ( bs == NULL) { puts("Error with malloc"); return false; }
							integer_to_string( buffer_size, bs);

							char *char_id; char_id = malloc( number_of_digits(id) + sizeof(char));
							if ( char_id == NULL) { puts("Error with malloc"); return false; }
							integer_to_string( id, char_id);

							execlp("./writer","./writer", IdNumber, char_id, common_dir, input_dir, mirror_dir, bs, log_file, NULL);
							puts("Error executing writer"); exit(1);
						}
						else
						{
							char *bs; bs = malloc( number_of_digits(buffer_size) + sizeof(char));
							if ( bs == NULL) { puts("Error with malloc"); return false; }
							integer_to_string( buffer_size, bs);

							char *char_id; char_id = malloc( number_of_digits(id) + sizeof(char));
							if ( char_id == NULL) { puts("Error with malloc"); return false; }
							integer_to_string( id, char_id);

							char *new_input_dir=malloc(sizeof(char));
							if ( new_input_dir == NULL) { puts("Error with malloc"); return false; }
							new_input_dir[0] = '\0';
							char *new_log_file=malloc(sizeof(char));
							if ( new_log_file == NULL) { puts("Error with malloc"); return false; }
							new_log_file[0] = '\0';
							char *filename=malloc(sizeof(char)*strlen(common_dir) + sizeof(char) + sizeof(char)*strlen(IdNumber) + sizeof(char) + sizeof(char)*2 + sizeof(char));
							if ( filename == NULL) { puts("Error with malloc"); return false; }
							strcpy(filename,common_dir);
							strcat(filename,"/");
							strcat(filename,IdNumber);
							strcat(filename,".id");

							FILE *temp; temp = fopen(filename,"r");
							if ( temp == NULL) { puts("Error with fopen"); return false; }
							free(filename);

							int c;
							while( (c=fgetc(temp)) != '\n') ;
							while( (c=fgetc(temp)) != '\n')
							{
								new_input_dir = realloc( new_input_dir, strlen(new_input_dir) + sizeof(char)*2);
								if ( new_input_dir == NULL) { puts("Error with realloc"); return false; }
								new_input_dir[strlen(new_input_dir)+1]='\0';
								new_input_dir[strlen(new_input_dir)]=c;
							}
							while( (c=fgetc(temp)) != EOF)
							{
								new_log_file = realloc( new_log_file, strlen(new_log_file) + sizeof(char)*2);
								if ( new_log_file == NULL) { puts("Error with realloc"); return false; }
								new_log_file[strlen(new_log_file)+1]='\0';
								new_log_file[strlen(new_log_file)]=c;
							}

							fclose(temp);

							execlp("./writer","./writer", char_id, IdNumber, common_dir, new_input_dir, mirror_dir, bs, new_log_file, NULL);
							puts("Error executing writer"); exit(1);
						}
					}
				}

				if ( sigusr2_counter >= 3) puts("Tried 3 times to create and all of them failed - no folder for current client");

				FILE *log_file_fp=fopen(log_file,"a");
				if ( log_file_fp == NULL) { puts("Error with fopen"); return false; }

				if ( sigusr1_check || (sigusr2_counter>=3) ) fprintf( log_file_fp, "5) %d-%d no\n",id,currentId );
				else fprintf( log_file_fp, "5) %d %d yes\n",id,currentId );

				fclose(log_file_fp);
			}
			free(IdNumber);

			// initialize IdNumber for the next file or directory
			IdNumber = malloc(sizeof(char));
			if ( IdNumber == NULL) { puts("Error with malloc"); return false; }
			IdNumber[0] = '\0';
			IdNumberLength = 1;


		}
		free(IdNumber);

		// close common_dir
		if ( closedir(dr) < 0) { puts("Error with closedir"); return false; }

		if ( end_check) break;

		// if an id disappeared, delete its id folder from mirror
		for ( i=0; i<visitedLength; i++)
		{
			char_id = malloc( sizeof(char)*number_of_digits(visited[i]) + sizeof(char));
			if ( char_id == NULL) { puts("Error with malloc"); return false; }
			integer_to_string( visited[i], char_id);

			pathname = malloc( strlen(common_dir) + sizeof(char) + strlen(char_id) + sizeof(char) + sizeof(char)*2 + sizeof(char) );
			if (pathname == NULL) { puts("Error with malloc"); return 1; }
			strcpy(pathname,common_dir);
			strcat(pathname,"/");
			strcat(pathname,char_id);
			strcat(pathname,".");
			strcat(pathname,"id");

			fp = fopen(pathname,"r");
			if (fp == NULL)
			{
				free(pathname);
				pathname = malloc( strlen(mirror_dir) + sizeof(char) + strlen(char_id) + sizeof(char));
				if ( pathname == NULL) { puts("Error with malloc"); return false; }
				strcpy(pathname,mirror_dir);
				strcat(pathname,"/");
				strcat(pathname,char_id);

				dr = opendir(pathname);
				if ( dr != NULL)
				{
					if ( !delete_all_folder_files( dr, pathname) ) return false;
					if ( closedir(dr) < 0) { puts("Error with closedir"); return false; }
					if ( rmdir(pathname) < 0) { puts("Error with rmdir"); return false; }
				}
			}
			else fclose(fp);

			free(char_id);
			free(pathname);
		}

		printf("\nJust checked, will check again in %d seconds\n",SECONDS);
	}
	if ( visited != NULL) free(visited);

	return true;
}

bool log_file_first( char *log_file)
{
	// each line in log file represents a stat
	// the line that represents that a client left the system starts with 6)
	// no more info needed after this, since we read 6) in log file, we know that a client left the system

	FILE *fp = fopen(log_file,"a");
	if (fp == NULL) { puts("Error with malloc"); return false; }

	fprintf( fp, "6)\n");

	fclose(fp);

	return true;
}

bool end_mirror_client( char *mirror_dir, int id, char *common_dir)
{
	// delete mirror_dir
	DIR *dr_end; dr_end = opendir(mirror_dir);
	if ( dr_end == NULL) { puts("Error with opendir"); return false; }
	if ( !delete_all_folder_files(dr_end,mirror_dir) ) return false;
	if ( closedir(dr_end) < 0) { puts("Error with closedir"); return false; }
	if ( rmdir(mirror_dir) < 0) { puts("Error with rmdir"); return false; }

	// delete file from common_dir
	char *char_id = malloc( number_of_digits(id) + sizeof(char));
	integer_to_string( id, char_id);
	char *last; last = malloc( strlen(common_dir) + sizeof(char) + strlen(char_id) + sizeof(char) + sizeof(char)*2 + sizeof(char)); if ( last == NULL) { puts("Error with malloc"); return false; }
	strcpy( last, common_dir);
	strcat( last, "/");
	strcat( last, char_id);
	strcat( last, ".");
	strcat( last, "id");
	if ( remove( last) < 0) { puts("Error with remove"); return false; }
	free(last);
	free(char_id);

	return true;
}

bool delete_all_folder_files( DIR *dr, char *str)
{
	char *pathname; pathname = malloc( sizeof(char)*strlen(str) + sizeof(char));
	if ( pathname == NULL) { puts("Error with malloc"); return false; }
	strcpy(pathname,str);
	pathname = realloc( pathname, strlen(pathname)+sizeof(char)*2);
	if ( pathname == NULL) { puts("Error with realloc"); return false; }
	pathname[strlen(pathname)+1] = '\0';
	pathname[strlen(pathname)] = '/';

	// read every folder file
	// if its a file, just remove it
	// if its a directory, call this function for this directory and then delete the directory
	char *fullname;
	struct dirent *de_end;
	DIR *dr_end;
	while ( (de_end = readdir(dr)) != NULL)
	{
		if ( !strcmp(de_end->d_name,".") || !strcmp(de_end->d_name,"..")) continue;

		fullname = malloc( strlen(pathname) + strlen(de_end->d_name) + sizeof(char));
		if ( fullname == NULL) { puts("Error with malloc"); return false; }
		strcpy( fullname, pathname);
		strcat( fullname, de_end->d_name);

		dr_end = opendir(fullname);

		if ( dr_end != NULL ) // fullname is a directory
		{
			if ( !delete_all_folder_files(dr_end,fullname)) return false;
			if ( closedir(dr_end) < 0) { puts("Error with closedir"); return false; }
			if ( rmdir(fullname) < 0) { puts("Error with rmdir"); return false; }
		}
		else // fullname is a file
		{
			if ( remove(fullname) < 0) { puts("error with remove"); return false; }
		}

		free(fullname);
	}

	free(pathname);
	return true;
}

bool write_file_to_common_dir( int id, char *common_dir, char *input_dir, char *log_file)
{
	// pathname will have the full filename
	// for example, if id is 1, then pathname is common_dir/1.id

	// char_p_id will have the process id of the current process

	int fd;
	int p_id = getpid();
	char *pathname;
	char *char_id; // char_id is id with characters, for example if id is 1 then char_id is "1"
	char *char_p_id;

	// initialize pathname
	char_id = malloc( number_of_digits(id)*sizeof(char) + sizeof(char));
	if ( char_id == NULL) { puts("Error with malloc"); return false; }
	integer_to_string( id, char_id);

	pathname = malloc( strlen(common_dir) + sizeof(char) + strlen(char_id) + sizeof(char) + sizeof(char)*2 + sizeof(char) );
	if ( pathname == NULL) { puts("Error with malloc"); return false; }
	strcpy(pathname,common_dir);
	strcat(pathname,"/");
	strcat(pathname,char_id);
	strcat(pathname,".");
	strcat(pathname,"id");

	// initialize process id
	char_p_id = malloc( number_of_digits(getpid())*sizeof(char) + sizeof(char));
	if ( char_p_id == NULL) { puts("Error with malloc"); return false; }
	integer_to_string( getpid(), char_p_id);

	if ( access( pathname, F_OK) != -1) { puts("Id given already exists"); return false; }

	// create the file
	FILE *temp;
	temp = fopen(pathname,"w+");
	if ( temp == NULL) { puts("Error with fopen"); return false; }

	fprintf( temp,"%s\n",char_p_id);
	fprintf( temp,"%s\n",input_dir);
	fprintf( temp,"%s",log_file);

	fclose( temp);

	free(char_id);
	free(pathname);
	free(char_p_id);

	return true;
}

bool take_info_from_command_line( int argc, char **argv, int *id, char **common_dir, char **input_dir, char **mirror_dir, int *buffer_size, char **log_file)
{
	// the exact number of arguemnts must be 13
	if ( argc != 13) { puts("Error with number of arguements from command line"); return false; }

	// in odd places of arguemnts in command line we do flag checking
	// we also check if each flag appears for first time in command line
	// in even places of arguemnts in command line we do arguement checking, based on the previous flag

	DIR *temp_dir;
	FILE *temp_file;
	int i = 1;
	while ( i < 13 )
	{
		if ( i == 1)
		{
			if ( !strcmp(argv[1],"-n") ) ;
			else if ( !strcmp(argv[1],"-c") ) ;
			else if ( !strcmp(argv[1],"-i") ) ;
			else if ( !strcmp(argv[1],"-m") ) ;
			else if ( !strcmp(argv[1],"-b") ) ;
			else if ( !strcmp(argv[1],"-l") ) ;
			else { puts("Error with 1st arguement from command line"); return false; }
		}
		else if ( i == 2)
		{
			if ( !strcmp(argv[i-1],"-n") )
			{
				if ( atoi(argv[i]) <= 0) { puts("Error with 2nd arguement given from command line"); return false; }

				*id = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-c") )
			{
				temp_dir = opendir(argv[i]);
				if ( temp_dir == NULL)
				{
					if ( mkdir(argv[i],0777) != 0) { puts("Error with mkdir1"); return false; }
				}
				else if ( closedir(temp_dir) != 0) { puts("Error with close dir"); return false; }

				*common_dir = argv[i];
			}
			else if ( !strcmp(argv[i-1],"-i") )
			{
				temp_dir = opendir(argv[i]);
				if ( temp_dir == NULL) { puts("Error with 2nd arguement given from command line"); return false; }
				if ( closedir(temp_dir) != 0) { puts("Error with close dir"); return false; }

				*input_dir = argv[i];
			}
			else if ( !strcmp(argv[i-1],"-m") )
			{
				temp_dir = opendir(argv[i]);
				if ( temp_dir != NULL) { puts("Error with 2nd arguement given from command line"); return false; }
				if ( mkdir(argv[i],0777) != 0) { puts("Error with mkdir"); return false; }

				*mirror_dir = argv[i];
			}
			else if ( !strcmp(argv[i-1],"-b") )
			{
				if ( atoi(argv[i]) <= 0) { puts("Error with 2nd arguement given from command line"); return false; }

				*buffer_size = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-l") )
			{
				temp_file = fopen(argv[i],"w+");
				if ( temp_file == NULL) { puts("Error with log file"); return false; }
				fclose(temp_file);

				*log_file = argv[i];
			}
		}
		else if ( i == 3)
		{
			if ( !strcmp(argv[3],"-n") && strcmp(argv[3],argv[1]) ) ;
			else if ( !strcmp(argv[3],"-c") && strcmp(argv[3],argv[1]) ) ;
			else if ( !strcmp(argv[3],"-i") && strcmp(argv[3],argv[1]) ) ;
			else if ( !strcmp(argv[3],"-m") && strcmp(argv[3],argv[1]) ) ;
			else if ( !strcmp(argv[3],"-b") && strcmp(argv[3],argv[1]) ) ;
			else if ( !strcmp(argv[3],"-l") && strcmp(argv[3],argv[1]) ) ;
			else { puts("Error with 3rd arguement from command line"); return false; }
		}
		else if ( i == 4)
		{
			if ( !strcmp(argv[i-1],"-n") )
			{
				if ( atoi(argv[i]) <= 0) { puts("Error with 4th arguement given from command line"); return false; }

				*id = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-c") )
			{
				temp_dir = opendir(argv[i]);
				if ( temp_dir == NULL)
				{
					if ( mkdir(argv[i],0777) != 0) { puts("Error with mkdir2"); return false; }
				}
				else if ( closedir(temp_dir) != 0) { puts("Error with close dir"); return false; }

				*common_dir = argv[i];
			}
			else if ( !strcmp(argv[i-1],"-i") )
			{
				temp_dir = opendir(argv[i]);
				if ( temp_dir == NULL) { puts("Error with 4th arguement given from command line"); return false; }
				if ( closedir(temp_dir) != 0) { puts("Error with close dir"); return false; }

				*input_dir = argv[i];
			}
			else if ( !strcmp(argv[i-1],"-m") )
			{
				temp_dir = opendir(argv[i]);
				if ( temp_dir != NULL) { puts("Error with 4th arguement given from command line"); return false; }
				if ( mkdir(argv[i],0777) != 0) { puts("Error with mkdir"); return false; }

				*mirror_dir = argv[i];
			}
			else if ( !strcmp(argv[i-1],"-b") )
			{
				if ( atoi(argv[i]) <= 0) { puts("Error with 4th arguement given from command line"); return false; }

				*buffer_size = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-l") )
			{
				temp_file = fopen(argv[i],"w+");
				if ( temp_file == NULL) { puts("Error with log file"); return false; }
				fclose(temp_file);

				*log_file = argv[i];
			}
		}
		else if ( i == 5)
		{
			if ( !strcmp(argv[5],"-n") && strcmp(argv[5],argv[3]) && strcmp(argv[5],argv[1]) ) ;
			else if ( !strcmp(argv[5],"-c") &&  strcmp(argv[5],argv[3]) && strcmp(argv[5],argv[1]) ) ;
			else if ( !strcmp(argv[5],"-i") &&  strcmp(argv[5],argv[3]) && strcmp(argv[5],argv[1]) ) ;
			else if ( !strcmp(argv[5],"-m") &&  strcmp(argv[5],argv[3]) && strcmp(argv[5],argv[1]) ) ;
			else if ( !strcmp(argv[5],"-b") &&  strcmp(argv[5],argv[3]) && strcmp(argv[5],argv[1]) ) ;
			else if ( !strcmp(argv[5],"-l") &&  strcmp(argv[5],argv[3]) && strcmp(argv[5],argv[1]) ) ;
			else { puts("Error with 5th arguement from command line"); return false; }
		}
		else if ( i == 6)
		{
			if ( !strcmp(argv[i-1],"-n") )
			{
				if ( atoi(argv[i]) <= 0) { puts("Error with 6th arguement given from command line"); return false; }

				*id = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-c") )
			{
				temp_dir = opendir(argv[i]);
				if ( temp_dir == NULL)
				{
					if ( mkdir(argv[i],0777) != 0) { puts("Error with mkdir3"); return false; }
				}
				else if ( closedir(temp_dir) != 0) { puts("Error with close dir"); return false; }

				*common_dir = argv[i];
			}
			else if ( !strcmp(argv[i-1],"-i") )
			{
				temp_dir = opendir(argv[i]);
				if ( temp_dir == NULL) { puts("Error with 6th arguement given from command line"); return false; }
				if ( closedir(temp_dir) != 0) { puts("Error with close dir"); return false; }

				*input_dir = argv[i];
			}
			else if ( !strcmp(argv[i-1],"-m") )
			{
				temp_dir = opendir(argv[i]);
				if ( temp_dir != NULL) { puts("Error with 6th arguement given from command line"); return false; }
				if ( mkdir(argv[i],0777) != 0) { puts("Error with mkdir"); return false; }

				*mirror_dir = argv[i];
			}
			else if ( !strcmp(argv[i-1],"-b") )
			{
				if ( atoi(argv[i]) <= 0) { puts("Error with 6th arguement given from command line"); return false; }

				*buffer_size = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-l") )
			{
				temp_file = fopen(argv[i],"w+");
				if ( temp_file == NULL) { puts("Error with log file"); return false; }
				fclose(temp_file);

				*log_file = argv[i];
			}
		}
		else if ( i == 7)
		{
			if ( !strcmp(argv[7],"-n") && strcmp(argv[7],argv[5]) && strcmp(argv[7],argv[3]) && strcmp(argv[7],argv[1]) ) ;
			else if ( !strcmp(argv[7],"-c") && strcmp(argv[7],argv[5]) && strcmp(argv[7],argv[3]) && strcmp(argv[7],argv[1]) ) ;
			else if ( !strcmp(argv[7],"-i") && strcmp(argv[7],argv[5]) && strcmp(argv[7],argv[3]) && strcmp(argv[7],argv[1]) ) ;
			else if ( !strcmp(argv[7],"-m") && strcmp(argv[7],argv[5]) && strcmp(argv[7],argv[3]) && strcmp(argv[7],argv[1]) ) ;
			else if ( !strcmp(argv[7],"-b") && strcmp(argv[7],argv[5]) && strcmp(argv[7],argv[3]) && strcmp(argv[7],argv[1]) ) ;
			else if ( !strcmp(argv[7],"-l") && strcmp(argv[7],argv[5]) && strcmp(argv[7],argv[3]) && strcmp(argv[7],argv[1]) ) ;
			else { puts("Error with 7th arguement from command line"); return false; }
		}
		else if ( i == 8)
		{
			if ( !strcmp(argv[i-1],"-n") )
			{
				if ( atoi(argv[i]) <= 0) { puts("Error with 8th arguement given from command line"); return false; }

				*id = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-c") )
			{
				temp_dir = opendir(argv[i]);
				if ( temp_dir == NULL)
				{
					if ( mkdir(argv[i],0777) != 0) { puts("Error with mkdir4"); return false; }
				}
				else if ( closedir(temp_dir) != 0) { puts("Error with close dir"); return false; }

				*common_dir = argv[i];
			}
			else if ( !strcmp(argv[i-1],"-i") )
			{
				temp_dir = opendir(argv[i]);
				if ( temp_dir == NULL) { puts("Error with 8th arguement given from command line"); return false; }
				if ( closedir(temp_dir) != 0) { puts("Error with close dir"); return false; }

				*input_dir = argv[i];
			}
			else if ( !strcmp(argv[i-1],"-m") )
			{
				temp_dir = opendir(argv[i]);
				if ( temp_dir != NULL) { puts("Error with 8th arguement given from command line"); return false; }
				if ( mkdir(argv[i],0777) != 0) { puts("Error with mkdir"); return false; }

				*mirror_dir = argv[i];
			}
			else if ( !strcmp(argv[i-1],"-b") )
			{
				if ( atoi(argv[i]) <= 0) { puts("Error with 8th arguement given from command line"); return false; }

				*buffer_size = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-l") )
			{
				temp_file = fopen(argv[i],"w+");
				if ( temp_file == NULL) { puts("Error with log file"); return false; }
				fclose(temp_file);

				*log_file = argv[i];
			}
		}
		else if ( i == 9)
		{
			if ( !strcmp(argv[9],"-n") && strcmp(argv[9],argv[7]) && strcmp(argv[9],argv[5]) && strcmp(argv[9],argv[3]) && strcmp(argv[9],argv[1]) ) ;
			else if ( !strcmp(argv[9],"-c") && strcmp(argv[9],argv[7]) && strcmp(argv[9],argv[5]) && strcmp(argv[9],argv[3]) && strcmp(argv[9],argv[1]) ) ;
			else if ( !strcmp(argv[9],"-i") && strcmp(argv[9],argv[7]) && strcmp(argv[9],argv[5]) && strcmp(argv[9],argv[3]) && strcmp(argv[9],argv[1]) ) ;
			else if ( !strcmp(argv[9],"-m") && strcmp(argv[9],argv[7]) && strcmp(argv[9],argv[5]) && strcmp(argv[9],argv[3]) && strcmp(argv[9],argv[1]) ) ;
			else if ( !strcmp(argv[9],"-b") && strcmp(argv[9],argv[7]) && strcmp(argv[9],argv[5]) && strcmp(argv[9],argv[3]) && strcmp(argv[9],argv[1]) ) ;
			else if ( !strcmp(argv[9],"-l") && strcmp(argv[9],argv[7]) && strcmp(argv[9],argv[5]) && strcmp(argv[9],argv[3]) && strcmp(argv[9],argv[1]) ) ;
			else { puts("Error with 9th arguement from command line"); return false; }
		}
		else if ( i == 10)
		{
			if ( !strcmp(argv[i-1],"-n") )
			{
				if ( atoi(argv[i]) <= 0) { puts("Error with 10th arguement given from command line"); return false; }

				*id = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-c") )
			{
				temp_dir = opendir(argv[i]);
				if ( temp_dir == NULL)
				{
					if ( mkdir(argv[i],0777) != 0) { puts("Error with mkdir5"); return false; }
				}
				else if ( closedir(temp_dir) != 0) { puts("Error with close dir"); return false; }

				*common_dir = argv[i];
			}
			else if ( !strcmp(argv[i-1],"-i") )
			{
				temp_dir = opendir(argv[i]);
				if ( temp_dir == NULL) { puts("Error with 10th arguement given from command line"); return false; }
				if ( closedir(temp_dir) != 0) { puts("Error with close dir"); return false; }

				*input_dir = argv[i];
			}
			else if ( !strcmp(argv[i-1],"-m") )
			{
				temp_dir = opendir(argv[i]);
				if ( temp_dir != NULL) { puts("Error with 10th arguement given from command line"); return false; }
				if ( mkdir(argv[i],0777) != 0) { puts("Error with mkdir"); return false; }

				*mirror_dir = argv[i];
			}
			else if ( !strcmp(argv[i-1],"-b") )
			{
				if ( atoi(argv[i]) <= 0) { puts("Error with 10th arguement given from command line"); return false; }

				*buffer_size = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-l") )
			{
				temp_file = fopen(argv[i],"w+");
				if ( temp_file == NULL) { puts("Error with log file"); return false; }
				fclose(temp_file);

				*log_file = argv[i];
			}
		}
		else if ( i == 11)
		{
			if ( !strcmp( argv[11],"-n") && strcmp(argv[11],argv[9]) && strcmp(argv[11],argv[7]) && strcmp(argv[11],argv[5]) && strcmp(argv[11],argv[3]) && strcmp(argv[11],argv[1]) ) ;
			else if ( !strcmp( argv[11],"-c") && strcmp(argv[11],argv[9]) && strcmp(argv[11],argv[7]) && strcmp(argv[11],argv[5]) && strcmp(argv[11],argv[3]) && strcmp(argv[11],argv[1]) )  ;
			else if ( !strcmp( argv[11],"-i") && strcmp(argv[11],argv[9]) && strcmp(argv[11],argv[7]) && strcmp(argv[11],argv[5]) && strcmp(argv[11],argv[3]) && strcmp(argv[11],argv[1]) ) ;
			else if ( !strcmp( argv[11],"-m") && strcmp(argv[11],argv[9]) && strcmp(argv[11],argv[7]) && strcmp(argv[11],argv[5]) && strcmp(argv[11],argv[3]) && strcmp(argv[11],argv[1]) ) ;
			else if ( !strcmp( argv[11],"-b") && strcmp(argv[11],argv[9]) && strcmp(argv[11],argv[7]) && strcmp(argv[11],argv[5]) && strcmp(argv[11],argv[3]) && strcmp(argv[11],argv[1]) ) ;
			else if ( !strcmp( argv[11],"-l") && strcmp(argv[11],argv[9]) && strcmp(argv[11],argv[7]) && strcmp(argv[11],argv[5]) && strcmp(argv[11],argv[3]) && strcmp(argv[11],argv[1]) ) ;
			else { puts("Error with 11th arguement from command line"); return false; }
		}
		else if ( i == 12)
		{
			if ( !strcmp(argv[i-1],"-n") )
			{
				if ( atoi(argv[i]) <= 0) { puts("Error with 12th arguement given from command line"); return false; }

				*id = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-c") )
			{
				temp_dir = opendir(argv[i]);
				if ( temp_dir == NULL)
				{
					if ( mkdir(argv[i],0777) != 0) { puts("Error with mkdir6"); return false; }
				}
				else if ( closedir(temp_dir) != 0) { puts("Error with close dir"); return false; }

				*common_dir = argv[i];
			}
			else if ( !strcmp(argv[i-1],"-i") )
			{
				temp_dir = opendir(argv[i]);
				if ( temp_dir == NULL) { puts("Error with 12th arguement given from command line"); return false; }
				if ( closedir(temp_dir) != 0) { puts("Error with close dir"); return false; }

				*input_dir = argv[i];
			}
			else if ( !strcmp(argv[i-1],"-m") )
			{
				temp_dir = opendir(argv[i]);
				if ( temp_dir != NULL) { puts("Error with 12th arguement given from command line"); return false; }
				if ( mkdir(argv[i],0777) != 0) { puts("Error with mkdir"); return false; }

				*mirror_dir = argv[i];
			}
			else if ( !strcmp(argv[i-1],"-b") )
			{
				if ( atoi(argv[i]) <= 0) { puts("Error with 12th arguement given from command line"); return false; }

				*buffer_size = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-l") )
			{
				temp_file = fopen(argv[i],"w+");
				if ( temp_file == NULL) { puts("Error with log file"); return false; }
				fclose(temp_file);

				*log_file = argv[i];
			}
		}

		i++;
	}

	return true;
}
