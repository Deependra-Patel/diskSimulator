# include <stdio.h>
# include "am.h"
# include "pf.h"

/* Inserts a key into a leaf node */
AM_InsertintoLeaf(pageBuf,attrLength,value,recId,index,status)
char *pageBuf;/* buffer where the leaf page resides */
int attrLength;
char *value;/* attribute value to be inserted*/
int recId;/* recid of the attribute to be inserted */
int index;/* index where key is to be inserted */
int status;/* Whether key is a new key or an old key */

{
	int recSize;
	char tempPage[PF_PAGE_SIZE];
	AM_LEAFHEADER head,*header;
	int errVal;


	/* initialise the header */
	header = &head;
	bcopy(pageBuf,header,AM_sl);

	recSize = attrLength + AM_ss;
	if (status == AM_FOUND)
		/* key is already present */ 
	{
		if (header->freeListPtr == 0)
			if ((header->recIdPtr - header->keyPtr) <(AM_si + AM_ss))
			{
				/* no room for one more record */
				return(FALSE);
			}
		/* insert into leaf - no need to split */
		AM_InsertToLeafFound(pageBuf,recId,index,header);
		bcopy(header,pageBuf,AM_sl);
		return(TRUE);
	}

	/* status == AM_NOTFOUND and so key is a new key */
	if ((header->freeListPtr) == 0)
		/* freelist empty */
		if ((header->recIdPtr - header->keyPtr) < (AM_si + AM_ss 
							     + recSize))
			return(FALSE);
		else
		{    
			AM_InsertToLeafNotFound(pageBuf,value,recId,index,
						header);
			header->numKeys++;
			bcopy(header,pageBuf,AM_sl);
			return(TRUE);
		}
	else /* freelist not empty */
	if ((header->recIdPtr - header->keyPtr) > recSize)
	{
		AM_InsertToLeafNotFound(pageBuf,value,recId,index,header);
		header->numKeys++;
		bcopy(header,pageBuf,AM_sl);
		return(TRUE);
	}
	else /* no place in the middle */
	if (((header->numinfreeList)*(AM_si + AM_ss) + header->recIdPtr -
	    header->keyPtr) > (recSize + AM_si + AM_ss))
	/*there is enough space in the freelist and in the middle put together */
	{
		/* Compact the freelist so that we get enough space in the middle                   so that the new key can be inserted */
		AM_Compact(1,header->numKeys,pageBuf,tempPage,header);
		
		bcopy(tempPage,pageBuf,PF_PAGE_SIZE);
		bcopy(pageBuf,header,AM_sl);
		/* Insert into leaf a new key - no need to split */
		AM_InsertToLeafNotFound(pageBuf,value,recId,index,header);
		header->numKeys++;
		bcopy(header,pageBuf,AM_sl);
		return(TRUE);
	}
	else /* there is not enough room in the page */
		return(FALSE);
}


/* Insert into leaf given the fact that the key is old */
AM_InsertToLeafFound(pageBuf,recId,index,header)
char *pageBuf;
int recId;
int index;
AM_LEAFHEADER *header;

{
	int recSize;
	short tempPtr;
	short oldhead;

	recSize = header->attrLength + AM_ss;
	if ((header->freeListPtr) == 0)
	{
		header->recIdPtr = header->recIdPtr - AM_si - AM_ss;
		tempPtr = header->recIdPtr;
	}
	else 
	{
		tempPtr = header->freeListPtr;
		header->numinfreeList--;
		bcopy(pageBuf + tempPtr + AM_si,(char *)&(header->freeListPtr)
		      ,AM_ss);
	}
	
	/* save  the old head of recId list */
	bcopy(pageBuf+AM_sl+(index-1)*recSize + header->attrLength,
	      (char *)&oldhead, AM_ss);

        /* Update the head of recId list to the new recid to be added */
	bcopy((char *)  &tempPtr,pageBuf+AM_sl + (index-1)*recSize + 
	       header->attrLength,AM_ss);

        /* Copy the recId*/
	bcopy((char *)&recId,pageBuf + tempPtr,AM_si);

	/* make the old head of list the second on list */
	bcopy((char *)&oldhead,pageBuf + tempPtr+AM_si,AM_ss);
}


/* Insert to a leaf given that the key is new */
AM_InsertToLeafNotFound(pageBuf,value,recId,index,header)
char *pageBuf;
char *value;
int recId;
int index;
AM_LEAFHEADER *header;

{
	int recSize;
	short null = AM_NULL;
	int i;

	recSize = header->attrLength + AM_ss;
	/* create space for the new key by pushing keys greater than that to 
								    the right */
	for(i = header->numKeys;i >= index; i--)
		{bcopy(pageBuf+AM_sl+(i-1)*recSize,pageBuf+AM_sl+i*recSize,recSize);
	 } 
	/* Update the header */
	header->keyPtr = header->keyPtr + recSize;
	
	/* copy the new key */
	bcopy(value,pageBuf+AM_sl+(index-1)*recSize,header->attrLength);
	
	/* make the head of list NULL*/
	bcopy((char *)&null,pageBuf+AM_sl+(index-1)*recSize+
	       header->attrLength,AM_ss);
	
	/* Now insert as if key were old key */
	AM_InsertToLeafFound(pageBuf,recId,index,header);
}


/* There may be quite a few entries in the freelist but there may not 
be space in the middle for a new key. This compacts all the recid's to the right
so that there is enough space in the middle */
AM_Compact(low,high,pageBuf,tempPage,header)

int low;
int high;
char *pageBuf;
char *tempPage;
AM_LEAFHEADER *header;

{

	short nextRec;
	AM_LEAFHEADER temphead,*tempheader;
	short recIdPtr;
	int recSize;
	int i,j;
	int offset1,offset2;

	tempheader = &temphead;
	bcopy(header,tempheader,AM_sl);
	
	recSize = header->attrLength + AM_ss;
	recIdPtr = PF_PAGE_SIZE - AM_si - AM_ss ;

	for (i = low, j = 1; i <= high; i++,j++)
	{
		offset1 = (i - 1) * recSize + AM_sl;
		offset2 = (j - 1) * recSize + AM_sl;
		bcopy(pageBuf + offset1,tempPage + offset2,header->attrLength);
		bcopy(pageBuf + offset1 + header->attrLength,(char *)&nextRec
		      ,AM_ss);
		bcopy((char *)&recIdPtr,tempPage + offset2 + header->attrLength,
		       AM_ss);
		while (nextRec != 0)
		{
			bcopy(pageBuf + nextRec,tempPage + recIdPtr,AM_si);
			recIdPtr = recIdPtr - AM_si - AM_ss;
			bcopy((char *)&recIdPtr,tempPage + recIdPtr + 2 * AM_si 
			       + AM_ss,
			AM_ss);
			bcopy(pageBuf + nextRec + AM_si,(char *)&nextRec,AM_ss);
		}
		bcopy((char *)&nextRec,tempPage + recIdPtr + 2 * AM_si + AM_ss,
		      AM_ss);
	}

	/* Initialise the header appropriately */
	tempheader->pageType = header->pageType;
	tempheader->nextLeafPage = header->nextLeafPage;
	tempheader->recIdPtr = recIdPtr + AM_si + AM_ss;
	tempheader->keyPtr = offset2 + recSize;
	tempheader->freeListPtr = 0;
	tempheader->numinfreeList = 0;
	tempheader->attrLength = header->attrLength;
	tempheader->numKeys = high - low + 1;
	tempheader->maxKeys = header->maxKeys;
	bcopy(tempheader,tempPage,AM_sl);

}

