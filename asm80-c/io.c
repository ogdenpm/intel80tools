#include <ctype.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "asm80.h"


static char *commandLine;
static char *commandLinePtr;
byte *memory;
pointer top;
#define AVAILMEM	0x7000


void sysInit(int argc, char **argv)
{
	int i, len;

	for (i = 1, len = 6; i < argc; i++)
		len += strlen(argv[i]) + 1;
	commandLinePtr = commandLine = malloc(len + 2);
	if (commandLine == NULL) {
		fprintf(stderr, "sysInit out of memory\n");
		exit(2);
	}
	strcpy(commandLine, "asm80 ");
	for (i = 1; i < argc; i++) {
		if (i != 1)
			strcat(commandLine, " ");
		strcat(commandLine, argv[i]);
	}
	strcat(commandLine, "\r\n");

	memory = (pointer) malloc(AVAILMEM);
	top = memory + AVAILMEM;
}


void mapfile(char *dfile, char *ifile)
{
	/* for now don't map drive names*/

	if (ifile[0] == ':' && tolower(ifile[1]) == 'f' && isdigit(ifile[2]) && ifile[3] == ':')
		ifile += 4;
	while (isalnum(*ifile) || *ifile == '.' || *ifile == ':')
		*dfile++ = tolower(*ifile++);
	*dfile = 0;
}


void Close(word conn, apointer statusP)
{
	*statusP = 0;
	if (conn > 1)	// don't close stdin or stdout	
		if (_close(conn) < 0)
			*statusP = 2;
}

void Delete(pointer pathP, apointer statusP)
{
	char path[_MAX_PATH];
	mapfile(path, pathP);
	*statusP = 0;

	if (_unlink(path) < 0)
		if (errno == EACCES)
			*statusP = 14;
		else if (errno == ENOENT)
			*statusP = 13;
		else
			*statusP = 30;
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

void Load(pointer pathP, word LoadOffset, word swt, word entryP, apointer statusP)
{
	fprintf(stderr, "load not implmented\n");
	exit(2);
}

void Open(apointer connP, pointer pathP, word access, word echo, apointer statusP)
{
	int mode, conn;
	char path[_MAX_PATH];
	mapfile(path, pathP);

	*statusP = 0;
	switch (access) {
	case READ_MODE: mode = _O_RDONLY | O_BINARY; break;
	case WRITE_MODE: mode = _O_WRONLY | _O_CREAT | _O_TRUNC | O_BINARY; break;
	case UPDATE_MODE: mode = _O_RDWR | _O_CREAT | _O_TRUNC | O_BINARY; break;
	default: fprintf(stderr, "bad access mode %d for %s\n", access, path);
		*statusP = 22;
		*connP = -1;
		return;
	}
	if (strcmp(path, ":ci:") == 0)
		*connP = 1;
	else if (strcmp(path, ":co:") == 0)
		*connP = 0;
	else if (*path == ':') {
		*statusP = 13;
		*connP = -1;
	}
	else if ((conn = _open(path, mode, _S_IREAD | _S_IWRITE)) >= 0)
		*connP = conn;
	else {
		switch (errno) {
		case EACCES: *statusP = 14; break;
		case EEXIST: *statusP = 6; break;
		case EMFILE: *statusP = 3; break;
		case ENOENT: *statusP = 13; break;
		default: fprintf(stderr, "unknown error %d for open %s", errno, path);
			*statusP = 30;
		}
		*connP = -1;
	}
}

void Read(word conn, pointer buffP, word count, apointer actualP, apointer statusP)
{
	int actual;
	
	if (conn == 1 && *commandLinePtr) {		// rescanning command line
		for (actual = 0; actual < count && *commandLinePtr; actual++)
			*buffP++ = *commandLinePtr++;
		*actualP = actual;
		*statusP = 0;
		return;
	}
	
	if (conn < 2)
		conn = 1 - conn;	// isis has stdout/stdin swapped cf. dos/unix

	if ((actual = _read(conn, buffP, count)) >= 0) {
		*actualP = actual;
		*statusP = 0;
	}
	else {
		*actualP = 0;
		if (errno == EBADF)
			*statusP = 2;
		else if (errno == EINVAL)
			*statusP = 33;
		else
			*statusP = 30;
	}
}
void Rescan(word conn, apointer statusP)
{
	if (conn == 1) {
		commandLinePtr = commandLine;
		*statusP = 0;
	}
	else
		*statusP = 21;
}



void Seek(word conn, word mode, apointer blockP, apointer byteP, apointer statusP)
{
	long offset = *blockP * 128 + *byteP;
	int origin;

	if (conn <= 1) {
		*statusP = 19;
		return;
	}

	switch (mode) {
	case SEEKABS:	origin = SEEK_SET; break;
	case SEEKBACK:	offset = -offset;
	case SEEKFWD:	origin = SEEK_CUR; break;
	case SEEKEND:	origin = SEEK_END; break;
	default: fprintf(stderr, "Unsupported seek mode %d\n", mode);
		*statusP = 27;
		return;
	}
	if (_lseek(conn, offset, origin) >= 0)
		*statusP = 0;
	else
		*statusP = 33;
}
void Write(word conn, pointer buffP, word count, apointer statusP)
{
	if (conn < 2)
		conn = 1 - conn;	// isis has stdout/stdin swapped cf. dos/unix
	if (_write(conn, buffP, count) == count)
		*statusP = 0;
	else
		switch (errno) {
		case ENOSPC: *statusP = 7; break;
		case EINVAL: *statusP = 33;
		default: *statusP = 30;
		}
}
