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

/* Remove trailing whitespace from a filename */
void trim(char *s)
{
	if (*s) {	// [Mark Ogden] - guard against s == ""
		char *p = s + strlen(s) - 1;
		while (*p == ' ' || *p == '\r' || *p == '\n' || *p == '\t')
		{
			*p = 0;
			--p;
		}
	}
}



void capitals(char *s)
{
	while (*s)
	{
		if (islower(*s)) *s = toupper(*s);
		++s;
	}
}




