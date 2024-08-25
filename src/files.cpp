//
#include "files.h"

#ifdef  __circle__
#else
void UpdateFILINFO(FILINFO* f)
{
   f->fattrib = f->data.dwFileAttributes;
   memset(f->fname, 0, sizeof(f->fname));
   strncpy(f->fname, f->data.cFileName, sizeof(f->fname));
}

FRESULT f_findfirst(
   DIR* dp,				/* Pointer to the blank directory object */
   FILINFO* fno,			/* Pointer to the file information structure */
   const char* path,		/* Pointer to the directory to open */
   const char* pattern	/* Pointer to the matching pattern */
)
{
   *dp = FindFirstFileA(path, &fno->data);		/* Open the target directory */
   fno->handle = *dp;
   UpdateFILINFO(fno);
   return (*dp == INVALID_HANDLE_VALUE) ? -1 : 0;
}

FRESULT f_findnext(DIR* dp, FILINFO* FileInfo)
{
   FRESULT res = FindNextFileA(*dp, &FileInfo->data) ? -1 : 0;
   UpdateFILINFO(FileInfo);
   return res == 0 ? -1 : 0;;

}
#endif