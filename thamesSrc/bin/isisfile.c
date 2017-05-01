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

#include "thames.h"

static int isis_close_file(ISIS_FILE *self);


static ISIS_FILE *root = NULL;


ISIS_FILE *conin = NULL, *conout = NULL, *devnull = NULL;



/* [Mark Ogden]
    const char *xlt_device(const char *dev);
    int isis_drive_exists(int n);
    moved to cmdline.c to tie in with disk name caching
*/

/* Is this filename for an ISIS device? 
 *
 * Returns:
 * 0: It is not
 * 1: It is a character device eg :CI: :BB:
 * 2: It is a block device     eg :F0: :F2:
 */
int isis_isdev(const char *isisname)
{
/* All devices have 4-character names */
    if (strlen(isisname) != 4) return 0; 

/* All devices have names of the form :??: */	
    if (isisname[0] != ':' || isisname[3] != ':') return 0;

/* Block devices of the form :F<digit>: */
    if (isdigit(isisname[2]) && 
            (isisname[1] == 'F' || isisname[1] == 'f')) return 2;

/* Everything else is a character device */
    return 1;
}

/* Find out which drive a file is on.
 * Returns 0-9, or -1 for a character device */
int isis_getdrive(const char *isisname)
{
    if (strlen(isisname) >= 4 &&
        isisname[0] == ':' &&
        isisname[3] == ':')
    {
            if ((isisname[1] == 'F' || isisname[1] == 'f') &&
             isdigit(isisname[2]))
        {
            return isisname[2] - '0';
        }
        return -1;
    }
    return 0;	/* If no device spec, assume :F0: */
}

/* [Mark Ogden] - Caution. Under unix if a path name is used the isis file name part
 * is not mapped to lower case.
 * This is only likely to be an issue with the isis program name.
 * With the -m option the file is mapped to an isis name so under unix ensure that the
 * isis file name is in lower case
*/

int isis_name2unix(const char *isisname, char *unixname)
{
    char destname[PATH_MAX+11];	// might have a / name & extent appended before testing length
    char isisdev[5];
    const char *src;
    char *dest;
    int havedev;

    memset(destname, 0, sizeof(destname));
    /* If filename starts with a . or a /, treat it as a UNIX filename */
#ifdef _WIN32	// [Mark Ogden] added support for dos filenames
    if (isisname[0] == '/' || isisname[0] == '.' || (isalpha(isisname[0]) &&  isisname[1] == ':') || isisname[0] == '\\')
#else
    if (isisname[0] == '/' || isisname[0] == '.')
#endif
    {
        if (strlen(isisname) >= PATH_MAX) 		// [Mark Ogden] check file name is not too long
            return ERROR_BADFILENAME;
        strcpy(unixname, isisname);
        return ERROR_SUCCESS;
    }

    /* It's an ISIS filename. If it doesn't start with a device
     * specifier, assume :F0: */
    if (strlen(isisname) < 4 || isisname[0] != ':' || isisname[3] != ':')
    {
        strcpy(isisdev, ":F0:");
        havedev = 0;
    }
    else
    {
        sprintf(isisdev, "%-4.4s", isisname);	
        havedev = 1;
    }
    capitals(isisdev);
/* The bit bucket (:BB:) is always defined, and is /dev/null */
    if (!strcmp(isisdev, ":BB:"))
    {
#ifdef _WIN32		// [Mark Ogden] added dos nul device support
        strcpy(unixname, "nul");
#else
        strcpy(unixname, "/dev/null");
        return ERROR_SUCCESS;
#endif
    }
/* Check for other mapped devices */
    if (isis_isdev(isisname) == 1)	/* Character device */
    {
        src = xlt_device(isisname);
        if (!src)
        {
            fprintf(stderr, "No UNIX mapping for ISIS "
                "character device %s\n", isisdev);
            return ERROR_BADDEVICE;
        }
        if (strlen(src) >= PATH_MAX)	// catch file name too long
            return ERROR_BADFILENAME;
        strcpy(unixname, src);
        return ERROR_SUCCESS;
    }
/* isisdev had just better be a valid block device by now */
    if (isis_isdev(isisdev) != 2) return ERROR_BADFILENAME;

    src = xlt_device(isisdev);
    if (!src)
    {
        fprintf(stderr, "No UNIX mapping for ISIS "
            "block device %s\n", isisdev);
        return ERROR_BADFILENAME;
    }

    if (strlen(src) >= PATH_MAX)	// catch file name too long
        return ERROR_BADFILENAME;

    strcpy(destname, src);
#ifdef _WIN32		// [Mark Ogden] map \ to / and handle x: fpr dps
    for (char *s = strchr(destname, '\\'); s; s = strchr(s, '\\'))	*s = '/';
    if (destname[2] || destname[1] == ':' || !isalpha(destname[0]))	// x: only shouldn't assume root
#endif
    {
        /* Append a path separator if there isn't one */
        if (destname[strlen(destname) - 1] != '/')
            strcat(destname, "/");
    }
    dest = destname + strlen(destname);
    src  = &isisname[havedev ? 4 : 0];

    while (*src && !isspace(*src))		// [Mark Ogden] assumes filename part has max 10 chars i.e name56.ext
    {
        *dest++ = tolower(*src++);
    }
    if (strlen(destname) >= PATH_MAX)	// [Mark Ogden] check file name path is still ok
        return ERROR_BADFILENAME;
    strcpy(unixname, destname);
    return ERROR_SUCCESS;
}




/* Create a new ISIS file with a unique handle */
static int new_isis_file(ISIS_FILE **pf)
{
    ISIS_FILE *result;
    byte handle_used[256];
    int handle;

    memset(handle_used, 0, sizeof(handle_used));
    /* Generate a new file handle that isn't in use. */
    for (result = root; result != NULL; result = result->next)
    {
        handle = result->handle;
        if (handle >= 0 && handle < 256) handle_used[handle] = 1;
    }
    for (handle = 0; handle < 256; handle++)
    {
        if (!handle_used[handle]) break;
    }
    if (handle > 255) 
    {
        *pf = NULL;
        return ERROR_NOHANDLES;	/* ISIS assumes a maximum of
                     * 256 file handles */
    }
    result = malloc(sizeof(ISIS_FILE));
    if (!result) 
    {
        *pf = NULL;
        return ERROR_NOMEM;
    }
    /* Link this file into the chain */
    memset(result, 0, sizeof(ISIS_FILE));
    result->next = root;
    result->handle = handle;
    root = result;
    *pf = result;
    return ERROR_SUCCESS;
}



void delete_isis_file(ISIS_FILE *f)
{
    ISIS_FILE *other;

    if (f->fp)
    {
        isis_close_file(f);
    }
    // [Mark Ogden] - bug fix don't delete :ci: or :co:
    if (f == conin || f == conout)
        return;
    /* Remove entry f from the linked list, if it's present */

    for (other = root; other != NULL; other = other->next)
    {
        if (other->next == f)
        {
            other->next = f->next;
        }
    }
    if (root == f)
    {
        root = f->next;
    }
    if (f->buffer)
    {
        delete_buffer(f->buffer);
    }
    free(f);
}

static int isis_close_file(ISIS_FILE *self)
{
    if (!self->fp) return ERROR_SUCCESS;

    /* These two stay open */
    if (self == conin || self == conout) 
    {
        fflush(self->fp);
        return ERROR_SUCCESS;
    }
    /* Don't actually close stdin / stdout / stderr */
    if (self->fp == stdin || self->fp == stdout || self->fp == stderr)
    {
        self->fp = NULL;
        return ERROR_SUCCESS;
    }
    fclose(self->fp);
    self->fp = NULL;
    return ERROR_SUCCESS;
}


LINE_BUFFER *new_buffer(int len)
{
    LINE_BUFFER *res = malloc(len + sizeof(LINE_BUFFER));

    if (!res) return NULL;
    memset(res, 0, len + sizeof(LINE_BUFFER));
    return res;
}


void delete_buffer(LINE_BUFFER *buf)
{
    free(buf);
}


int isis_open_stdio(void)
{
    int err;

    err = new_isis_file(&conout);
    if (err) return err;

    strcpy(conout->filename, ":CO:");
    conout->fp = stdout;
    conout->handle = 0;
    conout->access = 2;
    conout->echo = 0;

    err = new_isis_file(&conin);
    if (err) return err;

    strcpy(conin->filename, ":CI:");
    conin->fp = stdin;	/* [Mark Ogden] Should be stdin, not stdout */
    conin->handle = 1;
    conin->access = 1;
    conin->echo = 0;
    return ERROR_SUCCESS;
}


/* Detect attempts to open/close/delete the console, which is already open */
ISIS_FILE *isis_devspecial(const char *name)
{
    char filename[PATH_MAX + 1];

    /* Skip leading spaces, if any */
    while (isspace(*name)) ++name;

    strncpy(filename, name, PATH_MAX);
    filename[PATH_MAX] = 0;
    trim(filename);
    capitals(filename);

    if (!strcmp(filename, ":CI:")) return conin;
    if (!strcmp(filename, ":CO:")) return conout;

    return NULL;
}


int  isis_open(int *handle, const char *isisname, int access, int echo)
{
    ISIS_FILE *isf;
    char unixname[PATH_MAX + 1];
    int err;

/* Access modes: 
    1	"r"
    2	"w"
    3	If file exists, "r+" else "w+"
*/
    if (access < 1 || access > 3) return ERROR_BADACCESS;
/* XXX Handle Echo.
    If Echo is nonzero, the file is put in buffered mode and its low 
    byte is the handle of the file to which input should be echoed. 
    (So 0xFF00 -> file 0, :CO: )
*/ 

    /* Check for attempts to open the console, which is always open */
    isf = isis_devspecial(isisname);
    if (isf)
    {
        if (isf == conin  && access != 1) return ERROR_BADACCESS;
        if (isf == conout && access != 2) return ERROR_BADACCESS;
        isf->access = access;
        isf->echo   = echo;
        *handle = isf->handle;
        return ERROR_SUCCESS;
    }
    /* Not the console. We need a new file. */
    err = new_isis_file(&isf);
    if (err) return err;

    strncpy(isf->filename, isisname, PATH_MAX);
    isf->filename[PATH_MAX] = 0;
    isf->access = access;
    isf->echo   = echo;

    if (isf->echo)
    {
        /* Give the file a dummy buffer; isis_read() will 
         * populate it for real. */
        isf->buffer = new_buffer(1);
        isf->buffer->len = 1;
        isf->buffer->pos = 1;
    }

    err = isis_name2unix(isisname, unixname);
    if (err) 
    {
        delete_isis_file(isf);
        return err;
    }
    switch(access)
    {
        case 1: isf->fp = fopen(unixname, "rb"); 
//			printf("Open %s rb = %p\n", unixname, isf->fp);
            break;
        case 2: isf->fp = fopen(unixname, "wb"); 
//			printf("Open %s wb = %p\n", unixname, isf->fp);
            break;
        case 3: isf->fp = fopen(unixname, "r+b"); 
//			printf("Open %s r+b = %p\n", unixname, isf->fp);
            if (!isf->fp) 
            {
                isf->fp = fopen(unixname, "w+b");
//				printf("Open %s w+b = %p\n", unixname, isf->fp);
            }
            break;
    }	
    if (!isf->fp)
    {
        if (trace)
            fprintf(stderr, "Can't open '%s'\n", unixname);
        delete_isis_file(isf);
        return ERROR_FILENOTFOUND;
    }
    *handle = isf->handle;
    return ERROR_SUCCESS;
}



int isis_close(int handle)
{
    int err;
    ISIS_FILE *f = find_handle(handle);

    if (!f) return ERROR_NOTOPEN;

    err = isis_close_file(f);
    delete_isis_file(f);
    return err;
}


int isis_delete(const char *isisname)
{
    ISIS_FILE *fd;
    char realname[PATH_MAX];
    int err;

    /* If the filename refers to a device, it can't be deleted */
    if (isis_isdev(isisname)) return ERROR_ISDEVICE;

    /* This should never return true, because isis_isdev() ought to
     * have caught attempts to delete the console. But just in case... */
    if (isis_devspecial(isisname)) return ERROR_ISDEVICE;

    err = isis_name2unix(isisname, realname);
    if (err) return err;

    /* Check for attempts to delete an open file. Unix is happy with this,
     * but ISIS is not. */
    for (fd = root; fd != NULL; fd = fd->next) {
        if (!strcmp(realname, fd->filename))
            return ERROR_FILEINUSE;
    }

    if (remove(realname)) return ERROR_PERMISSIONS;
    /* Saves deleted files for future comparison.
        {
            int ok = 0;
            char altname[PATH_MAX + 1];
            int ver = 0;
            struct stat st;

            while (!ok)
            {
                sprintf(altname, "%s.%d", realname, ver);
                if (stat(altname, &st) < 0)
                {
                    rename(realname, altname);
                    return ERROR_SUCCESS;
                }
                ++ver;
            }

        }
    */

    return ERROR_SUCCESS;
}



int isis_read(int handle, byte *buffer, int count, int *actual)
{
    FILE *fp;
    ISIS_FILE *fd;
    int avail = 0;
    char input[ISIS_LINE_MAX + 1];
    int err = ERROR_SUCCESS;


    fd = find_handle(handle);

    if (!fd) return ERROR_NOTOPEN;

    if (fd->access == 2) return ERROR_NOREAD;

    if (fd->buffer)	/* File is in line mode */
    {
        avail = fd->buffer->len - fd->buffer->pos;

        if (avail == 0) /* Reload buffer */
        {
            if (!fgets(input, ISIS_LINE_MAX, fd->fp)) {
                if (fd->echo)
                    err = isis_write(fd->echo, "\x1a\r\n", 3);  // isis writes control Z, cr, lf
                *actual = 0;
                return err;
            }

            if (input[0] != 0 && input[strlen(input) - 1] == '\n') {
                input[strlen(input) - 1] = 0;
            }
            if (input[0] != 0 && input[strlen(input) - 1] == '\r') {	// [Mark Ogden] non unix systems may include \r as well
                input[strlen(input) - 1] = 0;
            }
            delete_buffer(fd->buffer);
            fd->buffer = new_buffer(2 + strlen(input));		// [Mark Ogden] note new_buffer allocates 1 additional byte
            strcpy(fd->buffer->data, input);
            strcat(fd->buffer->data, "\r\n");
            fd->buffer->pos = 0;
            /* [Mark Ogden] avail was not set */
            avail = fd->buffer->len = strlen(fd->buffer->data);
        }
        if (avail > count) avail = count;
        for (int i = 0; i < avail; i++)						// [Mark Ogden] modified to handle multiple lines in buffer
            if ((buffer[i] = fd->buffer->data[fd->buffer->pos++]) == '\n')
                avail = i + 1;
        err = ERROR_SUCCESS;
/* If there is an echo file set up, write to it */
        if (fd->echo != 0)
            err = isis_write(fd->echo, buffer, avail);

        *actual = avail;									// [Mark Ogden] pos updated in copy loop above
        return err;
    }
/* Unbuffered read */
    fp = fd->fp;

    if (fp == stdout) fp = stdin;

    avail = fread(buffer, 1, count, fp);
    *actual = avail;
    return ERROR_SUCCESS;
}


int isis_write(int handle, byte *buffer, int count)
{
    FILE *fp;
    ISIS_FILE *fd;
    int done;

    fd = find_handle(handle);

    if (!fd) return ERROR_NOTOPEN;

    if (fd->access == 1) return ERROR_NOWRITE;

    fp = fd->fp;

    if (fp == stdin) fp = stdout;
    if (fp == stdout)
        errCheck(buffer, count);		// intercept to check if app error

    done = fwrite(buffer, 1, count, fp);
    if (done < count) return ERROR_DISKFULL;
    return ERROR_SUCCESS;
}




int isis_seek(int handle, int mode, long *offset)
{
    ISIS_FILE *fd;
    long pos;

    fd = find_handle(handle);

    if (!fd) return ERROR_NOTOPEN;

    if (fd->access == 2) return ERROR_SEEKWRITE;
    if (fd->fp == stdin || fd->fp == stdout || fd->fp == stderr)
    {
        return ERROR_CANTSEEKDEV;
    }
    switch (mode)
    {
        case 0:	 /* Get position */
             pos = ftell(fd->fp);
             /* Can't get current position */
             if (pos < 0) return ERROR_CANTSEEKDEV;
            *offset = pos;
             break;

        case 1: /* Seek backward */
            pos = ftell(fd->fp);
             if (pos < 0) return ERROR_CANTSEEKDEV;
            if (fd->access == 2)  return ERROR_SEEKWRITE;
            if (pos - (*offset) < 0) 
            {
/* [Mark Ogden] Seeking off beginning is an error, but nevertheless
 * rewinds the file */
                fseek(fd->fp, 0L, SEEK_SET);
                return ERROR_OFFBEGINNING;
            }
            if (fseek(fd->fp, -(*offset), SEEK_CUR) < 0)
                return ERROR_CANTSEEKDEV;
            break;


        case 2: /* Seek absolute */
            if (fd->access == 2) return ERROR_SEEKWRITE;
            if (fseek(fd->fp, (*offset), SEEK_SET) < 0)
                return ERROR_CANTSEEKDEV;
            break;

        case 3: /* Seek forward */
            if (fd->access == 2) return ERROR_SEEKWRITE;
            if (fseek(fd->fp, (*offset), SEEK_CUR) < 0)
                return ERROR_CANTSEEKDEV;
            break;

        case 4:	/* Seek EOF */
            if (fd->access == 2) return ERROR_SEEKWRITE;
            if (fseek(fd->fp, 0, SEEK_END) < 0)
                return ERROR_CANTSEEKDEV;
            break;

        default: return ERROR_BADMODE; 
    }
    return ERROR_SUCCESS;
}

int isis_rename(const char *oldname, const char *newname)
{
    char unixold[1 + PATH_MAX];
    char unixnew[1 + PATH_MAX];
    int err;
    struct stat st;

    if (isis_isdev(oldname) || isis_isdev(newname)) return ERROR_ISDEVICE;

    err = isis_name2unix(oldname, unixold); if (err) return err;	
    err = isis_name2unix(newname, unixnew); if (err) return err;	

    if (isis_getdrive(oldname) != isis_getdrive(newname))
        return ERROR_RENACROSS;	/* Can't rename across drives */

    if (!stat(unixnew, &st)) return ERROR_EXISTS;	/* Target file exists */

    if (!rename(unixold, unixnew)) return ERROR_SUCCESS;
    
    return ERROR_PERMISSIONS;
}

int isis_console(const char *ciname, const char *coname)
{
    char unixname[1 + PATH_MAX];
    FILE *fp;
    int err;

/* If filename is :CI: or :CO:, don't reassign */
    if (isis_devspecial(ciname) == NULL)	
    {
        err = isis_name2unix(ciname, unixname); 
        if (err) return err;
        fp = fopen(unixname, "rb");
        if (!fp) return ERROR_FILENOTFOUND;

        if (conin->fp != stdin) fclose(conin->fp);
        conin->fp = fp;	
    }
    if (isis_devspecial(coname) == NULL)
    {
        err = isis_name2unix(coname, unixname); 
        if (err) return err;
        fp = fopen(unixname, "wb");
        if (!fp) return ERROR_DIRFULL;

        if (conout->fp != stdout && conout->fp != stderr) 
            fclose(conout->fp);
        conout->fp = fp;	
    }
    return ERROR_SUCCESS;
}


int isis_attrib(const char *isisname, int attr, int value)
{
    char unixname[1 + PATH_MAX];
    int err;
    struct stat st;
    mode_t mode;

    if (attr < 0 || attr > 3) return ERROR_BADATTRIB;
/* UNIX only supports the read-only attribute. */
    if (attr != 2) return ERROR_SUCCESS;	
    if (isis_isdev(isisname)) return ERROR_ISDEVICE;
    err = isis_name2unix(isisname, unixname);

    if (stat(unixname, &st) < 0) return ERROR_FILENOTFOUND;

    mode = st.st_mode;	
    if (value & 1) /* Read-only */
        mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
    else	mode |= S_IWUSR;

    if (mode == st.st_mode) return ERROR_SUCCESS;	/* Already set */
    if (chmod(unixname, mode) == 0) return ERROR_SUCCESS;
    return ERROR_PERMISSIONS;
}


int isis_rescan(int handle)
{
    ISIS_FILE *fd;

    fd = find_handle(handle);

    if (!fd || !fd->buffer) return ERROR_NOTLINEMODE;

    fd->buffer->pos = 0;
    return ERROR_SUCCESS;	
}


int isis_whocon(int handle, char *isisname)
{
    if (handle & 1) strcpy(isisname, conin->filename);
    else		strcpy(isisname, conout->filename);
    return ERROR_SUCCESS;	
}


int isis_spath(const char *isisname, ISIS_STAT *result)
{
    int err;
    int drive;
    char unixname[PATH_MAX];
    const char *namepart;
    ISIS_FILE *fd;
    char buf[17];
    char *pch;

    memset(result, 0, sizeof(*result));
/* See if the filename looks reasonable */
    err = isis_name2unix(isisname, unixname);
    if (err) return err;

    switch (isis_isdev(isisname))
    {
        case 0:	/* File. Remove drivespec if one is present */
            if (strlen(isisname) >= 4 &&
                isisname[0] == ':' &&
                isisname[3] == ':')
            {
                namepart = isisname + 4;
            }
            else
            {
                namepart = isisname;
            }
            drive = isis_getdrive(isisname);
            if (drive < 6)	
                result->device = drive;
            else	result->device = drive + 19;
            result->devtype = 3; /* Random access*/
            /* See if drive exists */
            if (isis_drive_exists(drive)) 
                result->drivetype = 4;
            else	result->drivetype = 0;
            /* Parse filename */
            sprintf(buf, "%-16.16s", namepart);
            trim(buf);
            pch = strchr(buf, '.');
            if (pch) *pch = 0;
            sprintf(result->filename, "%-6.6s", buf);
            pch = strchr(namepart, '.');
            if (pch)
                sprintf(result->ext, "%-3.3s", pch + 1);
            else	sprintf(result->ext, "%-3.3s", " ");
            break;

        case 1: /* Character device */
            fd = isis_devspecial(isisname);	
            if (fd == conin)
            {
                result->device = 10;	/* User console input */
                result->devtype = 0;	/* Input */
            }
            else if (fd == conout)
            {
                result->device = 11;	/* User console output*/
                result->devtype = 1;	/* Output */
            }
            else if (!strcmp(unixname, "/dev/null"))
            {
                result->device = 22;	/* Null device */
                result->devtype = 2;	/* Input & output */
            }
            else	/* Other device, characteristics unknown */
            {
                result->device = 14;	/* User reader 1 */
                result->devtype = 3;
            }
            break;	

        case 2:	/* Block device */
            drive = isisname[2] - '0';
            if (drive < 6)	
                result->device = drive;	    /* :F0: to :F5: */
            else	result->device = drive + 19; /* :F6: to :F9: */	
            result->devtype = 3;	/* Random */
            sprintf(buf, "%-4.4s", isisname);
            /* See if drive exists */
            if (xlt_device(buf)) result->drivetype = 4;
            else		     result->drivetype = 0;
            break;
    } 
    return ERROR_SUCCESS;
}




ISIS_FILE *find_handle(unsigned short h)
{
    ISIS_FILE *f;

    for (f = root; f != NULL; f = f->next)
    {
        if (f->handle == h)
        {
            return f;
        }
    }
    return NULL;
}

const char *isis_filename(int h)
{
    ISIS_FILE *f = find_handle(h);

    return f ? f->filename : "[Unknown handle]";
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


void isis_perror(const char *s, int err)
{
    fprintf(stderr, "%s: ", s);
    
    if (err >= 0 && err < MAXERROR)
    {
        fprintf(stderr, "%s.\n", error_strings[err]);
    }
    else
    {
        fprintf(stderr, "Unknown error %d.\n", err);
    }
}


