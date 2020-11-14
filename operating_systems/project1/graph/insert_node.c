#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "search_for.h"
#include "insert_node.h"
#include "free_Tree.h"
bool insert_node(TNode **Thead , char *str)
{
	// if the tree is empty
	if( *Thead == NULL)
	{
		//build new node
		TNode *new_node;
		new_node = malloc( sizeof(TNode) );
		if( new_node == NULL)
		{
			puts("Error with malloc");
			free_Tree(Thead);
			return false;
		}
		new_node->L = NULL;
		new_node->left = NULL;
		new_node->right = NULL;
		new_node->name = malloc( sizeof(char)*(strlen(str)+1) );
		if(new_node == NULL)
		{
			puts("Error with malloc");
			free(new_node);
			free_Tree(Thead);
			return false;
		}
		strcpy(new_node->name , str);

		*Thead = new_node;

		printf("- Inserted |%s|\n",str);
		return true;
	}
	else
	{
		TNode *temp;
		temp = *Thead;
		//go to the place the new node should be placed
		while(1)
		{
			if( strcmp(temp->name,str) < 0)
			{
				//if we reached the node's position
				if(temp->right == NULL)
				{
					//build new node
					TNode *new_node;
					new_node = malloc( sizeof(TNode) );
					if( new_node == NULL)
					{
						puts("Error with malloc");
						free_Tree(Thead);
						return false;
					}
					new_node->L = NULL;
					new_node->left = NULL;
					new_node->right = NULL;
					new_node->name = malloc( sizeof(char)*(strlen(str)+1) );
					if(new_node == NULL)
					{
						puts("Error with malloc");
						free(new_node);
						free_Tree(Thead);
						return false;
					}
					strcpy(new_node->name , str);

					temp->right = new_node;

					printf("- Inserted |%s|\n",str);
					return true;
				}
				else temp = temp->right;
			}
			else if( strcmp(temp->name,str) > 0)
			{
				//if we reached the node's position
				if(temp->left == NULL)
				{
					//build new node
					TNode *new_node;
					new_node = malloc( sizeof(TNode) );
					if( new_node == NULL)
					{
						puts("Error with malloc");
						free_Tree(Thead);
						return false;
					}
					new_node->L = NULL;
					new_node->left = NULL;
					new_node->right = NULL;
					new_node->name = malloc( sizeof(char)*(strlen(str)+1) );
					if(new_node == NULL)
					{
						puts("Error with malloc");
						free(new_node);
						free_Tree(Thead);
						return false;
					}
					strcpy(new_node->name , str);

					temp->left = new_node;

					printf("- Inserted |%s|\n",str);
					return true;
				}
				else temp = temp->left;
			}
			else
			{
				printf("- Node |%s| exists;\n",str);
				return true;
			}
		}
	}
}
