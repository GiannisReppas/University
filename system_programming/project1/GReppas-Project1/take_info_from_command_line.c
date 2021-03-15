#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "take_info_from_command_line.h"

bool take_info_from_command_line( int argc, char **argv, int *bCV, int *sHTNOE, int *rHTNOE, int *bS, FILE **bCBF_fp, FILE **tF_fp)
{
	// ./bitcoin can run only with 13 arguements from command line
	// -a --> bitCoinBalancesFile
	// -t --> transactionsFile
	// -v --> bitCoin value
	// -h1 --> size of senderHashTable
	// -h2 --> size of recieverHashTable
	// -b --> bucket size

	FILE *fp;

	if ( argc != 13) { printf("Error with number of arguements from command line\n"); return false;}

	// for every arguement, check if it is correct
	int i = 1;
	while ( i < 13)
	{
		if ( i == 1)
		{
			if ( !strcmp(argv[i],"-a")) ;
			else if ( !strcmp(argv[i],"-t")) ;
			else if ( !strcmp(argv[i],"-v")) ;
			else if ( !strcmp(argv[i],"-h1")) ;
			else if ( !strcmp(argv[i],"-h2")) ;
			else if ( !strcmp(argv[i],"-b")) ;
			else { puts("Error with 1st parameter from command line");} 
		}
		if ( i == 2)
		{
			if ( !strcmp(argv[i-1],"-a"))
			{
				fp = fopen(argv[i],"r");
				if ( fp == NULL) { printf("Error with bitCoinBalancesFile\n"); return false; }
				*bCBF_fp = fp;
			}
			else if ( !strcmp(argv[i-1],"-t"))
			{
				fp = fopen(argv[i],"r");
				if ( fp == NULL) { printf("Error with transactionsFile\n"); return false; }
				*tF_fp = fp;
			}
			else if ( !strcmp(argv[i-1],"-v"))
			{
				*bCV = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-h1"))
			{
				*sHTNOE = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-h2"))
			{
				*rHTNOE = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-b"))
			{
				*bS = atoi(argv[i]);
				if ( *bS < 59) { printf("Bucket Size has to be greater than 59\n"); return false; }
			}
		}
		if ( i == 3)
		{
			if ( ( !strcmp(argv[i],"-a")) && ( strcmp(argv[i],argv[i-2])) ) ;
			else if ( ( !strcmp(argv[i],"-t")) && ( strcmp(argv[i],argv[i-2])) ) ;
			else if ( ( !strcmp(argv[i],"-v")) && ( strcmp(argv[i],argv[i-2])) ) ;
			else if ( ( !strcmp(argv[i],"-h1")) && ( strcmp(argv[i],argv[i-2])) ) ;
			else if ( ( !strcmp(argv[i],"-h2")) && ( strcmp(argv[i],argv[i-2])) ) ;
			else if ( ( !strcmp(argv[i],"-b")) && ( strcmp(argv[i],argv[i-2])) ) ;
			else { puts("Error with 3rd parameter from command line"); return false;}
		}
		if ( i == 4)
		{
			if ( !strcmp(argv[i-1],"-a"))
			{
				fp = fopen(argv[i],"r");
				if ( fp == NULL) { printf("Error with bitCoinBalancesFile\n"); return false; }
				*bCBF_fp = fp;
			}
			else if ( !strcmp(argv[i-1],"-t"))
			{
				fp = fopen(argv[i],"r");
				if ( fp == NULL) { printf("Error with transactionsFile\n"); return false; }
				*tF_fp = fp;
			}
			else if ( !strcmp(argv[i-1],"-v"))
			{
				*bCV = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-h1"))
			{
				*sHTNOE = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-h2"))
			{
				*rHTNOE = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-b"))
			{
				*bS = atoi(argv[i]);
				if ( *bS < 59) { printf("Bucket Size has to be greater than 59\n"); return false; }
			}
		}
		if ( i == 5)
		{
			if ( ( !strcmp(argv[i],"-a")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) ) ;
			else if ( ( !strcmp(argv[i],"-t")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) ) ;
			else if ( ( !strcmp(argv[i],"-v")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) ) ;
			else if ( ( !strcmp(argv[i],"-h1")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) ) ;
			else if ( ( !strcmp(argv[i],"-h2")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) ) ;
			else if ( ( !strcmp(argv[i],"-b")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) ) ;
			else { puts("Error with 5th parameter from command line"); return false;}
		}
		if ( i == 6)
		{
			if ( !strcmp(argv[i-1],"-a"))
			{
				fp = fopen(argv[i],"r");
				if ( fp == NULL) { printf("Error with bitCoinBalancesFile\n"); return false; }
				*bCBF_fp = fp;
			}
			else if ( !strcmp(argv[i-1],"-t"))
			{
				fp = fopen(argv[i],"r");
				if ( fp == NULL) { printf("Error with transactionsFile\n"); return false; }
				*tF_fp = fp;
			}
			else if ( !strcmp(argv[i-1],"-v"))
			{
				*bCV = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-h1"))
			{
				*sHTNOE = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-h2"))
			{
				*rHTNOE = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-b"))
			{
				*bS = atoi(argv[i]);
				if ( *bS < 59) { printf("Bucket Size has to be greater than 59\n"); return false; }
			}
		}
		if ( i == 7)
		{
			if ( ( !strcmp(argv[i],"-a")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) && ( strcmp(argv[i],argv[i-6])) ) ;
			else if ( ( !strcmp(argv[i],"-t")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) && ( strcmp(argv[i],argv[i-6])) ) ;
			else if ( ( !strcmp(argv[i],"-v")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) && ( strcmp(argv[i],argv[i-6])) ) ;
			else if ( ( !strcmp(argv[i],"-h1")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) && ( strcmp(argv[i],argv[i-6])) ) ;
			else if ( ( !strcmp(argv[i],"-h2")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) && ( strcmp(argv[i],argv[i-6])) ) ;
			else if ( ( !strcmp(argv[i],"-b")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) && ( strcmp(argv[i],argv[i-6])) ) ;
			else { puts("Error with 7th parameter from command line"); return false;}
		}
		if ( i == 8)
		{
			if ( !strcmp(argv[i-1],"-a"))
			{
				fp = fopen(argv[i],"r");
				if ( fp == NULL) { printf("Error with bitCoinBalancesFile\n"); return false; }
				*bCBF_fp = fp;
			}
			else if ( !strcmp(argv[i-1],"-t"))
			{
				fp = fopen(argv[i],"r");
				if ( fp == NULL) { printf("Error with transactionsFile\n"); return false; }
				*tF_fp = fp;
			}
			else if ( !strcmp(argv[i-1],"-v"))
			{
				*bCV = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-h1"))
			{
				*sHTNOE = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-h2"))
			{
				*rHTNOE = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-b"))
			{
				*bS = atoi(argv[i]);
				if ( *bS < 59) { printf("Bucket Size has to be greater than 59\n"); return false; }
			}
		}
		if ( i == 9)
		{
			if ( ( !strcmp(argv[i],"-a")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) && ( strcmp(argv[i],argv[i-6])) && ( strcmp(argv[i],argv[i-8])) ) ;
			else if ( ( !strcmp(argv[i],"-t")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) && ( strcmp(argv[i],argv[i-6])) && ( strcmp(argv[i],argv[i-8])) ) ;
			else if ( ( !strcmp(argv[i],"-v")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) && ( strcmp(argv[i],argv[i-6])) && ( strcmp(argv[i],argv[i-8])) ) ;
			else if ( ( !strcmp(argv[i],"-h1")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) && ( strcmp(argv[i],argv[i-6])) && ( strcmp(argv[i],argv[i-8])) ) ;
			else if ( ( !strcmp(argv[i],"-h2")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) && ( strcmp(argv[i],argv[i-6])) && ( strcmp(argv[i],argv[i-8])) ) ;
			else if ( ( !strcmp(argv[i],"-b")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) && ( strcmp(argv[i],argv[i-6])) && ( strcmp(argv[i],argv[i-8])) ) ;
			else { puts("Error with 7th parameter from command line"); return false;}
		}
		if ( i == 10)
		{
			if ( !strcmp(argv[i-1],"-a"))
			{
				fp = fopen(argv[i],"r");
				if ( fp == NULL) { printf("Error with bitCoinBalancesFile\n"); return false; }
				*bCBF_fp = fp;
			}
			else if ( !strcmp(argv[i-1],"-t"))
			{
				fp = fopen(argv[i],"r");
				if ( fp == NULL) { printf("Error with transactionsFile\n"); return false; }
				*tF_fp = fp;
			}
			else if ( !strcmp(argv[i-1],"-v"))
			{
				*bCV = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-h1"))
			{
				*sHTNOE = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-h2"))
			{
				*rHTNOE = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-b"))
			{
				*bS = atoi(argv[i]);
				if ( *bS < 59) { printf("Bucket Size has to be greater than 59\n"); return false; }
			}
		}
		if ( i == 11)
		{
			if ( ( !strcmp(argv[i],"-a")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) && ( strcmp(argv[i],argv[i-6])) && ( strcmp(argv[i],argv[i-8])) && ( strcmp(argv[i],argv[i-10])) ) ;
			else if ( ( !strcmp(argv[i],"-t")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) && ( strcmp(argv[i],argv[i-6])) && ( strcmp(argv[i],argv[i-8])) && ( strcmp(argv[i],argv[i-10])) ) ;
			else if ( ( !strcmp(argv[i],"-v")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) && ( strcmp(argv[i],argv[i-6])) && ( strcmp(argv[i],argv[i-8])) && ( strcmp(argv[i],argv[i-10])) ) ;
			else if ( ( !strcmp(argv[i],"-h1")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) && ( strcmp(argv[i],argv[i-6])) && ( strcmp(argv[i],argv[i-8])) && ( strcmp(argv[i],argv[i-10])) ) ;
			else if ( ( !strcmp(argv[i],"-h2")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) && ( strcmp(argv[i],argv[i-6])) && ( strcmp(argv[i],argv[i-8])) && ( strcmp(argv[i],argv[i-10])) ) ;
			else if ( ( !strcmp(argv[i],"-b")) && ( strcmp(argv[i],argv[i-2])) && ( strcmp(argv[i],argv[i-4])) && ( strcmp(argv[i],argv[i-6])) && ( strcmp(argv[i],argv[i-8])) && ( strcmp(argv[i],argv[i-10])) ) ;
			else { puts("Error with 11th parameter from command line"); return false;}
		}
		if ( i == 12)
		{
			if ( !strcmp(argv[i-1],"-a"))
			{
				fp = fopen(argv[i],"r");
				if ( fp == NULL) { printf("Error with bitCoinBalancesFile\n"); return false; }
				*bCBF_fp = fp;
			}
			else if ( !strcmp(argv[i-1],"-t"))
			{
				fp = fopen(argv[i],"r");
				if ( fp == NULL) { printf("Error with transactionsFile\n"); return false; }
				*tF_fp = fp;
			}
			else if ( !strcmp(argv[i-1],"-v"))
			{
				*bCV = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-h1"))
			{
				*sHTNOE = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-h2"))
			{
				*rHTNOE = atoi(argv[i]);
			}
			else if ( !strcmp(argv[i-1],"-b"))
			{
				*bS = atoi(argv[i]);
				if ( *bS < 59) { printf("Bucket Size has to be greater than 59\n"); return false; }
			}
		}

		i++;
	}

	return true;
}
