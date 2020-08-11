#include "omf.h"
#include "rules.h"
#include "rulesInternal.h"

uint8_t const *rules86[] =
{
        "\f$P INVALID($R): Length($L)"       DMPDAT,
    /*6E*/  "\f$P RHEADR($R):"                FIXTOK "N B%MW%uW%uW%uD DDDD"          FIXSTR " '$0' - $1  $2 Segments  $3 Groups  $4 Overlays"
                                                                                            IF0("$4") " [Offset $5]" END0
                                                                                            "\f1Static Size $6 [Max $7]  Dynamic Size $8 [Max $9]",
    /*70*/  "\f$P REGINT($R):"                REPTOK "B" IF0("$0&1") "F"  ELSIF0("$0&0x81=0") "LW" ELSE0 "L" END0
                                                              FIXSTR "\f2" IF0("$0&0xc0=0") "CS:IP"       ELSIF0("$0&0xc0=0x40") "SS:SP"
                                                                           ELSIF0("$0&0xc0=0x80") "DS   " ELSE0  "ES   " END0 " = $1"
                                                                           IF0("$0&0x81=0") ":$2" END0,
    /*72*/  "\f$P REDATA($R):"                FIXTOK "LW"         FIXSTR " $0"  DMPDAT "$1",
    /*74*/  "\f$P RIDATA($R):"                FIXTOK "LW"         FIXSTR " $0:$1" DMPIDAT "$1",
    /*76*/  "\f$P OVLDEF($R):"                FIXTOK "No#oD%pB" IF0("$3&2") "Io" ELSE0 "#" END0 IF0("$3&1") "Io" END0
                                                              FIXSTR   " #$1\t22 At: $2 '$0'"
                                                                       IF0("$3&2") "\t53 Shared Overlay: $3" END0
                                                                       IF0("$3&1") "\t53 Adjacent Overlay: $4" END0,

    /*78*/  "\f$P ENDREC($R):"                FIXTOK "B"          FIXSTR " " IF0("$0=0") "Overlay" ELSIF0("$0=1") "Block" ELSE0 "Illegal" END0,
    /*7A*/  "\f$P BLKDEF($R):"                FIXTOK "LNb#bWWB" IF0("$5&0x80") "W" ELSE0 "#" END0 IF0("$1") "I" END0
                                                              FIXSTR " #" WIDE($2, -2) " '$1'  $0  start: $3  Length: $4"
                                                                    IF0("$5&0x80") IF1("$5&0x40") " FAR PROC" ELSE1 " NEAR PROC" END1 "  Return Offset: $6" END0
                                                                    IF0("$1") " Type: @$7" END0,
    /*7C*/  "\f$P BLKEND($R):"                FIXTOK,
    /*7E*/  "\f$P DEBSYM($R):"                FIXTOK "B" IF0("$0&7=0") "L" END1
                                                     ELSIF0("$0&7=1") "I%e"
                                                     ELSIF0("$0&7=2") "I%b"
                                                     END0
                                                              FIXSTR IF0("$0&7=0")   " $1"
                                                                     ELSIF0("$0&7=1") " Ext: '$1'"
                                                                     ELSIF0("$0&7=2") " Blk: '$1'"
                                                                     ELSE0 " ERROR" END0
                                                                     IF0("$1&0xc0=0x80") " 16bit Indirect" ELSIF0("$1&0xc0=0xc0") " 32bit Indirect" END0
                                          REPTOK "NWI"        REPSTR  "\f2$3 Type: @$4\t14 Name: '$2'",
    /*80*/  "\f$P THEADR($R):"                FIXTOK "N"          FIXSTR " '$0'",
    /*82*/  "\f$P LHEADR($R):"                FIXTOK "N"          FIXSTR " '$0'",
    /*84*/  "\f$P PEDATA($R):"                FIXTOK "BW"         DMPDAT "$0*16+$1",
    /*86*/  "\f$P PIDATA($R):"                FIXTOK "WB"         FIXSTR " $0:$1" DMPIDAT "$0*16+$1",
    /*88*/  "\f$P COMENT($R):"                FIXTOK "BB" IF0("$1=0xa0") "B"END0   FIXSTR " NoPurge=" IF0("$0&0x80") "Yes" ELSE0 "No " END0 " NoList="
                                                                                  IF0("$0&0x40") "Yes" ELSE0 "No " END0 " $C",

    /*8A*/  "\f$P MODEND($R):"                FIXTOK "B" IF0("$0&0x41=0x40") "F" ELSIF0("$0&0x40") "WW" END0
                                                              FIXSTR " " IF0("$0&0x80") "Main module" ELSE0 "Non-Main module" END0
                                                                     IF0("$0&0x41=0x40") "Start: $1" ELSIF0("$0&0x40") "Start: $1:$2" END0,
    /*8C*/  "\f$P EXTDEF($R):"                REPTOK "Ne#eI"      REPSTR "\f1#$1\t4 Type: @$2\t14 '$0'",
    /*8E*/  "\f$P TYPDEF($R):"                FIXTOK "Nt#t"       FIXSTR " #$1\f1$T",
    /*90*/  "\f$P PUBDEF($R):"                FIXTOK "L"          FIXSTR " $0" END0
                                          REPTOK "NWI"        REPSTR  "\f1$2 Type: @$3\t14 '$1'",
    /*92*/  "\f$P LOCSYM($R):"                FIXTOK "L"          FIXSTR  " $0"
                                          REPTOK "NWI"        REPSTR  "\f3$2 Type: @$1\t14 '$1'",
    /*94*/  "\f$P LINNUM($R):"                FIXTOK "L"          FIXSTR " $0"
                                          REPTOK "W%uW"       REPSTR "\f6$2 #$1",
    /*96*/  "\f$P LNAMES($R):"                REPTOK "Nl #l"      REPSTR "\f1#$1\t3 '$0'",
    /*98*/  "\f$P SEGDEF($R):"                FIXTOK "S#s"        FIXSTR " #$1\t22 $0",
    /*9A*/  "\f$P GRPDEF($R):"                FIXTOK "I%l g #g"   FIXSTR " #$1\t22 '$0'"
                                          REPTOK "G"          REPSTR  "\f1$2",
    /*9C*/  "\f$P FIXUPP($R):"                REPTOK "F"          REPSTR "\f1$0",
    /*9E*/  NULL,
    /*A0*/  "\f$P LEDATA($R):"                FIXTOK "I%sW"     FIXSTR " '$0'" DMPDAT "$1",
    /*A2*/  "\f$P LIDATA($R):"                FIXTOK "I%s W"    FIXSTR " Seg($0):$1" DMPIDAT "$1",
    /*A4*/  "\f$P LIBHED($R):"                FIXTOK "W%uWW"    FIXSTR  " $0 Modules Dictionary at $1:$2",
    /*A6*/  "\f$P LIBNAM($R):"                REPTOK "N"        REPSTR  "\f3'$0'",
    /*A8*/  "\f$P LIBLOC($R):"                                  FIXSTR  " Block:Byte"
                                          REPTOK "BB"       REPSTR  "\f8$0:$1",
    /*AA*/  "\f$P LIBDIC($R):"                REPTOK "N%z"       REPSTR  "\f3'$0'",
    /*AC*/  NULL,
    /*AE*/  NULL,
    /*B0*/  "\f$P COMDEF($R):"                REPTOK "NIB" IF0("$2=0x61") "VV" ELSIF0("$2=0x62") "V" ELSE0 "%s" END0
                                              REPSTR "\f1$0\t30 Type: $2" IF0("$2=0x61") " FAR Elements: $3 each $4 bytes"
                                                                          ELSIF0("$2=0x62") " NEAR Size: $3 bytes"
                                                                          ELSE0 " Seg: $2",
    /*B2*/  "\f$P BAKPAT($R):"                FIXTOK "IsB"      FIXSTR " Seg: '$0' " IF0("$1=0") "LoByte" ELSIF0("$1=1") "Offset" ELSE0 "LocType($1)" END0
                                          REPTOK "WW"       REPSTR "\f3Offset: $2  Value: $3",
    /*B4*/  "\f$P LEXTDEF($R):"               REPTOK "Ne#eI"      REPSTR "\f1#$1\t4 Type : @$2\t14 '$0'",
    /*B6*/  "\f$P LPUBDEF($R):"               FIXTOK "L"          FIXSTR " $0" END0
                                          REPTOK "NWI"        REPSTR  "\f2$2 Type: @$3\t14 '$1'",
    /*B8*/  "\f$P LCOMDEF($R):"               REPTOK "NIB" IF0("$2=0x61") "VV" ELSIF0("$2=0x62") "V" ELSE0 "%s" END0
                                              REPSTR "\f1$0\t30 Type: $2" IF0("$2=0x61") " FAR Elements: $3 each $4 bytes"
                                                                          ELSIF0("$2=0x62") " NEAR Size: $3 bytes"
                                                                          ELSE0 " Seg: $2",
    /*BA*/  NULL,
    /*BC*/  "\f$P CEXTDEF($R):"                REPTOK "IlI"     REPSTR "\f1$0\t40 Type: @$1",
    /*BE*/  NULL,
    /*C0*/  NULL,
    /*C2*/  "\f$P COMDAT($R):"                DMPDAT,
    /*C4*/  "\f$P LINSYM($R):"                FIXTOK "BN"       FIXSTR " $1" IF0("$0&1") " Continuation" ELSE0 "New instance" END0
                                          REPTOK "W%uW"       REPSTR "\f4$3 #$2",
    /*C6*/  "\f$P ALIAS($R):"                 REPTOK "NN"          "\f1$0\t40 => %1",
    /*C8*/  "\f$P NBKPAT($R):"                FIXTOK "BN"      FIXSTR " COMDAT: '$1' " IF0("$0 = 0") "LoByte" ELSIF0("$0 = 1") "Offset" ELSE0 "LocType($1)" END0
                                          REPTOK "WW"       REPSTR "\f3Offset: $2  Value: $3",
    /*CA*/  "\f$P LLNAMES($R):"               REPTOK "Nl #l"      REPSTR "\f2#$1\t3 '$0'",
    /*CC*/  "\f$P VERNUM($R):"                FIXTOK  "N"         FIXSTR " Version: $0",
    /*CE*/  "\f$P VENDEXT($R):"                FIXTOK "W"         FIXSTR " Vendor: $0" DMPDAT,

}; 

char const *enumModDat86[5] = { "ModDat", "ABSOLUTE", "RELOCATABLE", "PIC", "LTL" };
char const *enumAlign86[8] = { "Align", "Abs ", "Byte", "Word", "Para", "Page", "Abs ", "LTL " };
char const *enumCombine86[9] = { "Combine", "Private ", "Memory  ", "Public  ", "Reserved", "PubNogap", "Stack   ", "Common  ", "CommonHi" };

void init86() {
    resetStrings();

    index_t *p = initIndex('l', "Name", 1, 256);
    p->names[0] = "<None>";
    p = initIndex('e', "Ext", 1, 256);
    p->names[0] = "**INVALID**";
    p = initIndex('b', "SCOPE", 1, 256);
    p->names[0] = "**INVALID**";
    p = initIndex('s', "Seg", 1, 256);
    p->names[0] = "Unnamed Abs";
    p = initIndex('g', "Grp", 1, 256);
    p->names[0] = "<None>";
    p = initIndex('o', "Ovl", 1, 256);
    p->names[0] = "<None>";
    p = initIndex('t', "Type", 1, 0);
    omfVer = OMF86;
    rules = rules86;
    minRecType = 0x6E;
    maxRecType = 0xCE;
}

void printCommentText(FILE *fpout) {
    uint32_t c;
    if (curPos)
        putch('\n', fpout);
    while (getRecNumber(1, &c)) {
        if (curPos > MAXCOL - 10)
            putch('\n', fpout);
        if (curPos == 0)
            curPos = fprintf(fpout, "%*s", INDENT, "");

        if (' ' <= c && c <= '~')
            putch(c, fpout);
        else if (c == '\n' || c == '\r') {
            putch('\\', fpout);
            putch(c == '\n' ? 'n' : 'r', fpout);
        } else
            curPos += fprintf(fpout, "\\x%02x", c);
    }
}

enum {
    H_TEXT, H_DUMP, H_80, H_A001, H_A002, H_A003, H_A005, H_A3, H_A7,
    H_A8, H_A9
};

struct {
    uint16_t cclass;
    char *label;
    uint8_t handler;
} classInfo[] = {
    { 0,    "Translator", H_TEXT},
    { 1,    "Intel copyright", H_TEXT},
    { 0x80, "Translator", H_TEXT},
    { 0x81, "Library specifier - obsolete", H_TEXT},
    { 0x9c, "MS-DOS version - obsolete", H_TEXT},
    { 0x9d, "Memory model", H_TEXT},
    { 0x9e, "DOSSEG", H_DUMP},
    { 0x9f, "Default library search name", H_TEXT},
    {0xa001, "IMPDEF", H_A001},
    {0xa002, "EXPDEF", H_A002},
    {0xa003, "LIBMOD", H_A003},
    {0xa005, "LNKDIR", H_A005},

    { 0xa1, "New OMF extension", H_TEXT},
    { 0xa2, "Link Pass Separator", H_TEXT},
    { 0xa3, "LIBMOD", H_A3},
    { 0xa4, "EXESTR", H_TEXT},
    { 0xa6, "INCERR", H_DUMP},
    { 0xa7, "NOPAD", H_A7},
    { 0xa8, "WKEXT", H_A8},
    { 0xa8, "LZEXT", H_A9},
    { 0xffff}
};


void printComment86(FILE *fpout) {
    uint32_t num1, num2, num3;
    char str1[256], str2[256], str3[256];
    int i;

    bool ok = true;
    uint32_t pos = getRecPos();
    uint16_t cclass = vars[1].ival;
    if (cclass == 0xa0)
        cclass = (cclass << 8) + vars[2].ival;

    for (i = 0; classInfo[i].cclass != 0xffff; i++) {
        if (classInfo[i].cclass == cclass) {
            fprintf(fpout, "%s(%02X)\n", classInfo[i].label, cclass);
            curPos = 0;
            switch (classInfo[i].handler) {
            case H_TEXT:
                printCommentText(fpout);
                break;
            case H_DUMP:
                printData(fpout, 0);
                break;

            case H_A001:
                if (ok = getRecNumber(1, &num1) && getRecName(NULL, str1) && getRecName(NULL, str2))
                    ok = (num1 == 0) ? getRecName(NULL, str3) : getRecNumber(2, &num2);
                if (ok) {
                    fprintf(fpout, "%*sInternal: %s \n%*sModule:  %s%*sEntryID: ", INDENT, "", str1, INDENT, "", str2, INDENT, "");
                    curPos = INDENT + 9;
                    if (num1 == 0)
                        curPos += fprintf(fpout, "%s", *str3 ? str3 : str1);
                    else
                        curPos += fprintf(fpout, "@%u", num2);
                }
                break;
            case H_A002:
                if (ok = getRecNumber(1, &num1) && getRecName(NULL, str1) && getRecName(NULL, str2))
                    ok = (num1 & 0x80) == 0 || getRecNumber(2, &num2);
                if (ok) {
                    fprintf(fpout, "%*sExported: %s\n%*sInternal: %s\n%*s", INDENT, "", str1, INDENT, "", *str2 ? str2 : str1, INDENT - 1, "");
                    curPos = INDENT;
                    if (num1 & 0x80)
                        curPos += fprintf(fpout, " @%u", num2);
                    if (num1 & 0x40)
                        curPos += fprintf(fpout, " Resident");
                    if (num1 & 0x20)
                        curPos += fprintf(fpout, " NoData");
                    curPos += fprintf(fpout, " Parm: %u", num1 & 0x1f);
                }
                break;
            case H_A003:
                if (ok = getRecNumber(2, &num1) && getRecNumber(2, &num2))
                    curPos = fprintf(fpout, "%*s EXTDEF delta: %d  LINNUM delta: %d", INDENT, "", (int16_t)num1, (int16_t)num2);
                break;

            case H_A005:
                if (ok = getRecNumber(1, &num1) && getRecNumber(1, &num2) && getRecNumber(1, &num3)) {
                    curPos = fprintf(fpout, "%*s Pseudocode v%u.%u  CodeView v%u.%u", INDENT, "", num2 >> 4, num2 & 4, num3 >> 4, num3 & 4);
                    if (num1 & 1)
                        curPos += fprintf(fpout, " New .EXE");
                    if (num1 & 2)
                        curPos += fprintf(fpout, " Omit CV $PUBLICS");
                    if (num1 & 4)
                        curPos += fprintf(fpout, " Run MPC");
                }
                break;
            case H_A3:
                if (ok = getRecName(NULL, str1))
                    curPos = fprintf(fpout, "%*s%s", INDENT, "", str1);
                break;
            case H_A7:
                while (getRecIndex(&num1)) {
                    fprintf(fpout, "%*s'%s'\n", INDENT, "", getIndexName('s', num1));
                    curPos = 0;
                }
                break;

            case H_A8: case H_A9:
                if (ok = getRecIndex(&num1) && getRecIndex(&num2))
                    curPos += fprintf(fpout, "%*s'%s' => '%s'", INDENT, "", getIndexName('e', num1), getIndexName('e', num2));
                break;

            }
            break;
        }
    }
    if (!ok)
        setRecPos(pos);
    if (classInfo[i].cclass == 0xffff)
        fprintf(fpout, cclass <= 0x9b ? "Intel(%02X)" : "User(%02X)", cclass);
    printData(fpout, 0);        // dump any unused data
}


/*
    parse an OMF86b fixup item
    THREAD SubRecord  TRD ?INDEX/FRAME
    TRD bits 7=0  6 -> D   2-4 -> Method   0-1 -> Thred
    if D then method %= 4
    INDEX if Method < 3
    FRAME if Method = 3

    Fixup SubRecord Locat FixData ?FrameDatum(INDEX/FRAME) ?TargetDatum(INDEX/FRAME) ?TargetDisplacement
    Locat reverse order word bits as follows
    15=0  14 -> M  10-13 -> Location  0-9 -> Data Record Offset
    FixData bits  7 -> F  4-6 -> Frame  3 -> T  2 -> P  0-1 -> Targt

    ?FrameDatum if F=0 & frame < 3
    ?TargetDatum if T == 0
    ?TargetDisplacement if P == 0

*/

bool getIndexOrFrame(uint8_t method, uint32_t *val) {
    if (method != 3)
        return getRecIndex(val);
    else
        return getRecNumber(2, val);
}

bool parseFixup86(uint8_t const **ps, var_t *pvar) {
    uint32_t dat;
    uint32_t pos = getRecPos();
    uint32_t indexOrFrame;

    do {
        if (!getRecNumber(1, &dat))
            break;
        pvar->sval[0] = 0;

        if (!(dat & 0x80)) {       // thread
            uint8_t method = (dat >> 2) & 7;
            if (!(dat & 0x40))
                method %= 4;

            uint8_t thred = dat & 3;
            aSprintf(pvar->sval, "%s thread[%d] reference: ", (dat & 0x40) ? "FRAME " : "TARGET", thred);

            if (method < 4 && !getIndexOrFrame(method, &indexOrFrame))
                break;
            switch (method) {
            case 0:  aSprintf(pvar->sval, "SI[%s]", getIndexName('s', indexOrFrame)); break;
            case 1:  aSprintf(pvar->sval, "GI[%s]", getIndexName('g', indexOrFrame)); break;
            case 2:  aSprintf(pvar->sval, "EI[%s]", getIndexName('e', indexOrFrame)); break;
            case 3:  aSprintf(pvar->sval, "%04X", indexOrFrame); break;
            case 4:  aSprintf(pvar->sval, "LOCATION"); break;
            case 5:  aSprintf(pvar->sval, "TARGET"); break;
            case 6:  aSprintf(pvar->sval, "NONE"); break;
            default: aSprintf(pvar->sval, "ERROR"); break;
            }

        } else {                // fixup
            uint32_t locat;
            uint32_t fixdat;
            if (!getRecNumber(1, &locat) || !getRecNumber(1, &fixdat))
                break;

            locat += dat << 8;
            uint8_t loc = (locat >> 10) & 0x7;
            uint8_t offset = locat & 0x3ff;
            uint8_t frame = (fixdat >> 4) & 7;
            uint8_t targt = fixdat & 7;     // include the p bit

            uint32_t frameDatum, targetDatum, targetDisplacement;
            if (!(fixdat & 0x80) && frame < 4 && !getIndexOrFrame(frame, &frameDatum))
                break;
            if (!(fixdat & 8) && !getIndexOrFrame(targt, &targetDatum))
                break;
            if (targt < 4 && !getRecNumber(2, &targetDisplacement))
                break;

            char *locStr;
            switch (loc) {
            case 0: locStr = "Lobyte "; break;
            case 1: locStr = "Offset "; break;
            case 2: locStr = "Base   "; break;
            case 3: locStr = "Pointer"; break;
            case 4: locStr = "Hibyte "; break;
            default: locStr = "Unknown"; break;
            }
            aSprintf(pvar->sval, "FIXUP  %04X %s %s FRAME: ", offset, locStr, locat & 0x4000 ? "self" : "seg ");
            char *s = strchr(pvar->sval, 0);               // mark end of string & use padStr(., 20) rather than appendStrFormat(., 20,...)
            if (fixdat & 0x80)
                aSprintf(pvar->sval, "thread[%u]", frame & 3);
            else
                switch (frame) {
                case 0:  aSprintf(pvar->sval, "SI[%s]", getIndexName('s', frameDatum)); break;
                case 1:  aSprintf(pvar->sval, "GI[%s]", getIndexName('g', frameDatum)); break;
                case 2:  aSprintf(pvar->sval, "EI[%s]", getIndexName('e', frameDatum)); break;
                case 3:  aSprintf(pvar->sval, "%04X", frameDatum); break;
                case 4:  aSprintf(pvar->sval, "LOCATION"); break;
                case 5:  aSprintf(pvar->sval, "TARGET"); break;
                case 6:  aSprintf(pvar->sval, "NONE"); break;
                default: aSprintf(pvar->sval, "ERROR"); break;
                }
            padStr(s, 20);
            strcat(s, " TARGET: ");
            if ((fixdat & 8))
                aSprintf(s, "THREAD[%u]", targt & 3);
            else
                switch (targt) {
                case 0:  aSprintf(s, "SI[%s],%04X", getIndexName('s', targetDatum), targetDisplacement); break;
                case 1:  aSprintf(s, "GI[%s],%04X", getIndexName('g', targetDatum), targetDisplacement); break;
                case 2:  aSprintf(s, "EI[%s],%04X", getIndexName('e', targetDatum), targetDisplacement); break;
                case 3:  aSprintf(s, "%04X,%04X", targetDatum, targetDisplacement); break;
                case 4:  aSprintf(s, "SI[%s]", getIndexName('s', targetDatum)); break;
                case 5:  aSprintf(s, "GI[%s]", getIndexName('g', targetDatum)); break;
                case 6:  aSprintf(s, "EI[%s]", getIndexName('e', targetDatum)); break;
                default: aSprintf(s, "ERROR"); break;
                }
        }

        ++*ps;
        return true;
    } while (0);

    setRecPos(pos);
    return false;
}


bool parseSegDef86(uint8_t const **ps, var_t *pvar) {
    char *s;
    char const *className = "";
    char const *overlayName = "";

    uint32_t pos = getRecPos();

    do {
        uint32_t acbp;
        if (!getRecNumber(1, &acbp))
            break;
        uint8_t alignment = (acbp >> 5) & 7;
        uint8_t combination = (acbp >> 2) & 7;

        uint32_t frame, offset, ltldat = 0, maxlen;
        // WARNING: alignment 5 conflicts with Microsoft extension
        if ((alignment == 0 || alignment == 5) && (!getRecNumber(2, &frame) || !getRecNumber(1, &offset)))
            break;
        if (alignment == 6 && (!getRecNumber(1, &ltldat) || !getRecNumber(2, &maxlen) || !getRecNumber(2, &offset)))
            break;
        if (ltldat & 1)
            maxlen = 0x10000;

        uint32_t seglen;
        if (!getRecNumber(2, &seglen))
            break;
        if (acbp & 2)
            seglen = 0x10000;
        uint32_t segIndex, classIndex, overlayIndex;
        if (alignment != 5 && (!getRecIndex(&segIndex) || !getRecIndex(&classIndex) || !getRecIndex(&overlayIndex)))
            break;

        char *endSegName = NULL;            // used to fixup synthetic full seg name
        strcpy(pvar->sval, "'");            // quote seg name
        if (alignment != 5) {
            strcpy(pvar->sval + 1, getIndexName('l', segIndex));
            endSegName = strchr(pvar->sval, 0);
            strcpy(endSegName, "\x80");             // add the SEGSHORT marker

            className = classIndex ? getIndexName('l', classIndex) : "";
            overlayName = overlayIndex ? getIndexName('l', overlayIndex) : "";
            if (*className || *overlayName)         // add the SEGFULL information
                aSprintf(endSegName, "%s%s%s", className, *overlayName ? "/" : "", overlayName);
        }
        else
            aSprintf(pvar->sval, "<Unnamed %u>", getAllocatedIndex('s') + 1);  // +1 to get the to be allocated number

        char *astr = "s";                   // trick to use parseUpdateIndex
        parseUpdateIndex(&astr, pvar->sval + 1);      // to install the new seg name
        
        if (endSegName)                     // remove SEGFULL info
            *endSegName = 0;

        strcat(pvar->sval, "'");        // closing quote
        if (alignment != 5) {
            padStr(pvar->sval, 20);            // pad to 20 chars
            if (classIndex) aSprintf(pvar->sval, " Class: %s", *className ? className :"null");
            pad(pvar->sval, 35);
            if (overlayIndex) aSprintf(pvar->sval, "Ovl: %s", *overlayName ? overlayName : "null");
        }
        padStr(pvar->sval, 52);
        fmtEnum(5, combination, strchr(pvar->sval, 0));     // put the combine method
        
        s = strchr(pvar->sval, 0);
        *s = ' ';
        fmtEnum(4, alignment, s + 1);                       // put any additional attibutes
        if (acbp & 1)
            strcat(pvar->sval, " InPage");
        if (ltldat & 0x80)
            strcat(pvar->sval, " Group");
        if (alignment == 0 || alignment == 5)
            aSprintf(s, " at %04X:%02X)", frame, offset);
        padStr(pvar->sval, 76);
        aSprintf(pvar->sval, "Len: %04X", seglen);
        if (alignment == 6)
            aSprintf(s, "-%04X", maxlen);
        ++*ps;
        return true;

    } while (0);
    setRecPos(pos);
    return false;
}

bool parseGroupComponent86(uint8_t const **ps, var_t *pvar) {
    bool ok = true;
    uint32_t pos = getRecPos();

    do {
        uint32_t type;
        uint32_t seIndex, cIndex, oIndex, frame;
        uint32_t ltldat, maxlen, len;
        if (!getRecNumber(1, &type))
            break;
        switch (type) {
        case 0xff:
        case 0xfe:
            ok = getRecIndex(&seIndex);
            if (ok) {
                if (type == 0xff)
                    sprintf(pvar->sval, "SI[%s]", getIndexName('s', seIndex));
                else
                    sprintf(pvar->sval, "EI[%s]", getIndexName('e', seIndex));
            }
            break;
        case 0xfd:
            ok = getRecIndex(&seIndex) && getRecIndex(&cIndex) && getRecIndex(&oIndex);
            if (ok) {
                sprintf(pvar->sval, "SCO: Seg: %s", getIndexName('l', seIndex));
                if (cIndex)
                    aSprintf(pvar->sval, "  Class: %s", getIndexName('l', cIndex));
                if (oIndex)
                    aSprintf(pvar->sval, "  Ovl: %s", getIndexName('l', oIndex));
            }
            break;
        case 0xfb:
            ok = getRecNumber(1, &ltldat) && getRecNumber(2, &maxlen) && getRecNumber(2, &len);
            if (ok) {
                if (ltldat & 1) maxlen = 0x10000;
                if (ltldat & 2) len = 0x10000;
                sprintf(pvar->sval, "LTL: Len: %04X-%04X", len, maxlen);
            }
            break;
        case 0xfa:
            ok = getRecNumber(2, &frame);
            if (ok)
                sprintf(pvar->sval, "FRAME: %04X", frame);
            break;
        default:
            ok = false;
        }

        if (ok) {
            ++*ps;
            return true;
        }

    } while (0);
    setRecPos(pos);
    return false;

}

bool parseBase86(uint8_t const **ps, var_t *pvar) {
    uint32_t pos = getRecPos();

    do {
        uint32_t gIndex, sIndex, frame;
        if (!getRecIndex(&gIndex) || !getRecIndex(&sIndex))
            break;
        if (!gIndex && !sIndex)
            if (getRecNumber(2, &frame))
                sprintf(pvar->sval, "FRAME: %04X", frame);
            else
                break;
        else {
            if (gIndex)
                sprintf(pvar->sval, "Grp: %s%s", getIndexName('g', gIndex), sIndex ? "  " : "");
            if (sIndex)
                aSprintf(pvar->sval, "Seg: %s", getIndexName('s', sIndex));
        }
        ++*ps;
        return true;
    } while (0);
    setRecPos(pos);
    return false;

}







#if 0
bool proforma(uint8_t const **ps, var_t *pvar) {
    uint32_t pos = getRecPos();

    do {
        1;
    } while (0);
    setRecPos(pos);
    return false;

}
#endif
