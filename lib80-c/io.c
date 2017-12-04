// vim:ts=4:shiftwidth=4:expandtab:
#include <ctype.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lib.h"

// access modes are 1-3
#define NOTOPEN	0

// device types stored << 2 in aft type
#define CHARDEV	1
#define BBDEV	2
#define FILEDEV	3



#define CO_DEV	0
#define CI_DEV	1

#define AFTSIZE	18
static struct {
	int	fd;
	int type;
} aft[AFTSIZE] = { {1, WRITE_MODE | (CHARDEV << 2)}, {0, READ_MODE | (CHARDEV << 2)} }; // :CO:, :CI:

#define MAXLL	122
static char _commandLine[MAXLL + 1];
static char *_commandLinePtr;
pointer MEMORY;
#define AVAILMEM	0x9000


/* make sure ISIS name is clean - maps device to upper case, adding :F0: if none present
   converts rest of name to lower case
   returnd standard error codes as appropriate
*/
static word ParseIsisName(char *cleanIsisName, const char *isisPath)
{
	strcpy(cleanIsisName, ":F0:");	// default to F0 drive

	if (isisPath[0] == ':') {	// check for :Fn: or :XX: where X is a character
		if (strlen(isisPath) < 4 || isisPath[3] != ':')
			return ERROR_BADDEVICE;
		if (toupper(isisPath[1]) == 'F' && isdigit(isisPath[2]))
			cleanIsisName[2] = isisPath[2];
		else if (isalpha(isisPath[1]) && isalpha(isisPath[2])) {
			cleanIsisName[1] = toupper(isisPath[1]);	
			cleanIsisName[2] = toupper(isisPath[2]);
		}
		else
			return ERROR_BADFILENAME;
		
		isisPath += 4;
	} else if (!isalnum(isisPath[0]))
		return ERROR_NOFILENAME;

	if (!isdigit(cleanIsisName[2]))
		if (strcmp(cleanIsisName, ":CO:") == 0 || strcmp(cleanIsisName, ":CI:") == 0
		  || strcmp(cleanIsisName, ":BB:") == 0)
			return isalnum(*isisPath) ? ERROR_BADFILENAME :  ERROR_SUCCESS;
		else
			return ERROR_BADDEVICE;

		int i;
		char *s;

		s = &cleanIsisName[4];
		for (i = 0; i < 6 && isalnum(*isisPath); i++)
			*s++ = tolower(*isisPath++);
		if (i == 0 || isalnum(*isisPath))
			return ERROR_BADFILENAME;
		if (*isisPath == '.') {
			*s++ = *isisPath++;
			for (i = 0; i < 3 && isalnum(*isisPath); i++)
				*s++ = tolower(*isisPath++);
			if (i == 0 || isalnum(*isisPath))
				return ERROR_BADEXT;
		}
		*s = 0;
		return ERROR_SUCCESS;
}




static word MapFile(char *realFile, const char *isisPath)
{
	char isisName[15];
	const char *src;
	char buf[8];
	word status;

	*realFile = 0;
	if ((status = ParseIsisName(isisName, isisPath)) != ERROR_SUCCESS) // get canocial name
		return status;
	if (!isdigit(isisName[2])) {		// all devices except Fn are two alpha chars
		strcpy(realFile, isisName);
		return ERROR_SUCCESS;
	}

	sprintf(buf, "ISIS_F%c", isisName[2]);
	if (src = getenv(buf)) {
		strcpy(realFile, src);
		/* Append a path separator if there isn't one */
		if (*realFile && strchr(realFile, 0)[-1] != '/' && strchr(realFile, 0)[-1] != '\\')
			strcat(realFile, "/");
	}
	else if (isisName[2] == '0')	// always allow default F0 of current directory
		*realFile = 0;
	else {
		fprintf(stderr, "No mapping for ISIS "
			"block device :F%c:\n", isisName[2]);
		return ERROR_BADDEVICE;
	}

	strcat(realFile, isisName + 4);	// append the file name
	return ERROR_SUCCESS;
}



int main(int argc, char **argv)
{
	int i;
	size_t len;

	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);
	aft[CO_DEV].fd = _fileno(stdout);
	aft[CI_DEV].fd = _fileno(stdin);

	strcpy(_commandLine, "lib80");
	len = strlen(_commandLine);

	for (i = 1; i < argc && len + strlen(argv[i]) + 1 < MAXLL - 2; i++) {	// add args if room
			len += strlen(argv[i]) + 1;
			strcat(_commandLine, " ");
			strcat(_commandLine, argv[i]);
		}
	if (i < argc)
			fprintf(stderr, "command line truncated\n");

	_commandLinePtr = strcat(_commandLine, "\r\n");

	MEMORY = (pointer)malloc(AVAILMEM);
	Start();
}

pointer MemCk()
{
	return MEMORY + AVAILMEM - 1;	// address of last isis user memory
}

void Close(word conn, wpointer statusP)
{
	if (conn >= AFTSIZE) { 
		*statusP = ERROR_BADPARAM;
		return;
	}
	if (!(aft[conn].type & 3)) {	// no access mode set
		*statusP = ERROR_NOTOPEN;
		return;
	}

	*statusP = ERROR_SUCCESS;

	switch (aft[conn].type >> 2) {
	case CHARDEV: return;					// ignore reqests to close CO & CI
	case BBDEV: aft[conn].type = 0; return;
	case FILEDEV:
		if (_close(aft[conn].fd) < 0)
			*statusP = ERROR_NOTOPEN;
		aft[conn].type = 0;
	}
}


void Delete(pointer pathP, wpointer statusP)
{
	char path[_MAX_PATH];

	if ((*statusP = MapFile(path, pathP)) == ERROR_SUCCESS) {
		if (*path == ':')
			*statusP = ERROR_ISDEVICE;
		else if (_unlink(path) < 0) {
			if (errno == EACCES)
				*statusP = ERROR_PERMISSIONS;
			else if (errno == ENOENT)
				*statusP = ERROR_FILENOTFOUND;
			else
				*statusP = ERROR_NOTREADY;
		}
	}
}

static const char *error_strings[] =
{
	"Success",
	"No memory available for buffer",
	"File is not open",
	"No more file handles available",
	"Invalid pathname",
	"Bad device name in filename",
	"Trying to write a file opened in read mode",
	"Disk is full",
	"Trying to read a file opened in write mode",
	"Cannot create file",
	"Cannot rename across devices",
	"Destination file exists",
	"File is already open",
	"File not found",
	"Permissions error",
	"Attempting to overwrite operating system",
	"Invalid executable image",
	"Attempt to rename or delete a device",
	"Invalid function number",
	"Can't seek on a device",
	"Can't seek to before start of file",
	"File is not open in line mode",
	"Bad access mode",
	"No filename specified",
	"Disk error",
	"Invalid echo file specified",
	"Bad attribute",
	"Bad seek mode",
	"Null file extension",
	"End of file on console input",
	"Drive not ready",
	"Can't seek in a write-only file",
	"Can't delete an open file",
	"Bad system call parameter",
	"Bad switch argument in load",
	"Cannot seek past EOF in file open for read",
};

#define MAXERROR (sizeof(error_strings) / sizeof(error_strings[0]))


void Error(word ErrorNum)
{
	if (ErrorNum >= 0 && ErrorNum < MAXERROR)
		fprintf(stderr, "%s.\n", error_strings[ErrorNum]);
	else
		fprintf(stderr, "Unknown error %d.\n", ErrorNum);
}

void Exit()
{
	_exit(1);
}

void Load(pointer pathP, word LoadOffset, word swt, word entryP, wpointer statusP)
{
	fprintf(stderr, "load not implmented\n");
	exit(2);
}




void Open(wpointer connP, pointer pathP, word access, word echo, wpointer statusP)
{
	int mode, conn;
	char path[_MAX_PATH];
	int handle;

	*connP = -1;		// unused conn
	if ((*statusP = MapFile(path, pathP)) != ERROR_SUCCESS)
		return;

	switch (access) {
	case READ_MODE: mode = _O_RDONLY | O_BINARY; break;
	case WRITE_MODE: mode = _O_WRONLY | _O_CREAT | _O_TRUNC | O_BINARY; break;
	case UPDATE_MODE: mode = _O_RDWR | _O_CREAT | _O_TRUNC | O_BINARY; break;
	default: fprintf(stderr, "bad access mode %d for %s\n", access, path);
		*statusP = ERROR_BADACCESS;
		return;
	}
	if (strcmp(path, ":CI:") == 0) {
		if (access != READ_MODE)
			*statusP = ERROR_BADACCESS;
		else
			aft[CI_DEV].type = (CHARDEV << 2) | READ_MODE;
		*connP = CI_DEV;
		return;
	}

	if (strcmp(path, ":CO:") == 0) {
		if (access != WRITE_MODE)
			*statusP = ERROR_BADACCESS;
		else
			aft[CO_DEV].type = (CHARDEV << 2) | WRITE_MODE;
		*connP = CO_DEV;
		return;
	}

	// allocate a handle
	for (handle = 2; handle < AFTSIZE; handle++)
		if (aft[handle ].type == 0)
			break;

	// if BB check not already open
	if (strcmp(path, ":BB:") == 0)
		for (int i = 0; i < AFTSIZE; i++)
			if ((aft[i].type >> 2) == BBDEV) {
				*statusP = ERROR_ALREADYOPEN;
				*connP = i;
				return;
			}

	if (handle >= AFTSIZE) {
		*statusP = ERROR_NOHANDLES;
		return;
	}

	if (strcmp(path, ":BB:") == 0) {
		*connP = handle;
		aft[handle].type = (BBDEV << 2) + access;	// fd not used as BB handled internally
		return;
	}


	conn = _open(path, mode, _S_IREAD | _S_IWRITE);

	if (conn < 0)
		switch (errno) {
		case EACCES: *statusP = ERROR_PERMISSIONS; break;
		case EEXIST: *statusP = ERROR_NOWRITE; break;
		case EMFILE: *statusP = ERROR_NOHANDLES; break;
		case ENOENT: *statusP = ERROR_FILENOTFOUND; break;
		default: fprintf(stderr, "unknown error %d for open %s", errno, path);
			*statusP = ERROR_NOTREADY;
		}
	else {
		aft[handle].fd = conn;
		aft[handle].type = (FILEDEV << 2) + access;
		*connP = handle;
	}
}

char *ReadLine(char *buf)
{
	int c;
	int i;
	bool trunc = false;

	i = 0;
	while ((c = getchar()) != EOF && c != '\n' && c != '\r') {
		if (i < MAXLL - 2)
			buf[i++] = c;
		else
			trunc = true;
	}
	if (c == '\r' && (c = getchar()) != '\n')		// pick up \r\n else singleton \r
		ungetc(c, stdin);
	strcpy(buf + i, "\r\n");
	if (trunc)
		fprintf(stderr, ":CI: line truncated\n");
	return buf;
}


void Read(word conn, pointer buffP, word count, wpointer actualP, wpointer statusP)
{
	int actual;

	if (conn >= AFTSIZE) {
		*statusP = ERROR_BADPARAM;
		return;
	}

	switch(aft[conn].type & 3) {
	case 0: *statusP = ERROR_NOTOPEN; return;
	case WRITE_MODE: *statusP = ERROR_NOREAD; return;
	}

	*statusP = 0;
	switch (aft[conn].type >> 2) {
	case CHARDEV:
		if (!*_commandLinePtr)
			_commandLinePtr = ReadLine(_commandLine);

		for (actual = 0; actual < count && *_commandLinePtr; actual++)
			*buffP++ = *_commandLinePtr++;
		*actualP = actual;
		return;

	case BBDEV: *actualP = 0; return;
	case FILEDEV:
		if ((actual = _read(aft[conn].fd, buffP, count)) >= 0) {
			*actualP = actual;
			*statusP = 0;
		}
		else {
			*actualP = 0;
			if (errno == EBADF)
				*statusP = ERROR_NOTOPEN;
			else if (errno == EINVAL)
				*statusP = ERROR_BADPARAM;
			else
				*statusP = ERROR_NOTREADY;
		}
	}
}
void Rescan(word conn, wpointer statusP)
{
	if (conn == CI_DEV) {
		_commandLinePtr = _commandLine;
		*statusP = ERROR_SUCCESS;
	}
	else
		*statusP = ERROR_NOTLINEMODE;
}



void Seek(word conn, word mode, wpointer blockP, wpointer byteP, wpointer statusP)
{
	long offset;
	int origin;

	if (conn >= AFTSIZE) {
		*statusP = ERROR_BADPARAM;
		return;
	}


	switch (aft[conn].type & 3) {
	case 0: *statusP = ERROR_NOTOPEN; return;
	case WRITE_MODE: *statusP = ERROR_SEEKWRITE; return;
	}
	if ((aft[conn].type >> 2) != FILEDEV) {
		*statusP = ERROR_CANTSEEKDEV;
		return;
	}

	if (mode != SEEKTELL)
		offset = *blockP * 128 + *byteP;

	switch (mode) {
	case SEEKTELL:
		offset = _tell(aft[conn].fd);
		*blockP = (word) (offset / 128);
		*byteP = offset % 128;
		*statusP = 0;
		return;
	case SEEKABS:	origin = SEEK_SET; break;
	case SEEKBACK:	offset = -offset;
	case SEEKFWD:	origin = SEEK_CUR; break;
	case SEEKEND:	origin = SEEK_END; break;
	default: fprintf(stderr, "Unsupported seek mode %d\n", mode);
		*statusP = ERROR_BADMODE;
		return;
	}
	if (_lseek(aft[conn].fd, offset, origin) >= 0)
		*statusP = 0;
	else
		*statusP = ERROR_BADPARAM;
}
void Write(word conn, pointer buffP, word count, wpointer statusP)
{
	if (conn >= AFTSIZE) {
		*statusP = ERROR_BADPARAM;
		return;
	}

	switch (aft[conn].type & 3) {
	case 0: *statusP = ERROR_NOTOPEN; return;
	case READ_MODE: *statusP = ERROR_NOREAD; return;
	}


	if ((aft[conn].type >> 2) == BBDEV)
		*statusP = ERROR_SUCCESS;
	if (_write(aft[conn].fd, buffP, count) == count)
		*statusP = ERROR_SUCCESS;
	else
		switch (errno) {
		case ENOSPC: *statusP = ERROR_DISKFULL; break;
		case EINVAL: *statusP = ERROR_BADPARAM;
		default: *statusP = ERROR_NOTREADY;
		}
}

void Rename(pointer oldP, pointer newP, wpointer statusP)
{
	char oldFile[_MAX_PATH], newFile[_MAX_PATH];

	if ((*statusP = MapFile(oldFile, oldP)) != ERROR_SUCCESS || (*statusP = MapFile(newFile, newP)) != ERROR_SUCCESS)
		return;
	if (*oldFile == ':' || *newFile == ':')
		*statusP = ERROR_ISDEVICE;
	else if (rename(oldFile, newFile) == 0)
		*statusP = 0;
	else
		switch (errno) {
		case EACCES: *statusP = ERROR_EXISTS; break;
		case ENOENT: *statusP = ERROR_FILENOTFOUND; break;
		case EINVAL: *statusP = ERROR_BADFILENAME; break;
		default: *statusP = ERROR_BADPARAM; break;
		}
}
