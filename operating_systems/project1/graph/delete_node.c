#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "search_for.h"
#include "delete_node.h"
void delete_in_edges(TNode **Thead, char *str)
{
	//pre-order
	if(*Thead == NULL) return;

	//for the current node
	LNode *temp1;
	LNode *temp2;
	temp2 = (*Thead)->L;
	while(temp2 != NULL)
	{
		// if current node points to str, delete the edge
		if( temp2 == (*Thead)->L )
		{
			if( !strcmp(temp2->name,str) )
			{
				(*Thead)->L = temp2->next;
				free(temp2->name);
				temp2->next = NULL;
				free(temp2);
				temp2 = (*Thead)->L;
			}
			else
			{
				temp1 = temp2;
				temp2 = temp2->next;
			}
		}
		else
		{
			if( !strcmp(temp2->name,str) )
			{
				temp1->next = temp2->next;
				free(temp2->name);
				temp2->next = NULL;
				free(temp2);
				temp2 = temp1->next;
			}
			else
			{
				temp1 = temp2;
				temp2 = temp2->next;
			}
		}
	}

	delete_in_edges( &(*Thead)->left,str );
	delete_in_edges( &(*Thead)->right,str );
	return;
}
void delete_node(TNode **Thead,char *str)
{
	// if the node we want to delete doesnt exist
	if( !search_for(*Thead,str) )
	{
		printf("- Node |%s| does not exist - abort-d;\n",str);
		return;
	}

	// delete first its in_edges
	delete_in_edges(Thead,str);

	TNode *current;
	TNode *previous;
	previous = NULL;
	current = *Thead;
	bool last;
	//go to node str
	while( current != NULL)
	{
		if( !strcmp(current->name,str) )
		{
			//keep the binary tree formation
			if(current != *Thead)
			{
				if( (current->left == NULL)&&(current->right == NULL) )
				{
					if( last == true) previous->right = NULL;
					else previous->left = NULL;
				}
				else if( ( (current->left == NULL)&&(current->right != NULL) ) || ( (current->left != NULL)&&(current->right == NULL) ) )
				{
					if( current->left == NULL)
					{
						if( last == true) previous->right = current->right;
						else previous->left = current->right;
					}
					else
					{
						if(last == true) previous->right = current->left;
						else previous->left = current->left;
					}
				}
				else
				{
					TNode *next;
					TNode *temp;
					if( last == true)
					{
						previous->right = current->left;
						next = current->right;
						temp = current->left;
						while( 1 )
						{
							if( temp->right != NULL) temp = temp->right;
							else
							{
								if( temp->left != NULL) temp = temp->left;
								else break;
							}
						}
						temp->right = next;
					}
					else
					{
						previous->left = current->right;
						next = current->left;
						temp = current->right;
						while( 1 )
						{
							if( temp->left != NULL) temp = temp->left;
							else
							{
								if( temp->right != NULL) temp = temp->right;
								else break;
							}
						}
						temp->left = next;
					}
				}
			}
			else
			{
				if( (current->left == NULL)&&(current->right == NULL) )	*Thead=NULL;
				else if( ( (current->left == NULL)&&(current->right != NULL) ) || ( (current->left != NULL)&&(current->right == NULL) ) )
				{
					if(current->left == NULL) *Thead = current->right;
					else *Thead = current->left;
				}
				else
				{
					TNode *next;
					TNode *temp;
					*Thead = current->right;
					next = current->left;
					temp = current->right;
					while( 1 )
					{
						if( temp->left != NULL) temp = temp->left;
						else
						{
							if( temp->right != NULL) temp = temp->right;
							else break;
						}
					}
					temp->left = next;
				}
			}
			//delete the node and its list/name
			current->left = NULL;
			current->right = NULL;
			free(current->name);
			LNode *temp1;
			LNode *temp2;
			temp2 = current->L;
			while( temp2 != NULL )
			{
				temp1 = temp2;
				temp2 = temp2->next;
				free(temp1->name);
				free(temp1);
			}
			free(current);

			printf("- Deleted |%s|\n",str);
			return;
		}
		else if( strcmp(current->name,str) < 0)
		{
			previous = current;
			current = current->right;
			last = true;
		}
		else
		{
			previous = current;
			current = current->left;
			last = false;
		}
	}
}
