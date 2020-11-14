bool create_my_structure_and_tree_structure_and_put_names_in_hash_tables( bucket **senderHashTable, bucket **recieverHashTable, myBucket **myHashTable, treeBucket **treeHashTable, int senderHashTableNumberOfEntries, int recieverHashTableNumberOfEntries, FILE *bCBF_fp, int bucketSize, int bitCoinValue);
bool put_name_in_reciever_hash_table( bucket **recieverHashTable, int recieverHashTableNumberOfEntries, char *str, int bucketSize);
bool put_name_in_sender_hash_table( bucket **senderHashTable, int senderHashTableNumberOfEntries, char *str, int bucketSize);

