//

#ifdef  __circle__
#include <SDCard/emmc.h>
#include <fatfs/ff.h>

#define DRIVE		"SD:"
#define PATH_CARTIRDGE "SD:/CART"
#define PATH_DISK "SD:/DISK"
#define PATH_TAPE "SD:/TAPE"
#define PATH_CONFIGS "SD:/AMSTRAD"
#define PATH_RES "SD:/RES"

#define PATH_QUICK_SNA "SD:/quick.sna"
#define INTER_FILE "/"

#else
# include <windows.h>

#define DRIVE		"."
#define PATH_CARTIRDGE ".\\CART"
#define PATH_DISK ".\\DISK"
#define PATH_TAPE ".\\TAPE"
#define PATH_CONFIGS ".\\AMSTRAD"
#define PATH_RES ".\\RES"

#define PATH_QUICK_SNA ".\\quick.sna"
#define INTER_FILE "\\"

#define FSIZE_t int 
#define FRESULT int
#define DIR HANDLE 
#define FR_OK 0
#define FF_LFN_BUF		255
#define FF_SFN_BUF		12

typedef struct {
   FSIZE_t	fsize;			/* File size */
   WORD	fdate;			/* Modified date */
   WORD	ftime;			/* Modified time */
   BYTE	fattrib;		/* File attribute */
   //char fname[12 + 1];	/* File name */
   char	altname[FF_SFN_BUF + 1];/* Alternative file name */
   char fname[FF_LFN_BUF + 1];	/* Primary file name */
   WIN32_FIND_DATAA data;
   HANDLE handle;
} FILINFO;

/* File attribute bits for directory entry (FILINFO.fattrib) */
#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */

void UpdateFILINFO(FILINFO* f);

FRESULT f_findfirst(
   DIR* dp,				/* Pointer to the blank directory object */
   FILINFO* fno,			/* Pointer to the file information structure */
   const char* path,		/* Pointer to the directory to open */
   const char* pattern	/* Pointer to the matching pattern */
);

FRESULT f_findnext(DIR* dp, FILINFO* FileInfo);

#endif

