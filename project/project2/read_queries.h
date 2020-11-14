#ifndef READ_QUERIES
#define READ_QUERIES

#include "types.h"
#include "execute_query.h"
#include "sums.h"

/* read queries the users gives */
void read_queries( fileNode*, int, FILE*);

/* takes a string input and splits it into parts based on toBreak char. Results are written in array */
int split( char*, char, char***);

/* takes an array of predicates and puts the filters in the first places of the array */
void swap_predicates( char***, int);

#endif
