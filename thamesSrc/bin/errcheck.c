/***************************************************************************
*                                                                         *
*    THAMES: Partial ISIS-II emulator                                     *
*    Copyright (C) 2011 John Elliott <jce@seasip.demon.co.uk>             *
*                                                                         *
*    Extensions to detect ISIS application errors and return error code   *
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

#define MAXERRSTR	72

static char lineBuffer[MAXERRSTR + 1];
static int col;
int appError;

int appType;
enum apps {
    UNKNOWN = 0,
    ASM80, PLM80, LIB80, LINK80, LOC80, IXREF, BASIC
};


static int match(char *line);

static struct {
    char *signature;
    enum apps appType;
} signatures[] = {
    {"ISIS-II PL/M-80 COMPILER", PLM80},
    {"ISIS-II 8080/8085 MACRO ASSEMBLER", ASM80},
    {"ISIS-II OBJECT LOCATER", LOC80},
    {"ISIS-II OBJECT LINKER", LINK80},
    {"ISIS-II LIBRARIAN", LIB80},
    {"ISIS-II IXREF", IXREF},
	{"ISIS-II BASIC-80", BASIC},
    {NULL, UNKNOWN}
};


static struct {
    char *prefix;
    int len;
} isisErrors[] = {
    { "(0", 2},
    { "ARGUMENT",8 },
    { "ATTEMPT",7 },
    { "ATTEMPTED",9},
    { "ATTEMPTING", 10},
    { "BAD",3, },
    { "CAN'T",5 },
    { "CANNOT",6 },
    { "CHECKSUM",8 },
    { "COMMAND", 7},
    { "COMMON",6, },
    { "COMPILER", 8},
    { "DESTINATION", 11},
    { "DEVICE",6 },
    { "DISK",4 },
    { "DRIVE", 5},
    { "DUPLICATE",9 },
    { "DYNAMIC", 7},
    { "END", 3},
    { "ERROR:", 6},
    { "FILE",4 },
    { "FILENAME",8 },
    { "FIXUP",5 },
    { "ILLEGAL",7 },
    { "INCORRECTLY",11 },
    { "INPAGE", 5},
    { "INPUT", 5},
    { "INSUFFICIENT",12 },
    { "INVALID",7},
    { "LEFT",4},
    { "LIMIT", 5},
    { "MISSING",7},
    { "MODULE",6},
    { "MORE", 4},
    { "MULTIPLY", 8},
    { "NAME",4},
    { "NO",2},
    { "NON-DISK",8},
    { "NOT",3 },
    { "NULL",4 },
    { "OBJECT",6 },
    { "PERMISSIONS", 11},
    { "PHASE",5 },
    { "PREMATURE",9 },
    { "PROGRAM",7 },
    { "RECORD",6 },
    { "RELO",4 },
    { "RIGHT",5 },
    { "SEEK",4 },
    { "SEGMENT",7 },
    { "STACK", 5},
    { "TO'",3 },
    { "TOO",3 },
    { "TRYING", 6},
    { "UNASSIGNED",10 },
    { "UNEQUAL", 7},
    { "UNKNOWN", 7},
    { "UNRECOGNIZED",12 },
    { "WRITE",5 },
    {0, 0}

};


void errCheck(char *buffer, int count)
{

    for (int i = 0; !appError && i < count; i++) {
        if (col == 0 && (isspace(buffer[i]) || buffer[i] == '*'))	// ignore leading space and prompts
            continue;
        if (buffer[i] == '\r') {
            lineBuffer[col] = 0;
            appError = match(lineBuffer);
#ifdef SHOWERROR
            if (appError)
                printf("\n***%s\n", lineBuffer);
#endif
            col = 0;
        }
        else if (buffer[i] != '\n' && col < MAXERRSTR)
            lineBuffer[col++] = buffer[i];
    }
}


static int stdErrChk(char *line)
{
    while (*line == ' ')
        line++;

    for (int i = 0, cmp = 0; isisErrors[i].prefix && cmp >= 0; i++)
        if ((cmp = strncmp(line, isisErrors[i].prefix, isisErrors[i].len) == 0) &&			// prefix match
            line[isisErrors[i].len] == ' ' && isalnum(line[isisErrors[i].len + 1]))  // followed by space and alpha
            return 1;
    return 0;
}


enum {
    BOTH = 0,
    START, AFTER
};


static int match(char *line)
{
    int where = BOTH;		// where 0-> start of line or after ,:-, 1->start only, 2-> after ,:-

    if (!*line)
        return 0;

    switch (appType) {
    case UNKNOWN:
        for (int i = 0; signatures[i].signature; i++)
            if (strncmp(line, signatures[i].signature, strlen(signatures[i].signature)) == 0) {
                appType = signatures[i].appType;
                return 0;
            }
        break;
    case PLM80:
        if (strncmp(line, "PL/M-80 COMPILATION COMPLETE.", 29) == 0)
            return !!(strncmp(line + 34, " 0 PROGRAM ERROR", 16));
        else
            return !!strncmp(line, "COMPILATION TERMINATED", 22);	// PLM is well behaved so only need to look for these 2 error options
        break;
    case ASM80:
		if (strncmp(line, "ASSEMBLY COMPLETE,", 18) == 0) {
			for (line = line + 18; *line == ' '; line++)
				;
			return !!strncmp(line, "NO ERROR", 8);
		}
        where = START;
        break;
    case IXREF:
    case LIB80:
        where = AFTER;
        break;
    case LINK80:
        if (strncmp(line, "UNRESOLVED EXTERNAL", 19) == 0)
            return !uOption;
        where = BOTH;
        break;
    case LOC80:
        if (strncmp(line, "UNSATISFIED EXTERNAL", 19) == 0 || strncmp(line, "REFERENCE TO UNSATISFIED EXTERNAL", 33) == 0)
            return !uOption;
        if (strncmp(line, "(MEMORY OVERLAP", 15) == 0)
            return !oOption;
        where = BOTH;
        break;
	case BASIC:	// don't error check basic
		return 0;
    }
    if (where != AFTER && stdErrChk(line))
        return 1;
	if (where != START)
		while (*++line && *(line += strcspn(line, ",:-")))
			if (stdErrChk(line + 1))
				return 1;
    return 0;
}
