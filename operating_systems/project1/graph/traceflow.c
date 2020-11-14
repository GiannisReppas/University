#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "search_for.h"
#include "list.h"
#include "traceflow.h"
bool traceflow_rec(TNode **Thead,char *one,char *str1,char *str2,int current,int l,VNode **Vhead,bool check)
{
	TNode *temp;
	temp = *Thead;
	// we go to the node we are looking for
	while( temp != NULL)
	{
		if( !strcmp(temp->name,one) )
		{
			LNode *t;
			t = temp->L;
			//for each node in the list of the node
			while( t != NULL)
			{
				// if we found the end of the path
				if( ( !strcmp(t->name, str2) )&&( t->weight + current <= l) )
				{
					//print the path
					printf("- Cir-found |%s|->",str1);
					print_list(*Vhead);
					printf("%d->|%s|\n",t->weight,str2);
					check = true;

					//keep looking for more paths
					current += t->weight;
					add_list_end(Vhead,t->name,t->weight);
					check = traceflow_rec(Thead,t->name,str1,str2,current,l,Vhead,check);
					delete_list_last(Vhead);
					current -= t->weight;
				}
				//if we can visit more nodes
				else if( t->weight + current <= l )
				{
					current += t->weight;
					add_list_end(Vhead,t->name,t->weight);
					check = traceflow_rec(Thead,t->name,str1,str2,current,l,Vhead,check);
					delete_list_last(Vhead);
					current -= t->weight;
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

void traceflow(TNode **Thead,char *str1,char *str2,int l)
{
	//we check if the nodes we take exist
	if( !search_for(*Thead,str1) )
	{
		printf("- |%s| does not exist - abort-t;\n",str1);
		return;
	}
	if( !search_for(*Thead,str2) )
	{
		printf("- |%s| does not exist - abort-t;\n",str2);
		return;
	}

	bool check;
	check = false;
	VNode *Vhead;
	Vhead = NULL;
	check = traceflow_rec(Thead,str1,str1,str2,0,l,&Vhead,check);
	//if nothing was printed at traceflow_rec
	if( check == false) printf("- No-trace from |%s| to |%s|\n",str1,str2);
	return;
}
