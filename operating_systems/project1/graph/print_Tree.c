#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "search_for.h"
#include "print_Tree.h"
void print_Tree(TNode *Thead,int space)
{
	if (Thead == NULL) return;

	space +=10;

	print_Tree(Thead->right, space);

	printf("\n");

	for (int i = 10; i < space; i++) printf(" ");
	printf("%s  ( ", Thead->name);

	LNode *t;
	t = Thead->L;
	while( t != NULL)
	{
		printf("%s %d ,",t->name,t->weight);
		t = t->next;
	}
	printf("\b)\n");

	print_Tree(Thead->left, space);
}
