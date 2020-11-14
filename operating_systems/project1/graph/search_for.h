//////////////////////////////
/* This is a node of the list that contains the edges of a node*/
typedef struct ListNode
{
	int weight;
	char *name;
	struct ListNode *next;
}LNode;
/* This a node for a graph node*/
typedef struct TreeNode
{
	char *name;
	struct ListNode *L;
	struct TreeNode *left;
	struct TreeNode *right;
}TNode;
//////////////////////////////
/* This kind of nodes will be used for questions 7,8,9*/
/* even though they have same structure as LNode, will use them in a different way*/
typedef struct VisitedNode
{
	char *name;
	int weight;
	struct VisitedNode *next;
}VNode;
//////////////////////////////

bool search_for(TNode *Thead , char *str);
/* checks if the given node exists in the graph*/
