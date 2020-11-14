#ifndef QUICKSORT
#define QUICKSORT

#include "types.h"

/** quicksort **/
void quickSort( relation *rel, int start, int end);
int partition ( relation *rel, int start, int end);
void swap( uint64_t *a, uint64_t *b);

#endif
