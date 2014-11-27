/* hash.c: Functions to facilitate finding the buffer page given
a file descriptor and a page number */
#include <stdio.h>
#include "pf.h"
#include "pftypes.h"

/* hash table */
static PFhash_entry *PFhashtbl[PF_HASH_TBL_SIZE];

extern char *malloc();

void PFhashInit()
/****************************************************************************
SPECIFICATIONS:
	Init the hash table entries. Must be called before any of the other
	hash functions are used.

AUTHOR: clc

RETURN VALUE: none

GLOBAL VARIABLES MODIFIED:
	PFhashtbl
*****************************************************************************/
{
int i;

	for (i=0; i < PF_HASH_TBL_SIZE; i++)
		PFhashtbl[i] = NULL;
}


PFbpage *PFhashFind(fd,page)
int fd;		/* file descriptor */
int page;	/* page number */
/****************************************************************************
SPECIFICATIONS:
	Given the file descriptor "fd", and page number "page",
	find the buffer address of this particular page.

AUTHOR: clc

RETURN VALUE:
	NULL	if not found.
	Buffer address, if found.

*****************************************************************************/
{
int bucket;	/* bucket to look for the page*/
PFhash_entry *entry; /* ptr to entries to search */


	/* See which bucket it is in */
	bucket = PFhash(fd,page);

	/* go through the linked list of this bucket */
	for (entry=PFhashtbl[bucket]; entry != NULL; 
				entry = entry->nextentry){
		if (entry->fd == fd && entry->page == page )
			/* found it */
			return(entry->bpage);
	}

	/* not found */
	return(NULL);
}

PFhashInsert(fd,page,bpage)
int fd;		/* file descriptor */
int page;	/* page number */
PFbpage *bpage;	/* buffer address for this page */
/*****************************************************************************
SPECIFICATIONS:
	Insert the file descriptor "fd", page number "page", and the
	buffer address "bpage" into the hash table. 

AUTHOR: clc

RETURN VALUE:
	PFE_OK	if OK
	PFE_NOMEM	if nomem
	PFE_HASHPAGEEXIST if the page already exists.
	
GLOBAL VARIABLES MODIFIED:
	PFhashtbl
*****************************************************************************/
{
int bucket;	/* bucket to insert the page */
PFhash_entry *entry; /* pointer to new entry */


	if (PFhashFind(fd,page) != NULL){
		/* page already inserted */
		PFerrno = PFE_HASHPAGEEXIST;
		return(PFerrno);
	}

	/* find the bucket for this page */
	bucket = PFhash(fd,page);
	
	/* allocate mem for new entry */
	if ((entry=(PFhash_entry *)malloc(sizeof(PFhash_entry)))== NULL){
		/* no mem */
		PFerrno = PFE_NOMEM;
		return(PFerrno);
	}

	/* Insert entry as head of list for this bucket */
	entry->fd = fd;
	entry->page = page;
	entry->bpage = bpage;
	entry->nextentry = PFhashtbl[bucket];
	entry->preventry = NULL;
	if (PFhashtbl[bucket] != NULL)
		PFhashtbl[bucket]->preventry = entry;
	PFhashtbl[bucket] = entry;

	return(PFE_OK);
}

PFhashDelete(fd,page)
int fd;		/* file descriptor */
int page;	/* page number */
/****************************************************************************
SPECIFICATIONS:
	Delete the entry whose file descriptor is "fd", and whose page number
	is "page" from the hash table.

AUTHOR: clc

RETURN VALUE:
	PFE_OK	if OK
	PFE_HASHNOTFOUND if can't find the entry

GLOBAL VARIABLES MODIFIED:
	PFhashtbl
*****************************************************************************/
{
int bucket;	/* bucket for this page */
PFhash_entry *entry;	/* entry to look for */

	/* find the bucket */
	bucket = PFhash(fd,page);

	/* See if the entry is in this bucket */
	for (entry=PFhashtbl[bucket]; entry != NULL; entry = entry->nextentry)
		if (entry->fd == fd && entry->page == page)
			break;

	if (entry == NULL){
		/* not found */
		PFerrno = PFE_HASHNOTFOUND;
		return(PFerrno);
	}

	/* get rid of this entry */
	if (entry == PFhashtbl[bucket])
		PFhashtbl[bucket] = entry->nextentry;
	if (entry->preventry != NULL)
		entry->preventry->nextentry = entry->nextentry;
	if (entry->nextentry != NULL)
		entry->nextentry->preventry = entry->preventry;
	free((char *)entry);

	return(PFE_OK);
}


PFhashPrint()
/****************************************************************************
SPECIFICATIONS:
	Print the hash table entries.

AUTHOR: clc

RETURN VALUE: None
*****************************************************************************/
{
int i;
PFhash_entry *entry;

	for (i=0; i < PF_HASH_TBL_SIZE; i++){
		printf("bucket %d\n",i);
		if (PFhashtbl[i] == NULL)
			printf("\tempty\n");
		else{
			for (entry = PFhashtbl[i]; entry != NULL;
					entry = entry->nextentry)
				printf("\tfd: %d, page: %d %d\n",
					entry->fd, entry->page,entry->bpage);
		}
	}
}
