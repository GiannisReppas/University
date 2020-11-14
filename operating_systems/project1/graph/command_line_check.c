#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "command_line_check.h"
bool command_line_check( int i , char *arguements[] , FILE *in_file , FILE *out_file)
{
	if( i == 1) return true;
	else if( i == 3)
	{
		if( !strcmp(arguements[1],"-i") )
		{
			in_file = fopen( arguements[2] , "r");
			if( in_file == NULL)
			{
				puts("Error with the input file given");
				return false;
			}
			fclose(in_file);
			return true;
		}
		else if( !strcmp(arguements[1],"-o") )
		{
			out_file = fopen( arguements[2] , "w");
			if( out_file == NULL)
			{
				puts("Error with the output file given");
				return false;
			}
			fclose(out_file);
			return true;
		}
		else
		{
			puts("Wrong input at command line.(1st arguement)");
			return false;
		}
	}
	else if( i == 5)
	{
		if( !strcmp(arguements[1],"-i") )
		{
			in_file = fopen( arguements[2] , "r");
			if( in_file == NULL)
			{
				puts("Error with the input file given");
				return false;
			}
			if( !strcmp(arguements[3],"-o") )
			{
				out_file = fopen( arguements[4] , "w");
				if( out_file == NULL)
				{
					fclose(in_file);
					puts("Error with the output file given");
					return false;
				}
				fclose(in_file);
				fclose(out_file);
				return true;
			}
			else
			{
				fclose(in_file);
				puts("Wrong input at command line.(3rd arguement)");
				return false;
			}
		}
		if( !strcmp(arguements[1],"-o") )
		{
			out_file = fopen( arguements[2] , "w");
			if( out_file == NULL)
			{
				puts("Error with the output file given");
				return false;
			}
			if( !strcmp(arguements[3],"-i") )
			{
				in_file = fopen(arguements[4] , "r");
				if( in_file == NULL)
				{
					fclose(out_file);
					puts("Error with the input file given");
					return false;
				}
				fclose(in_file);
				fclose(out_file);
				return true;
			}
			else
			{
				fclose(out_file);
				puts("Wrong input at command line.(3st arguement)");
				return false;
			}
		}
		else
		{
			puts("Wrong input at command line.(1st arguement)");
			return false;
		}
	}
	else
	{
		puts("Wrong input at command line.(number of arguements)");
		return false;
	}
}
