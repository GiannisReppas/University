bool check_if_name_is_valid( char *name, myBucket **myHashTable, int senderHashTableNumberOfEntries, int recieverHashTableNumberOfEntries);
bool check_if_bitCoin_is_valid( int bitCoinId, treeBucket **treeHashTable, int senderHashTableNumberOfEntries, int recieverHashTableNumberOfEntries);
bool check_if_transaction_is_valid(int *lastTransactionId, char **lastDateTime, int transactionId, char *sender, char *reciever, char *value, char *date, char *time, bucket **senderHashTable, bucket **recieverHashTable, myBucket **myHashTable, treeBucket **treeHashTable, int senderHashTableNumberOfEntries, int recieverHashTableNumberOfEntries);