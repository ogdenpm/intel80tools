/***************************************************************************
*                                                                         *
*    THAMES: Partial ISIS-II emulator                                     *
*    Copyright (C) 2011 John Elliott <jce@seasip.demon.co.uk>             *
*                                                                         *
*    Extensions to support command line unix/dos to isis file mapping     *
*    Copyright (C) 2017 Mark Ogden <mark.pm.ogden@btinternet.com>         *
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

// structure giving mapping of ISIS drives to unix path; dynamic is non zero if path is auto allocated
static struct {
    char *path;
    int dynamic;
} disks[10];

int mOption;	// map drives in command line
int uOption;	// ignore unresolved / unsatisfied errors
int oOption;	// ingore overlap errors
int iOption;	// ignore load errors
int tOption;    // map tmp files to windows/unix tmp files to allow parallel compiles
char *depFile;  // used to create dependency file
char* outputExt = ".obj";   /* default output extent */



/* Given a device name (eg, :F0:) look up the appropriate environment
* variable (eg: ISIS_F0) */
const char *xlt_device(const char *dev)
{
    char buf[20];

    if (strlen(dev) != 4 || dev[0] != ':' || dev[3] != ':')
        return NULL;

    if (dev[1] == 'F' || dev[1] == 'f' && isdigit(dev[2]))	// for F0-F9 returned cached disk path
        return disks[dev[2] - '0'].path;

    sprintf(buf, "ISIS_%-2.2s", dev + 1);

    return getenv(buf);
}

int isis_drive_exists(int n)
{
    if (n < 0 || n > 9) return 0;	/* Bad drive number */

    return disks[n].path != NULL;		// use cached disk info

}


// dump the drive mapping for F0-F9
static void showDriveMapping()
{
    for (int i = 0; i < 10; i++)
        if (disks[i].path)
            printf("ISIS_F%d=%s [%s]\n", i, disks[i].path, disks[i].dynamic ? "dynamic" : "static");
}


void getDriveMapping()
{
    char envpath[] = { "ISIS_F?" };
    char *s, *t;
    for (int i = 0; i < 10; i++) {		// process F0-F9
        envpath[6] = i + '0';
        s = getenv(envpath);
        if (s) {						// exists
            disks[i].path = (char *)malloc(strlen(s) + 2);	// room for null and possibly trailing /
            strcpy(disks[i].path, s);
#ifdef _WIN32
            for (s = strchr(disks[i].path, '\\'); s; s = strchr(s, '\\')) *s = '/';		// map \ to /
#endif
            t = strchr(disks[i].path, 0) - 1;	// add trailing / if needed
            if (*t != '/')
                strcat(t, "/");
        
        } else if (i == 0) {					// F0 treated specially, defaults to ./ if not specified
            disks[i].path = "./";
            disks[i].dynamic = 1;
        }
    }
    if (trace) {
        printf("Initial drive mapping\n");
        showDriveMapping();

    }
}


void usage() {
    printf(
        "usage: thames [-m] [-i] [-o] [-u] [-MF file] [-ME ext] [-T] isisprog isisargs...\n"
        "Options are:\n"
        "    -m         map os path names to ISIS drives, isisargs updated to use ISIS drives\n"
        "    -i         ingnore crc errors in load file\n"
        "    -o         treat overlap errors as warnings\n"
        "    -u         treat unresolved/unsatisfied errors as warnings\n"
        "    -MF file   create make dependency file\n"
        "    -ME ext    treat files with given ext as targets for dependency file - default .obj\n"
        "    -T         map application tmp files to os tmp files to allow parallel compiles\n"
        "Environment variables ISIS_Fx (where x is 0..9) map isis drive to os path\n"
        "Using -m option dynamically creates the mapping F0 -> . unless  ISIS_F0 specified\n"
        "Note thames attempts to detect application errors and return an appropriate exit code\n");
}

/* parse command line options */

int parseOptions(int argc, char *argv[])
{
    int isisProgArg;

    for (isisProgArg = 1; isisProgArg < argc && *argv[isisProgArg] == '-'; isisProgArg++) {
        if (strcmp(argv[isisProgArg], "-m") == 0)
            mOption = 1;
        else if (strcmp(argv[isisProgArg], "-u") == 0)
            uOption = 1;
        else if (strcmp(argv[isisProgArg], "-o") == 0)
            oOption = 1;
        else if (strcmp(argv[isisProgArg], "-i") == 0)
            iOption = 1;
        else if (strcmp(argv[isisProgArg], "-MF") == 0 && isisProgArg + 1 < argc)
            depFile = argv[++isisProgArg];
        else if (strcmp(argv[isisProgArg], "-ME") == 0 && isisProgArg + 1 < argc) {
            outputExt = argv[++isisProgArg];
        } else if (strcmp(argv[isisProgArg], "-T") == 0)
            tOption = 1;
        else if (strcmp(argv[isisProgArg], "-h") == 0) {
            usage();
            exit(0);
        }
        else
            printf("Unknown option %s\n", argv[isisProgArg]);
    }
    return isisProgArg;
}



/* code to map unix style command line file names to isis format
it splits long lines into multiple lines using the &\r\n standard
and supports & to force line break as per &\r\n standard
and && to force a simple line break
These characters may need to be quoted / escaped in your shell
*/



/* return the ISIS drive for a given unix path
if none specified then allocate new drive and
record as a dynamic allocation
the path parameter is overwritten with the
ISIS :Fx: drive name
Note path must be terminated with / (or : for Windows) */


void path2Isis(char *path)
{
    int i;
    for (i = 0; i < 10; i++)
        if (disks[i].path && PATHCMP(disks[i].path, path) == 0)
            break;

    if (i == 10) {
        /* scan for an unallocated mapping */
        for (i = 0; i < 10; i++) {
            if (disks[i].path == NULL) {
                disks[i].path = strdup(path);
                disks[i].dynamic = 1;
                break;
            }
        }
        if (i == 10) {
            fprintf(stderr, "too many drives allocated\n");
            showDriveMapping();
            exit(1);
        }
    }
    if (i == 0)		// don't need :F0:
        *path = 0;
    else {
        strcpy(path, ":F0:");
        path[2] = i + '0';		// fix the drive number
    }
}

/* parse the string -> inArgs and return the parsed argument converted to ISIS format
inArgs is updated to point to the rest of the input string */

static char *unixArg2Isis(char **inArgs)
{
    static char arg[PATH_MAX];		// returned value of parsed Arg
    char *s = *inArgs;
    char *t = arg;
    char *path, *filename;
    int inParen = 0;				// set to none 0 inside () for an option

    while (*s && *s != '&') {
        if (*s == '\t') *s = ' ';	// convert tabs to space outside quotes
        if (!inParen && (*s == ' ' || *s == ',') || inParen && *s == ')')	// space outside () and ) also end arg
            break;
        if (*s == ' ' || *s == ',' || *s == ')')
            *t++ = *s++;
        else if (*s == '(') {			// start of option arg
            *t++ = *s++;
            inParen = 1;
        }
        else if (*s == '\'') {	// copy string
            while (*t++ = *s++)
                if (*s == '\'') {
                    *t++ = *s++;	// '' is handled by starting new string
                    break;
                }
        }
        else if (strlen(s) >= 4 && s[0] == ':' && s[3] == ':') {		// ISIS device + possibly file name
            path = t;
            while (isalnum(*s) || *s == ':' || *s == '.')				// copy the name
                *t++ = *s++;
            /* if device is F0-F9 then check the environvent variable is defined else error */
            if ((path[1] == 'F' || path[1] == 'f') && isdigit(path[2])) {
                *t = 0;													// mark end to allow filename to be displayed if error
                if (!disks[path[2] - '0'].path) {							// not allocated to allocate to current working directory
                    disks[path[2] - '0'].path = "./";
                    disks[path[2] - '0'].dynamic = 1;
                }
                if (disks[path[2] - '0'].dynamic)					// already auto allocated
                    fprintf(stderr, "WARNING ISIS_F%c not explicitly defined for file %s - using %s\n", path[2], path, disks[path[2] - '0'].path);
            }
        }
        else {		// standard file name or an option name
            filename = path = t;	// start of filename path or option
            while (*s && *s != ' ' && *s != ',' && *s != '(' && *s != '&' && *s != ')') {
#ifdef _WIN32
                if (*s == '\\') *s = '/';
#endif
                if ((*t++ = *s++) == '/')	// copy chars updating start of file name if needed
                    filename = t;
            }
            *t = 0;		// mark end
#ifdef _WIN32
            if (filename == path && path[1] == ':')		// check for dos x: if no directory found
                filename = path + 2;
#endif

            if (filename != path) {		// we had a directory so need to fixit
                s = s - strlen(filename);	// backup inArgs to filename part
                path[strlen(path) - strlen(filename)] = 0;	// remove filename part from path
                path2Isis(path);
                t = strchr(path, 0);	// find new end
            }
        }
    }
    if (*s == ',' || *s == ')' || (t == arg && *s))		// tag comma to end of arg. Space and & treated as single chars
        if (s[0] == '&' && s[1] == '&') {					// && treated as simple new line
            *t++ = '\n';
            s += 2;
        }
        else
            *t++ = *s++;
    *t = 0;
    *inArgs = s;
    return arg;
}


void unix2Isis()
{
    char *inArgs = conin->buffer->data;
    char *arg, *sline;
    LINE_BUFFER *newLineBuffer;
    char *newCmdLine;


    getDriveMapping();
    if (!mOption)
        return;

    sline = newCmdLine = (char *)malloc(MAXBUFSIZE + 1);		//  big command line buffer

    *sline = 0;
    while (*inArgs) {
        arg = unixArg2Isis(&inArgs);

        if (strlen(newCmdLine) + strlen(arg) + 3 > MAXBUFSIZE) {
            fprintf(stderr, "Command line too long. Recompile thames with large MAXBUFSIZE\n");
            exit(1);
        }
        if (*arg == '&') {
            if (*sline == 0)	// already at start of line so ignore
                continue;
            strcat(sline, "&");
        }
        if (*arg == '\n' || *arg == '&') {		// && or &
            strcat(sline, "\r\n");
            sline = strchr(sline, 0);
        }
        else {
            if (strlen(sline) + strlen(arg) > ISIS_LINE_MAX - 3) {	// need room for &\r\n
                strcat(sline, "&\r\n");
                sline = strchr(sline, 0);
            }
            if (*arg != ' ' || *sline)		// surpress ' ' at start of line
                strcat(sline, arg);
        }
    }

    newLineBuffer = new_buffer((int)strlen(newCmdLine) + 3);
    strcpy(newLineBuffer->data, newCmdLine);
    free(conin->buffer);
    conin->buffer = newLineBuffer;
    free(newCmdLine);

    if (trace) {
        printf("\nDrive mapping post command line processing\n");
        showDriveMapping();
    }
}

