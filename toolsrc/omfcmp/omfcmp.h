#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef unsigned char byte;
typedef unsigned short word;

#include "util.h"
#include "file.h"
#include "omf.h"
#include "library.h"


#define MAXNAME	31
#define INITIALCHUNK 10

enum {
    MODHDR = 0x2,
    MODEND = 0x4,
    CONTENT = 0x6,
    LINNUM = 0x8,
    EOFREC = 0xE,
    PARENT = 0x10,
    LOCNAM = 0x12,
    PUBNAM = 0x16,
    EXTNAM = 0x18,
    EXTREF = 0x20,
    RELOC = 0x22,
    INTSEG = 0x24,
    LIBHDR = 0x2C,
    LIBNAM = 0x28,
    LIBLOC = 0x26,
    LIBDIC = 0x2a,
    COMMON = 0x2e,
};

extern int returnCode;
void diffBinary(omf_t *left, omf_t *right);
void cmpModule(omf_t *lomf, omf_t *romf);
void usage(char *fmt, ...);