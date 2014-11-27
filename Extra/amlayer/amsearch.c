# include <stdio.h>
# include "am.h"
# include "pf.h"

/* searches for a key in a binary tree - returns FOUND or NOTFOUND and
returns the pagenumber and the offset where key is present or could 
be inserted */
AM_Search(fileDesc,attrType,attrLength,value,pageNum,pageBuf,indexPtr)
int fileDesc;
char attrType;
int attrLength;
char *value;
int *pageNum; /* page number of page where key is present or can be inserted*/
char **pageBuf; /* pointer to buffer in memory where leaf page corresponding                                                        to pageNum can be found */
int *indexPtr; /* pointer to index in leaf where key is present or 
                                                            can be inserted */

{
	int errVal;
	int nextPage; /* next page to be followed on the path from root to leaf*/
	int retval; /* return value */
	AM_LEAFHEADER lhead,*lheader; /* local pointer to leaf header */
	AM_INTHEADER ihead,*iheader; /* local pointer to internal node header */

        /* initialise the headeers */	
	lheader = &lhead;
	iheader = &ihead;

        /* get the root of the B+ tree */

	errVal = PF_GetFirstPage(fileDesc,pageNum,pageBuf);
	AM_Check;
	if (**pageBuf == 'l' ) 
		/* if root is a leaf page */
	{
		bcopy(*pageBuf,lheader,AM_sl);
		if (lheader->attrLength != attrLength)
			return(AME_INVALIDATTRLENGTH);
		
	}
	else /* root is not a leaf */
	{
		bcopy(*pageBuf,iheader,AM_sint);
		if (iheader->attrLength != attrLength)
			return(AME_INVALIDATTRLENGTH);
	}
	/* find the leaf at which key is present or can be inserted */
	while ((**pageBuf) != 'l')
	{
		/* find the next page to be followed */
		nextPage = AM_BinSearch(*pageBuf,attrType,attrLength,value,
					indexPtr,iheader);

		/* push onto stack for backtracking and splitting nodes if 
		needed later */
		AM_PushStack(*pageNum,*indexPtr);

		errVal = PF_UnfixPage(fileDesc,*pageNum,FALSE);
		AM_Check;

		/* set pageNum to the next page to be followed */
		*pageNum = nextPage;

		/* Get the next page to be followed */
		errVal = PF_GetThisPage(fileDesc,*pageNum,pageBuf);
		AM_Check;

		if (**pageBuf == 'l' ) 
		{
			/* if next page is a leaf */
			bcopy(*pageBuf,lheader,AM_sl);
			if (lheader->attrLength != attrLength)
				return(AME_INVALIDATTRLENGTH);
		}
		else
		{
			/* if next page is an internal node */
			bcopy(*pageBuf,iheader,AM_sint);
			if (iheader->attrLength != attrLength)
				return(AME_INVALIDATTRLENGTH);
		}
	}
	/* find whether key is in leaf or not */
	return(AM_SearchLeaf(*pageBuf,attrType,attrLength,value,indexPtr,lheader));
}


/* Finds the place (index) from where the next page to be followed is got*/
AM_BinSearch(pageBuf,attrType,attrLength,value,indexPtr,header)
char *pageBuf; /* buffer where the page is found */
char attrType; 
int attrLength;
char *value; /* attribute value for which search is called */
int *indexPtr;  
AM_INTHEADER *header;

{
	int low,high,mid; /* for binary search */
	int compareVal; /* result of comparison of key with value */
	int recSize; /* size in bytes of a key,ptr pair */
	int pageNum; /* page number of node to be followed along the B+ tree */

	recSize = AM_si  + attrLength;
	low = 1;
	high = header->numKeys;

	while ((high - low ) > 1)
	{
		/* get the middle key */
		mid = (low + high) / 2;

		/* compare the value with the middle key */
		compareVal = AM_Compare(pageBuf + AM_sint + AM_si + 
		(mid - 1)*recSize,attrType,attrLength,value); 
		
		if (compareVal < 0) 
			high = mid - 1 ;
		else if (compareVal > 0)
			low = mid + 1;
		else 
	        {   
			/* value = middle key */
			bcopy(pageBuf + AM_sint + mid*recSize,(char *)&pageNum,
			      AM_si);
			*indexPtr = mid;
			return(pageNum);
	        }
	}

	
	/* check the border cases */
	if ((high - low) == 0)
		if(AM_Compare(pageBuf+AM_sint+AM_si +(low - 1)*recSize,attrType,
		attrLength, value) < 0)
		{
			bcopy(pageBuf+AM_sint+(low-1)*recSize,(char *)&pageNum,
			      AM_si);
			*indexPtr = low -1;
			return(pageNum);
		}
		else
		{
			bcopy(pageBuf+AM_sint+low*recSize,(char *)&pageNum,
			      AM_si);
			*indexPtr = low;
			return(pageNum);
		}

	if ((high - low) == 1)
		if(AM_Compare(pageBuf+AM_sint+AM_si +(low - 1)*recSize,attrType,
		attrLength, value) < 0)
		{
			bcopy(pageBuf+AM_sint+(low-1)*recSize,(char *)&pageNum,
		              AM_si);
			*indexPtr = low -1;
			return(pageNum);
		}
		else
			if(AM_Compare(pageBuf+AM_sint+AM_si +low*recSize,
			attrType,attrLength, value) < 0)
			{
				bcopy(pageBuf+AM_sint+low*recSize,
				      (char *)&pageNum,AM_si);
				*indexPtr = low;
				return(pageNum);
			}
			else 
		        {
			bcopy(pageBuf+AM_sint+(low+1)*recSize,(char *)&pageNum,
			      AM_si);
			*indexPtr = low + 1;
			return(pageNum);
		        }

}



/* search a leaf node for the key- returns the place where it is found or can
be inserted */
AM_SearchLeaf(pageBuf,attrType,attrLength,value,indexPtr,header)
char *pageBuf; /* buffer where the leaf page resides */
char attrType;
int attrLength;
char *value; /* attribute value to be compared with */
int *indexPtr;/* pointer to the index where key is found or can be inserted */
AM_LEAFHEADER *header;


{
	int low,high,mid; /* for binary search */
	int compareVal; /* result of comparison of key with value */
	int recSize; /* size in bytes of a key,ptr pair */

	recSize = AM_ss + attrLength;
	low = 1;
	high = header->numKeys;

	/* The leaf is Empty */
	if (high == 0) 
	{
		*indexPtr = 1;
		return(AM_NOT_FOUND);
	}

	while ((high - low ) > 1)
	{
		/* get the middle key */
		mid = (low +high) / 2;

		/* compare value with the middle key */
		compareVal = AM_Compare(pageBuf + AM_sl + (mid - 1)*recSize,
		attrType,attrLength,value);

		if (compareVal < 0) 
			high = mid - 1 ;
		else if (compareVal > 0)
			low = mid + 1;
		else 
		    {   
			*indexPtr = mid;
			return(AM_FOUND);
		}
	}

	if ((high - low) == 0)
	{ 
		compareVal = AM_Compare(pageBuf+AM_sl+(low - 1)*recSize,
		attrType ,attrLength, value);


		if (compareVal < 0)
		{
			*indexPtr = low; 
			return(AM_NOT_FOUND);
		}
		else if (compareVal > 0)
		{
			*indexPtr = low + 1;
			return(AM_NOT_FOUND);
		}
		else 
		{
			*indexPtr = low;
			return(AM_FOUND);
		}
	}

	if ((high - low) == 1)
	{
		compareVal = AM_Compare(pageBuf+AM_sl+(low - 1)*recSize,
		attrType ,attrLength, value); 

		if (compareVal < 0)
		{
			*indexPtr = low; 
			return(AM_NOT_FOUND);
		}
		else if (compareVal == 0)
		{
			*indexPtr = low;
			return(AM_FOUND);
		}
		else
		{ 
			compareVal = AM_Compare(pageBuf+AM_sl+low*recSize,
			attrType, attrLength, value);

			if (compareVal < 0)
			{
				*indexPtr = low + 1;
				return(AM_NOT_FOUND);
			}
			else if (compareVal > 0) 
			{
				*indexPtr = low + 2;
				return(AM_NOT_FOUND);
			}
			else
			{
				*indexPtr = low + 1;
				return(AM_FOUND);
			}
		}
	}

}



/* Compare value in bufPtr with value in valPtr - returns -1 ,0 or 1 according
to whether value in valPtr is less than , equal to or greater than value 
in BufPtr*/
AM_Compare(bufPtr,attrType,attrLength,valPtr)
char *bufPtr;
char attrType;
char *valPtr;
int attrLength;

{
	int bufint,valint;/* temporary aligned storage for comparison */
	float buffloat,valfloat;/* temporary aligned storage for comparison */

	switch(attrType)
	{
	case 'i' : 
		{
			bcopy(bufPtr,(char *)&bufint,AM_si);
			bcopy(valPtr,(char *)&valint,AM_si);
			if (valint < bufint) return(-1);
			else if (valint > bufint) return(1);
			else return(0);
		}
	case 'f' : 
		{
			bcopy(bufPtr,(char *)&buffloat,AM_sf);
			bcopy(valPtr,(char *)&valfloat,AM_sf);
			if (valfloat < buffloat) return(-1);
			else if (valfloat > buffloat) return(1);
			else return(0);
		}
	case 'c' : 
		{
			return(strncmp(valPtr,bufPtr,attrLength));
		}
	}
}




