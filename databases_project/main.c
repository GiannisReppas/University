#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HT.h"
#include "BF.h"

int command_line_check( int argc, char *argv[]);

int main( int argc, char *argv[])
{
	int f = command_line_check( argc, argv);
	char inputFile[14];
	if ( f == 1000) strcpy(inputFile,"records1K.txt");
	else if ( f == 5000) strcpy(inputFile,"records5K.txt");
	else if ( f == 10000) strcpy(inputFile,"records10K.txt");
	else if ( f == 15000) strcpy(inputFile,"records15K.txt");
	else { puts("Wrong input from command line"); return -1; }
	FILE *fp; fp=fopen(inputFile,"r"); if ( fp == NULL) { printf("Error with fopen()\n"); exit(1); }
	int id; char name[15]; char surname[20]; char address[40]; char *line=NULL; char *token; size_t len = 0;
	int i;

	BF_Init();

	char* fileName="primary.index";
	char attrType='i';
	char* attrName="id";
	int attrLength=4;
	int buckets=5;

	char* sfileName="secondary.index";
	char sAttrType='c';
	char* sAttrName="name";
	int sAttrLength=15;
	int sBuckets=15;

	/*
	C1: Create the  index.
	*/
	printf("@Checkpoint 1: Create and Open Index\n");
	int createNumCode=HT_CreateIndex(fileName,attrType,attrName,attrLength,buckets);
	if (createNumCode!=0)
	{
		printf("Checkpoint Result 1: FAIL\n");
	}
	HT_info* hi;
	/*
	C1: Open index.
	*/
	hi=HT_OpenIndex(fileName);
	if(hi!=NULL && hi->attrType==attrType && strcmp(hi->attrName,attrName)==0)
	{
		printf("Checkpoint Result 1: SUCCESS\n");
	}
	else
	{
		printf("Checkpoint Result 1: FAIL\n");
	}
	/*
	C2: Insert records.
	*/
	Record record;
	printf("@Checkpoint 2: Insert Records\n");
	for ( i=0; i<f/2 ;i++)
	{
		token = NULL;

		if(getline(&line, &len, fp) == -1)
			break;

		/* Extract id */
		fscanf(fp,"{%d",&id);

		/* Extract name */
		strtok(line,"\"");
		token = strtok(NULL,"\"");
		strcpy(name, token);

		/* Extract surname */
		strtok(NULL,"\"");
		token = strtok(NULL,"\"");
		strcpy(surname, token);

		/* Extract address */
		strtok(NULL,"\"");
		token = strtok(NULL,"\"");
		strcpy(address, token);

		record.id = id-1; strcpy(record.name, name); strcpy(record.surname, surname); strcpy(record.address, address);
		HT_InsertEntry(*hi,record);
	}
	/*
	C3: Get all entries that have even id.
	*/
	printf("@Checkpoint 4: Get all entries (Expecting greater than zero return code)\n");
	int ch4=0;
	for (i=0 ;i<f/2 ;i++)
	{
		record.id=i;
		sprintf(record.name,"name_%d",i);
		sprintf(record.surname,"surname_%d",i);
		sprintf(record.address,"address_%d",i);
		int err;
		if ( record.id % 2 == 0)
		{
			err=HT_GetAllEntries(*hi,(void*)&record.id);
			if (err<0)
			{
				ch4+=1;
			}
		}
	}
	if(ch4>0)
	{
		printf("Checkpoint Result 3: FAIL\n");
	}
	else
	{
		printf("Checkpoint Result 3: SUCCESS\n");
	}
	/*
	Secondary index part.
	*/
	/*
	C4: Create/Open secondary index.
	*/
	printf("@Checkpoint 4: Create Secondary/ Open Index\n");
	int createErrorCode=SHT_CreateSecondaryIndex(sfileName,sAttrName,sAttrLength,sBuckets,fileName);
	if (createErrorCode<0)
	{
		printf("Checkpoint Result 4: FAILED\n");
		return -1;
	}

	SHT_info* shi=SHT_OpenSecondaryIndex(sfileName);
	if(shi!=NULL)
	{
		printf("Checkpoint Result 4 SUCCESS\n");
	}
	else
	{
		printf("Checkpoint Result 4: FAIL\n");
	}
	/*
	Secondary index insert records.
	*/
	/*
	C5: Insert entries to both indexes.
	*/
	printf("@Checkpoint 5: Insert Records Secondary\n");
	int ch9=0;
	for (i=f/2; i<f; i++)
	{
		token = NULL;

		if(getline(&line, &len, fp) == -1)
			break;

		/* Extract id */
		fscanf(fp,"{%d",&id);

		/* Extract name */
		strtok(line,"\"");
		token = strtok(NULL,"\"");
		strcpy(name, token);

		/* Extract surname */
		strtok(NULL,"\"");
		token = strtok(NULL,"\"");
		strcpy(surname, token);

		/* Extract address */
		strtok(NULL,"\"");
		token = strtok(NULL,"\"");
		strcpy(address, token);

		record.id = id-1; strcpy(record.name, name); strcpy(record.surname, surname); strcpy(record.address, address); if ( i == f-1) record.id++;
		/*
		We need to do two inserts:
			* One in the HT.
			* One in the SHT.
		*/
		int blockId=HT_InsertEntry(*hi,record);
		if (blockId>0)
		{
			SecondaryRecord sRecord;
			sRecord.record=record;
			sRecord.blockId=blockId;
			int sInsertError=SHT_SecondaryInsertEntry(*shi,sRecord);
			if(sInsertError<0)
			{
				ch9+=1;
			}
		}
	}
	if (ch9==0)
	{
		printf("Checkpoint Result 5: SUCCESS\n");
	}
	else
	{
		printf("Checkpoint Result 5: Fail\n");
	}
	/*
	C6: Get all entries using the secondary index, that first number is 2
	*/
	printf("@Checkpoint 6: SHT Get all entries (All should exist except the deleted)\n");
	int ch10=0;
	for (i=0; i<f; i++)
	{
		record.id=i;
		sprintf(record.name,"name_%d",i);
		sprintf(record.surname,"surname_%d",i);
		sprintf(record.address,"address_%d",i);
		int err;
		if ( record.name[5] == 50)
		{
			SHT_SecondaryGetAllEntries(*shi,*hi,(void*)record.name);
			if (err<0)
			{
				ch10+=1;
			}
		}
	}
	if(ch10>0)
	{
		printf("Checkpoint Result 6: FAIL\n");
	}
	else
	{
		printf("Checkpoint Result 6: SUCCESS\n");
	}
	/* close */
	int htCloseError=HT_CloseIndex(hi);
	int shtCloseError=SHT_CloseSecondaryIndex(shi);
	if (htCloseError!=0 || shtCloseError!=0)
	{
		printf("Fail to close the index\n");
	}

	fclose(fp);
	free(line);

	return 0;
}

int command_line_check( int argc, char *argv[])
{
	if ( argc != 2) return -1;

	if ( !strcmp("records1K.txt",argv[1]) ) return 1000;
	else if ( !strcmp("records5K.txt",argv[1]) ) return 5000;
	else if ( !strcmp("records10K.txt",argv[1]) ) return 10000;
	else if ( !strcmp("records15K.txt",argv[1]) ) return 15000;
	else return -1;
}
