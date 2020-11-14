#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "search_for.h"
#include "list.h"
#include "find_circle.h"
bool search_for_visit_f(char *str,VNode *Vhead,int w)
{
	VNode *temp;
	temp = Vhead;
	//for each node in the list
	while(temp != NULL)
	{
		// if the edge exists
		if( ( !strcmp(temp->name,str) ) && (temp->weight == w) )return true;
		temp = temp->next;
	}
	return false;
}

bool find_circle_rec(TNode **Thead,char *one,char *str,int k,VNode **Vhead,bool check)
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
			//for ech node in the list
			while( t != NULL)
			{
				//if we found the circle
				if( !strcmp(t->name, str) )
				{
					printf("- Cir-found |%s|->",str);
					print_list(*Vhead);
					printf("%d->|%s|\n",t->weight,str);
					check = true;
				}
				else if( (t->weight >= k) &&( !search_for_visit_f(t->name,*Vhead,t->weight)  ) )
				{
					add_list_end(Vhead,t->name,t->weight);
					check = find_circle_rec(Thead,t->name,str,k,Vhead,check);
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

void find_circle(TNode **Thead, char *str, int k)
{
	//we check if the node exists
	if( !search_for(*Thead,str) )
	{
		printf("- |%s| does not exist - abort-f;\n",str);
		return;
	}

	bool check;
	VNode *Vhead;
	Vhead = NULL;
	check = false;
	check = find_circle_rec(Thead,str,str,k,&Vhead,check);
	//if nothing was printed
	if( check == false) printf("- No-circle-found involving |%s|\n",str);
	return;
}
