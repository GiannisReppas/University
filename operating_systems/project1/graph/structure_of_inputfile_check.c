#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "search_for.h"
#include "structure_of_inputfile_check.h"
bool structure_of_inputfile_check(int a , char *arguements[] , FILE *in_file)
{
	in_file = fopen(arguements[a] , "r");
	if(in_file == NULL)
	{
		puts("Error with inputfile");
		return false;
	}

	int c;
	int temp;
	char *tmp_str1;
	char *tmp_str2;
	while( 1 )
	{
		//first string is a node name
		do
		{
			c = getc(in_file);
		}while( (c==' ') || (c=='\t') || (c=='\n') );
		if(c==EOF)
		{
			break;
		}
		do
		{
			c = getc(in_file);
		}while( (c!=' ') && (c!='\t') && (c!='\n') && (c!=EOF) );
		if( (c==EOF) || (c=='\n') )
		{
			printf("Wrong structure of input file\n");
			fclose(in_file);
			return false;
		}

		//second string is a node name
		do
		{
			c = getc(in_file);
		}while( (c==' ') || (c=='\t') );
		if( (c==EOF) || (c=='\n') )
		{

			printf("Wrong structure of input file\n");
			fclose(in_file);
			return false;
		}
		do
		{
			c = getc(in_file);
		}while( (c!=' ') && (c!='\t') && (c!='\n') && (c!=EOF) );
		if( (c==EOF) || (c=='\n') )
		{
			printf("Wrong structure of input file\n");
			fclose(in_file);
			return false;
		}

		//third string is a number
		do
		{
			c = getc(in_file);
		}while( (c==' ') || (c=='\t') );
		if( (c==EOF) || (c=='\n') )
		{

			printf("Wrong structure of input file\n");
			fclose(in_file);
			return false;
		}
		tmp_str1 = (char *) malloc(1*sizeof(char));
		if( tmp_str1 == NULL )
		{
			printf("Error with malloc\n");
			fclose(in_file);
			return false;
		}
		tmp_str1[0]='\0';
		do
		{
			if( (c != ' ') && (c != '\t') && (c != EOF) && (c != '\n') )
			{
				tmp_str2 = (char *) malloc(2*sizeof(char));
				if( tmp_str2 == NULL )
				{
					printf("Error with malloc\n");
					free(tmp_str1);
					fclose(in_file);
					return false;
				}
				tmp_str2[0] = c;
				tmp_str2[1] = '\0';

				tmp_str1 = (char *) realloc(tmp_str1 , strlen(tmp_str1)+2 );
				if( tmp_str1 == NULL)
				{
					printf("Error with realloc\n");
					free(tmp_str2);
					fclose(in_file);
					return false;
				}
				tmp_str1 = strcat(tmp_str1,tmp_str2);

				free(tmp_str2);
			}
			else
			{
				// if it is a string and not a number
				temp = atoi(tmp_str1);
				if(temp==0)
				{
					printf("Wrong value for an edge\n");
					free(tmp_str1);
					fclose(in_file);
					return false;
				}
				break;
			}
			c = getc(in_file);
		}while( 1 );
		free(tmp_str1);
		if(c==EOF)
		{
			break;
		}
		if( c != '\n' )
		{
			do
			{
				c = getc(in_file);
			}while( (c==' ') || (c=='\t') );
			if(c!='\n')
			{
				printf("Wrong structure of input file\n");
				fclose(in_file);
				return false;
			}
		}
	}

	fclose(in_file);
	return true;
}
