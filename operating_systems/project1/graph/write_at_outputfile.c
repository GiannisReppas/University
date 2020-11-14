#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "search_for.h"
#include "write_at_outputfile.h"
void write_at_outputfile_rec(TNode **Thead, FILE *out_file)
{
	//pre-order
	if( *Thead == NULL) return;

	//write the info of the node at the output file
	fprintf(out_file,"|%s|\n",(*Thead)->name);
	LNode *temp;
	temp = (*Thead)->L;
	while( temp != NULL )
	{
		fprintf(out_file,"\t-%d->|%s|\n",temp->weight,temp->name);
		temp = temp->next;
	}

	write_at_outputfile_rec( &(*Thead)->right,out_file );
	write_at_outputfile_rec( &(*Thead)->left,out_file );
}

bool write_at_outputfile(TNode **Thead,int a,char *arguements[], FILE *out_file)
{
	//we check if the output file opened successfully
	out_file = fopen(arguements[a] , "w");
	if(out_file == NULL)
	{
		puts("Error with outputfile");
		return false;
	}

	write_at_outputfile_rec(Thead,out_file);

	fclose(out_file);

	return true;
}
