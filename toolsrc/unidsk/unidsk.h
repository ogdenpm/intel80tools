#pragma once
#include <stdbool.h>
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned __int32 dword;

#define	TSIZE	32768				// Max size of track

#define MAXCYLINDER	80
#define	MAXSECTOR	52
#define MAXHEAD		2

// disk types
enum { UNKNOWN, ISIS_SD, ISIS_DD, ISIS_III, ISIS_IV };
#define ISIS_SD_SIZE   256256
#define ISIS_DD_SIZE   512512

// Status index values
#define	ST_TOTAL	0				// Total sector count
#define	ST_COMP		1				// Number Compressed
#define	ST_DAM		2				// Number Deleted
#define	ST_BAD		3				// Number Bad
#define	ST_UNAVAIL	4				// Number unavailable



#define MAXDIR      200
#define MAXCOMMENT  2048
#define MAXLINE 512




typedef struct {
    char name[11];
    byte attrib;
    int len;
    int checksum;
}  isisDir_t;

#pragma pack(push, 1)
typedef struct {
    byte name[9];
    byte version[2];
    byte leftOver[38];
    byte crlf[2];
    byte fmtTable[77];
} label_t;
#pragma pack(pop)

extern int osChecksum;

void mkRecipe(char *name, isisDir_t  *isisDir, char *comment, int diskType);