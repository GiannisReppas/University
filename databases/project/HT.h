#ifndef HT_H
#define HT_H

typedef struct
{
	int id;
	char name[15];
	char surname[20];
	char address[40];
}Record;

typedef struct
{
	int fileDesc;
	char attrType;
	char *attrName;
	int attrLength;
	long int numBuckets;
}HT_info;

typedef struct
{
	Record record;
	int blockId;
}SecondaryRecord;

typedef struct
{
	int fileDesc;
	char* attrName;
	int attrLength;
	long int numBuckets;
	char *fileName;
}SHT_info;


extern int primaryFileDescriptor;

int SHT_CreateSecondaryIndex( char *sfileName, char* attrName, int attrLength, int buckets, char* fileName);
SHT_info* SHT_OpenSecondaryIndex(char *sfileName);
int SHT_CloseSecondaryIndex( SHT_info* header_info );
int SHT_SecondaryInsertEntry( SHT_info header_info, SecondaryRecord record);
int SHT_SecondaryGetAllEntries( SHT_info header_info_sht, HT_info header_info_ht, void *value);

int HashStatistics( char *fileName);

int HT_DeleteEntry( HT_info header_info, void *value);
int HT_GetAllEntries( HT_info header_info, void *value);
int HT_InsertEntry( HT_info header_info, Record record);
int HT_CreateIndex( char *fileName, char attrType, char *attrName, int attrLength, int buckets);
int HT_CloseIndex( HT_info *header_info);
HT_info *HT_OpenIndex( char *fileName);

int universalHashingInt( int M, int x);
int universalHashingString( int M, char *str);


#endif
