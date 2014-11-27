/* testhash.c: tests the hash table functions */
#include <stdio.h>
#include "pf.h"

main()
{
int i,k;
long j;

	PFhashInit();
	/* insert a few entries */
	for (i=1; i < 11; i++)
		for (j=1; j < 11; j ++){
			if (PFhashInsert(i,j,i+j) != PFE_OK){
				printf("PFhashInsert failed\n");
				exit(1);
			}
		}

	/* Now, find all the entries */
	for (i=1; i < 11; i++)
		for (j=1; j < 11; j++){
			k = PFhashFind(i,j);
			if (k == NULL){
				printf("PFfind failed at %d %d\n",i,j);
				exit(1);
			}
			else
			printf("found \n");
		}

	/* Now, delete them in reverse */
	for (j =10; j > 0; j--)
		for (i=10; i > 0; i--)
			if (PFhashDelete(i,j) != PFE_OK){
				printf("PFhashDelete failed at %d %d",i,j);
				exit(1);
			}

	/* print the hash table out */
	PFhashPrint();
}
