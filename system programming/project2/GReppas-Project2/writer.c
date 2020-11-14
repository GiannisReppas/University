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

void take_info_from_command_line( int argc, char **argv, int *new_id, int *id, char **common_dir, char **input_dir, char **mirror_dir, int *buffer_size, char **log_file);
bool write_files_in_fifo( DIR *dr, char *str, int fd, int *butes_written, int *files_written);
int get_other_pid( DIR *dr, int new_id, char *common_dir);

int main( int argc, char *argv[])
{
	int new_id;
	int id;
	char *common_dir;
	char *input_dir;
	char *mirror_dir;
	int buffer_size;
	char *log_file;

	take_info_from_command_line( argc, argv, &new_id, &id, &common_dir, &input_dir, &mirror_dir, &buffer_size, &log_file);

	// make fifo
	char *ida; char *idb;
	ida = malloc( number_of_digits(id)+sizeof(char)); if ( ida == NULL) { puts("Error with malloc"); return -1; }
	integer_to_string( id, ida);
	idb = malloc( number_of_digits(new_id)+sizeof(char)); if ( idb == NULL) { puts("Error with malloc"); return -1; }
	integer_to_string( new_id, idb);

	char *pathname;
	pathname = malloc( strlen(common_dir)+sizeof(char)+sizeof(char)*2+number_of_digits(id)+sizeof(char)*4+sizeof(char)*2+number_of_digits(new_id)+sizeof(char)*5+sizeof(char));
	if ( pathname == NULL) { puts("Error with malloc"); return -1; }
	strcpy( pathname, common_dir);
	strcat( pathname, "/");
	strcat( pathname, "id");
	strcat( pathname, ida);
	strcat( pathname, "_to_");
	strcat( pathname, "id");
	strcat( pathname, idb);
	strcat( pathname, ".fifo");

	mkfifo(pathname, S_IRWXU | S_IRWXG | S_IRWXO );

	// open fifo
	int fd;	if ( (fd=open( pathname, O_WRONLY)) < 0) { puts("Error with open"); return -1; }

	// write every file and subdirectory of input_dir in fifo
	int bytes_written=0;
	int files_written=0;

	DIR *dr = opendir( input_dir);
	if ( dr == NULL) { puts("Error with open_dir"); return -1; }
sleep(30);
	if ( !write_files_in_fifo(dr,input_dir,fd,&bytes_written,&files_written)) return -1;

	// finished writing files in sender fifo
	short int int_temp1=0;
	if ( write( fd, &int_temp1, sizeof(short int)) < 0) { puts("Error with write"); return -1; }

	bytes_written += sizeof(short int);

	// close sender fifo
	if ( close(fd) < 0) { puts("Error with close"); return -1; }

	// close input_dir
	if ( closedir( dr) < 0) { puts("Error with closedir"); return -1; }

	// write to log_file
	FILE *log_file_fp = fopen(log_file,"a");
	if ( log_file_fp == NULL) { puts("Error with fopen"); return 1; }

	fprintf( log_file_fp, "1) %d\n", bytes_written);
	fprintf( log_file_fp, "2) %d\n", files_written);

	fclose(log_file_fp);

	free(pathname); free(ida); free(idb);

	return 0;
}

bool write_files_in_fifo( DIR *dr, char *str, int fd, int *bytes_written, int *files_written)
{
	char *pathname; pathname = malloc( sizeof(char)*strlen(str) + sizeof(char));
	if ( pathname == NULL) { puts("Error with malloc"); return false; }
	strcpy(pathname,str);
	pathname = realloc( pathname, strlen(pathname)+sizeof(char)*2);
	if ( pathname == NULL) { puts("Error with realloc"); return false; }
	pathname[strlen(pathname)+1] = '\0';
	pathname[strlen(pathname)] = '/';

	short int int_temp1;
	int int_temp2;

	char *fullname;
	struct dirent *de;
	DIR *dr_2;
	while ( (de = readdir(dr)) != NULL)
	{
		if ( !strcmp(de->d_name,".") || !strcmp(de->d_name,"..")) continue;

		fullname = malloc( strlen(pathname) + strlen(de->d_name) + sizeof(char));
		if ( fullname == NULL) { puts("Error with malloc"); return false; }
		strcpy( fullname, pathname);
		strcat( fullname, de->d_name);

		dr_2 = opendir(fullname);

		if( dr_2 != NULL) // directory
		{
			// 2 bytes - length of name of the directory
			int_temp1 = strlen( fullname);
			if ( write( fd, &int_temp1, sizeof(short int)) < 0) { puts("Error with write"); return false; }

			*bytes_written += sizeof(short int);

			// name of the directory
			if ( write( fd, fullname, strlen(fullname)) < 0) { puts("Error with write"); return false; }

			*bytes_written += strlen(fullname);

			// same function for the subdirectory
			if ( !write_files_in_fifo( dr_2, fullname, fd, bytes_written, files_written)) return false;

			if ( closedir(dr_2) < 0) { puts("Error with closedir1"); return false; }
		}
		else // file
		{
			// 2 bytes - length of name of the file
			int_temp1 = strlen( fullname);
			if ( write( fd, &int_temp1, sizeof(short int) ) < 0) { puts("Error with write"); return false; }

			*bytes_written += sizeof(short int);

			// name of the file
			if ( write( fd, fullname, strlen(fullname)) < 0) { puts("Error with write"); return false; }

			*bytes_written += strlen(fullname);

			// 4 bytes - length of file
			struct stat st;
			stat(fullname,&st);
			int_temp2 = st.st_size;
			if ( write( fd, &int_temp2, sizeof(int)) < 0) { puts("Error with write"); return false; }

			*bytes_written += sizeof(int);

			// file
			FILE *fp;
			fp = fopen(fullname,"r");
			if ( fp == NULL) { puts("Error with fopen"); return false; }
			int smallBuffer[1];
			rewind(fp);
			while ( (smallBuffer[0] = fgetc(fp)) != EOF)
			{
				if ( write( fd, smallBuffer, sizeof(char)) < 0) { puts("Error with write"); return false; }

				*bytes_written += 1;
			}

			fclose(fp);

			*files_written += 1;
		}

		free(fullname);
	}

	free(pathname);
	return true;
}

void take_info_from_command_line( int argc, char **argv, int *new_id, int *id, char **common_dir, char **input_dir, char **mirror_dir, int *buffer_size, char **log_file)
{
	*new_id = atoi(argv[1]);
	*id = atoi(argv[2]);
	*common_dir = argv[3];
	*input_dir = argv[4];
	*mirror_dir = argv[5];
	*buffer_size = atoi(argv[6]);
	*log_file = argv[7];
	return;
}
