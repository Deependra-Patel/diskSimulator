extern "C" int AM_OpenIndexScan(int fileDesc, char attrType, int attrLength, int op, char *value);

extern "C" int AM_FindNextEntry(int scanDesc);

extern "C" int AM_CloseIndexScan(int scanDesc);

extern  "C" int PFerrno;		/* error number of last error */
extern  "C" void PF_Init();
extern  "C" void PF_PrintError();

extern "C" int PF_CreateFile(char *fname);
extern "C" int PF_DestroyFile(char *fname);
extern "C" int PF_OpenFile(char *fname);
extern "C" int PF_CloseFile(int fd);
extern "C" int PF_GetFirstPage(int fd, int *pagenum, char **pagebuf);
extern "C" int PF_GetThisPage(int fd, int pagenum,char **pagebuf);
extern "C" int PF_AllocPage(int fd, int *pagenum,char **pagebuf);
extern "C" int PF_DisposePage(int fd, int pagenum);
extern "C" int PF_UnfixPage(int fd, int pagenum, int dirty);

extern "C" int AM_CreateIndex(char *fileName, int indexNo,char attrType, int attrLength);

extern "C" int AM_DestroyIndex(char *fileName, int indexNo);

extern "C" int AM_DeleteEntry(int fileDesc, char attrType, int attrLength, char *value, int recId);

extern "C" int AM_InsertEntry(int fileDesc, char attrType, int attrLength, char *value, int recId);

extern "C" void AM_PrintError(char *s);
