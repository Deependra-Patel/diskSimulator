/* test3.c: tests deletion and scan. */
#include <stdio.h>
#include "am.h"
#include "pf.h"
#include "testam.h"

#define MAXRECS	10000	/* max # of records to insert */
#define FNAME_LENGTH 80	/* file name size */

main()
{
int fd;	/* file descriptor for the index */
char fname[FNAME_LENGTH];	/* file name */
int recnum;	/* record number */
int sd;	/* scan descriptor */
int numrec;	/* # of records retrieved */
int testval;	

	/* init */
	printf("initializing\n");
	PF_Init();

	/* create index */
	printf("creating index\n");
	AM_CreateIndex(RELNAME,0,INT_TYPE,sizeof(int));

	/* open the index */
	printf("opening index\n");
	sprintf(fname,"%s.0",RELNAME);
	fd = PF_OpenFile(fname);

	/* first, make sure that simple deletions work */
	printf("inserting into index\n");
	for (recnum=0; recnum < 20; recnum++){
		AM_InsertEntry(fd,INT_TYPE,sizeof(int),(char *)&recnum,
				recnum);
	}
	printf("deleting odd number records\n");
	for (recnum=1; recnum < 20; recnum += 2)
		AM_DeleteEntry(fd,INT_TYPE,sizeof(int),(char *)&recnum,
					recnum);

	printf("retrieving even number records\n");
	numrec= 0;
	sd = AM_OpenIndexScan(fd,INT_TYPE,sizeof(int),EQ_OP,NULL);
	while((recnum=AM_FindNextEntry(sd))>= 0){
		printf("%d\n",recnum);
		numrec++;
	}
	printf("retrieved %d records\n",numrec);
	AM_CloseIndexScan(sd);

	printf("deleting even number records\n");
	for (recnum=0; recnum < 20; recnum += 2)
		AM_DeleteEntry(fd,INT_TYPE,sizeof(int),(char *)&recnum,
					recnum);

	printf("retrieving from empty index\n");
	numrec= 0;
	sd = AM_OpenIndexScan(fd,INT_TYPE,sizeof(int),EQ_OP,NULL);
	while((recnum=AM_FindNextEntry(sd))>= 0){
		printf("%d\n",recnum);
		numrec++;
	}
	printf("retrieved %d records\n",numrec);
	AM_CloseIndexScan(sd);


	/* insert into index */
	printf("begin test of complex delete\n");
	printf("inserting into index\n");
	for (recnum=0; recnum < MAXRECS; recnum+=2){
		AM_InsertEntry(fd,INT_TYPE,sizeof(int),(char *)&recnum,
				recnum);
	}
	for (recnum=1; recnum < MAXRECS; recnum+=2)
		AM_InsertEntry(fd,INT_TYPE,sizeof(int),(char *)&recnum,
			recnum);

	/* delete everything */
	printf("deleting everything\n");
	for (recnum=1; recnum < MAXRECS; recnum += 2)
		AM_DeleteEntry(fd,INT_TYPE,sizeof(int),(char *)&recnum,
					recnum);
	for (recnum=0; recnum < MAXRECS; recnum +=2)
		AM_DeleteEntry(fd,INT_TYPE,sizeof(int),(char *)&recnum,
				recnum);


	/* print out what remains */
	printf("printing empty index\n");
	numrec= 0;
	sd = AM_OpenIndexScan(fd,INT_TYPE,sizeof(int),EQ_OP,NULL);
	while((recnum=AM_FindNextEntry(sd))>= 0){
		printf("%d\n",recnum);
		numrec++;
	}
	printf("retrieved %d records\n",numrec);
	AM_CloseIndexScan(sd);

	/* insert everything back */
	printf("inserting everything back\n");
	for (recnum=0; recnum < MAXRECS; recnum++){
		AM_InsertEntry(fd,INT_TYPE,sizeof(int),(char *)&recnum,
				recnum);
	}
	/* delete records less than 100, using scan!! */
	printf("delete records less than 100\n");
	testval = 100;
	sd = AM_OpenIndexScan(fd,INT_TYPE,sizeof(int),LT_OP,(char *)&testval);
	while((recnum=AM_FindNextEntry(sd))>= 0){
		if (recnum >= 100){
			printf("invalid recnum %d\n",recnum);
			exit(1);
		}
		AM_DeleteEntry(fd,INT_TYPE,sizeof(int),(char *)&recnum,
		recnum);
	}
	AM_CloseIndexScan(sd);

	/* delete records greater than 150, using scan */
	printf("delete records greater than 150\n");
	testval = 150;
	sd = AM_OpenIndexScan(fd,INT_TYPE,sizeof(int),GT_OP,(char *)&testval);
	while((recnum=AM_FindNextEntry(sd))>= 0){
		if (recnum <= 150){
			printf("invalid recnum %d\n",recnum);
			exit(1);
		}
		AM_DeleteEntry(fd,INT_TYPE,sizeof(int),(char *)&recnum,
					recnum);
	}
	AM_CloseIndexScan(sd);


	/* print out what remains */
   printf("printing between 100 and 150\n");
	numrec= 0;
	sd = AM_OpenIndexScan(fd,INT_TYPE,sizeof(int),EQ_OP,NULL);
	while((recnum=AM_FindNextEntry(sd))>= 0){
		printf("%d\n",recnum);
		numrec++;
	}
	printf("retrieved %d records\n",numrec);
	AM_CloseIndexScan(sd);

	/* destroy everything */
	printf("closing down\n");
	PF_CloseFile(fd);
		AM_DestroyIndex(RELNAME,0);

	printf("test3 done!\n");
}
