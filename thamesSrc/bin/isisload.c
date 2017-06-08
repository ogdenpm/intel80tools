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

/* Implementation of the isis_load() call: Load an ISIS executable image */

typedef struct loader_state
{
    char filename[1 + PATH_MAX];	/* Image file pathname (Unix) */
    FILE *fp;			/* Image file handle */
    long blockpos;			/* Base address of current block */
    int blocktype;			/* Type of current block */
    int blocklen;			/* Bytes remaining in current block */
    int initpc_result;		/* Initial program counter */
    int initpc_state;		/* Status of 'initial PC' state mch */
    int lowest_addr;		/* Lowest address used */
    int highest_addr;		/* Highest address used */
    int loadblock_state;		/* Status of 'load block' state mch */
    int loadblock_addr;		/* Load address of next byte */
    int loadblock_base;		/* Base load address */
    int trace;			/* Detailed errors? */
} LOADER_STATE;

void clear_loader_state(LOADER_STATE *self)
{
    memset(self, 0, sizeof(*self));
    self->fp = NULL;
    self->initpc_result = -1;
    self->initpc_state  = 0;
    self->lowest_addr = -1;
    self->highest_addr = -1;
    self->loadblock_state = 0;
    self->loadblock_addr = 0;
    self->loadblock_base = 0;
    self->blocktype = -1;
    self->blocklen = 0;
    self->trace = trace;
}


int readword(FILE *fp)
{
    int ch1, ch2;

    ch1 = fgetc(fp);
    if (ch1 == EOF) return -1;
    ch2 = fgetc(fp);
    if (ch2 == EOF) return -1;

    return (ch2 << 8) | (ch1 & 0xFF);
}

typedef int (*LOADBYTEFUNC)(LOADER_STATE *state, int ch);

int dropbyte(LOADER_STATE *state, int ch)
{
    return ERROR_SUCCESS;
}


static int initpc(LOADER_STATE *state, int ch)
{
    switch(state->initpc_state)
    {
        case 0:	// Subtype
            if (ch == 1) state->initpc_state = 1;
            else	     state->initpc_state = 5;
            break;
        case 1:	// Ignored
            ++state->initpc_state;
            break;
        case 2: // Low byte
            ++state->initpc_state;
            state->initpc_result = ch;
            break;
        case 3: // High byte
            ++state->initpc_state;
            state->initpc_result |= (ch << 8);
            break;
    }
    return ERROR_SUCCESS;
}


int loadblock(LOADER_STATE *state, int ch)
{
    int pos;

    switch (state->loadblock_state)
    {
        case 0:
        if (ch != 0)
        {
            if (state->trace) fprintf(stdout, 
                "Load error: Unsupported block 6 subtype %d", ch);
            return ERROR_BADIMAGE;
        }
        ++state->loadblock_state;
        return ERROR_SUCCESS;

        case 1:
        state->loadblock_addr = ch;
        ++state->loadblock_state;
        return ERROR_SUCCESS;
    
        case 2:
        state->loadblock_addr |= (ch << 8);
        ++state->loadblock_state;
        if (state->trace > 2)
        {
            fprintf(stdout, "Load block at offset %04x\n", 
                state->loadblock_addr);

        }
        return ERROR_SUCCESS;

        case 3:
        pos = state->loadblock_addr + state->loadblock_base;

        if (state->lowest_addr < 0  || state->lowest_addr > pos) 
            state->lowest_addr = pos;	
        if (state->highest_addr < 0 || state->highest_addr < pos) 
            state->highest_addr = pos;	
        /* isis overwrite changed to allow pascal80 - note isis io buffers ignored as thames doesn't use them */
        if (pos < 0x18 || (0x40 <= pos && pos < 0x3000) || pos > 0xF800)	/* Overwriting ISIS */
        {
            if (state->trace) fprintf(stdout, 
                "Attempt to overwrite ISIS at 0x%04x", pos);
            return ERROR_OVERWRITING;	
        }
        RAM[pos] = ch;
        ++state->loadblock_addr;
        return ERROR_SUCCESS;
    }
    return ERROR_DISKERROR;	/* Should not get here */
}




int isis_readblock(LOADER_STATE *state, LOADBYTEFUNC func)
{
    byte cksum = 0;
    int err;

    cksum = (state->blocktype & 0xFF) + 
        (state->blocklen  & 0xFF) + 
        (state->blocklen >> 8);
    while (state->blocklen)
    {
        int c = fgetc(state->fp);
        if (c == EOF)
        {
            if (state->trace) fprintf(stdout, 
                "%s: Unexpected EOF in block at 0x%lx\n", 
                state->filename, state->blockpos);
            return ERROR_BADIMAGE;
        }
/* If blocklen is 1, this is the checksum byte -- don't pass it through to 
  the callback */
        if (state->blocklen > 1)
        {
            err = func(state, c);
        }
        else err = ERROR_SUCCESS;
        if (err)
        {
            if (state->trace) fprintf(stdout, 
                "%s load failed in block at 0x%lx\n", 
                state->filename, state->blockpos);
            return err;
        }
        cksum += (c & 0xFF); 	
        --state->blocklen;
    }
    if (cksum != 0)
    {
        if (state->trace) fprintf(stdout, "%s: Bad checksum in "
            "block at 0x%lx\n", state->filename, state->blockpos);
        return ERROR_BADIMAGE;
    }
    return ERROR_SUCCESS;
}


int isis_load(const char *filename, int base, int *pinitpc)
{
    LOADER_STATE state;
    int err;

    clear_loader_state(&state);
    state.loadblock_base = base;
    err = isis_name2unix(filename, state.filename);
    if (err) return err;

    state.fp = fopen(state.filename, "rb");
    if (!state.fp) return ERROR_FILENOTFOUND;
    while (1)
    {
        state.blockpos  = ftell(state.fp);
        state.blocktype = fgetc(state.fp);
        state.blocklen = readword(state.fp);

        if (state.blocktype == EOF || state.blocklen < 0)
        {
            fprintf(stdout, "%s: Unexpected EOF in block header"
                " at 0x%lx\n", state.filename, 
                state.blockpos);
            fclose(state.fp);
            return ERROR_BADIMAGE;
        }
//		printf("Block type=%d length=%d\n", blocktype, blocklen);
        switch(state.blocktype)
        {
/* Skip block */
            case 2:		/* Module header */
            case 0x08:	/* line numbers */
			case 0x10:	/* ancestor */
            case 0x12:	/* local symbols */
            case 0x16:	/* public declarations */
				/* skip them */
                err = isis_readblock(&state, dropbyte);
                if (err)	
                {
                    fclose(state.fp);
                    return err;
                }	
                break;

            case 4:
                state.initpc_state = 0;
                state.initpc_result = -1;
                err = isis_readblock(&state, initpc);
                if (err)
                {
                    fclose(state.fp);
                    return err;
                }
                *pinitpc = state.initpc_result;
//				printf("Start address %04x\n", *pinitpc);
                break;	
            case 6:
                state.loadblock_state = 0;
                err = isis_readblock(&state, loadblock);
                if (err)
                {
                    fclose(state.fp);
                    return err;
                }	
                break;
            case 14:	/* EOF */
                err = isis_readblock(&state, dropbyte);
                if (err)
                {
                    fclose(state.fp);
                    return err;
                }
/* If no start address is given, start at the lowest address loaded */
                if (*pinitpc <= 0) 
                {
                    *pinitpc = state.lowest_addr;
                }
                if (state.trace > 1) printf("Memory used "
                    "from %04x to %04x\n",
                    state.lowest_addr, state.highest_addr + 2);
                if (state.trace > 2)
                {
                    dumpbuffer(state.lowest_addr,
                        &RAM[state.lowest_addr],
                        state.highest_addr - state.lowest_addr + 3);
                }
                fclose(state.fp);
                return ERROR_SUCCESS;
			/* the following are valid omf records but not in a loadable module */
			case 0x18:	/* external names */
			case 0x20:	/* external references */
			case 0x22:	/* relocation */
			case 0x24:	/* inter segment reference */
			case 0x26:	/* library module locations */
			case 0x28:	/* library module names */
			case 0x2A:	/* library dictionary */
			case 0x2C:	/* library header */
				if (state.trace) fprintf(stdout, "%s: illegal use of block type %d"
					" at 0x%lx\n", state.filename, state.blocktype,
					state.blockpos);
				fclose(state.fp);
				return ERROR_BADIMAGE;
            default:
            if (state.trace) fprintf(stdout, "%s: Unsupported block type %d"
                " at 0x%lx\n", state.filename, state.blocktype, 
                state.blockpos);
            fclose(state.fp);
            return ERROR_BADIMAGE;
        }
    }

    /* Should never get here */
    return ERROR_SUCCESS;
}




