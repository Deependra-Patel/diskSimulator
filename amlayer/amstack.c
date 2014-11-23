# include <stdio.h>
# include "am.h"
# include "pf.h"

# define AM_MAXSTACK 50

struct
    {
     int pageNumber;
     int offset;
    } AM_Stack[AM_MAXSTACK];

int AM_topofStackPtr = -1;

AM_PushStack(pageNum,offset)
int pageNum;
int offset;

{
AM_topofStackPtr++;
AM_Stack[AM_topofStackPtr].pageNumber  = pageNum;
AM_Stack[AM_topofStackPtr].offset  = offset;
}

AM_PopStack()

{
AM_topofStackPtr--;
}

AM_topofStack(pageNum,offset)
int *pageNum;
int *offset;
{
*pageNum = AM_Stack[AM_topofStackPtr].pageNumber ;
*offset = AM_Stack[AM_topofStackPtr].offset ;
}

AM_EmptyStack()

{
AM_topofStackPtr = -1;
}

