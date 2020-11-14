#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "search_for.h"
#include "modify_edge.h"
void modify_edge(TNode **Thead,char *str1,char *str2,int w,int nw)
{
	// if the nodes we took exist
	if( !search_for(*Thead,str1) )
	{
		printf("- |%s| does not exist - abort-m;\n",str1);
		return;
	}
	if( !search_for(*Thead,str2) )
	{
		printf("- |%s| does not exist - abort-m;\n",str2);
		return;
	}

	TNode *temp;
	temp = *Thead;
	//go to node with name str1
	while( temp != NULL)
	{
		if( !strcmp(temp->name,str1) )
		{
			LNode *t;
			t = temp->L;
			// go to node str2
			while( t != NULL)
			{
				//modify the edge
				if( (!strcmp(t->name,str2) )&&(t->weight == w) )
				{
					t->weight = nw;

					printf("- Mod-vertex |%s|->%d->|%s|\n",str1,nw,str2);
					return;
				}
				else t = t->next;
			}
			printf("- |%s|->%d->|%s| does not exist - abort-l;\n",str1,w,str2);
			return;
		}
		else if( strcmp(temp->name,str1) < 0 ) temp = temp->right;
		else temp = temp->left;
	}
}
