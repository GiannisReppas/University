#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "search_for.h"
#include "print_recieving_edges.h"
int print_recieving_edges_rec(TNode **Thead,char *str, int i)
{
	//pre-order
	if(*Thead == NULL) return i;

	// print the in-edges of the current node
	LNode *temp;
	temp = (*Thead)->L;
	while(temp != NULL)
	{
		if( !strcmp(temp->name,str) )
		{
			printf("- |%s|->%d->|%s|\n",(*Thead)->name,temp->weight,temp->name );
			i++;
		}
		temp = temp->next;
	}

	i += print_recieving_edges_rec( &(*Thead)->left,str,i );
	i += print_recieving_edges_rec( &(*Thead)->right,str,i );
	return i;
}

void print_recieving_edges(TNode **Thead,char *str)
{
	int i;
	i=0;
	i = print_recieving_edges_rec( Thead,str,i );
	//if nothing was printed
	if( i == 0) printf("- No-rec-edges %s\n",str);
}
