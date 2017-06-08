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
#ifdef _WIN32
#include <conio.h>
#endif

/* Global variables */

char *progname;
char **argv;
int argc;
int trace = 0;

byte RAM[65536]; /* The Z80's address space */



void dump_regs(FILE *fp, byte a, byte b, byte c, byte d, byte e, byte f,
             byte h, byte l, word pc, word ix, word iy)
{
    fprintf(fp, "\tAF=%02x%02x BC=%02x%02x DE=%02x%02x HL=%02x%02x\n"
                    "\tIX=%04x IY=%04x PC=%04x\n",
            a,f,b,c,d,e,h,l,pc,ix,iy);
}



char *parse_to_fcb(char *s, int afcb)
{
    byte *fcb = &RAM[afcb+1];

    RAM[afcb] = 0;
    memset(fcb, ' ', 11);

    while (1)
    {
        if (s[0] == 0) break;	
        if (s[0] == ' ') {++s; continue; }
        if (s[1] == ':')
        {
            RAM[afcb] = s[0] - '@';
            if (RAM[afcb] > 16) RAM[afcb] -= 0x20;
            s+=2;
            continue;
        }
        if (s[0] == '.')
        {
            ++s;
            fcb = &RAM[afcb+9];
            continue;
        }
        *fcb = *s;  if (islower(*fcb)) *fcb = toupper(*fcb);
        ++s;
        ++fcb;	
        if (fcb >= &RAM[afcb+12]) break;
    }
    return s;
}


void message(const char *s, ...)
{
#ifdef DEBUG
    va_list ap;

        va_start(ap, s);
    printf("%s trace: ", progname);
    vprintf(s, ap);
    fflush(stdout);
    if (s[strlen(s) - 1] == '\n') putchar('\r');
    va_end(ap);
#endif
}

void mdsbios(int func, byte *a, byte *b, byte *c, byte *d, byte *e,
            byte *h, byte *l)
{
    static byte iobyte = 0;
	static byte warned;

    switch(func)
    {
/* MDS BIOS traps implemented by ISX but not by THAMES are: */
        /* 6: LIST */
        /* 7: CONST */
	    case 7:
			if (_isatty(0))
				*a = _kbhit() ? 0xff : 0;
			else
				*a = 0xff;
			break;
        case 2:		/* CONIN */
			if (_isatty(0) && !(*a = _getch()))
				*a = _getch();
			else
				*a = getchar();
				break;
        case 4:		/* CONOUT */
            putchar(*c);
            break;

        case 8:		/* Set IOBYTE */
            RAM[6] = *a;
            break;
    
        case 9:		/* Get IOBYTE */
            *a = RAM[6];
            break;

        case 10:	/* Get top of RAM = F7FFh */
            *b = 0xF7;
            *a = 0xFF;
            if (trace > 1) printf("ISIS himem:  0x%02x%02x\n",
                *b, *a);
            break;

        default:
            fprintf(stderr, "Unsupported MDS BIOS trap %d\n", func);
            thames_exit(1);
    }

}


void ed_fe(byte *a, byte *b, byte *c, byte *d, byte *e, byte *f,
             byte *h, byte *l, word *pc, word *ix, word *iy, word *sp)
{
    switch(*a)
    {
        case 0xC4:
        isistrap(*c, (*d) * 256 + (*e), sp);
        break;

        case 0xC5:
        mdsbios(RAM[0xf990] / 3,  a, b, c, d, e, h, l);
        break;

        default:
        fprintf(stderr, "%s: Z80 encountered invalid trap\n", progname);
        dump_regs(stderr,*a,*b,*c,*d,*e,*f,*h,*l,*ix, *iy, *pc);
        thames_exit(1);

    }
}



unsigned int in() { return 0; }
unsigned int out() { return 0; }


/* main() loads an ISIS program and runs it. argv[1] is the name of the ISIS
  program to load; the remaining arguments are arguments for the ISIS program.

  main() also loads the vestigial MDS800 BIOS and does some sanity checks 
         on the endianness of the host CPU and the sizes of data types.
 */

int main(int ac, char **av)
{
    int n, lencmd, isisProgArg;		// [Mark Ogden]
    int pc;
    char *str;
    char isisProg[15];				// [Mark Ogden]

    argc = ac;
    argv = av;
#ifdef __PACIFIC__ 		/* Pacific C doesn't support argv[0] */
    progname = "THAMES";
#endif
    progname = argv[0];

    str = getenv("ISIS_TRACE");
    if (str) trace = atoi(str);
    if (trace > 3) fptrace = stdout;

    /* DJGPP includes the whole path in the program name, which looks
         * untidy...
         */
    str = strrchr(progname, '/');
    if (str) progname = str + 1;

    if (sizeof(int) > 8 || sizeof(byte) != 1 || sizeof(word) != 2)
    {
        fprintf(stderr, "%s: type lengths incorrect; edit typedefs "
            "and recompile.\n", progname);
        thames_exit(1);
    }

    if (isis_open_stdio())
    {
        fprintf(stderr, "%s: Could not set up ISIS console I/O\n",
            progname);
        thames_exit(1);
    }

    if (argc < 2)
    {
        fprintf(stderr, "%s: No ISIS program name provided.\n", progname);
        thames_exit(1);
    }

    /* [Mark Ogden] parse options returning index of isis program name */
    isisProgArg = parseOptions(argc, argv);
    /*Build an ISIS command line from our arguments */
    lencmd = 0;
    for (n = isisProgArg; n < argc; n++)
    {
        lencmd += 1 + strlen(argv[n]);
    }
    conin->buffer = new_buffer(lencmd + 3);
    if (!conin->buffer)
    {
        fprintf(stderr, "%s: No memory for command line\n", progname);
        thames_exit(1);
    }
    /* Like a UNIX command line, an ISIS one contains a program name. By default,
     * the file pointer of stdin is left pointing just after it, but RESCAN
     * rewinds the pointer to the start of the line. Clever. */
    for (n = isisProgArg; n < argc; n++)
    {
        strcat(conin->buffer->data, argv[n]);
        strcat(conin->buffer->data, " ");
    }
    trim(conin->buffer->data);

    /* [Mark Ogden] intercept the command line and if -m option then map file names to isis format
     * allocating drives dynamically.
     * Also initialises a cache of the drive to path mapping for :Fx:
     */
    unix2Isis();

    /* [Mark Ogden] calculate offset of first arg after application name - replaces npos */
    conin->buffer->pos = (str = strchr(conin->buffer->data, ' ')) ? str - conin->buffer->data + 1: strlen(conin->buffer->data);
    strcat(conin->buffer->data, "\r\n");
    conin->buffer->len = strlen(conin->buffer->data);

//	capitals(conin->buffer->data);			// [Mark Ogden] commented out to allow lower case chars

    if (mOption)	// [Mark Ogden] write the transformed command line
        fputs(conin->buffer->data, stdout);

/* This info is very useful when creating a client like zxc or zxas */
    if (trace)
    {
        printf("Command line is:\n");
        dumpbuffer(0, (byte *)conin->buffer->data, strlen(conin->buffer->data));
    }

/* Set up MDS800 jumpblock -- all entries are CALL F980 */
    for (n = 0; n < 42; n++)
    {
        RAM[0xF800 + 3*n] = 0xCD;
        RAM[0xF801 + 3*n] = 0x80;
        RAM[0xF802 + 3*n] = 0xF9;
    }
	/* modified to use only 8080 instructions */
    RAM[0xF980] = 0xE3;	/* XTHL - get callee */
    RAM[0xF981] = 0x22; /* SHLD 0F990h - save callee */
    RAM[0xF982] = 0x90;	
    RAM[0xF983] = 0xF9;
    RAM[0xF984] = 0xE1;	/* POP H - restore hl */
    RAM[0xF985] = 0x3E;	/* LD A, 0C5h */	
    RAM[0xF986] = 0xC5;	
    RAM[0xF987] = 0xED;	/* EDFE (trap to THAMES) */
    RAM[0xF988] = 0xFE;
    RAM[0xF989] = 0xC9;	/* RET */

    RAM[0x40] = 0xC3;
    RAM[0x41] = 0xF9;
    RAM[0x42] = 0xF9;	/* ISIS entry: JP F9F9 */

    RAM[0xF9F9] = 0x3E;	/* At F9F9: LD A, 0C4h */
    RAM[0xF9FA] = 0xC4;	
    RAM[0xF9FB] = 0xED;	/* EDFE (trap to THAMES) */
    RAM[0xF9FC] = 0xFE;
    RAM[0xF9FD] = 0xC9;	/* RET */

/* Load program */
    if (mOption)	// [Mark Ogden] get converted ISIS prog name :Fx:name12.ext
    {
        for (n = 0, str = conin->buffer->data; n < 14 && (isalnum(*str) || *str == ':' || *str == '.'); n++)
            isisProg[n] = *str++;
        isisProg[n] = 0;
    }

    n = isis_load(mOption ? isisProg : argv[isisProgArg], 0, &pc);
    if (n != ERROR_SUCCESS)
    {
        isis_perror(mOption ? isisProg : argv[isisProgArg], n);
        thames_exit(1);
    }    /* Start the Z80 at pc, with stack at 0xFE00 */
    if (pc < 0)
    {
        fprintf(stderr, "No start address supplied\n");
        thames_exit(1);
    }
    mainloop(pc, 0xFE00);
    return appError;	// [Mark Ogden] exit with any detected error
}

void thames_exit(int code)
{
    exit(code);
}

