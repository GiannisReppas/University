#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "search_for.h"
bool search_for(TNode *Thead , char *str)
{
	TNode *temp;
	temp = Thead;
	//start travelling to the node
	while( temp != NULL)
	{
		if( strcmp(temp->name,str) < 0) temp = temp->right;
		else if( strcmp(temp->name,str) > 0) temp = temp->left;
		else return true;
	}
	// if we reach a null node then it means the node we are looking for doesnt exist in the tree
	return false;
}
