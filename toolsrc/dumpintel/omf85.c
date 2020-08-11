#pragma once
#include "omf.h"
#include "rules.h"
#include "rulesInternal.h"

uint8_t const *rules85[] =
{
/* 00 */  "\f$P INVALID($R): Length($L)" DMPDAT,
/* 02 */  "\f$P MODHDR($R):"    FIXTOK "NB%tB%v"  FIXSTR " $0 - $1" IF0("$2") " v$2" END0
                            REPTOK "B%sWB%A"  REPSTR "\f4$3 [$4] $5",
/* 04 */  "\f$P MODEND($R):"    FIXTOK "BB%sW"    FIXSTR IF0("$0") " ENTRY $1:$2" END0,
/* 06 */  "\f$P CONTENT($R):"   FIXTOK "B%s W"    FIXSTR " Seg $0" DMPDAT "$1",
/* 08 */  "\f$P LINNUM($R):"    FIXTOK "B%s"      FIXSTR " Seg $0"
                            REPTOK "WW%u"     REPSTR "\f7$1 #$2",
/* 0A */  NULL,             
/* 0C */  NULL,             
/* 0E */  "\f$P EOF($R):",      
/* 10 */  "\f$P ANCESTOR($R):"  FIXTOK "N"        FIXSTR " $0",
/* 12 */  "\f$P LOCALS($R):"    FIXTOK "B%s"      FIXSTR " Seg $0"
                            REPTOK "WNB"      REPSTR "\f4$1 $2",
/* 14 */  NULL,             
/* 16 */  "\f$P PUBLICS($R):"   FIXTOK "B%s"      FIXSTR " Seg $0"
                            REPTOK "WNB"      REPSTR "\f4$1 $2",
/* 18 */  "\f$P EXTERN($R):"    REPTOK "Ne #eB"   REPSTR "\f4#$1\t5 $0",      // note use of # to get External Index
/* 1A */  NULL,             
/* 1C */  NULL,             
/* 1E */  NULL,             
/* 20 */  "\f$P EXTFIX($R):"    FIXTOK "B%F"      FIXSTR  " Type $0"
                            REPTOK "W%eW"     REPSTR  "\f4$2 $1",
/* 22 */  "\f$P FIXUP($R):"     FIXTOK "B%F"      FIXSTR  " Type $0"
                            REPTOK "W"        REPSTR  "\f8$1",        
/* 24 */  "\f$P SEGFIX($R):"    FIXTOK "B%sB%A"   FIXSTR  " $0 $1"
                            REPTOK "W"        REPSTR  "\f8$2",
/* 26 */  "\f$P LIBLOC($R):"                      FIXSTR  " Block:Byte"
                            REPTOK "BB"       REPSTR  "\f8$0:$1",
/* 28 */  "\f$P LIBNAM($R):"    REPTOK "N"        REPSTR  "\f3$0",
/* 2A */  "\f$P LIBDIC($R):"    REPTOK "N%z"       REPSTR  "\f3$0",
/* 2C */  "\f$P LIBHDR($R):"    FIXTOK "W%uWW"    FIXSTR  " $0 Modules Dictionary at $1:$2",
/* 2E */  "\f$P COMMON($R):"    REPTOK "B%uNs0"   REPSTR  "\f3" WIDE($0,-3) " $1"
};

char const *enumAlign85[5] = { "Align", "ABSOLUTE", "INPAGE", "PAGE", "BYTE" };
char const *enumFixup85[5] = { "Fixup", NULL, "LOW", "HIGH", "BOTH" };




void init85() {
    resetStrings();

    index_t *p = initIndex('s', "SEG", 256, 256);
    p->names[0] = "ABSOLUTE";
    p->names[1] = "CODE";
    p->names[2] = "DATA";
    p->names[3] = "STACK";
    p->names[4] = "MEMORY";
    p->names[5] = "RESERVED";
    p->names[255] = "//";
    p = initIndex('e', "EXT", 0, INDEXCHUNK);

    omfVer = OMF51;
    rules = rules85;
    minRecType = 2;
    maxRecType = 0x2E;
}

bool isTrn85(uint8_t trn) {
    return trn < 3;
}


void fmtTrn85(var_t *pvar) {
    char const *trn[] = {"UKN80", "PL/M-80", "FORT80"};
    if (isTrn85(pvar->ival))
        strcpy(pvar->sval, trn[pvar->ival]);
}
