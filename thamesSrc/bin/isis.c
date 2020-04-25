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
#define ISIS_PATH_MAX	15
void get_isis_filename(int addr, char *buf)
{
	int n;

	/* Skip leading spaces */
	while(isspace(RAM[addr]))
		++addr;

	for (n = 0; n < ISIS_PATH_MAX - 1; n++, addr++)
	{
		int c = RAM[addr];
		if (!isalnum(c) && c != '.' && c != ':')
			break;
		buf[n] = c;
	}
	buf[n] = 0;
}

void dumpbufline(int offset, byte *data, int len)
{
	int n;

	printf("%04x: ", offset);
	for (n = 0; n < len; n++) printf("%02x ", data[n]);
	for (; n < 16; n++) printf("   ");
	putchar(' ');
	for (n = 0; n < len; n++) 
		if (isprint(data[n])) putchar(data[n]); else putchar('.');
	putchar('\n');	
}

void dumpbuffer(int base, byte *data, int len)
{
	int n;

	for (n = 0; n < len; n += 16)
	{
		int remaining = len - n;

		if (remaining > 16) remaining = 16;
		dumpbufline(n + base, &data[n], remaining);
	}
}



void putresult(byte *b, int n)
{
	b[0] = n & 0xFF;
	b[1] = n >> 8;
}


int paramword(int p, int n)
{
	int r = RAM[p + 2 * n + 1];

	r = (r << 8) | RAM[p + 2 * n];
	return r;
}



void isis_wrapopen(int param)
{
	byte *handle   = &RAM[paramword(param, 0)];
	int access     = paramword(param, 2);
	int echo       = paramword(param, 3);
	byte *status   = &RAM[paramword(param, 4)];
	int err = ERROR_SUCCESS;
	int fh = -1;
	char name[PATH_MAX + 1];

	get_isis_filename(paramword(param, 1), name);
	
	err = isis_open(&fh, name, access, echo);

	if (trace > 1) printf("ISIS open:   path=%s "
				"access=%04x echo=%04x "
				"handle=%d result=%d\n",
				name, access, echo, fh, err);

	putresult(status, err);
	putresult(handle, fh);
}


void isis_wrapload(int param, word *sp)
{
	int offset     = paramword(param, 1);
	int nswitch    = paramword(param, 2);
	byte *entry    = &RAM[paramword(param, 3)];
	byte *status   = &RAM[paramword(param, 4)];
	int err = ERROR_SUCCESS;
	int initpc = -1;
	char filename[PATH_MAX + 1];

	get_isis_filename(paramword(param, 0), filename);

	if (trace > 1) printf("ISIS load:   path=%s offset=%04x "
				"nswitch=%04x ",
				filename, offset, nswitch);

	if (nswitch != 0 && nswitch != 1)
	{
		err = ERROR_BADLOADSW;
	}
	else
	{	
		err = isis_load(filename, offset, &initpc);
		if (nswitch == 0)	/* Return to caller */
		{
			putresult(entry,  initpc);
			putresult(status, err);
		}
		else	/* Jump to new program. Rewind stack */
		{
/* Don't write the status back; it may now be overwriting the new program */
			sp[0] = 0xFE00;
			--sp[0];
			--sp[0];
			putresult(&RAM[sp[0]], initpc);
		}
	}
	if (trace > 1) printf(" initpc=%04x result=%d\n", initpc, err);

}



void isis_wraprescan(int param)
{
	int handle     = paramword(param, 0);
	byte *status   = &RAM[paramword(param, 1)];
	int err;

	err = isis_rescan(handle);	
	if (trace > 1)
	{
		printf("ISIS rescan: handle=%d [%s] result=%d\n",
			handle, isis_filename(handle), err);
	}
	putresult(status, err);
}



void isis_wrapread(int param)
{
	int handle     = paramword(param, 0);
	byte *data     = &RAM[paramword(param, 1)];
	int count      = paramword(param, 2);
	byte *actual   = &RAM[paramword(param, 3)];
	byte *status   = &RAM[paramword(param, 4)];

	int nactual = 0;
	int err;

	err = isis_read(handle, data, count, &nactual);
	if (trace > 1)
	{
		printf("ISIS read:   handle=%d [%s] address=0x%04x count=%d "
		" actual=%d result=%d\n",
			handle, isis_filename(handle), paramword(param, 1), 
			count, nactual, err);
		if (trace > 2) dumpbuffer(paramword(param, 1), data, nactual);
	}
	putresult(actual, nactual);
	putresult(status, err);
}



void isis_wrapwrite(int param)
{
	int handle     = paramword(param, 0);
	byte *data     = &RAM[paramword(param, 1)];
	int len        = paramword(param, 2);
	byte *status   = &RAM[paramword(param, 3)];
	int err;
	
	err = isis_write(handle, data, len);
	putresult(status, err);
	if (trace > 1)
	{
		printf("ISIS write:  handle=%d [%s] address=0x%04x count=%d "
			"result=%d\n",
			handle, isis_filename(handle), 
			paramword(param, 1), len, err);
		if (trace > 2) dumpbuffer(paramword(param,1), data, len);
	}
}


void isis_wrapseek(int param)
{
	int handle     = paramword(param, 0);
	int mode       = paramword(param, 1);
	byte *block    = &RAM[paramword(param, 2)];
	byte *bbyte    = &RAM[paramword(param, 3)];
	byte *status   = &RAM[paramword(param, 4)];
	int err = ERROR_SUCCESS;
	int nblocki, nbytei;
	int nblocko, nbyteo;
	long offseti, offseto;

	nblocki = block[0] + 256 * block[1];
	nbytei  = bbyte[0] + 256 * bbyte[1];
/* [Mark Ogden] nbytei can go above 127 */
	offseti = offseto = nbytei + (nblocki % 32768) * 128;
	err = isis_seek(handle, mode, &offseto);

	nbyteo  = offseto % 128;
	nblocko = offseto / 128;
	if (trace > 1)
	{
		printf("ISIS seek:   handle=%d [%s] mode=%d "
				"block=%04x [%d -> %d] "
				"byte=%04x [%d -> %d] offset=%ld -> %ld "
				"result=%04x\n",
			handle, isis_filename(handle), mode, 
			paramword(param, 2), nblocki, nblocko,
			paramword(param, 3), nbytei,  nbyteo,
			offseti, offseto, err);
	}
	putresult(bbyte, nbyteo);
	putresult(block, nblocko);
	putresult(status, err);
}


void isis_wraprename(int param)
{
	byte *status   = &RAM[paramword(param, 2)];
	int err = ERROR_SUCCESS;
	char oldname    [PATH_MAX + 1];
	char newname    [PATH_MAX + 1];

	get_isis_filename(paramword(param, 0), oldname);
	get_isis_filename(paramword(param, 1), newname);
	err = isis_rename(oldname, newname);

	if (trace > 1) 
	{
		printf("ISIS rename: from=%s to=%s result=%d\n",
				oldname, newname, err);
	}
	putresult(status, err);
}

void isis_wrapconsole(int param)
{
	byte *status   = &RAM[paramword(param, 2)];
	int err = ERROR_SUCCESS;
	char ciname    [PATH_MAX + 1];
	char coname    [PATH_MAX + 1];

	get_isis_filename(paramword(param, 0), ciname);
	get_isis_filename(paramword(param, 1), coname);
	err = isis_console(ciname, coname);

	if (trace > 1) 
	{
		printf("ISIS consol: conin=%s conout=%s result=%d\n",
				ciname, coname, err);
	}
	putresult(status, err);
}


void isis_wrapclose(int param)
{
	int handle     = paramword(param, 0);
	byte *status   = &RAM[paramword(param, 1)];
	int err;

	if (trace > 1)
	{
		printf("ISIS close:  handle=%d [%s] ",
			handle, isis_filename(handle));
	}
	err = isis_close(handle);
	if (trace > 1)
	{
		printf("result=%d\n", err);
	}
	putresult(status, err);
}

void isis_wrapdelete(int param)
{
	byte *status   = &RAM[paramword(param, 1)];
	int err = ERROR_SUCCESS;
	char name    [PATH_MAX + 1];

	get_isis_filename(paramword(param, 0), name);
	err = isis_delete(name);

	if (trace > 1) 
	{
		printf("ISIS delete: path=%s result=%d\n", name, err);
	}
	putresult(status, err);
}


void isis_wrapexit(int param)
{
	if (trace > 0) printf("Program termination.\n");
	thames_exit(appError); // [Mark Ogden] - exit with any detected errors
}

void isis_wrapattrib(int param)
{
	int attr  = paramword(param, 1);
	int value = paramword(param, 2);
	byte *status   = &RAM[paramword(param, 3)];
	int err;
	char name[PATH_MAX + 1];

	get_isis_filename(paramword(param, 0), name);

	err = isis_attrib(name, attr, value);
	if (trace > 1)
	{
		printf("ISIS attrib: path=%s attr=%d value=%d result=%d\n",
			name, attr, value, err);
	}
	putresult(status, err);
}




void isis_wraperror(int param, int sp)
{
	int pc;
	int err = paramword(param, 0);
	byte *status   = &RAM[paramword(param, 1)];
	char buf[80];

	pc = RAM[sp] + 256 * RAM[sp+1];
	sprintf(buf, "Error at PC=0x%04x", pc);
	isis_perror(buf, err);
	if (pc != 0)			// [Mark Ogden] record error occured (0->no error)
		appError = 1;
	putresult(status, ERROR_SUCCESS);
}

void isis_wrapwhocon(int param)
{	
	int handle     = paramword(param, 0);
	byte *buffer   = &RAM[paramword(param, 1)];

	char filename[PATH_MAX + 1];

	isis_whocon(handle, filename);
	filename[15] = 0;	/* Result is at most 15 bytes */
	filename[14] = 0;	/* The last one of which is a terminating sp */	
	strcat(filename, " ");
	strcpy((char *)buffer, filename);	

	if (trace > 1) printf("ISIS whocon: Device=%d result=%s\n", 
			handle, filename);
}


void isis_wrapspath(int param)
{
	byte *result   = &RAM[paramword(param, 1)];
	byte *status   = &RAM[paramword(param, 2)];
	int err;
	char name[PATH_MAX + 1];
	ISIS_STAT stat;

	get_isis_filename(paramword(param, 0), name);

	err = isis_spath(name, &stat);
	result[0] = stat.device;
	trim(stat.filename);
	trim(stat.ext);
	memcpy(result + 1, stat.filename, 6);
	memcpy(result + 7, stat.ext, 3);
	result[10] = stat.devtype;
	result[11] = stat.drivetype;
	putresult(status, err);
	if (trace > 1)
	{
		printf("ISIS spath:  path=%s array=%02x '%s.%s' %02x %02x result=%d\n",
			name, stat.device, stat.filename, stat.ext, 
			stat.devtype, stat.drivetype, err);

	}	
}


void isistrap(int func, int param, word *sp)
{
	/* For debugging only: Dump memory state at every syscall. 
	 * 
	 * printf("ISIS call: func=%02x param=%04x\n", func, param);
	 * dumpbuffer(0x3800, &RAM[0x3800], 0xbe00);
	 */
	switch(func)
	{
		case  0: isis_wrapopen(param); break;
		case  1: isis_wrapclose(param); break;
		case  2: isis_wrapdelete(param); break;
		case  3: isis_wrapread(param); break;
		case  4: isis_wrapwrite(param); break;
		case  5: isis_wrapseek(param); break;
		case  6: isis_wrapload(param, sp); break;
		case  7: isis_wraprename(param); break;
		case  8: isis_wrapconsole(param); break;
		case  9: isis_wrapexit(param); break; 
		case 10: isis_wrapattrib(param); break;
		case 11: isis_wraprescan(param); break;
		case 12: isis_wraperror(param, *sp); break;
		case 13: isis_wrapwhocon(param); break;
		case 14: isis_wrapspath(param); break;

		default:
			fprintf(stderr, "Unknown ISIS trap 0x%02x, "
				"DE=0x%04x\n", func, param);
			exit(1);
			break;
	} 
}
