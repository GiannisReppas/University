bool search_for_visit_f(char *str,VNode *Vhead,int w);/* searches if a node has been visited following the critiria of question 8*/
void find_circle(TNode **Thead, char *str, int k);/* searches for all the circles in a graph, while each edge is greter than k(question 8)*/
bool find_circle_rec(TNode **Thead, char *one, char *str, int k, VNode **Vhead, bool check);/* recursive part of question 8*/
