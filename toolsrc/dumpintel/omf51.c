#include "omf.h"
#include "rules.h"
#include "rulesInternal.h"

uint8_t const *rules51[] =
{
/* 00 */  "\f^P INVALID($R): Length(^L)" DMPDAT,
          
/* 02 */  "\f^P MODHDR($R):"    FIXTOK "NB%t B"                 FIXSTR " ^0 - ^1",
/* 04 */  "\f^P MODEND($R):"    FIXTOK "NWBBm"                  FIXSTR "^0" IF0("^2") " REGMSK ^2" END0,
/* 06 */  "\f^P CONTENT($R):"   FIXTOK "B%s W"                  FIXSTR " Seg ^0" DMPDAT "^1",
/* 08 */  "\f^P FIXUP($R):"     REPTOK "WB%F BB" IF0("$2<2") "%s" ELSIF0("$2=2") "%e" END0 "W"
                              REPSTR "\f2$0 $1\t17 OFFSET=$4 BASE=" IF0("$2=1") "$3.PSEG" ELSIF0("$2<2") "$3" ELSE0 "#$3" END0,
/* 0A */  NULL,
/* 0C */  NULL,
/* 0E */  "\f^P SEGDEF($R):"                                     FIXSTR "\f1SEGID BASE:SIZE NAME\t51 SEGDEF\t72RELTYP\n"
                            REPTOK "B%u B%i B%R BWWNs0"     REPSTR WIDE($0,-5) " $3:$4" IF0("$0") "$0" ELSE0 "**ABS**" END0 "\t48 $1\t63$2",
/* 10 */  "\f^P SCOPEDEF($R):"  FIXTOK  "B%S N"                 FIXSTR " ^1:^0",
/* 12 */  "\f^P DBGINF($R):"    FIXTOK "B"
                              IF0("^0=0")    FIXSTR " LOCALS\f1NAME\t32 SYMINFO\t53 SEG:OFFSET"      REPTOK "B%s B%y BN" REPSTR "\f1$5\t32 $2\t53 $1:$3"
                              ELSIF0("^0=1") FIXSTR " PUBLICS\f1NAME\t32 SYMINFO\t53 SEG:OFFSET"     REPTOK "B%s B%y BN" REPSTR "\f1$5\t32 $2\t53 $1:$3"
                              ELSIF0("^1=2") FIXSTR " SEGMENTS\f1NAME\t32 SEGINFO\t47 SEG:OFFSET"    REPTOK "B%s B%y BN" REPSTR "\f1$5\t32 $2\t47 $1:$3"
                              ELSIF0("^1=3") FIXSTR " LINNUMS\f2LINE  SEG:OFFSET\f2LINE  SEG:OFFSET" REPTOK "B%s WW%u"   REPSTR "\f2" WIDE($2,6) "$1:$3"
                              ELSE0 DMPDAT
                              END0,
          
/* 14 */  NULL,
/* 16 */  "\f^P PUBLICS($R):"                                   FIXSTR "\f1NAME\t32SEG:OFFSET\t69SYMINFO"
                            REPTOK "B%s B%y WBN"            REPSTR "\f1$4\t32$0:$2\t69$1",
/* 18 */  "\f^P EXTERN($R):"                                    FIXSTR "\f1EXTID SYMINFO\t37NAME"
                            REPTOK "BB B%y BNe1"            REPTOK "\f1$1\t5 $2\t37 $4",
/* 1A */  NULL,
/* 1C */  NULL,
/* 1E */  NULL,
/* 20 */  NULL,
/* 22 */  NULL,
/* 24 */  NULL,
/* 26 */  "\f^P LIBLOC($R):"                                    FIXSTR  " Block:Byte"
                            REPTOK "BB"                     REPSTR  "\f8$0:$1",
/* 28 */  "\f^P LIBNAM($R):"    REPTOK "N"                      REPSTR  "\f3$0",
/* 2A */  "\f^P LIBDIC($R):"    REPTOK "N%z"                     REPSTR  "\f3$0",
/* 2C */  "\f^P LIBHDR($R):"    FIXTOK "W%uWW"                  FIXSTR  " ^0 Modules Dictionary at ^1:^2",
};

char const *enumRelType51[7] = { "RefType", "ABS", "UNIT", "BITADDRESSABLE", "INPAGE", "INBLOCK", "PAGE" };
char const *enumRefType51[9] = { "Scope", "LOW", "BYTE", "RELATIVE", "HIGH", "WORD", "INBLOCK", "BIT", "CONV" };
char const *enumScope51[7] = { "RelType", "MODULE",  "DO", "PROCEDURE", "MODULE END", "DO END", "PROCEDURE END" };
char const *enumUsage51[7] = { "Usage", "CODE", "XDATA", "DATA", "IDATA", "BIT", "NUMBER" };
char const *enumSegType51[6] = { "SegType", "CODE", "XDATA", "DATA", "IDATA", "BIT" };

void init51() {
    resetStrings();

    index_t *p = initIndex('s', "SEG", 1, 256);
    p->names[0] = "ABS";
    p = initIndex('e', "EXT", 0, 256);
    omfVer = OMF85;
    rules = rules51;
    minRecType = 2;
    maxRecType = 0x2C;
}

bool isTrn51(uint8_t trn) {
    return 0xfd <= trn && trn <= 0xff;
}

void fmtTrn51(var_t *pvar) {
    static char *trn[] = { "ASM51", "PL/M-51", "RL51" };

    if (isTrn51(pvar->ival))
        strcpy(pvar->sval, trn[pvar->ival - 0xfd]);
}

void fmtRegMask51(var_t *pvar) {
    if (pvar->ival == 0)
        strcpy(pvar->sval, "None");
    else {
        uint8_t *s = pvar->sval;
        *s = '#';
        if (pvar->ival & 1) *s++ = '0';
        if (pvar->ival & 2) *s++ = '1';
        if (pvar->ival & 4) *s++ = '2';
        if (pvar->ival & 8) *s++ = '4';
        *s = 0;
    }
}

void fmtSegInfo51(var_t *pvar) {
    fmtEnum(1, pvar->ival & 7, pvar->sval);
    if (pvar->ival & 0x80)
        strcpy(pad(pvar->sval, 7), "EMPTY");
    else if (pvar->ival & 0x20)
        sprintf(pad(pvar->sval, 7), "OVL #%u", (pvar->ival >> 4) & 3);
}

void fmtSymInfo51(var_t *pvar) {
    fmtEnum(2, pvar->ival & 7, pvar->sval);
    if (pvar->ival & 0x40) // variable
        strcpy(pad(pvar->sval, 7), "VAR ");
    else {
        strcpy(pad(pvar->sval, 7), "PROC ");
        if (pvar->ival & 0x80)
            strcat(pvar->sval, "IND ");
        if (pvar->ival & 20)
            sprintf(strchr(pvar->sval, 0), "#%u", (pvar->ival >> 4) & 3);
        else
            strcat(pvar->sval, "#0123");
    }
}
