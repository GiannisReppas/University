// for sender and reciever Hash Table
typedef struct listNode
{
	char *info;
	struct listNode *next;
}listNode;

// for buckets of sender and reciever hash table
typedef struct walletId
{
	char *name;
	listNode *lhead;
}walletId;

// buckets for sender and reciever hash table
typedef struct bucket
{
	walletId *walletIdArray;
	int arraySize;
	int remainingSpace;
	struct bucket *next;
}bucket;

// for myHashTable
typedef struct bitCoinNode
{
	int bitCoinId;
	int rest;
	struct bitCoinNode *next;
}bitCoinNode;

// buckets for myHashTable
typedef struct myBucket
{
	char *name;
	int balance;
	bitCoinNode *lhead;
	struct myBucket *next;
}myBucket;

// for tree Hash Table
typedef struct treeNode
{
	char *wId;
	int money;
	struct treeNode *right;
	struct treeNode *left;
	listNode *tran;
	bool duplicated;
}treeNode;

// buckets for tree Hash Table
typedef struct treeBucket
{
	int bCId;
	treeNode *thead;
	struct treeBucket *next;
}treeBucket;
