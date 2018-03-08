// $Id: io.cpp,v 1.1 2003/10/04 21:08:48 Mark Ogden Exp $
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef __GNUG__
#include <unistd.h>
#define _tell	tell
#define _lseek	lseek
#include <errno.h>
#define _open open
#define _S_IREAD S_IREAD
#define _S_IWRITE S_IWRITE
#endif

#ifdef __BORLAND__
#define _tell tell
#define _open open
#endif
#include "plm.hpp"
int gargc;
char **gargv;

static char cmdLine[256];

enum {STDIN = 1, STDOUT = 0};   // flipped from dos/unix standard



static int fixFd(int fd) {
    return fd > STDIN ? fd : 1 - fd;
}

void Close(FILE *conn, word * status_p) {
    *status_p = fclose(conn) < 0 ? errno : 0;
   	
}

void Load(char *path_p, word load_offset, word sw, word * entry_p, word * status_p) {
}


char *mapFile(char *path_p) {
    static char path[128];
    char *s = path;
    char drive;
    if (path_p[0] == ':' && path_p[1] == 'F' && path_p[3] == ':') {
        drive = path_p[2];
		path_p += 4;
		if (drive == '1') {	// map drive 1 to current directory
			s = path;
			*s = 0;
		} else {
			strcpy(path, "./f");
			s = strchr(path, 0);
			*s++ = drive;
			*s++ = '/';
		}
	} else {
		s = path;
		*s = 0;
	}
    
    while (*path_p != ' ')
        *s++ = *path_p++;
    *s = 0;
    return path;
}


void Open(FILE **conn_p, char *path_p, word access, word echo, word * status_p) {
    char *mode;
    FILE *fp; 

	errno = 0;
    if (strncmp(path_p, ":CI:", 4) == 0) {  // console input
        fp = stdin;
		_setmode(_fileno(fp), _O_BINARY);
    } else if (strncmp(path_p, ":CO:", 4) == 0) {
        fp = stdout;
		_setmode(_fileno(fp), _O_BINARY);
    } else {
        switch (access) {
        case 1: mode = "rb"; break;
        case 2: mode = "wb"; break;
        case 3: mode = "w+b"; break;
        }
        fp = fopen(mapFile(path_p), mode);
    }
    *conn_p = fp;
    *status_p = (fp == 0) ? errno : 0;
}

void Read(FILE *conn, void *buf_p, word count, word * actual_p, word * status_p)
{
    int actual;
    if (conn == stdin && cmdLine[0]) {
        int len = strlen(cmdLine);
        actual = len <= count ? len : count;
        memcpy(buf_p, cmdLine, actual);
        if (len <= count)
            cmdLine[0] = 0;
        else
            memmove(cmdLine, cmdLine + actual, len - actual);
    } else
        actual = fread(buf_p, 1, count, conn);
    *actual_p = actual;
    *status_p = (actual < 0) ? errno: 0;
}

void Rescan(word conn, word *status_p) {
    // rescan is only used for command line
    int i = gargc - 1;
    char **p = gargv + 1;
    char *s, *t;

    strcpy(cmdLine, ":F0:");
	s = gargv[0];
	if (t = strrchr(s, '\\'))
		s = t + 1;
	if (t = strrchr(s, '/'))
		s = t + 1;
	strcat(cmdLine, s);
	if (t = strrchr(cmdLine, '.'))
		*t = 0;
    while (i-- > 0) {
        strcat(cmdLine, " ");
        strcat(cmdLine, *p++);
    }
    strcat(cmdLine, "\r\n");
    *status_p = 0;
}



void Write(FILE *conn, void *buff_p, word count, word * status_p)
{
    *status_p = fwrite(buff_p, 1, count, conn) < 0 ? errno : 0;
}

void Delete(char *path, word *status_p)
{
    *status_p = _unlink(mapFile(path)) < 0 ? errno : 0;
}


#ifdef _DEBUG
void copyFile(char *src, char* dst) // handles isis src file name
{
    char buffer[2048];
    size_t actual;
    word status;
    FILE *srcfp, *dstfp;

    Open(&srcfp, src, 1, 0, &status);
    dstfp = fopen(dst, "wb");
    if (status != 0 || dstfp == NULL) {
        fprintf(stderr, "copyFile(%s, %s) fatal error\n", src, dst);
        exit(1);
    }

    while ((actual = fread(buffer, 1, sizeof(buffer), srcfp)) > 0)
        fwrite(buffer, 1, actual, dstfp);
    fclose(srcfp);
    fclose(dstfp);
}
#endif