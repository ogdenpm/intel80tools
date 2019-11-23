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

#pragma once


#define MAXBUFSIZE ((ISIS_LINE_MAX) * 40)

void getDriveMapping();
void path2Isis(char *path);
void unix2Isis();
const char *xlt_device(const char *dev);
int isis_drive_exists(int n);
int parseOptions(int argc, char *argv[]);

extern int mOption;
extern int uOption;
extern int oOption;
extern int iOption;
extern int tOption;
extern char *depFile;
extern char* outputExt;

#define UNRESOLVED	1
#define UNSATISFIED 2
#define OVERLAP		4