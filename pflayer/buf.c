/* buf.c: buffer management routines. The interface routines are:
PFbufGet(), PFbufUnfix(), PFbufAlloc(), PFbufReleaseFile(), PFbufUsed() and
PFbufPrint() */
#include <stdio.h>
#include "pf.h"
#include "pftypes.h"

static int PFnumbpage = 0;	/* # of buffer pages in memory */
static PFbpage *PFfirstbpage= NULL;	/* ptr to first buffer page, or NULL */
static PFbpage *PFlastbpage = NULL;	/* ptr to last buffer page, or NULL */
static PFbpage *PFfreebpage= NULL;	/* list of free buffer pages */

extern char *malloc();

static void PFbufInsertFree(bpage)
PFbpage *bpage;
/****************************************************************************
SPECIFICATIONS:
	Insert the buffer page pointed by "bpage" into the free list.

AUTHOR: clc
*****************************************************************************/
{
	bpage->nextpage = PFfreebpage;
	PFfreebpage = bpage;
}


static void PFbufLinkHead(bpage)
PFbpage *bpage;		/* pointer to buffer page to be linked */
/****************************************************************************
SPECIFICATIONS:

	Link the buffer page pointed by "bpage" as the head
	of the used buffer list. No other field of bpage is modified.

AUTHOR: clc

RETURN VALUE:
	none.

GLOBAL VARIABLES MODIFIED:
	PFfirstbpage, PFlastbpage.

*****************************************************************************/
{

	bpage->nextpage = PFfirstbpage;
	bpage->prevpage = NULL;
	if (PFfirstbpage != NULL)
		PFfirstbpage->prevpage = bpage;
	PFfirstbpage = bpage;
	if (PFlastbpage == NULL)
		PFlastbpage = bpage;
}
	
void PFbufUnlink(bpage)
PFbpage *bpage;		/* buffer page to be unlinked from the used list */
/****************************************************************************
SPECIFICATIONS:
	Unlink the page pointed by bpage from the buffer list. Assume
	that bpage is a valid pointer.  Set the "prevpage" and "nextpage"
	fields to NULL. The caller is responsible to either place
	the unlinked page into the free list, or insert it back
	into the used list.

AUTHOR: clc

RETURN VALUE:
	none

GLOBAL VARIABLES MODIFIED:
	PFfirstbpage,PFlastbpage.
*****************************************************************************/
{

	if (PFfirstbpage == bpage)
		PFfirstbpage = bpage->nextpage;
	
	if (PFlastbpage == bpage)
		PFlastbpage = bpage->prevpage;
	
	if (bpage->nextpage != NULL)
		bpage->nextpage->prevpage = bpage->prevpage;
	
	if (bpage->prevpage != NULL)
		bpage->prevpage->nextpage = bpage->nextpage;

	bpage->prevpage = bpage->nextpage = NULL;

}


static PFbufInternalAlloc(bpage,writefcn)
PFbpage **bpage;	/* pointer to pointer to buffer bpage to be allocated*/
int (*writefcn)();
/****************************************************************************
SPECIFICATIONS:
	Allocate a buffer page and set *bpage to point to it. *bpage
	is set to NULL if one can not be allocated.
	The "nextpage" and "prevpage" fields of *bpage are linked as
	the head of the list of used buffers.All the other fields are undefined.
	writefcn() is used to write pages. (See PFbufGet()).

ALGORITHM:
	If there is something on the free list, then use it.
	If free list is empty, and there are less than PF_MAX_BUFS 
	number of pages allocated, then malloc() one.
	Otherwise, choose a victim to write out, and then use that
	page as the page to be used.
	If a victim cannot be chosen (because all the pages are fixed),
	then return error.

AUTHOR: clc

RETURN VALUE:

	PFE_OK	if no error.
	PF_NOMEM	if no memory.
	PF_NOBUF	if no buffer space left because all pages are fixed.

GLOBAL VARIABLES MODIFIED:
	PFnumbpage, PFfirstbpage, PFlastbpage, PFfreebpage
*****************************************************************************/
{
PFbpage *tbpage;	/* temporary pointer to buffer page */
int error;		/* error value returned*/

	/* Set *bpage to the buffer page to be returned */
	if (PFfreebpage != NULL){
		/* Free list not empty, use the one from the free list. */
		*bpage = PFfreebpage;
		PFfreebpage = (*bpage)->nextpage;
	}
	else if (PFnumbpage < PF_MAX_BUFS){
		/* We have not reached max buffer limit, so
		malloc() a new one */
		if ((*bpage=(PFbpage *)malloc(sizeof(PFbpage)))==NULL){
			/* no mem */
			*bpage = NULL;
			PFerrno = PFE_NOMEM;
			return(PFerrno);
		}
		/* increment # of pages allocated */
		PFnumbpage++;
	}
	else {
		/* we have reached max buffer limit */
		/* choose a victim from the buffer*/

		*bpage = NULL;		/* set initial return value */

		for (tbpage=PFlastbpage;tbpage!=NULL;tbpage=tbpage->prevpage){
			if (!tbpage->fixed)
				/* found a page that can be swapped out */
				break;
		}

		if (tbpage == NULL){
			/* couldn't find a free page */
			PFerrno = PFE_NOBUF;
			return(PFerrno);
		}

		/* write out the dirty page */
		if (tbpage->dirty&&((error=(*writefcn)(tbpage->fd,
				tbpage->page,&tbpage->fpage))!= PFE_OK))
			return(error);
		tbpage->dirty = FALSE;

		/* unlink from hash table */
		if ((error=PFhashDelete(tbpage->fd,tbpage->page))!= PFE_OK)
			return(error);
		
		/* unlink from buffer list */
		PFbufUnlink(tbpage);

		*bpage = tbpage;

	}

	/* Link the page as the head of the used list */
	PFbufLinkHead(*bpage);
	return(PFE_OK);
}


/************************* Interface to the Outside World ****************/

PFbufGet(fd,pagenum,fpage,readfcn,writefcn)
int fd;	/* file descriptor */
int pagenum;	/* page number */
PFfpage **fpage;	/* pointer to pointer to file page */
int (*readfcn)();	/* function to read a page */
int (*writefcn)();	/* function to write a page */
/****************************************************************************
SPECIFICATIONS:
	Get a page whose number is "pagenum" from the file pointed
	by "fd". Set *fpage to point to the data for that page.
	This function requires two functions:
		readfcn(fd,pagenum,fpage) 
		int fd;
		int pagenum;
		PFfpage *fpage;
	which will read one page whose number is "pagenum" from the file "fd"
	into the buffer area pointed by "fpage".
		writefcn(fd,pagenum,fpage)
		int fd;
		in pagenum;
		PFpage *fpage;
	which will write one page into the file.
	It is an error to read a page already fixed in the buffer.

RETURN VALUE:
	PFE_OK	if no error.
	PF error code if error.
	If error code is PFE_PAGEFIXED, *fpage is still set to point to the buffer
	page of the page in memory.

GLOBAL VARIABLES MODIFIED:
*****************************************************************************/
{
PFbpage *bpage;	/* pointer to buffer */
int error;

	if ((bpage=PFhashFind(fd,pagenum)) == NULL){
		/* page not in buffer. */
		
		/* allocate an empty page */
		if ((error=PFbufInternalAlloc(&bpage,writefcn))!= PFE_OK){
			/* error */
			*fpage = NULL;
			return(error);
		}
		
		/* read the page */
		if ((error=(*readfcn)(fd,pagenum,&bpage->fpage))!= PFE_OK){
			/* error reading the page. put buffer back into 
			the free list, and return gracefully */
			PFbufUnlink(bpage);
			PFbufInsertFree(bpage);
			*fpage = NULL;
			return(error);
		}

		/* insert new page into hash table */
		if ((error=PFhashInsert(fd,pagenum,bpage))!=PFE_OK){
			/* failed to insert into hash table */
			/* put page into free list */
			PFbufUnlink(bpage);
			PFbufInsertFree(bpage);
			return(error);
		}

		/* set the fields for this page*/
		bpage->fd = fd;
		bpage->page = pagenum;
		bpage->dirty = FALSE;
	}
	else if (bpage->fixed){
		/* page already in memory, and is fixed, so we can't
		get it again. */
		*fpage = &bpage->fpage;
		PFerrno = PFE_PAGEFIXED;
		return(PFerrno);
	}

	/* Fix the page in the buffer then return*/
	bpage->fixed = TRUE;
	*fpage = &bpage->fpage;
	return(PFE_OK);
}

PFbufUnfix(fd,pagenum,dirty)
int fd;		/* file descriptor */
int pagenum;	/* page number */
int dirty;	/* TRUE if page is dirty */
/****************************************************************************
SPECIFICATIONS:
	Unfix the file page whose number is "pagenum" from the buffer.
	If dirty is TRUE, then mark the buffer as having been modified.
	Otherwise, the dirty flag is left unchanged.

AUTHOR: clc

RETURN VALUE:
	PFE_OK if no error.
	PF error codes if error occurs.

*****************************************************************************/
{
PFbpage *bpage;

	if ((bpage= PFhashFind(fd,pagenum))==NULL){
		/* page not in buffer */
		PFerrno = PFE_PAGENOTINBUF;
		return(PFerrno);
	}

	if (!bpage->fixed){
		/* page already unfixed */
		PFerrno = PFE_PAGEUNFIXED;
		return(PFerrno);
	}

	if (dirty)
		/* mark this page dirty */
		bpage->dirty = TRUE;
	
	/* unfix the page */
	bpage->fixed = FALSE;
	
	/* unlink this page */
	PFbufUnlink(bpage);

	/* insert it as head of linked list to make it most recently used*/
	PFbufLinkHead(bpage);

	return(PFE_OK);
}

PFbufAlloc(fd,pagenum,fpage,writefcn)
int fd;		/* file descriptor */
int pagenum;	/* page number */
PFfpage **fpage;	/* pointer to file page */
int (*writefcn)();
/****************************************************************************
SPECIFICATIONS:
	Allocate a buffer and mark it belonging to page "pagenum"
	of file "fd".  Set *fpage to point to the buffer data.
	The function "writefcn" is used to write out pages. (See PFbufGet()).

AUTHOR: clc

RETURN VALUE:
	PFE_OK if successful.
	PF error codes if unsuccessful
*****************************************************************************/
{
PFbpage *bpage;
int error;

	*fpage = NULL;	/* initial value of fpage */

	if ((bpage=PFhashFind(fd,pagenum))!= NULL){
		/* page already in buffer*/
		PFerrno = PFE_PAGEINBUF;
		return(PFerrno);
	}

	if ((error=PFbufInternalAlloc(&bpage,writefcn))!= PFE_OK)
		/* can't get any buffer */
		return(error);
	
	/* put ourselves into the hash table */
	if ((error=PFhashInsert(fd,pagenum,bpage))!= PFE_OK){
		/* can't insert into the hash table */
		/* unlink bpage, and put it into the free list */
		PFbufUnlink(bpage);
		PFbufInsertFree(bpage);
		return(error);
	}

	/* init the fields of bpage and return */
	bpage->fd = fd;
	bpage->page = pagenum;
	bpage->fixed = TRUE;
	bpage->dirty = FALSE;

	*fpage = &bpage->fpage;
	return(PFE_OK);
}


PFbufReleaseFile(fd,writefcn)
int fd;		/* file descriptor */
int (*writefcn)();	/* function to write a page of file */
/****************************************************************************
SPECIFICATIONS:
	Release all pages of file "fd" from the buffer and
	put them into the free list 

AUTHOR: clc

RETURN VALUE:
	PFE_OK if no error.
	PF error code if error.

IMPLEMENTATION NOTES:
	A linear search of the buffer is performed.
	A better method is not needed because # of buffers are small.
*****************************************************************************/
{
PFbpage *bpage;	/* ptr to buffer pages to search */
PFbpage *temppage;
int error;		/* error code */

	/* Do linear scan of the buffer to find pages belonging to the file */
	bpage = PFfirstbpage;
	while (bpage != NULL){
		if (bpage->fd == fd){
			/* The file descriptor matches*/
			if (bpage->fixed){
				PFerrno = PFE_PAGEFIXED;
				return(PFerrno);
			}

			/* write out dirty page */
			if (bpage->dirty&&((error=(*writefcn)(fd,bpage->page,
					&bpage->fpage))!= PFE_OK))
				/* error writing file */
				return(error);
			bpage->dirty = FALSE;

			/* get rid of it from the hash table */
			if ((error=PFhashDelete(fd,bpage->page))!= PFE_OK){
				/* internal error */
				printf("Internal error:PFbufReleaseFile()\n");
				exit(1);
			}

			/* put the page into free list */
			temppage = bpage;
			bpage = bpage->nextpage;
			PFbufUnlink(temppage);
			PFbufInsertFree(temppage);

		}
		else	bpage = bpage->nextpage;
	}
	return(PFE_OK);
}


PFbufUsed(fd,pagenum)
int fd;		/* file descriptor */
int pagenum;	/* page number */
/****************************************************************************
SPECIFICATIONS:
	Mark page numbered "pagenum" of file descriptor "fd" as used.
	The page must be fixed in the buffer. Make this page most
	recently used.

AUTHOR: clc

RETURN VALUE: PF error codes.

*****************************************************************************/
{
PFbpage *bpage;	/* pointer to the bpage we are looking for */

	/* Find page in the buffer */
	if ((bpage=PFhashFind(fd,pagenum))==NULL){
		/* page not in the buffer */
		PFerrno = PFE_PAGENOTINBUF;
		return(PFerrno);
	}

	if (!(bpage->fixed)){
		/* page not fixed */
		PFerrno = PFE_PAGEUNFIXED;
		return(PFerrno);
	}

	/* mark this page dirty */
	bpage->dirty = TRUE;

	/* make this page head of the list of buffers*/
	PFbufUnlink(bpage);
	PFbufLinkHead(bpage);

	return(PFE_OK);
}

void PFbufPrint()
/****************************************************************************
SPECIFICATIONS:
	Print the current page buffers.

AUTHOR: clc

*****************************************************************************/
{
PFbpage *bpage;

	printf("buffer content:\n");
	if (PFfirstbpage == NULL)
		printf("empty\n");
	else {
		printf("fd\tpage\tfixed\tdirty\tfpage\n");
		for(bpage = PFfirstbpage; bpage != NULL; bpage= bpage->nextpage)
			printf("%d\t%d\t%d\t%d\t%d\n",
				bpage->fd,bpage->page,(int)bpage->fixed,
				(int)bpage->dirty,(int)&bpage->fpage);
	}
}
