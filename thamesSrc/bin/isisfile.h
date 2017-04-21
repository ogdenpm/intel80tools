/***************************************************************************
 *                                                                         *
 *    THAMES: Partial ISIS-II emulator                                     *
 *    Copyright (C) 2011 John Elliott <jce@seasip.demon.co.uk>             *
 *                                                                         *
 *    This library is free software; you can redistribute it and/or        *
 *    modify it under the terms of the GNU Library General Public          *
 *    License as published by the Free Software Foundation; either         *
 *    version 2 of the License, or (at your option) any later version.     *
 *                                                                         *
 *    This library is distributed in the hope that it will be useful,      *
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    *
 *    Library General Public License for more details.                     *
 *                                                                         *
 *    You should have received a copy of the GNU Library General Public    *
 *    License along with this library; if not, write to the Free           *
 *    Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,      *
 *    MA 02111-1307, USA                                                   *
 *                                                                         *
 ***************************************************************************/

#define ISIS_LINE_MAX 120	/* Max length of a line in line mode */


/* For a line-mode file: the buffer containing the current line */
typedef struct line_buffer
{
	int len;
	int pos;
	char data[1];
} LINE_BUFFER;

/* Wrapper for an ISIS file */
typedef struct isis_file
{
	struct isis_file *next;	     /* Open files are held in a linked list */
	char filename[PATH_MAX + 1]; /* ISIS filename */
	unsigned short handle;	     /* ISIS file handle */
	FILE *fp;		     /* Underlying stdio file */
	int access;		     /* ISIS access mode: 1=R 2=W 3=RW */
	int echo;		     /* ISIS echo file handle */
	LINE_BUFFER *buffer;	     /* Line buffer for line-mode files */
} ISIS_FILE;

/* An ISIS parsed filename */
typedef struct isis_stat
{
	int device;		/* Device on which the file is */
	char filename[7];	/* Filename, 1-6 characters */
	char ext[4];		/* Extension, 0-3 characters */
	int devtype;		/* Device type: 0-2=char dev, 3=block dev */
	int drivetype;		/* Drive type: 0=None, 4=hard drive */
} ISIS_STAT;


/* Allocate a new line buffer of the specified size */
LINE_BUFFER *new_buffer(int len);
/* Free a line buffer */
void delete_buffer(LINE_BUFFER *buf);

/* Standard input and output, from ISIS's point of view */
extern ISIS_FILE *conin, *conout;

/* Is this filename for an ISIS device? 
 *
 * Returns:
 * 0: It is not
 * 1: It is a character device eg :CI: :BB:
 * 2: It is a block device     eg :F0: :F2:
 */
int isis_isdev(const char *isisname);

/* Convert an ISIS filename to a UNIX filename 
 * unixname needs to point at a buffer of at least PATH_MAX characters
 * Returns 0 if OK, else ISIS error number */
int isis_name2unix(const char *isisname, char *unixname);

/* Find an ISIS file by handle */
ISIS_FILE *find_handle(unsigned short h);

/* Free memory for an ISIS file */
void delete_isis_file(ISIS_FILE *f);

/* Check for :CO: and :CI:, and return the console if so */
ISIS_FILE *isis_devspecial(const char *name);

/* Open handle 0 (stdout) and handle 1 (stdin) */
int isis_open_stdio(void);

/* Trace aid: What name is associated with a given file handle? */
const char *isis_filename(int handle);

/* ISIS file I/O syscalls: */
int isis_open  (int *handle, const char *isisname, int access, int echo);
int isis_close (int handle);
int isis_delete(const char *isisname);
int isis_read  (int handle, byte *buffer, int count, int *actual);
int isis_write (int handle, byte *buffer, int count);
int isis_seek  (int handle, int mode, long *offset);
int isis_rename(const char *oldname, const char *newname);
int isis_console(const char *conin, const char *conout);
int isis_attrib(const char *isisname, int attr, int value);
int isis_rescan(int handle);
int isis_whocon(int handle, char *isisname);
int isis_spath(const char *isisname, ISIS_STAT *info);

/* Expand error message */
void isis_perror(const char *txt, int error);

/* ISIS error codes, based on 
 *	<http://www.bitsavers.org/pdf/intel/ISIS_II/ISIS_internals.pdf>:
 */

#define ERROR_SUCCESS	   0x00	/* Operation succeeded */
#define ERROR_NOMEM        0x01	/* No memory available for buffer */
#define ERROR_NOTOPEN      0x02	/* File is not open */
#define ERROR_NOHANDLES    0x03 /* No more file handles available */
#define ERROR_BADFILENAME  0x04	/* Invalid pathname */
#define ERROR_BADDEVICE    0x05	/* Bad device name in filename */
#define ERROR_NOWRITE      0x06 /* Trying to write a file open for read */
#define ERROR_DISKFULL	   0x07	/* Cannot write, disk full */
#define ERROR_NOREAD       0x08	/* Trying to read a file open for write */
#define ERROR_DIRFULL      0x09	/* Cannot create file: Directory full */
#define ERROR_RENACROSS	   0x0A /* Cannot rename across devices */
#define ERROR_EXISTS       0x0B /* Cannot rename: other file already exists. */
#define ERROR_ALREADYOPEN  0x0C	/* File already open */
#define ERROR_FILENOTFOUND 0x0D	/* File not found */
#define ERROR_PERMISSIONS  0x0E /* File is write protected */
#define ERROR_OVERWRITING  0x0F /* Attempt to overwrite ISIS-II */
#define ERROR_BADIMAGE     0x10 /* Invalid executable image format */
#define ERROR_ISDEVICE     0x11 /* Attempt to rename/delete a device */
#define ERROR_BADFUNCTION  0x12	/* Invalid function number */
#define ERROR_CANTSEEKDEV  0x13 /* Can't seek on a device */
#define ERROR_OFFBEGINNING 0x14 /* Can't seek to before beginning of file */
#define ERROR_NOTLINEMODE  0x15	/* Can't rescan a file not opened in line mode*/
#define ERROR_BADACCESS    0x16	/* Bad access mode */
#define ERROR_NOFILENAME   0x17	/* No filename */
#define ERROR_DISKERROR    0x18	/* Disk I/O error */
#define ERROR_BADECHOFILE  0x19 /* Invalid echo file in OPEN */
#define ERROR_BADATTRIB    0x1A /* Incorrect attribute in ATTRIB */
#define ERROR_BADMODE      0x1B /* Incorrect mode in SEEK */
#define ERROR_BADEXT       0x1C /* Bad file extension */
#define ERROR_EOF          0x1D /* End of file on console input */
#define ERROR_NOTREADY     0x1E /* Drive not ready */
#define ERROR_SEEKWRITE    0x1F /* Can't seek on a write-only file */
#define ERROR_FILEINUSE    0x20 /* Can't delete an open file */
#define ERROR_BADPARAM     0x21 /* Bad system call parameter */
#define ERROR_BADLOADSW	   0x22	/* Invalid nswitch argument to LOAD */
#define ERROR_SEEKPASTEOF  0x23 /* Seek past EOF on file open for read */

