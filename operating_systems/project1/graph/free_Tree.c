#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "search_for.h"
#include "free_Tree.h"
void free_Tree(TNode **Thead)
{
	// if its NULL there is no need to free anything-we may have bugs
	if( *Thead != NULL)
	{
		//go to the rightest node
		free_Tree( &((*Thead)->right ) );

		//free node's info
		free( (*Thead)->name );
		LNode *temp1;
		LNode *temp2;
		temp2 = (*Thead)->L;
		while( temp2 != NULL)
		{
			free( temp2->name);
			temp1 = temp2->next;
			free(temp2);
			temp2 = temp1;
		}

		//go to the leftest node
		free_Tree( &((*Thead)->left) );

		//free current
		free( *Thead );
	}
	return;
}
