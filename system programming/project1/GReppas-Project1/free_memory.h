void free_memory( bucket ***senderHashTable, bucket ***recieverHashTable, myBucket ***myHashTable, treeBucket ***treeHashTable, int senderHashTableNumberOfEntries, int recieverHashTableNumberOfEntries);
void free_buckets( bucket *current);
void free_buckets_for_my_structure( myBucket *current);
void free_list( listNode *current);
void free_bitcoin_list( bitCoinNode *current);
void free_tree( treeNode *current);
void free_buckets_for_tree_structure( treeBucket *current);
