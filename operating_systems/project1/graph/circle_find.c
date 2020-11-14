#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "search_for.h"
#include "list.h"
#include "circle_find.h"
bool search_for_visit_c(char *str,VNode *Vhead)
{
	VNode *temp;
	temp = Vhead;
	//for each node in the list
	while(temp != NULL)
	{
		// if the node exists in the list
		if( !strcmp(temp->name,str) ) return true;
		temp = temp->next;
	}
	return false;
}

bool circle_find_rec(TNode **Thead,char *one,char *str,VNode **Vhead,bool check)
{
	TNode *temp;
	temp = *Thead;
	//we find the node in the tree
	while( temp != NULL)
	{
		if( !strcmp(temp->name,one) )
		{
			LNode *t;
			t = temp->L;
			// for each node in the list
			while( t != NULL)
			{
				// if we found the circle
				if( !strcmp(t->name, str) )
				{
					printf("- Cir-found |%s|->",str);
					print_list(*Vhead);
					printf("%d->|%s|\n",t->weight,str);
					check = true;
				}
				else if( !search_for_visit_c(t->name,*Vhead)  )
				{
					add_list_end(Vhead,t->name,t->weight);
					check = circle_find_rec(Thead,t->name,str,Vhead,check);
					delete_list_last(Vhead);
				}
				t = t->next;
			}
			break;
		}
		else if( strcmp(temp->name,one) < 0 ) temp = temp->right;
		else temp = temp->left;
	}

	return check;
}

void circle_find(TNode **Thead,char *str)
{
	//we check if the node exists
	if( !search_for(*Thead,str) )
	{
		printf("- |%s| does not exist - abort-c;\n",str);
		return;
	}

	bool check;
	VNode *Vhead;
	Vhead = NULL;
	check = false;
	check = circle_find_rec(Thead,str,str,&Vhead,check);
	// if nothing was printed
	if( check == false) printf("- No-circle-found involving |%s|\n",str);
	return;
}
