#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "search_for.h"
#include "command_line_check.h"
#include "structure_of_inputfile_check.h"
#include "create.h"
#include "insert_node.h"
#include "insert_edge.h"
#include "delete_node.h"
#include "delete_edge.h"
#include "modify_edge.h"
#include "list.h"
#include "print_Tree.h"
#include "write_at_outputfile.h"
#include "circle_find.h"
#include "find_circle.h"
#include "traceflow.h"
#include "free_Tree.h"
#include "print_recieving_edges.h"
//////////////////////////////
int main(int argc,char *argv[])
{
	FILE *in_file;
	FILE *out_file;

	/*We check if the command line the user gave is correct*/
	if ( !command_line_check(argc ,  argv , in_file , out_file) ) return -1;

	/* if the user gave an input file at the command line, check if its structure its correct*/
	int i;
	bool check;
	check = false;
	for(i=1 ; i<argc ;i++)
	{
		if( !strcmp("-i",argv[i]) )
		{
			check = true;
			break;
		}
	}

	if(check == true)
	{
		if( !structure_of_inputfile_check(i+1, argv , in_file) ) return -1;
	}

	/* if the user gave an input file at the command line, create the graph with the nodes and the edges in it*/
	TNode *TreeHead;
	TreeHead = NULL;

	if(check == true)
	{
		if( !create(i+1, argv, in_file, &TreeHead) ) return -1;
	}
	puts("Tree right after it was created:");
	print_Tree(TreeHead,0);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	char *final;
	final = malloc(1*sizeof(char));
	if( final == NULL)
	{
		puts("Error with malloc");
		exit(-1);
	}
	final[0]='\0';
	char *input;
	char help[3] = " \n";
	char *str1;
	char *str2;
	char *str3;
	char *str4;
	char *str5;
	int tmp;
	int tmp2;

	while(1)
	{
		puts("-----------------------------------------------------------------------------------------------------------");
		puts("1. i Ni --> insert new node Ni");
		puts("2. n Ni Nj weight --> insert new edge from Ni to Nj with weight ");
		puts("3. d Ni --> delete node Ni (with its incoming and outcoming edges as well)");
		puts("4. l Ni Nj weight --> delete edge (if no weight is given, all edges between Ni and Nj get deleted)");
		puts("5. m Ni Nj weight nweight --> modify edge");
		puts("6. r Ni --> print all the in edges of Ni");
		puts("7. c Ni --> finds simple circles of Ni");
		puts("8. f Ni k --> finds all circles in the graph starting from Ni where each edge has at least weight k");
		puts("9. t Ni Nj l --> finds all paths from Ni to Nj, having greatest length equal to l");
		puts("10. e --> exit");
		puts("-----------------------------------------------------------------------------------------------------------");

		/* the user gives the input*/
		do
		{
			input = malloc(2*sizeof(char));
			input = fgets(input,2*sizeof(char),stdin);
			if(input[0] == '\n') break;
			final = realloc( final , (strlen(final)+2)*sizeof(char) );
			strcat( final,input );
			free(input);
		}while(1);
		free(input);

		str1 = strtok(final,help);

		if( !strcmp(str1,"i")  )
		{
			str1 = strtok(NULL,help);
			if( str1 == NULL ) puts("\n\n Wrong menu input \n\n");
			else
			{
				str2 = str1;
				str1 = strtok(NULL,help);
				if( str1 != NULL ) puts("\n\n Wrong menu input \n\n");
				else if( !insert_node(&TreeHead,str2) )
				{
					puts("Error at node insertion");
					return -1;
				}
			}
		}
		else if ( !strcmp(str1,"n") )
		{
			str1 = strtok(NULL,help);
			if( str1 == NULL ) puts("\n\n Wrong menu input \n\n");
			else
			{
				str2 = str1;
				str1 = strtok(NULL,help);
				if( str1 == NULL ) puts("\n\n Wrong menu input \n\n");
				else
				{
					str3 = str1;
					str1 = strtok(NULL,help);
					if( str1 == NULL ) puts("\n\n Wrong menu input \n\n");
					else
					{
						str4 = str1;
						str1 = strtok(NULL,help);
						if( str1 != NULL ) puts("\n\n Wrong menu input \n\n");
						else
						{
							tmp = atoi(str4);
							if( tmp == 0 ) puts("\n\n Wrong manu input \n\n");
							else if( !insert_edge(&TreeHead,str2,str3,tmp) )
							{
								puts("Error at edge insertion");
								return -1;
							}
						}
					}
				}
			}
		}
		else if ( !strcmp(str1,"d" ) )
		{
			str1 = strtok(NULL,help);
			if( str1 == NULL ) puts("\n\n Wrong menu input \n\n");
			else
			{
				str2 = str1;
				str1 = strtok(NULL,help);
				if( str1 != NULL ) puts("\n\n Wrong menu input \n\n");
				else delete_node(&TreeHead,str2);
			}
		}
		else if ( !strcmp(str1,"l") )
		{
			str1 = strtok(NULL,help);
			if( str1 == NULL ) puts("\n\n Wrong menu input \n\n");
			else
			{
				str2 = str1;
				str1 = strtok(NULL,help);
				if( str1 == NULL ) puts("\n\n Wrong menu input \n\n");
				else
				{
					str3 = str1;
					str1 = strtok(NULL,help);
					if(str1 == NULL) delete_edge(&TreeHead,str2,str3,0);
					else
					{
						str4 = str1;
						str1 = strtok(NULL,help);
						if( str1 != NULL ) puts("\n\n Wrong menu input \n\n");
						else
						{
							tmp = atoi(str4);
							if( tmp == 0) puts("\n\n Wrong menu input \n\n");
							else delete_edge(&TreeHead,str2,str3,tmp);
						}
					}
				}
			}
		}
		else if ( !strcmp(str1,"m") )
		{
			str1 = strtok(NULL,help);
			if( str1 == NULL ) puts("\n\n Wrong menu input \n\n");
			else
			{
				str2 = str1;
				str1 = strtok(NULL,help);
				if( str1 == NULL ) puts("\n\n Wrong menu input \n\n");
				else
				{
					str3 = str1;
					str1 = strtok(NULL,help);
					if( str1 == NULL ) puts("\n\n Wrong menu input \n\n");
					else
					{
						str4 = str1;
						tmp = atoi(str4);
						if( tmp == 0) puts("\n\n Wrong menu input \n\n");
						else
						{
							str1 = strtok(NULL,help);
							if( str1 == NULL ) puts("\n\n Wrong menu input \n\n");
							else
							{
								str5 = str1;
								tmp2 = atoi(str5);
								if( tmp == 0) puts("\n\n Wrong menu input \n\n");
								else
								{
									str1 = strtok(NULL,help);
									if( str1 != NULL ) puts("\n\n Wrong menu input \n\n");
									else modify_edge(&TreeHead,str2,str3,tmp,tmp2);
								}
							}
						}
					}
				}
			}
		}
		else if ( !strcmp(str1,"r") )
		{
			str1 = strtok(NULL,help);
			if( str1 == NULL) puts("\n\n Wrong menu input \n\n");
			else
			{
				str2 = str1;
				str1 = strtok(NULL,help);
				if( str1 != NULL ) puts("\n\n Wrong menu input \n\n");
				else print_recieving_edges(&TreeHead,str2);
			}
		}
		else if ( !strcmp(str1,"c") )
		{
			str1 = strtok(NULL,help);
			if( str1 == NULL) puts("\n\n Wrong menu input \n\n");
			else
			{
				str2 = str1;
				str1 = strtok(NULL,help);
				if( str1 != NULL) puts("\n\n Wrong menu input \n\n");
				else circle_find(&TreeHead,str2);
			}
		}
		else if ( !strcmp(str1,"f") )
		{
			str1 = strtok(NULL,help);
			if( str1 == NULL) puts("\n\n Wrong menu input \n\n");
			else
			{
				str2 = str1;
				str1 = strtok(NULL,help);
				if( str1 == NULL) puts("\n\n Wrong menu input \n\n");
				else
				{
					str3 = str1;
					str1 = strtok(NULL,help);
					if(str1 != NULL) puts("\n\n Wrong menu input \n\n");
					else
					{
						tmp = atoi(str3);
						if( tmp == 0) puts("\n\n Wrong menu input \n\n");
						else find_circle(&TreeHead,str2,tmp);
					}
				}
			}
		}
		else if ( !strcmp(str1,"t") )
		{
			str1 = strtok(NULL,help);
			if( str1 == NULL) puts("\n\n Wrong menu input \n\n");
			else
			{
				str2 = str1;
				str1 = strtok(NULL,help);
				if( str1 == NULL) puts("\n\n Wrong menu input \n\n");
				else
				{
					str3 = str1;
					str1 = strtok(NULL,help);
					if(str1 == NULL) puts("\n\n Wrong menu input \n\n");
					else
					{
						str4 = str1;
						str1 = strtok(NULL,help);
						if( str1 != NULL) puts("\n\n Wrong menu input \n\n");
						else
						{
							tmp = atoi(str4);
							if( tmp == 0) puts("\n\n Wrong menu input \n\n");
							else traceflow(&TreeHead,str2,str3,tmp);
						}
					}
				}
			}
		}
		else if ( !strcmp(str1,"e") )
		{
			str1 = strtok(NULL,help);
			if( str1 == NULL )
			{
				puts("exit program");
				break;
			}
			else puts("\n\n Wrong menu input \n\n");
		}
		else puts("\n\n Wrong menu input \n\n");

		free(final);
		final = malloc(1*sizeof(char));
		final[0] = '\0';
	}
	free(final);

	puts("\nTree at the end of execution:");
	print_Tree(TreeHead,0);

	/* if the user gave an output file at the command line, print the output to the outputfile*/
	check = false;
	for(i=1 ; i<argc ;i++)
	{
		if( !strcmp("-o",argv[i]) )
		{
			check = true;
			break;
		}
	}

	if(check == true)
	{
		if( !write_at_outputfile(&TreeHead,i+1, argv , out_file) ) return -1;
	}

	/* free the memory that was allocated to represent the graph*/
	free_Tree(&TreeHead);

	return 0;
}
