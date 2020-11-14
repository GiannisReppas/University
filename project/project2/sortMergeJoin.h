#include "types.h"
#include "radixsort.h"

/** Sort Merge Join **/
resultBucket* SortMergeJoin( relation *relR, relation *relS,resultBucket* Res);

/** MergeJoin **/
resultBucket* MergeJoin(relation*,relation*,resultBucket*);

/** function used for the list that contains the result **/
resultBucket* addToList(resultBucket* Res , uint64_t payload1, uint64_t payload2,resultBucket** Last,uint64_t intpayload1, uint64_t intpayload2);

/** free list **/
void free_list( resultBucket*);
