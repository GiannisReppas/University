void traceflow(TNode **Thead,char *str1,char *str2,int l);/* finds all the paths from a node to another node , while the path is lower than l*/
bool traceflow_rec(TNode **Thead,char *one,char *str1,char *str2,int current,int l,VNode **Vhead,bool check);/* recursive part of question 9*/
