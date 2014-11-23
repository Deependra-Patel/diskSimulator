# include <stdio.h>
# include "am.h"
# include "pf.h"


/* splits a leaf node */

AM_SplitLeaf(fileDesc,pageBuf,pageNum,attrLength,recId,value,status,index,key)
int fileDesc; /* file descriptor */
char *pageBuf; /* pointer to buffer */
int *pageNum; /* pagenumber of new leaf created */
int attrLength; 
int recId;
char *value; /* attribute value for insert */

int status; /* Whether key was found or not in the tree */
int index; /* place where key is to be inserted */
char *key; /* returns the key to be filled in the parent */
{

	AM_LEAFHEADER head,temphead; /* local header */
	AM_LEAFHEADER *header,*tempheader;
	char tempPage[PF_PAGE_SIZE]; /* temporary page for manipulation on the 
								         page */
	char *tempPageBuf,*tempPageBuf1;/* buffers for new pages to be
								    allocated */
	int errVal; 
	int tempPageNum,tempPageNum1;/* pagenumbers for pages to be allocated */

	/* initialise pointers to headers */
	header = &head;
	tempheader = &temphead;

	/* copy header from buffer */
	bcopy(pageBuf,header,AM_sl);

	/* compact half the keys into temporary page */
	AM_Compact(1,(header->numKeys)/2,pageBuf,tempPage,header);

	/* Allocate a new page for the other half of the leaf*/
	errVal = PF_AllocPage(fileDesc,&tempPageNum,&tempPageBuf);
	AM_Check;

	/* compact the other half keys */
	AM_Compact((header->numKeys)/2 + 1,header->numKeys
			      ,pageBuf,tempPageBuf,header);

	/*check where key has to be inserted */
	if (index <= ((header->numKeys)/2))
	{
		/*value to be inserted is in first half */
		errVal = AM_InsertintoLeaf(tempPage,attrLength,value,recId,
					   index,status);
	}
	else
	{
		/* value to be inserted in second half */
		index = index - ((header->numKeys)/2);
		errVal = AM_InsertintoLeaf(tempPageBuf,attrLength,value,
					   recId,index,status);
	}

	/* change the next leafpage of first half of leaf to second half */
	bcopy(tempPage,tempheader,AM_sl);
	tempheader->nextLeafPage = tempPageNum;
	bcopy(tempheader,tempPage,AM_sl);
	bcopy(tempPage,pageBuf,PF_PAGE_SIZE);

	/* copy the value of key to be written onto the parent */

	bcopy(tempPageBuf + AM_sl,key,attrLength);


	/*check if the split page is root */
	if ((*pageNum) == AM_RootPageNum)
	{
		/* the page being split is the root*/
		/* Allocate a new page for another leaf as a new root has 
		to be created*/
		errVal = PF_AllocPage(fileDesc,&tempPageNum1,&tempPageBuf1);
		AM_Check;

		AM_LeftPageNum = tempPageNum1; /* this will remain the 
							   leftmost page hence*/

		/* copy the old first half(actually the root) into a new page */ 
		bcopy(pageBuf,tempPageBuf1,PF_PAGE_SIZE);
		/* Initialise the new root page */ 

		AM_FillRootPage(pageBuf,tempPageNum1,tempPageNum,key,
		header->attrLength ,header->maxKeys);
		errVal = PF_UnfixPage(fileDesc,tempPageNum1,TRUE);
		AM_Check;
	}

	errVal = PF_UnfixPage(fileDesc,*pageNum,TRUE);
	AM_Check;

	errVal = PF_UnfixPage(fileDesc,tempPageNum,TRUE);
	AM_Check;

	if ((*pageNum) == AM_RootPageNum)
		return(FALSE);
	else
	{
		*pageNum = tempPageNum;
		return(TRUE);
	}
}

/* Adds to the parent(on top of the path stack) attribute value and page Number*/
AM_AddtoParent(fileDesc,pageNum,value,attrLength)
int fileDesc;
int pageNum; /* page Number to be added to parent */
char *value; /*  pointer to attribute value to be added - 
                 gives back the attribute value to be added to it's parent*/
int attrLength;

{
	char tempPage[PF_PAGE_SIZE];/* temporary page for manipulating page */
	int pageNumber; /* pageNumber of parent to which key is to be added- 
			                                        got from stack*/
	int offset; /* Place in parent where key is to be added - 
								got from stack*/
	int errVal; 
	int pageNum1,pageNum2; /* pagenumber of new pages to be allocated */

	char *pageBuf,*pageBuf1,*pageBuf2;
	AM_INTHEADER head,*header;


	/* initialise header */
	header = &head;
	/* Get the top of stack values for the page number of the parent 
						 and offset of the key */
	AM_topofStack(&pageNumber,&offset);
	AM_PopStack();

	/* Get the parent node */
	errVal = PF_GetThisPage(fileDesc,pageNumber,&pageBuf);
	AM_Check;

	/* copy the header from buffer */
	bcopy(pageBuf,header,AM_sint);

	/* check if there is room in this node for another key */
	if ((header->numKeys) < (header->maxKeys))
	{
		/* add the attribute value to the node */ 
		AM_AddtoIntPage(pageBuf,value,pageNum,header,offset);

		/* copy the updated header into buffer*/
		bcopy(header,pageBuf,AM_sint) ;

		errVal = PF_UnfixPage(fileDesc,pageNumber,TRUE);
		AM_Check;
		return(AME_OK);
	}
	else
	{
		/* not enough room for another key */ 
		errVal = PF_AllocPage(fileDesc,&pageNum1,&pageBuf1);
		AM_Check;

		/* split the internal node */
		AM_SplitIntNode(pageBuf,tempPage,pageBuf1,header,
					 value,pageNum,offset);

		/* check if page being split is root */
		if (pageNumber == AM_RootPageNum)
		{
			/* allocate a new page for a new root */
			errVal = PF_AllocPage(fileDesc,&pageNum2,&pageBuf2);
			AM_Check;

			/* copy the first half into another buffer */
			bcopy(tempPage,pageBuf2,PF_PAGE_SIZE);

			/* fill the header of new root page and the 
			attribute value */
			AM_FillRootPage(pageBuf,pageNum2,pageNum1,value,
			header->attrLength, header->maxKeys);

			errVal = PF_UnfixPage(fileDesc,pageNumber,TRUE);
			AM_Check;

			errVal = PF_UnfixPage(fileDesc,pageNum1,TRUE);
			AM_Check;

			errVal = PF_UnfixPage(fileDesc,pageNum2,TRUE);
			AM_Check;

			return(AME_OK);
		}
		else
		{
			bcopy(tempPage,pageBuf,PF_PAGE_SIZE);

			errVal = PF_UnfixPage(fileDesc,pageNumber,TRUE);
			AM_Check;

			errVal = PF_UnfixPage(fileDesc,pageNum1,TRUE);
			AM_Check;

			/* recursive call to add to the parent of this 
			internal node*/
			errVal =  AM_AddtoParent(fileDesc,pageNum1,value,
						 attrLength);
			AM_Check;
		}
	}
	return(AME_OK);
}



/* adds a key to an internal node */
AM_AddtoIntPage(pageBuf,value,pageNum,header,offset)
char *pageBuf;
char *value; /* value to be added to the node */
int pageNum; /* page number of child to be inserted */
int offset; /* place where key is to be inserted */
AM_INTHEADER *header;

{
	int recSize;
	int i;

	recSize = header->attrLength + AM_si;

	/* shift all the keys greater than the one to be added to the right to
	make way for new key */
	for(i = header->numKeys; i > offset;i--)
		bcopy(pageBuf + AM_sint + (i-1)*recSize + AM_si,pageBuf + 
		      AM_sint +i*recSize + AM_si,recSize);

	/* copy the attribute value into the appropriate place */
	bcopy(value,pageBuf + AM_sint + offset*recSize + 
	      AM_si,header->attrLength);

	/* copy the pagenumber of the child */
	bcopy((char *)&pageNum,pageBuf + AM_sint + (offset+1)*recSize,AM_si);

	/* one more key added*/
	header->numKeys++;

}


/* Fills the header and inserts a key into a new root */
AM_FillRootPage(pageBuf,pageNum1,pageNum2,value,attrLength,maxKeys)
char *pageBuf;/* buffer to new root */
int pageNum1,pageNum2;/* pagenumbers of it;s two children*/
char *value; /* attr value to be inserted */
short attrLength,maxKeys; /* some info about the header */

{
	AM_INTHEADER temphead,*tempheader;

	tempheader = &temphead;

	/* fill the header */
	tempheader->pageType = 'i';
	tempheader->attrLength = attrLength;
	tempheader->maxKeys = maxKeys;
	tempheader->numKeys = 1;
	bcopy((char *)&pageNum1,pageBuf + AM_sint ,AM_si);
	bcopy(value,pageBuf + AM_sint + AM_si ,attrLength);
	bcopy((char *)&pageNum2,pageBuf + AM_sint + AM_si + attrLength,AM_si);
	bcopy(tempheader,pageBuf,AM_sint);

}


/* Split an internal node */
AM_SplitIntNode(pageBuf,pbuf1,pbuf2,header,value,pageNum,offset)
char *pageBuf;/* internal node to be split */
char *pbuf1,*pbuf2; /* the buffers for the two halves */
char *value; /*  pointer to key to be added and to be returned to parent*/
AM_INTHEADER *header;
int pageNum,offset;

{
	AM_INTHEADER temphead,*tempheader;
	int recSize;
	char tempPage[PF_PAGE_SIZE + AM_MAXATTRLENGTH];/* temp page for 
	                                               manipulating pageBuf */
	int length1,length2;

	tempheader = &temphead;
	recSize = header->attrLength + AM_si;

	tempheader->pageType = header->pageType;
	tempheader->attrLength = header->attrLength;
	tempheader->maxKeys = header->maxKeys;

	length1 = AM_si + (offset*recSize);
	/* copy the keys to the left of the key to be added */
	bcopy(pageBuf + AM_sint,tempPage,length1);

	/* copy the key itself */
	bcopy(value,tempPage + length1,header->attrLength);

	/* copy the pagenumber of the child node */
	bcopy((char *)&pageNum,tempPage + length1 + header->attrLength ,AM_si);

	length2 = (header->maxKeys - offset)*recSize;

	/* copy the rest of the keys */
	bcopy(pageBuf + AM_sint + length1,tempPage + length1 + header->attrLength 
	    + AM_si,length2);

	/* number of keys in each half */
	length1 = (header->maxKeys)/2;

	length2 = AM_si + length1*recSize;
	/* copy the first half into pbuf1 */
	bcopy(tempPage,pbuf1 + AM_sint,length2);
	tempheader->numKeys = length1;

	/* copy header into pbuf1 */
	bcopy(tempheader,pbuf1,AM_sint);

	/* copy the middle key into value to be passed back to parent */
	bcopy(tempPage + AM_si + length1 * recSize,value,header->attrLength);

	/* copy the second half into pbuf2*/
	bcopy(tempPage + AM_si + length1 * recSize + header->attrLength,
	      pbuf2 + AM_sint,length2); 
	bcopy(tempheader,pbuf2,AM_sint);

}

bcopy(char* s1, char *s2, int nbytes)
{
memcpy(s2,s1,nbytes);
}
