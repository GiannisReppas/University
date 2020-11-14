#ifndef UPDATES
#define UPDATES

#include "types.h"

/* updates a node of intermediate data structure in case join contains 2 arrays of the same node*/
bool updateSameNodeFilter( fileNode*, iNode**, char*, char*, char*, char*, int, int*);

/* in case of a join in the same array, calculates the filters */
bool updateSameArrayFilter( fileNode*, iNode**, char*, char*, char*, int, int*);

/*updates the when a numeric filter is to be executed, and the relation has been filtered before*/
bool updateNumericFilter(fileNode*,iNode**,char*,char*,char,int,int,int*);

/* inserts a new node after a join calculation in the intermediate data structure */
void intermediateInsertNewJoin( iNode**, resultBucket*, int, int);

/* inserts a new node after a filter calculation in the intermediate data structure */
void intermediateInsertNewFilter( iNode**, uint64_t*, uint64_t, char*);

/* puts another column in an array of a node in the intermediate data structure */
void updateIntermediateOneNode( iNode**, resultBucket*, char*, char*, int*, int, int);

/* takes the result after a sortMergeJoin of two arrays and merges it with the old nodes */
void updateIntermediateTwoNodes( iNode**, resultBucket*, char*, char*, int*, int , int);

#endif
