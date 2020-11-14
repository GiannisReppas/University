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

void take_info_from_command_line( int argc, char **argv, int *new_id, int *id, char **common_dir, char **input_dir, char **mirror_dir, int *buffersize, char **log_file);
bool read_from_pipe_and_write_to_mirror_dir( int fd, char *mirror_dir, char *input_dir, int buffersize, int id, int ida, char *common_dir, char *fifo_path, int *bytes_read, int *files_read, char *log_file);
void send_error_to_parent( char *pathname, char *mirror_dir, char *id, char *common_dir, int int_ida, int int_idb, int *bytes_read, int *files_read, char *log_file);
bool delete_all_folder_files( DIR *dr, char *str);

int main( int argc, char *argv[])
{
	int new_id;
	int id;
	char *common_dir;
	char *input_dir;
	char *mirror_dir;
	int buffersize;
	char *log_file;

	take_info_from_command_line( argc, argv, &new_id, &id, &common_dir, &input_dir, &mirror_dir, &buffersize, &log_file);

	// make fifo
	char *ida; char *idb;
	ida = malloc( number_of_digits(id)+sizeof(char)); if ( ida == NULL) { puts("Error with malloc"); return 1; }
	integer_to_string( id, ida);
	idb = malloc( number_of_digits(new_id)+sizeof(char)); if ( idb == NULL) { puts("Error with malloc"); return 1; }
	integer_to_string( new_id, idb);

	char *pathname;
	pathname = malloc( strlen(common_dir)+sizeof(char)+sizeof(char)*2+number_of_digits(id)+sizeof(char)*4+sizeof(char)*2+number_of_digits(new_id)+sizeof(char)*5+sizeof(char));
	if ( pathname == NULL) { puts("Error with malloc"); return 1; }
	strcpy( pathname, common_dir);
	strcat( pathname, "/");
	strcat( pathname, "id");
	strcat( pathname, idb);
	strcat( pathname, "_to_");
	strcat( pathname, "id");
	strcat( pathname, ida);
	strcat( pathname, ".fifo");

	mkfifo(pathname, S_IRWXU | S_IRWXG | S_IRWXO);

	// open fifo
	int fd;	if ( (fd=open( pathname, O_RDONLY | O_NONBLOCK )) < 0) { puts("Error with open"); return 1; }

	// wait for writer to write in pipe	
	struct pollfd fdarray[1];
	fdarray[0].fd = fd;
	fdarray[0].events = POLLIN;
	int rc = poll( fdarray, 1, 30000);
	if ( rc == 0)
	{
		if ( kill( getppid(), SIGUSR1) < 0) { puts("Error with kill"); return 1; }

		short int temp; if ( read( fd, &temp, sizeof(short int)) >= 0) printf("fifo -> %d\n",temp);

		// close fifo
		if ( close(fd) < 0) { puts("Error with close"); return 1; }

		// remove fifo
		if ( remove(pathname) < 0) { puts("Error with remove"); return 1; }

		free(pathname); free(ida); free(idb);
		exit(1);
	}
	else if ( (rc!=1) || (fdarray[0].revents!=POLLIN) || (fdarray[0].fd != fd) )
	{
		// close fifo
		if ( close(fd) < 0) { puts("Error with close"); return 1; }

		// remove fifo
		if ( remove(pathname) < 0) { puts("Error with remove"); return 1; }

		free(pathname); free(ida); free(idb);
		puts("Error with poll");
		exit(1);
	}

	// read info from pipe
	int bytes_read=0;
	int files_read=0;

	if ( !read_from_pipe_and_write_to_mirror_dir( fd, mirror_dir, input_dir, buffersize, new_id, id, common_dir, pathname, &bytes_read, &files_read, log_file) ) { close(fd); return 1; }

	// close fifo
	if ( close(fd) < 0) { puts("Error with close"); return 1; }

	// remove fifo
	if ( remove(pathname) < 0) { puts("Error with pathname"); return 1; }

	FILE *log_file_fp=fopen(log_file,"a");
	if ( log_file_fp == NULL) { puts("Error with fopen"); return 1; }

	fprintf( log_file_fp, "3) %d\n", bytes_read);
	fprintf( log_file_fp, "4) %d\n", files_read);

	fclose(log_file_fp);

	free(pathname); free(ida); free(idb);

	return 1;
}

bool read_from_pipe_and_write_to_mirror_dir( int fd, char *mirror_dir, char *input_dir, int buffersize, int id, int ida, char *common_dir, char *fifo_path, int *bytes_read, int *files_read, char *log_file)
{
	char *buffer;
	char *pathname;
	short int int_temp1;
	int int_temp2;
	int i, remain;
	FILE *fp;
	DIR *dr;

	char *char_id;
	char_id = malloc( sizeof(char)*number_of_digits(id) + sizeof(char) );
	if ( char_id == NULL) { puts("Error with malloc"); return false; }
	integer_to_string( id, char_id);

	pathname = malloc( strlen(mirror_dir) + sizeof(char) + sizeof(char)*strlen(char_id) + sizeof(char) );
	if ( pathname == NULL) { puts("Error with malloc"); return false; }
	strcpy(pathname,mirror_dir);
	strcat(pathname,"/");
	strcat(pathname,char_id);
	if ( mkdir( pathname, S_IRWXU | S_IRWXG | S_IRWXO ) < 0 ) { puts("Error with mkdir"); return false; }

	free(pathname);

	while (1)
	{
		// read length of file name
		if ( read( fd, &int_temp1, sizeof(short int)) < 0) { printf("Error with read\n%s\n",strerror(errno)); send_error_to_parent(fifo_path, mirror_dir, char_id, common_dir, ida, id, bytes_read, files_read, log_file); return false; }

		*bytes_read += sizeof(short int);

		// check if we reached the end of fifo
		if ( int_temp1 == 0) break;

		// read file name
		buffer = malloc(sizeof(char)*int_temp1 + sizeof(char));
		if ( buffer == NULL) { puts("Error with malloc"); return false; }
		if ( read( fd, buffer, int_temp1) < 0) { printf("Error with read\n%s\n",strerror(errno)); send_error_to_parent(fifo_path, mirror_dir, char_id, common_dir, ida, id, bytes_read, files_read, log_file); return false; }
		buffer[int_temp1] = '\0';

		*bytes_read += int_temp1;

		//check if it is a file or a directory
		dr = opendir(buffer);
		if ( dr == NULL) // file
		{
			// create the file
			i = 0;
			while ( i != strlen(input_dir) ) i++;
			i++;
			pathname = malloc( strlen(mirror_dir) + sizeof(char) + strlen(char_id) + sizeof(char) + strlen(buffer+i) + sizeof(char));
			if (pathname == NULL) { puts("Error with malloc"); return false; }
			strcpy(pathname,mirror_dir);
			strcat(pathname,"/");
			strcat(pathname,char_id);
			strcat(pathname,"/");
			strcat(pathname,buffer+i);
			if ( (fp=fopen(pathname,"w+")) == NULL) { puts("Error with fopen"); return false; }
			free(buffer);

			// read length of file
			if ( read( fd, &int_temp2, sizeof(int)) < 0) { printf("Error with read\n%s\n",strerror(errno)); send_error_to_parent(fifo_path, mirror_dir, char_id, common_dir, ida, id, bytes_read, files_read, log_file); return false; }

			*bytes_read += sizeof(int);

			// read file from fifo and write it in mirror dir
			remain = int_temp2;
			while ( 1 )
			{
				buffer = malloc(sizeof(char)*buffersize + sizeof(char));
				if ( buffer == NULL) { puts("Error with malloc"); return false; }

				if ( remain > buffersize)
				{
					if ( read( fd, buffer, buffersize) < 0) { printf("Error with read\n%s\n",strerror(errno)); send_error_to_parent(fifo_path, mirror_dir, char_id, common_dir, ida, id, bytes_read, files_read, log_file); return false; }

					*bytes_read += buffersize;

					buffer[buffersize] = '\0';
					remain -= buffersize;

					fprintf( fp, "%s", buffer);
				}
				else
				{
					if ( read( fd, buffer, remain) < 0) { printf("Error with read\n%s\n",strerror(errno)); send_error_to_parent(fifo_path, mirror_dir, char_id, common_dir, ida, id, bytes_read, files_read, log_file); return false; }

					*bytes_read += remain;

					buffer[remain] = '\0';
					remain = 0;

					fprintf( fp,"%s", buffer);
				}

				free(buffer);
				if (remain <= 0) break;
			}

			*files_read += 1;

			free(pathname);
			fclose(fp);
		}
		else // directory
		{
			// create directory
			i = 0;
			while ( i != strlen(input_dir) ) i++;
			i++;
			pathname = malloc( strlen(mirror_dir) + sizeof(char) + strlen(char_id) + sizeof(char) + strlen(buffer+i) + sizeof(char));
			if (pathname == NULL) { puts("Error with malloc"); return false; }
			strcpy(pathname,mirror_dir);
			strcat(pathname,"/");
			strcat(pathname,char_id);
			strcat(pathname,"/");
			strcat(pathname,buffer+i);
			if ( mkdir(pathname, S_IRWXU | S_IRWXG | S_IRWXO ) < 0) { puts("Error with mkdir"); return false; }

			free(buffer);
			free(pathname);
		}
	}
	free(char_id);

	return true;
}

void send_error_to_parent( char *pathname, char *mirror_dir, char *id, char *common_dir, int int_ida, int int_idb, int *bytes_read, int *files_read, char *log_file)
{
	if ( kill( getppid(), SIGUSR2) < 0) { puts("Error with kill"); exit(1); }

	char *folder;
	folder = malloc(sizeof(char)*strlen(mirror_dir) + sizeof(char) + strlen(id) + sizeof(char) );
	if ( folder == NULL) { puts("Error with malloc"); exit(1); }
	strcpy(folder,mirror_dir);
	strcat(folder,"/");
	strcat(folder,id);

	DIR *dr; dr = opendir(folder);
	if ( dr == NULL) { puts("Error with opendir"); exit(1); }
	if ( !delete_all_folder_files(dr,folder) ) exit(1);
	if ( closedir(dr) < 0) { puts("Error with closedir"); exit(1); }
	if ( rmdir(folder) < 0) { puts("Error with rmdir"); exit(1); }

	free(folder);

	if ( remove(pathname) < 0) { puts("Error with remove1"); exit(1); }

	FILE *log_file_fp=fopen(log_file,"a");
	if ( log_file_fp == NULL) { puts("Error with fopen"); exit(1); }

	fprintf( log_file_fp, "3) %d\n", *bytes_read);
	fprintf( log_file_fp, "4) %d\n", *files_read);

	fclose(log_file_fp);

	return;
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

		if ( dr_end != NULL )
		{
			if ( !delete_all_folder_files(dr_end,fullname)) return false;
			if ( closedir(dr_end) < 0) { puts("Error with closedir"); return false; }
			if ( rmdir(fullname) < 0) { puts("Error with rmdir"); return false; }
		}
		else
		{
			if ( remove(fullname) < 0) { puts("error with remove"); return false; }
		}

		free(fullname);
	}

	free(pathname);
	return true;
}

void take_info_from_command_line( int argc, char **argv, int *new_id, int *id, char **common_dir, char **input_dir, char **mirror_dir, int *buffersize, char **log_file)
{
	*new_id = atoi(argv[1]);
	*id = atoi(argv[2]);
	*common_dir = argv[3];
	*input_dir = argv[4];
	*mirror_dir = argv[5];
	*buffersize = atoi(argv[6]);
	*log_file = argv[7];
	return;
}
