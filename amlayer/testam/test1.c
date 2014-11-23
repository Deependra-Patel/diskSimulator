/**********************************************************************
test1.c: tests simple index insertion and scans. 
************************************************************************/
#include <stdio.h>
#include "am.h"
#include "testam.h"

#define MAXRECS	50
#define MAX_FNAME_LENGTH 80	/* max length for file name */

main()
{
int id0,id1; /* index descriptor */
char ch;
int sd0,sd1; /* scan descriptors */
int i;
RecIdType recid;	/* record id */
char buf[NAMELENGTH]; /* buffer to store chars */
char fnamebuf[MAX_FNAME_LENGTH];	/* file name buffer */
int recnum;	/* record number */
int numrec;		/* # of records retrieved*/

	/* init */
	printf("initializing\n");
	PF_Init();

	/* create index on the both field of the record*/
	printf("creating indices\n");
	xAM_CreateIndex(RELNAME,RECNAME_INDEXNO,CHAR_TYPE,NAMELENGTH);
	xAM_CreateIndex(RELNAME,RECVAL_INDEXNO,INT_TYPE,sizeof(int));

	/* open the index */
	printf("opening indices\n");
	sprintf(fnamebuf,"%s.%d",RELNAME,RECNAME_INDEXNO);
	id0 = xPF_OpenFile(fnamebuf);
	sprintf(fnamebuf,"%s.%d",RELNAME,RECVAL_INDEXNO);
	id1 = xPF_OpenFile(fnamebuf);

	/* insert into index on character */
	printf("inserting into index on char\n");
	for( ch='a', recnum=0; ch <= 'z'; ch= succ(ch), recnum++){
		sprintf(buf,"%c%d",ch,recnum);
		xAM_InsertEntry(id0,CHAR_TYPE,NAMELENGTH,buf,IntToRecId(recnum));
	}

	printf("opening index scan on char\n");
	sd0 = xAM_OpenIndexScan(id0,CHAR_TYPE,NAMELENGTH,EQ_OP,NULL);
	printf("retrieving recid's from scan descriptor %d\n",sd0);
	numrec = 0;
	while((recnum=RecIdToInt(xAM_FindNextEntry(sd0)))>= 0){
		printf("%d\n",recnum);
		numrec++;
	}
	printf("retrieved %d records\n",numrec);

	/* insert into index on integer */
	printf("inserting into index of integer\n");
	for (recnum=0; recnum < MAXRECS; recnum++){
		xAM_InsertEntry(id1,INT_TYPE,sizeof(int),&recnum,IntToRecId(recnum));
	}

	/* Let's see if the insert works */
	printf("opening index scan on integer\n");
	sd1 = xAM_OpenIndexScan(id1,INT_TYPE,sizeof(int),EQ_OP,NULL);
	printf("retrieving recid's from scan descriptor %d\n",sd1);
	numrec = 0;
	while((recnum=RecIdToInt(xAM_FindNextEntry(sd1)))>= 0){
		printf("%d\n",recnum);
		numrec++;
	}
	printf("retrieved %d records\n",numrec);


	/* destroy everything */
	printf("closing down\n");
	xAM_CloseIndexScan(sd0);
	xAM_CloseIndexScan(sd1);
	xPF_CloseFile(id0);
	xPF_CloseFile(id1);
	xAM_DestroyIndex(RELNAME,RECNAME_INDEXNO);
	xAM_DestroyIndex(RELNAME,RECVAL_INDEXNO);

	printf("test1 done!\n");
}
