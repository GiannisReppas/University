#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "search_for.h"
#include "delete_edge.h"

void delete_edge1(TNode **Thead,char *str1,char *str2,int w)
{
	TNode *temp;
	temp = *Thead;
	//go to node str1
	while(temp != NULL)
	{
		if( !strcmp(temp->name,str1) )
		{
			LNode *t1;
			LNode *t2;
			t2 = temp->L;
			//go to node str2
			while( t2 != NULL)
			{
				//delete the edge
				if( t2 == temp->L )
				{
					if( (!strcmp(t2->name,str2) )&&(t2->weight == w) )
					{
						temp->L = t2->next;
						free(t2->name);
						t2->next = NULL;
						free(t2);
						t2 = temp->L;

						printf("- Del-vertex |%s|->%d->|%s|\n",str1,w,str2 );
						return;
					}
					else
					{
						t1=t2;
						t2=t2->next;
					}
				}
				else
				{
					if( (!strcmp(t2->name,str2) )&&(t2->weight == w) )
					{
						t1->next = t2->next;
						free(t2->name);
						t2->next = NULL;
						free(t2);
						t2 = t1->next;

						printf("- Del-vertex |%s|->%d->|%s|\n",str1,w,str2 );
						return;
					}
					else
					{
						t1=t2;
						t2=t2->next;
					}
				}
			}
			printf("- |%s|->%d->|%s| does not exist - abort-l;\n",str1,w,str2);
			return;
		}
		else if( strcmp(temp->name,str1) < 0 ) temp = temp->right;
		else temp = temp->left;
	}
}

void delete_edge2(TNode **Thead,char *str1,char *str2)
{
	TNode *temp;
	temp = *Thead;
	//go to node str1
	while( temp != NULL)
	{
		if( !strcmp(str1,temp->name) )
		{
			//go to node str2
			LNode *t1;
			LNode *t2;
			t2 = temp->L;
			while( t2 != NULL)
			{
				//if the node is str2 delete the edge
				if( t2 == temp->L)
				{
					if( !strcmp(str2,t2->name) )
					{
						temp->L = t2->next;
						free(t2->name);
						free(t2);
						t2 = temp->L;
					}
					else
					{
						t1 = t2;
						t2 = t2->next;
					}
				}
				else
				{
					if( !strcmp(str2,t2->name) )
					{
						t1->next = t2->next;
						free(t2->name);
						free(t2);
						t2 = t1->next;
					}
					else
					{
						t1 = t2;
						t2 = t2->next;
					}
				}
			}
			printf("- All vertexes from %s to %s have been deleted\n",str1,str2);
			return;
		}
		else if( strcmp(temp->name,str1) < 0) temp = temp->right;
		else temp = temp->left;
	}
}

void delete_edge(TNode **Thead,char *str1,char *str2,int w)
{
	//if the nodes we took exist
	if( !search_for(*Thead,str1) )
	{
		printf("- |%s| does not exist - abort-l;\n",str1);
		return;
	}
	if( !search_for(*Thead,str2) )
	{
		printf("- |%s| does not exist - abort-l;\n",str2);
		return;
	}

	// if the user gave edge value
	if(w!=0) delete_edge1(Thead,str1,str2,w);
	// if the user didnt give a value
	else delete_edge2(Thead,str1,str2);
}
