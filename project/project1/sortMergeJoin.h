#include "types.h"
#include "radixsort.h"

/** Sort Merge Join **/
resultBucket* SortMergeJoin( relation *relR, relation *relS,resultBucket* Res, bool testing);

/** MergeJoin **/
resultBucket* MergeJoin(relation*,relation*,resultBucket*);

/** function used for the list that contains the result **/
resultBucket* addToList(resultBucket*, uint64_t, uint64_t,resultBucket**);
