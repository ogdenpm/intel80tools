// vim:ts=4:shiftwidth=4:expandtab:
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Generated/version.h"

#ifdef _WIN32
#include <io.h>
#define close   _close
#define open    _open
#define read    _read
#define write   _write
#define unlink  _unlink
#define lseek   _lseek
#else
#include <unistd.h>
#include <errno.h>
#define _MAX_PATH 4096
#define O_BINARY    0
#endif

#include "loc.h"

#define RANDOM_ACCESS	4	// additional access mode

typedef struct {
	byte	deviceId; // isis device Id
	byte	modes;	  // supported modes READ_MODE, WRITE_MODE, UPDATE_MODE and RANDOM_ACCESS
	byte	name[_MAX_PATH];
} osfile_t;

#define STDIN	0
#define STDOUT	1

#define CO_DEV	0
#define CI_DEV	1
#define BB_DEV	2		// always provide BB it is harmless and simplifies things

#define AFTSIZE	20		// more files than usually available to ISIS

static struct {
	int	fd;
	int mode;
} aft[AFTSIZE] = { { 1 , WRITE_MODE }, {0, READ_MODE}, {0, UPDATE_MODE} };	// CO, CI, BB

#define MAXLL	122
static char _commandLine[MAXLL + 1];
static char *_commandLinePtr;
pointer MEMORY;
#define AVAILMEM	0x9000

struct {
	char *dev;
	unsigned char devtype;
} deviceMap[] = {
	{ "F0", 3 },{ "F1", 3 },{ "F2", 3 },{ "F3", 3 },	// 0 - 3
	{ "F4", 3 },{ "F5", 3 },{ "TI", 0 },{ "TO", 1 },	// 4 - 7
	{ "VI", 0 },{ "VO", 1 },{ "I1", 0 },{ "O1", 1 },	// 8 - 11
	{ "TR", 0 },{ "HR", 0 },{ "R1", 0 },{ "R2", 0 },	// 12 - 15
	{ "TP", 1 },{ "HP", 1 },{ "P1", 1 },{ "P2", 1 },	// 16 - 19
	{ "LP", 1 },{ "L1", 1 },{ "BB", 2 },{ "CI", 0 },	// 20 - 23
	{ "CO", 1 },{ "F6", 3 },{ "F7", 3 },{ "F8", 3 },	// 24 - 27
	{ "F9", 3 } };										// 28

const int nDevices = (sizeof(deviceMap) / sizeof(deviceMap[0]));

void showVersion(char *description) {
	fputs(description, stdout);
	fputs(" - " GIT_VERSION, stdout);
#ifdef _DEBUG
	fputs(" {debug}", stdout);
#endif
	fputs(" (C)" GIT_YEAR "\n", stdout);
	fputs(sizeof(void *) == 4 ? "32bit target" : "64bit target", stdout);
	fputs(" Git: " GIT_SHA1 " [" GIT_CTIME " GMT]", stdout);
#if GIT_BUILDTYPE == 2
	fputs(" +uncommitted files", stdout);
#elif GIT_BUILDTYPE == 3
	fputs(" +untracked files", stdout);
#endif
	fputc('\n', stdout);
	exit(0);
}

/* preps the deviceId and filename of an spath info record
   returns standard error codes as appropriate
*/
static word ParseIsisName(spath_t *pInfo, const char *isisPath)
{
	int i;
	char dev[3];

	strcpy(dev,"F0");	// default device is F0

	memset(pInfo, 0, sizeof(spath_t));
	pInfo->deviceId = 0xff;		// prefill incase of error

	if (isisPath[0] == ':') {	// check for :XX:
		if (strlen(isisPath) < 4 || isisPath[3] != ':')
			return ERROR_BADFILENAME;
		dev[0] = toupper(isisPath[1]);
		dev[1] = toupper(isisPath[2]);
		isisPath += 4;
	}

	for (i = 0; i < nDevices; i++) // look up device
		if (strcmp(dev, deviceMap[i].dev) == 0)
			break;
	if (i >= nDevices)
		return ERROR_BADDEVICE;
	pInfo->deviceId = i;
	pInfo->deviceType = deviceMap[i].devtype;

	if (pInfo->deviceType == 3) { // parse file name if file device
		for (i = 0; i < 6 && isalnum(*isisPath); i++)
			pInfo->name[i] = toupper(*isisPath++);
		if (i == 0)
			return ERROR_BADFILENAME;
		if (*isisPath == '.') {
			isisPath++;
			for (i = 0; i < 3 && isalnum(*isisPath); i++)
				pInfo->ext[i] = toupper(*isisPath++);
			if (i == 0)
				return ERROR_BADEXT;
		}
	}
	return isalnum(*isisPath) ? ERROR_BADFILENAME : ERROR_SUCCESS;
}


/*
	map an isis file into a real file
	the realFile contains is prefixed with two bytes
	deviceId and deviceType
	return isis error status
 */

static word MapFile(osfile_t *osfileP, const char *isisPath)
{
	spath_t info;
	const char *src;
	char buf[8];
	int status;
	static byte modes[] = { READ_MODE, WRITE_MODE, UPDATE_MODE, UPDATE_MODE + RANDOM_ACCESS };

	if ((status = ParseIsisName(&info, isisPath)) != ERROR_SUCCESS) // get canocial name
		return status;

	osfileP->deviceId = info.deviceId;
	osfileP->modes = modes[info.deviceType];

	if (22 <= info.deviceId && info.deviceId <= 24) // BB, CI, CO
		return ERROR_SUCCESS;

	/* see if user has provided a device map */
	sprintf(buf, "ISIS_%s", deviceMap[info.deviceId].dev);	// look for any mapping provided
	src = getenv(buf);

	if (!src && info.deviceId != 0) { // no mapping and not :F0:
		fprintf(stderr, "No mapping for :%s:\n", deviceMap[info.deviceId].dev);
		return ERROR_NOTREADY;
	}
	if (src && *src) {
		strcpy(osfileP->name, src);
		/* for files append a path separator if there isn't one */
		if (info.deviceType == 3)
			if (strchr(osfileP->name, 0)[-1] != '/' && strchr(osfileP->name, 0)[-1] != '\\')
				strcat(osfileP->name, "/");
	}
	else
		osfileP->name[0] = 0;
	if (info.deviceType == 3) {	// add file name
		char *s = strchr(osfileP->name, 0);		// get end of string
		int i;
		for (i = 0; i < 6 && info.name[i]; i++)
			*s++ = tolower(info.name[i]);
		if (info.ext[0]) {
			*s++ = '.';
			for (i = 0; i < 3 && info.ext[i]; i++)
				*s++ = tolower(info.ext[i]);
		}
		*s = 0;
	}
	return ERROR_SUCCESS;
}



int main(int argc, char **argv)
{
	int i;
	size_t len;
	char *s, *progname;

	if (argc == 2 && strcmp(argv[1], "-v") == 0)
		showVersion("C port of Intel's ISIS-II LOCATE v3.0 by Mark Ogden");
#ifdef _WIN32
	(void)_setmode(_fileno(stdin), O_BINARY);
	(void)_setmode(_fileno(stdout), O_BINARY);
#endif
	/* find program name */
	for (progname = argv[0]; s = strpbrk(progname, ":/\\"); progname = s + 1)
		;
	/* only allow alpha numeric char names otherwise it may confuse isis program */
	/* .exe is also excluded*/
	for (s = _commandLine, i = 0; isalnum(*progname) && i < 6; i++)
		*s++ = *progname++;
	*s = 0;

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

word ChkMode(word conn, word mode)
{
	if (conn >= AFTSIZE)
		return ERROR_BADPARAM;
	if (aft[conn].mode == 0)
		return ERROR_NOTOPEN;
	if (mode != 0 && (mode & aft[conn].mode) == 0) {
		if (mode == READ_MODE)
			return ERROR_NOREAD;
		if (mode == WRITE_MODE)
			return ERROR_NOWRITE;
		if (mode == RANDOM_ACCESS)
			return ERROR_CANTSEEKDEV;
	}
	return ERROR_SUCCESS;
}


void Close(word conn, wpointer statusP)
{
	if ((*statusP = ChkMode(conn, 0)) != ERROR_SUCCESS)
		return;

	if (conn <= BB_DEV)					// ingore requests to close CO, CI or BB
		return;

	if (close(aft[conn].fd) < 0)
		*statusP = ERROR_NOTOPEN;
	aft[conn].mode = 0;
}


void Delete(pointer pathP, wpointer statusP)
{
	osfile_t osfile;

	if ((*statusP = MapFile(&osfile, pathP)) == ERROR_SUCCESS) {
		if (!(osfile.modes & RANDOM_ACCESS))
			*statusP = ERROR_ISDEVICE;
		else if (unlink(osfile.name) < 0) {
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
	osfile_t osfile;
	int handle;

	*connP = -1;		// unused conn
	if ((*statusP = MapFile(&osfile, pathP)) != ERROR_SUCCESS)
		return;

	switch (access) {
	case READ_MODE: mode = O_RDONLY | O_BINARY; break;
	case WRITE_MODE: mode = O_WRONLY | O_CREAT | O_TRUNC | O_BINARY; break;
	case UPDATE_MODE: mode = O_RDWR | O_CREAT | O_TRUNC | O_BINARY; break;
	default: fprintf(stderr, "bad access mode %d for %s\n", access, osfile.name);
		*statusP = ERROR_BADPARAM;
		return;
	}

	if ((osfile.modes & access) != access) {
		*statusP = ERROR_BADACCESS;
		return;
	}

	if (osfile.deviceId == 23) { // CI
		*connP = CI_DEV;
		return;
	}
	else if (osfile.deviceId == 24) { // CO
		*connP = CO_DEV;
		return;
	}
	else if (osfile.deviceId == 22) { // BB
		*connP = BB_DEV;
		return;
	}

	// allocate a handle
	for (handle = 2; handle < AFTSIZE; handle++)
		if (aft[handle ].mode == 0)
			break;

	if (handle >= AFTSIZE) {
		*statusP = ERROR_NOHANDLES;
		return;
	}

	conn = open(osfile.name, mode, S_IREAD | S_IWRITE);

	if (conn < 0)
		switch (errno) {
		case EACCES: *statusP = ERROR_PERMISSIONS; break;
		case EEXIST: *statusP = ERROR_NOWRITE; break;
		case EMFILE: *statusP = ERROR_NOHANDLES; break;
		case ENOENT: *statusP = ERROR_FILENOTFOUND; break;
		default: fprintf(stderr, "unknown error %d for open %s", errno, osfile.name);
			*statusP = ERROR_NOTREADY;
		}
	else {
		aft[handle].fd = conn;
		aft[handle].mode = access + (osfile.modes & RANDOM_ACCESS);
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

	if ((*statusP = ChkMode(conn, READ_MODE)) != ERROR_SUCCESS)
		return;

	if (conn == CI_DEV) {
		if (!*_commandLinePtr)
			_commandLinePtr = ReadLine(_commandLine);

		for (actual = 0; actual < count && *_commandLinePtr; actual++)
			*buffP++ = *_commandLinePtr++;
		*actualP = actual;
		return;
	}
	else if (conn == BB_DEV) {
		*actualP = 0;
		return;
	}
	if ((actual = read(aft[conn].fd, buffP, count)) >= 0) {
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

	if ((*statusP = ChkMode(conn, RANDOM_ACCESS)) != ERROR_SUCCESS)
		return;

	if (aft[conn].mode == RANDOM_ACCESS + WRITE_MODE) {
		*statusP = ERROR_SEEKWRITE;
		return;
	}

	if (mode != SEEKTELL)
		offset = *blockP * 128 + *byteP;

	switch (mode) {
	case SEEKTELL:
		offset = lseek(aft[conn].fd, 0L, SEEK_CUR);
		*blockP = (word) (offset / 128);
		*byteP = offset % 128;
		*statusP = 0;
		return;
	case SEEKABS:	origin = SEEK_SET; break;
	case SEEKBACK:	offset = -offset;
	case SEEKFWD:	origin = SEEK_CUR; break;
	case SEEKEND:	origin = SEEK_END; offset = 0;  break;
	default: fprintf(stderr, "Unsupported seek mode %d\n", mode);
		*statusP = ERROR_BADMODE;
		return;
	}
	if (lseek(aft[conn].fd, offset, origin) >= 0)
		*statusP = 0;
	else
		*statusP = ERROR_BADPARAM;
}
void Write(word conn, pointer buffP, word count, wpointer statusP)
{
	if ((*statusP = ChkMode(conn, WRITE_MODE)) != ERROR_SUCCESS)
		return;

	if (conn == BB_DEV)
		return;

	if (write(aft[conn].fd, buffP, count) != count)
		switch (errno) {
		case ENOSPC: *statusP = ERROR_DISKFULL; break;
		case EINVAL: *statusP = ERROR_BADPARAM;
		default: *statusP = ERROR_NOTREADY;
		}
}

void Rename(pointer oldP, pointer newP, wpointer statusP)
{
	osfile_t oldFile, newFile;

	if ((*statusP = MapFile(&oldFile, oldP)) != ERROR_SUCCESS || (*statusP = MapFile(&newFile, newP)) != ERROR_SUCCESS)
		return;
	if (oldFile.deviceId != newFile.deviceId)
		*statusP = ERROR_RENACROSS;
	else if ((oldFile.modes & RANDOM_ACCESS) == 0)
		*statusP = ERROR_ISDEVICE;
	else if (rename(oldFile.name, newFile.name) != 0)
		switch (errno) {
		case EACCES: *statusP = ERROR_EXISTS; break;
		case ENOENT: *statusP = ERROR_FILENOTFOUND; break;
		case EINVAL: *statusP = ERROR_BADFILENAME; break;
		default: *statusP = ERROR_BADPARAM; break;
		}
}


void Spath(pointer pathP, spath_t *infoP, wpointer statusP)
{
	if ((*statusP = ParseIsisName(infoP, pathP)) != ERROR_SUCCESS)
		return;
	if (infoP->deviceType == 3)
		infoP->driveType = 4;
}
