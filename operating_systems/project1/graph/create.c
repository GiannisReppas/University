#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "search_for.h"
#include "insert_node.h"
#include "insert_edge.h"
#include "create.h"
bool create(int a , char *arguements[] , FILE *in_file , TNode **Thead)
{
	// the file must open successfully
	in_file  = fopen( arguements[a] , "r" );
	if ( in_file == NULL )
	{
		printf("Error with input file\n");
		return false;
	}

	int c=0;
	char *tmp_str1;
	char *tmp_str2;
	char *tmp1;
	char *tmp2;
	int number;

	// the file may have lines with only whitespaces, so we ignore them if we find them
	while( c != EOF )
	{
		//first line

		//first node-word
		//we read the word character by character
		do
		{
			c = getc(in_file);
		}while( (c==' ') || (c=='\t') || (c=='\n') );
		if(c==EOF)
		{
			break;
		}
		tmp_str1 = (char *) malloc(1*sizeof(char));
		if( tmp_str1 == NULL )
		{
			fclose(in_file);
			puts("Error with malloc");
			return false;
		}
		tmp_str1[0]='\0';
		do
		{
			tmp_str2 = (char *) malloc(2*sizeof(char));
			if( tmp_str2 == NULL )
			{
				fclose(in_file);
				free(tmp_str1);
				puts("Error with malloc");
				return false;
			}
			tmp_str2[0] = c;
			tmp_str2[1] = '\0';
			tmp_str1 = (char *) realloc(tmp_str1 , strlen(tmp_str1)+2 );
			if( tmp_str1 == NULL)
			{
				fclose(in_file);
				puts("Error with realloc");
				return false;
			}
			tmp_str1 = strcat(tmp_str1,tmp_str2);
			free(tmp_str2);
			c = getc(in_file);
		}while( (c != ' ') && (c != '\t') );

		if( !insert_node( Thead , tmp_str1) )
		{
			free(tmp_str1);
			fclose(in_file);
			return false;
		}

		tmp1 = (char *) malloc( (strlen(tmp_str1) + 1)*sizeof(char) );
		if( tmp1 == NULL)
		{
			free(tmp_str1);
			fclose(in_file);
			puts("Error with malloc");
			return false;
		}
		strcpy( tmp1 , tmp_str1 );
		free(tmp_str1);

		//second node-word
		do
		{
			c = getc(in_file);
		}while( (c==' ') || (c=='\t') );
		tmp_str1 = (char *) malloc(1*sizeof(char));
		if( tmp_str1 == NULL )
		{
			free(tmp1);
			fclose(in_file);
			puts("Error with malloc");
			return false;
		}
		tmp_str1[0]='\0';
		do
		{
			tmp_str2 = (char *) malloc(2*sizeof(char));
			if( tmp_str2 == NULL )
			{
				free(tmp1);
				free(tmp_str1);
				fclose(in_file);
				puts("Error with malloc");
				return false;
			}
			tmp_str2[0] = c;
			tmp_str2[1] = '\0';
			tmp_str1 = (char *) realloc(tmp_str1 , strlen(tmp_str1)+2 );
			if( tmp_str1 == NULL)
			{
				free(tmp1);
				free(tmp_str2);
				fclose(in_file);
				puts("Error with realloc");
				return false;
			}
			tmp_str1 = strcat(tmp_str1,tmp_str2);
			free(tmp_str2);
			c = getc(in_file);
		}while( (c != ' ') && (c != '\t') );

		if( !insert_node(Thead,tmp_str1) )
		{
			free(tmp1);
			free(tmp_str1);
			fclose(in_file);
			return false;
		}

		tmp2 = (char *) malloc( (strlen(tmp_str1)+1)*sizeof(char) );
		if( tmp2 == NULL )
		{
			free(tmp1);
			free(tmp_str1);
			fclose(in_file);
			puts("Error with malloc");
			return false;
		}
		strcpy( tmp2 , tmp_str1 );
		free(tmp_str1);

		//number - edge
		// at variables tmp1 and tmp2 we hold the first two words, so we can use them for insert_edge
		do
		{
			c = getc(in_file);
		}while( (c==' ') || (c=='\t') );
		tmp_str1 = (char *) malloc(1*sizeof(char));
		if( tmp_str1 == NULL )
		{
			free(tmp1);
			free(tmp2);
			fclose(in_file);
			puts("Error with malloc");
			return false;
		}
		tmp_str1[0]='\0';
		do
		{
			tmp_str2 = (char *) malloc(2*sizeof(char));
			if( tmp_str2 == NULL )
			{
				free(tmp1);
				free(tmp2);
				fclose(in_file);
				free(tmp_str1);
				puts("Error with malloc");
				return false;
			}
			tmp_str2[0] = c;
			tmp_str2[1] = '\0';
			tmp_str1 = (char *) realloc(tmp_str1 , strlen(tmp_str1)+2 );
			if( tmp_str1 == NULL)
			{
				free(tmp1);
				free(tmp2);
				fclose(in_file);
				puts("Error with realloc");
				return false;
			}
			tmp_str1 = strcat(tmp_str1,tmp_str2);
			free(tmp_str2);
			c = getc(in_file);
		}while( (c != ' ') && (c != '\t') && (c != '\n') );
		number = atoi(tmp_str1);
		free(tmp_str1);
		if( !insert_edge(Thead,tmp1,tmp2,number) )
		{
			free(tmp1);
			free(tmp2);
			fclose(in_file);
			return false;
		}

		free(tmp1);
		free(tmp2);

		while(c!='\n')
		{
			c=getc(in_file);
			if(c==EOF) break;
		}
	}

	fclose(in_file);
	return true;
}
