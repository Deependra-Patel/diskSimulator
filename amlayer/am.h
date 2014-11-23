typedef struct am_leafheader
	{
		char pageType;
		int nextLeafPage;
		short recIdPtr;
		short keyPtr;
		short freeListPtr;
		short numinfreeList;
		short attrLength;
		short numKeys;
		short maxKeys;
	}  AM_LEAFHEADER; /* Header for a leaf page */

typedef struct am_intheader 
	{
		char pageType;
		short numKeys;
		short maxKeys;
		short attrLength;
	}	AM_INTHEADER ; /* Header for an internal node */

extern int AM_RootPageNum; /* The page number of the root */
extern int AM_LeftPageNum; /* The page Number of the leftmost leaf */
extern int AM_Errno; /* last error in AM layer */
extern char *calloc();
extern char *malloc();

# define AM_Check if (errVal != PFE_OK) {AM_Errno = AME_PF; return(AME_PF) ;}
# define AM_si sizeof(int)
# define AM_ss sizeof(short)
# define AM_sl sizeof(AM_LEAFHEADER)
# define AM_sint sizeof(AM_INTHEADER)
# define AM_sc sizeof(char)
# define AM_sf sizeof(float)
# define AM_NOT_FOUND 0 /* Key is not in tree */
# define AM_FOUND 1 /* Key is in tree */
# define AM_NULL 0 /* Null pointer for lists in a page */
# define AM_MAX_FNAME_LENGTH 80
# define AM_NULL_PAGE -1 
# define FREE 0 /* Free is chosen to be zero because C initialises all 
	   variablesto zero and we require that our scan table be initialised */
# define FIRST 1 
# define BUSY 2
# define LAST 3
# define OVER 4
# define ALL 0
# define EQUAL 1
# define LESS_THAN 2
# define GREATER_THAN 3
# define LESS_THAN_EQUAL 4
# define GREATER_THAN_EQUAL 5
# define NOT_EQUAL 6
# define MAXSCANS 20
# define AM_MAXATTRLENGTH 256


# define AME_OK 0
# define AME_INVALIDATTRLENGTH -1
# define AME_NOTFOUND -2
# define AME_PF -3
# define AME_INTERROR -4
# define AME_INVALID_SCANDESC -5
# define AME_INVALID_OP_TO_SCAN -6
# define AME_EOF -7
# define AME_SCAN_TAB_FULL -8
# define AME_INVALIDATTRTYPE -9
# define AME_FD -10
# define AME_INVALIDVALUE -11
