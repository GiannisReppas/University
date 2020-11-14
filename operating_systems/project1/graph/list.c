#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "search_for.h"
#include "list.h"
void delete_list_last(VNode **Vhead)
{
	//if the list has a last node(not empty)
	if(*Vhead != NULL)
	{
		VNode *current;
		VNode *previous;
		current = *Vhead;
		// if the list has only one node
		if( current->next == NULL)
		{
			*Vhead = NULL;
			free(current->name);
			free(current);
		}
		else
		{
			// go to the end
			while(current->next != NULL)
			{
				previous = current;
				current = current->next;
			}
			previous->next = NULL;
			free(current->name);
			free(current);
		}
	}
}

void print_list(VNode *Vhead)
{
	VNode *temp;
	temp = Vhead;
	//visit each node and print info
	while( temp != NULL)
	{
		printf("%d->|%s|->",temp->weight,temp->name);
		temp = temp->next;
	}
}

void add_list_end(VNode **Vhead,char *str,int w)
{
	//if list is empty
	if( *Vhead == NULL)
	{
		//build new node
		VNode *new_node;
		new_node = malloc(sizeof(VNode));
		if(new_node == NULL)
		{
			puts("Error with malloc");
			exit(1);
		}
		new_node->next = NULL;
		new_node->weight = w;
		new_node->name = malloc( sizeof(char)*(strlen(str)+1) );
		if(new_node->name == NULL)
		{
			puts("Error with malloc");
			exit(1);
		}
		strcpy(new_node->name,str);

		*Vhead = new_node;
	}
	else
	{
		VNode *temp;
		temp = *Vhead;
		//go to the end
		while(temp->next != NULL) temp = temp->next;

		//build new node
		VNode *new_node;
		new_node = malloc(sizeof(VNode));
		if(new_node == NULL)
		{
			puts("Error with malloc");
			exit(1);
		}
		new_node->next = NULL;
		new_node->weight = w;
		new_node->name = malloc( sizeof(char)*(strlen(str)+1) );
		if(new_node->name == NULL)
		{
			puts("Error with malloc");
			exit(1);
		}
		strcpy(new_node->name,str);

		temp->next = new_node;
	}
}
