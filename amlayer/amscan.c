
# include <stdio.h>
# include "am.h"
# include "pf.h"

/* The structure of the scan Table */
struct {
         int fileDesc;
         int op;
         int attrType;
         int pageNum;
         short index;
         short actindex;
         int nextpageNum;
         char nextvalue[AM_MAXATTRLENGTH];
         short nextIndex;
         short nextRecIdPtr;
         int lastpageNum;
         short lastIndex;
         int status;
       } AM_scanTable[MAXSCANS];


/* Opens an index scan */
AM_OpenIndexScan(fileDesc,attrType,attrLength,op,value)
int fileDesc; /* file Descriptor */

char attrType; /* 'i' or 'c' or 'f' */
int attrLength; /* 4 for 'i' or 'f' , 1-255 for 'c' */
int op; /* operator for comparison */
char *value; /* value for comparison */

{
int scanDesc; /* index into scan table */
int status; /* whether value is found or not in the tree */
int index; /* index of value in leaf */
int pageNum;/* page number of leaf page where value is found */
int recSize; /* size of key,ptr pair in leaf */
char *pageBuf; /* buffer for page */
int errVal; /* return value of functions */
AM_LEAFHEADER head,*header; /* local header */
int searchpageNum;



/* check the parameters */
if (fileDesc < 0)
  {
   AM_Errno = AME_FD;
   return(AME_FD);
  }

if ((attrType != 'i') && (attrType != 'c') && (attrType != 'f'))
  {
  AM_Errno = AME_INVALIDATTRTYPE;
  return(AME_INVALIDATTRTYPE);
  }

/* initialise header */
header = &head;

/* find a vacant place in the scan table */
for (scanDesc = 0; scanDesc <  MAXSCANS;scanDesc++)
  if (AM_scanTable[scanDesc].status == FREE) break;

/* scan table is full */
if (scanDesc > MAXSCANS - 1) 
 {
 AM_Errno = AME_SCAN_TAB_FULL;
 return(AME_SCAN_TAB_FULL);
 }

/* there is room */
AM_scanTable[scanDesc].status = FIRST;
AM_scanTable[scanDesc].attrType = attrType;

/* initialise AM_LeftPageNum */
AM_LeftPageNum = GetLeftPageNum(fileDesc);

/* scan of all keys */
if (value == NULL)
  {
   AM_scanTable[scanDesc].fileDesc = fileDesc;
   AM_scanTable[scanDesc].op = ALL;
   AM_scanTable[scanDesc].nextpageNum = AM_LeftPageNum;
   AM_scanTable[scanDesc].nextIndex = 1;
   AM_scanTable[scanDesc].actindex = 1;
   errVal = PF_GetThisPage(fileDesc,AM_LeftPageNum,&pageBuf);
   AM_Check;
   bcopy(pageBuf + AM_sl + attrLength,&AM_scanTable[scanDesc].nextRecIdPtr,AM_ss);
   errVal = PF_UnfixPage(fileDesc,AM_LeftPageNum,FALSE);
   AM_Check;
   return(scanDesc);
  }
  
/* search for the pagenumber and index of value */
status = AM_Search(fileDesc,attrType,attrLength,value,&pageNum,&pageBuf,&index);
searchpageNum = pageNum;
/* check for errors */
if (status < 0) 
  { AM_scanTable[scanDesc].status = FREE;
    AM_Errno = status;
    return(status);
  }

bcopy(pageBuf,header,AM_sl);
recSize = attrLength + AM_ss;
AM_scanTable[scanDesc].fileDesc = fileDesc;
AM_scanTable[scanDesc].op = op;

/* value is not in leaf but if inserted will have to be inserted after the last
key */
if (index > header->numKeys) 
  if (header->nextLeafPage != AM_NULL_PAGE)
  {
  errVal = PF_GetThisPage(fileDesc,header->nextLeafPage,&pageBuf);
  AM_Check;
  bcopy(pageBuf,header,AM_sl);
  errVal = PF_UnfixPage(fileDesc,header->nextLeafPage,FALSE);
  AM_Check;
  pageNum = header->nextLeafPage;
  index = 1;
  }
  else 
   pageNum = AM_NULL_PAGE;

AM_scanTable[scanDesc].pageNum = pageNum;
AM_scanTable[scanDesc].index = index;



/* case on op */
switch(op) 
 {
  case EQUAL : 
               {
                /* value not in leaf - no match */
		if (status != AM_FOUND)
                   AM_scanTable[scanDesc].status = OVER;
                else
                 {
                  AM_scanTable[scanDesc].nextpageNum = pageNum;
                  AM_scanTable[scanDesc].nextIndex = index;
                  AM_scanTable[scanDesc].actindex = index;
                  bcopy(pageBuf + AM_sl + (index - 1)*recSize + attrLength,
                          &AM_scanTable[scanDesc].nextRecIdPtr,AM_ss);
                  AM_scanTable[scanDesc].lastpageNum  = pageNum;
                  AM_scanTable[scanDesc].lastIndex  = index;
                 }
                break;
               }
  case LESS_THAN : 
               {
                AM_scanTable[scanDesc].nextpageNum = AM_LeftPageNum;
                AM_scanTable[scanDesc].nextIndex = 1;
                AM_scanTable[scanDesc].actindex = 1;
                if (searchpageNum != AM_LeftPageNum)
		 { errVal = PF_GetThisPage(fileDesc,AM_LeftPageNum,&pageBuf);
                  AM_Check;
                 }
                bcopy(pageBuf + AM_sl + attrLength,
                        &AM_scanTable[scanDesc].nextRecIdPtr,AM_ss);
                if (searchpageNum != AM_LeftPageNum)
		 {
		  errVal = PF_UnfixPage(fileDesc,AM_LeftPageNum,FALSE);
                  AM_Check;
                 }
                AM_scanTable[scanDesc].lastpageNum  = pageNum;
                AM_scanTable[scanDesc].lastIndex  = index - 1 ;
                break;
               }
  case GREATER_THAN :
               {
                if (status == AM_FOUND)
                 if ((index + 1) <= (header->numKeys))
                  {
                   AM_scanTable[scanDesc].nextpageNum = pageNum;
                   AM_scanTable[scanDesc].nextIndex = index + 1;
                   AM_scanTable[scanDesc].actindex = index + 1;
                   bcopy(pageBuf + AM_sl + (index)*recSize + attrLength,
                             &AM_scanTable[scanDesc].nextRecIdPtr,AM_ss);
                  }
                 else
                   /* got to start from next leaf page */
		   if (header->nextLeafPage != AM_NULL_PAGE)
                   {
                   AM_scanTable[scanDesc].nextpageNum = header->nextLeafPage;
                   AM_scanTable[scanDesc].nextIndex =  1;
                   AM_scanTable[scanDesc].actindex = 1;
                   errVal =PF_GetThisPage(fileDesc,header->nextLeafPage,&pageBuf);
                   AM_Check;
                   bcopy(pageBuf + AM_sl + attrLength,
                           &AM_scanTable[scanDesc].nextRecIdPtr,AM_ss);
                   errVal = PF_UnfixPage(fileDesc,header->nextLeafPage,FALSE);
                   AM_Check;
                   }
                   else /* Nextleafpage is not last NULL page */
                     AM_scanTable[scanDesc].status = OVER;
                else /* status == AM_NOT_FOUND */ 
                 {
                   AM_scanTable[scanDesc].nextpageNum = pageNum;
                   AM_scanTable[scanDesc].nextIndex = index ;
                   AM_scanTable[scanDesc].actindex = index;
                   bcopy(pageBuf + AM_sl + (index - 1)*recSize + attrLength,
                              &AM_scanTable[scanDesc].nextRecIdPtr,AM_ss);
                 }
                break;
               }
  case LESS_THAN_EQUAL :
               {
               AM_scanTable[scanDesc].nextpageNum = AM_LeftPageNum;
               AM_scanTable[scanDesc].nextIndex = 1;
               AM_scanTable[scanDesc].actindex = 1;
                if (searchpageNum != AM_LeftPageNum)
		 { errVal = PF_GetThisPage(fileDesc,AM_LeftPageNum,&pageBuf);
                  AM_Check;
                 }
               bcopy(pageBuf + AM_sl + attrLength,
                                 &AM_scanTable[scanDesc].nextRecIdPtr,AM_ss);
               if (searchpageNum != AM_LeftPageNum)
		 {
		  errVal = PF_UnfixPage(fileDesc,AM_LeftPageNum,FALSE);
                  AM_Check;
                 }		   
               AM_scanTable[scanDesc].lastpageNum  = pageNum;
               if (status == AM_FOUND)
                AM_scanTable[scanDesc].lastIndex  = index ;
               else
                AM_scanTable[scanDesc].lastIndex  = index - 1;
               break;
               }
  case GREATER_THAN_EQUAL :
                
               {
                AM_scanTable[scanDesc].nextpageNum = pageNum;
                AM_scanTable[scanDesc].nextIndex = index;
                AM_scanTable[scanDesc].actindex = index;
                bcopy(pageBuf + AM_sl + (index - 1)*recSize + attrLength,
                        &AM_scanTable[scanDesc].nextRecIdPtr,   AM_ss);
                break;
               }
  case NOT_EQUAL :
               {
               if(status == AM_FOUND)
                {
                AM_scanTable[scanDesc].nextpageNum = AM_LeftPageNum;
                AM_scanTable[scanDesc].nextIndex = 1;
                AM_scanTable[scanDesc].actindex = 1;
                if (searchpageNum != AM_LeftPageNum)
		  {
		  errVal = PF_GetThisPage(fileDesc,AM_LeftPageNum,&pageBuf);
                  AM_Check;
		  }
                bcopy(pageBuf + AM_sl + attrLength,
                              &AM_scanTable[scanDesc].nextRecIdPtr,   AM_ss);
                if (searchpageNum != AM_LeftPageNum)
		 { errVal = PF_UnfixPage(fileDesc,AM_LeftPageNum,FALSE);
                  AM_Check;
                 }
                }
               else 
                AM_scanTable[scanDesc].pageNum = AM_NULL_PAGE;
               break;
               }
  default : {
             AM_scanTable[scanDesc].status = FREE;
	     AM_Errno = AME_INVALID_OP_TO_SCAN;
	     return(AME_INVALID_OP_TO_SCAN);
             break;
            }
 }

errVal = PF_UnfixPage(fileDesc,searchpageNum,FALSE);
AM_Check;
return(scanDesc);
}

/* returns the record id of the next record that satisfies the conditions
specified for index scan associated with scanDesc */
AM_FindNextEntry(scanDesc)
int scanDesc;/* index scan descriptor */

{
int recId; /* recordId to be returned */
char *pageBuf;/* buffer for page */
int errVal;/* return value for functions */
AM_LEAFHEADER head,*header; /* local header */
int recSize;/* size of key,ptr pair for leaf */
int compareVal; /* value returned by compare routine */


/* check if scanDesc is valid */
if ((scanDesc < 0) || (scanDesc > MAXSCANS - 1))
  {
   AM_Errno = AME_INVALID_SCANDESC;
   return(AME_INVALID_SCANDESC);
  }

/* check if scan is over */
if (AM_scanTable[scanDesc].status == OVER)
      return(AME_EOF);

if (AM_scanTable[scanDesc].nextpageNum == AM_NULL_PAGE)
 {
  AM_scanTable[scanDesc].status = OVER;
  return(AME_EOF);
 }

header = &head;
errVal = PF_GetThisPage(AM_scanTable[scanDesc].fileDesc
                        ,AM_scanTable[scanDesc].nextpageNum,&pageBuf);
AM_Check;

bcopy(pageBuf,header,AM_sl);
recSize = header->attrLength + AM_ss;

errVal = PF_UnfixPage(AM_scanTable[scanDesc].fileDesc
           ,AM_scanTable[scanDesc].nextpageNum,FALSE);
AM_Check;

/* Get next non empty leaf page */
while(header->numKeys == 0)
  if(header->nextLeafPage == AM_NULL_PAGE)
   {
    AM_scanTable[scanDesc].status = OVER; 
    return(AME_EOF);
   }
  else
   {
    errVal = PF_GetThisPage(AM_scanTable[scanDesc].fileDesc,header->nextLeafPage,&pageBuf);
    AM_Check;
    errVal = PF_UnfixPage(AM_scanTable[scanDesc].fileDesc,header->nextLeafPage,FALSE);
    AM_Check;
    AM_scanTable[scanDesc].nextpageNum = header->nextLeafPage;
    AM_scanTable[scanDesc].nextIndex = 1;
    AM_scanTable[scanDesc].actindex = 1;
    bcopy(pageBuf,header,AM_sl);
    bcopy(pageBuf + AM_sl +  (AM_scanTable[scanDesc].nextIndex-1)*recSize
    + header->attrLength, &AM_scanTable[scanDesc].nextRecIdPtr,AM_ss);
    AM_scanTable[scanDesc].status = FIRST;
   }

/* if op is < or <= check if you are done - might have overshot while scanning
empty pages*/
if ((AM_scanTable[scanDesc].op == LESS_THAN) || 
  (AM_scanTable[scanDesc].op == LESS_THAN_EQUAL))
 if ((AM_scanTable[scanDesc].lastpageNum <= AM_scanTable[scanDesc].nextpageNum)
 && (AM_scanTable[scanDesc].lastIndex == 0))
 {
  AM_scanTable[scanDesc].status = OVER;
  return(AME_EOF);
 }

/* if op is not equal then check if we have to skip this value */
if (AM_scanTable[scanDesc].op == NOT_EQUAL)
  if ((AM_scanTable[scanDesc].pageNum == AM_scanTable[scanDesc].nextpageNum)
    && (AM_scanTable[scanDesc].index == AM_scanTable[scanDesc].actindex))
       
       /*skip this value */
       if ((AM_scanTable[scanDesc].nextIndex + 1) <= (header->numKeys))
         {
          AM_scanTable[scanDesc].nextIndex++;
          AM_scanTable[scanDesc].actindex++;
          bcopy(pageBuf + AM_sl +  (AM_scanTable[scanDesc].nextIndex-1)*recSize
          + header->attrLength, &AM_scanTable[scanDesc].nextRecIdPtr,AM_ss);
         }
       else
          if (header->nextLeafPage == AM_NULL_PAGE)
            return(AME_EOF); 
          else
           {
            AM_scanTable[scanDesc].nextpageNum = header->nextLeafPage;
            AM_scanTable[scanDesc].nextIndex =  1;
            AM_scanTable[scanDesc].actindex = 1;
            errVal =PF_GetThisPage(AM_scanTable[scanDesc].fileDesc,
                  header->nextLeafPage,&pageBuf);
            AM_Check;
            bcopy(pageBuf + AM_sl + header->attrLength,
               &AM_scanTable[scanDesc].nextRecIdPtr,AM_ss);
            bcopy(pageBuf,header,AM_sl);
            errVal = PF_UnfixPage(AM_scanTable[scanDesc].fileDesc
                ,header->nextLeafPage,FALSE);
            AM_Check;
           }
/* if not the first call to findnextentry , check if previous record has 
been deleted */
if (AM_scanTable[scanDesc].status != FIRST)
 {
  compareVal = AM_Compare(pageBuf + (AM_scanTable[scanDesc].nextIndex - 1)
                *recSize + AM_sl,AM_scanTable[scanDesc].attrType,
		header->attrLength,AM_scanTable[scanDesc].nextvalue);
  if (compareVal != 0)
   {
    /* prev record deleted */
    AM_scanTable[scanDesc].nextIndex--;
    bcopy(pageBuf + AM_sl + (AM_scanTable[scanDesc].nextIndex -1)*recSize + 
    header->attrLength, &AM_scanTable[scanDesc].nextRecIdPtr,AM_ss);
   }
 }
else 
  /* make the status busy - no more the first call */
  { AM_scanTable[scanDesc].status = BUSY;
    bcopy(pageBuf + AM_sl + (AM_scanTable[scanDesc].nextIndex - 1)*recSize,
    AM_scanTable[scanDesc].nextvalue, header->attrLength);
  }

/* copy the recId to be returned */
bcopy(pageBuf + AM_scanTable[scanDesc].nextRecIdPtr,&recId,AM_si);

/* copy the place for next recId */
bcopy(pageBuf + AM_scanTable[scanDesc].nextRecIdPtr + AM_si,
          &AM_scanTable[scanDesc].nextRecIdPtr,AM_ss);


/* check if this keys list is over */
if (AM_scanTable[scanDesc].nextRecIdPtr == (short)0)
   if ((AM_scanTable[scanDesc].nextIndex + 1) <= (header->numKeys))
    {
     AM_scanTable[scanDesc].nextIndex++;
     AM_scanTable[scanDesc].actindex++;
     bcopy(pageBuf + AM_sl + (AM_scanTable[scanDesc].nextIndex - 1)*recSize + 
     header->attrLength , &AM_scanTable[scanDesc].nextRecIdPtr, AM_ss);
     bcopy(pageBuf + AM_sl + (AM_scanTable[scanDesc].nextIndex -1 )*recSize,
     AM_scanTable[scanDesc].nextvalue,header->attrLength); 
    }
   else
    /* got to go to next page */
    if (header->nextLeafPage == AM_NULL_PAGE)
      AM_scanTable[scanDesc].status = OVER;
    else
     {
      AM_scanTable[scanDesc].nextpageNum = header->nextLeafPage;
      AM_scanTable[scanDesc].nextIndex =  1;
      AM_scanTable[scanDesc].actindex = 1;
      errVal =PF_GetThisPage(AM_scanTable[scanDesc].fileDesc,
        header->nextLeafPage,&pageBuf);
      AM_Check;
      bcopy(pageBuf + AM_sl + header->attrLength,
         &AM_scanTable[scanDesc].nextRecIdPtr,AM_ss);
      errVal = PF_UnfixPage(AM_scanTable[scanDesc].fileDesc
                ,header->nextLeafPage,FALSE);
      AM_Check;
      bcopy(pageBuf + AM_sl + (AM_scanTable[scanDesc].nextIndex -1 )*recSize,
      AM_scanTable[scanDesc].nextvalue,header->attrLength); 
      bcopy(pageBuf,header,AM_sl);
     }

/* If op is equal then see if you are done */
if (AM_scanTable[scanDesc].op == EQUAL)
  if ((AM_scanTable[scanDesc].pageNum != AM_scanTable[scanDesc].nextpageNum)
    || (AM_scanTable[scanDesc].index != AM_scanTable[scanDesc].actindex))
     AM_scanTable[scanDesc].status = OVER;

/* see if you are at the last record if op is < or <= */
if ((AM_scanTable[scanDesc].op == LESS_THAN) || 
  (AM_scanTable[scanDesc].op == LESS_THAN_EQUAL))
   if ((AM_scanTable[scanDesc].lastpageNum == AM_scanTable[scanDesc].nextpageNum)
    && (AM_scanTable[scanDesc].lastIndex == AM_scanTable[scanDesc].actindex))
       AM_scanTable[scanDesc].status = LAST;
  else  if ((AM_scanTable[scanDesc].lastpageNum == AM_scanTable[scanDesc].nextpageNum)
    && (AM_scanTable[scanDesc].lastIndex  <  AM_scanTable[scanDesc].actindex))
        AM_scanTable[scanDesc].status = OVER;
    else
      if (AM_scanTable[scanDesc].status == LAST)
        AM_scanTable[scanDesc].status = OVER;
        

return(recId);
}


/* terminates an index scan */
AM_CloseIndexScan(scanDesc)
int scanDesc;/* scan Descriptor*/

{
if ((scanDesc < 0) || (scanDesc > MAXSCANS - 1))
  {
   AM_Errno = AME_INVALID_SCANDESC;
   return(AME_INVALID_SCANDESC);
  }
AM_scanTable[scanDesc].status = FREE;
return(AME_OK);
}


GetLeftPageNum(fileDesc)
int fileDesc;

{
char *pageBuf;
int pageNum;
int errVal;

errVal = PF_GetFirstPage(fileDesc,&pageNum,&pageBuf);
AM_Check;
if (*pageBuf == 'l')
  AM_LeftPageNum = pageNum;
else
  AM_LeftPageNum = 2;
errVal = PF_UnfixPage(fileDesc,pageNum,FALSE);
AM_Check;
return(AM_LeftPageNum);

}
