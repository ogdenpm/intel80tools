#include "omf.h"
#include "rules.h"
#include "rulesInternal.h"

uint8_t const *rules96[] =
{
    /* 00 */  "\f$P INVALID($R): Length($L)" DMPDAT,
    /* 02 */  "\f$P MODHDR($R):"    FIXTOK "NB%tQ"  FIXSTR " $0 - $1 $2" 
                                    REPTOK "B%sWB%A"  REPSTR "\f4$3 [$1] $5",
    /* 04 */  "\f$P MODEND($R):"    FIXTOK "BB"    FIXSTR IF0("$0") "Main" ELSE0 "Other" END0 " " IF0("$1") "Erroneous" ELSE0 "Valid" END0,
    /* 06 */  "\f$P CONTENT($R):"   FIXTOK "B%qW"    FIXSTR " Seg: $0  Offset: $1" DMPDAT "$1",
    /* 08 */  "\f$P LINNUM($R):"    FIXTOK "B%q"   FIXSTR "Seg: $0"  REPTOK "WW%u" REPSTR "\f4$1 #$2",
    /* 0A */  "\f$P BLKDEF($R):"    DMPDAT,
    /* 0C */  "\f$P BLKEND($R):"    FIXTOK,
    /* 0E */  "\f$P EOF($R):"       FIXTOK,
    /* 10 */  "\f$P ANCESTOR($R):"  FIXTOK "N"        FIXSTR " $0",
    /* 12 */  "\f$P LOCALS($R):"    FIXTOK "B%q"      FIXSTR " Seg: $0"
                                    REPTOK "WNI"      REPSTR "\f2$1 Type($3) $2",
    /* 14 */  "\f$P TYPDEF($R):"    DMPDAT,
    /* 16 */  "\f$P PUBLICS($R):"   FIXTOK "B%q"      FIXSTR " Seg: $0"
                                    REPTOK "WNB"      REPSTR "\f3$1 Type($3) $2",
    /* 18 */  "\f$P EXTERN($R):"    FIXTOK "B%q"     FIXSTR " Seg: $0"
                                    REPTOK "Ne #eB"   REPSTR "\f4$2\t3 Type($3)\t12 $1",      // note use of # to get External Index
    /* 1A */  NULL,
    /* 1C */  NULL,
    /* 1E */  NULL,
    /* 20 */  "\f$P SEGDEF($R):"    REPTOK "B%q" IF0("$0&0x80") "B%Q" ELSE0 "W" END0 "W"
                                    REPSTR  "\f1Seg: $0\t23 Size: $2 " IF0("$0&0x80") "Align: $1" ELSE0 "Base: $1" END0,
    /* 22 */  "\f$P FIXUP($R):"     DMPDAT,
    /* 24 */  NULL,
    /* 26 */  "\f$P LIBLOC($R):"                      FIXSTR  " Block:Byte"
                                REPTOK "BB"       REPSTR  "\f8$0:$1",
    /* 28 */  "\f$P LIBNAM($R):"    REPTOK "N"        REPSTR  "\f3$0",
    /* 2A */  "\f$P LIBDIC($R):"    REPTOK "N%z"       REPSTR  "\f3$0",
    /* 2C */  NULL,
    /* 2E */  "\f$P LIBHDR($R):"    FIXTOK "W%uWW"    FIXSTR  " $0 Modules Dictionary at $1:$2",
};


char const *enumAlign96[4] = { "Align", "BYTE", "WORD", "LONG"};

void init96() {
    resetStrings();
    omfVer = OMF96;
    rules = rules96;
    minRecType = 2;
    maxRecType = 0x2E;
}

bool isTrn96(uint8_t trn) {
    return strchr("\x04\x06\x20\x24\x26\x40\x44\x46\xe0\xe4\xe6", trn & 0xfe) != NULL;
}


void fmtTrn96(var_t *pvar) {
    char const *s;
    if (isTrn96(pvar->ival)) {
        switch (pvar->ival & 0xe1) {
        case 0: s = "ASM96"; break;
        case 1: s = "ASM96|RL96"; break;
        case 0x20: s = "PL/M-96"; break;
        case 0x21: s = "PL/M-96|RL96"; break;
        case 0x40: s = "C96"; break;
        case 0x41: s = "C96|RL96"; break;
        case 0xe0: s = "TRANSLATOR96"; break;
        case 0xe1: s = "RL96"; break;
        }
        strcpy(pvar->sval, s);
        switch (pvar->ival & 0x1e) {
        case 0: s = "OMFV96V14"; break;
        case 4: s = "OMFV96V20"; break;
        case 6: s = "OMFV96V30"; break;
        }
        strcat(pvar->sval, s);
    }
}

void fmtSegId96(var_t *pvar) {
    static char const *segType[] = { "SEG_NULL", "CODE", "DATA", "STACK", "REGISTER", "OVERLAY", "DYNAMIC", "???" };
    strcpy(pvar->sval, pvar->ival & 0x80 ? "Relocatable" : "Absolute");
    strcat(pvar->sval, pvar->ival & 0x40 ? " Based " : " NonBased ");
    strcat(pvar->sval, segType[pvar->ival & 7]);
}