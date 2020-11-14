#ifndef SUMS
#define SUMS

#include "types.h"

/* inserts in sumS list */
void insertSums( sumsNode**, uint64_t**, uint64_t);

/* prints the list with all the collected sums */
void printSumsList( sumsNode*);

/* frees all memory allocated for sums list */
void freeSumsList( sumsNode**);

/* calculates the sums of all columns the user gave */
uint64_t *calcSums( fileNode*, iNode *head, char**, int, int*);

#endif
