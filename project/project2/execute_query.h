#ifndef EXECUTE_QUERY
#define EXECUTE_QUERY

#include "types.h"
#include "updates.h"
#include "sums.h"
#include "sortMergeJoin.h"

/* takes info of a query and executes it */
uint64_t* execute_query( fileNode*, int, char**, int, char**, int, char**, int);

/* checks if a relation exists in the intermediate data structure */
int existsInIntermediate( char*, iNode*);

/* takes a relation(relR or relS) and initialize it based on the array in the intermediate data structure */
void createRelationFromIntermediate( fileNode*, iNode*, relation*, char*, char*, int*, int);

/* free all intermediate data structure */
void freeIntermediate( iNode**);

#endif
