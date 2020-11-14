#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "search_for.h"
#include "free_Tree.h"
#include "insert_node.h"
#include "insert_edge.h"
bool insert_edge(TNode **Thead , char *str1 , char *str2 , int number)
{
	//if the nodes dont exist, we insert them in the tree
	if( !search_for(*Thead,str1) ) if( !insert_node(Thead,str1) ) return false;
	if( !search_for(*Thead,str2) ) if( !insert_node(Thead,str2) ) return false;

	TNode *temp;
	temp = *Thead;
	//go to node str1
	while( 1 )
	{
		if( strcmp(temp->name,str1) < 0) temp = temp->right;
		else if( strcmp(temp->name,str1) > 0) temp = temp->left;
		else
		{
			//inser the edge at the end of the list
			if( temp->L == NULL )
			{
				//build new edge
				LNode *new_node;
				new_node = malloc( 1*sizeof(LNode) );
				if( new_node == NULL )
				{
					puts("Error with malloc");
					free(Thead);
					return false;
				}
				new_node->weight = number;
				new_node->next = NULL;
				new_node->name = malloc( (strlen(str2)+1)*sizeof(char) );
				if( new_node->name == NULL)
				{
					puts("Error with malloc");
					free(new_node);
					free_Tree(Thead);
					return false;
				}
				strcpy( new_node->name , str2 );

				temp->L = new_node;

				printf("- Inserted |%s|->%d->|%s|\n",str1,number,str2);
				return true;
			}
			else
			{
				LNode *t;
				t = temp->L;
				while( t->next != NULL) t = t->next;

				//build new edge
				LNode *new_node;
				new_node = malloc( 1*sizeof(LNode) );
				if( new_node == NULL )
				{
					puts("Error with malloc");
					free(Thead);
					return false;
				}
				new_node->weight = number;
				new_node->next = NULL;
				new_node->name = malloc( (strlen(str2)+1)*sizeof(char) );
				if( new_node->name == NULL)
				{
					puts("Error with malloc");
					free(new_node);
					free_Tree(Thead);
					return false;
				}
				strcpy( new_node->name , str2 );

				t->next = new_node;

				printf("- Inserted |%s|->%d->|%s|\n",str1,number,str2);
				return true;
			}
		}
	}
}
