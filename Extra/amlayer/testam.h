/* testam.h: include for testam */

/* You don't have to do anything if your AM routines use
integers for recid's. Otherwise, you should typedef 
RecIdType to the appropriate type, and also
redefine RecIdToInt() and IntToRecId() */

typedef int RecIdType;		/* type for recid */

#define RecIdToInt(recid)	(recid)	/* converts record id to int */
#define IntToRecId(intval)	(intval) /* converts int to record id */

/*
 *  Attribute types
 */
#define CHAR_TYPE  'c'
#define INT_TYPE   'i'
#define FLOAT_TYPE 'f'
#define INT_SIZE   sizeof(int)
#define FLOAT_SIZE sizeof(float)

/*
 *  Compare operators
 */
#define EQ_OP	1
#define LT_OP	2
#define GT_OP	3
#define LE_OP   4
#define GE_OP	5
#define NE_OP	6

#define RELNAME "testrel" /* name of relation */

/* record of our test relation */
#define NAMELENGTH 11 	/* length of recname */
#define RECNAME_INDEXNO 0 /* index number of recname field */
#define RECVAL_INDEXNO 1 /* index number of recval field */
typedef struct smallrec {
	char recname[NAMELENGTH];
	int recval;
} smallrec;

/* successor function, assuming ch is a character */
#define succ(ch) ((char)((int)(ch)+1))

extern RecIdType xAM_FindNextEntry();
