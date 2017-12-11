/*
 * vim:ts=4:shiftwidth=4:expandtab:
 */
#include "link.h"

word inFile;
word tofilefd;
word printFileNo;
word pad_4565;
word tmpfilefd;
word statusIO;
word actRead;
byte inFileName[16];
byte toFileName[16];
byte printFileName[16];
byte filePath[16];
byte linkTmpFile[16];
bool mapWanted;
byte outModuleName[32];
byte modEndModTyp;
byte outTranId;
byte outTranVn;
byte modEndSegId;
word modEndOffset;
word segLen[6];
byte alignType[6];
byte segmap[256];
pointer membot;
pointer topHeap;
record_t *inRecordP;
pointer erecP;
pointer inP;
word recNum;
word recLen;
word npbuf;
pointer sbufP;
pointer bufP;
pointer ebufP;
pointer soutP;
pointer outP;
pointer eoutP;
library_t *objFileHead;
library_t *curObjFile;
module_t *curModule;
symbol_t *hashTab[128];
symbol_t *headSegOrderLink;
symbol_t *comdefInfoP;
symbol_t *symbolP;
word unresolved;
word maxExternCnt;
symbol_t *headUnresolved;
byte CRLF[2] = "\r\n";
byte recErrMsg[] =  " RECORD TYPE XXH, RECORD NUMBER *****\r\n";
                    //         13-^               32-^ 
word inBlk;
word inByt;
pointer inbP;
byte inCRC;

byte COPYRIGHT[] = "[C] 1976, 1977, 1979 INTEL CORP'";
byte VERSION[] = "V3.0";
byte DUMMYREC[] = {0,0,0};

/* EXTERNALS */
extern byte overlayVersion[4];

void ConOutStr(pointer pstr, word count)
{
    Write(0, pstr, count, &statusIO);
} /* ConOutStr() */

void FatalErr(byte errCode)
{
    ConOutStr(" ", 1);
    ConOutStr(&inFileName[1], inFileName[0]);
    if (curModule)
    {
        ConOutStr("(", 1);
        ConOutStr(&curModule->name[1], curModule->name[0]);
        ConOutStr(")", 1);
    }
    ConOutStr(",", 1);
    ReportError(errCode);
    BinAsc(inRecordP->rectyp, 16, '0', &recErrMsg[13], 2);
    if (recNum > 0 )
        BinAsc(recNum, 10, ' ', &recErrMsg[32], 5);
    ConOutStr(recErrMsg, sizeof(recErrMsg) - 1);
    Exit();
} /* FatalErr() */

void IllFmt()
{
    FatalErr(ERR218);   /* Illegal() record format */
} /* IllFmt() */

void IllegalRelo()
{
    FatalErr(ERR212);   /* Illegal() relo record */
} /* IllegalRelo() */

void BadRecordSeq()
{
    FatalErr(ERR224);   /* Bad() record sequence */
} /* BadRecordSeq() */

void Pstrcpy(pointer psrc, pointer pdst)
{
    memmove(pdst, psrc, psrc[0] + 1);
} /* Pstrcpy() */

byte HashF(pointer pstr)
{
    byte i, j;

    j = 0;
    for (i = 0; i <= pstr[0]; i++) {
        j = RorB(j, 1) ^ pstr[i];
    }
    return j & 0x7F;
} /* HashF() */

bool Lookup(pointer pstr, symbol_t **pitemRef, byte mask)
{
    symbol_t *p;
    byte i;

    i = pstr[0] + 1;     /* Size() of string including length() byte */
    *pitemRef = (p = (symbol_t *)&hashTab[HashF(pstr)]);
    p = p->hashLink;
    while (p > 0) {     /* chase down the list to look for the name */
        *pitemRef = p;
        if ((p->flags & mask) != AUNKNOWN ) /* ignore undef entries */
            if (Strequ(pstr, p->name, i) )
                return TRUE;
        p = p->hashLink;  /* next */
    }
    return false;
} /* Lookup() */

void WriteBytes(pointer bufP, word count)
{    
    Write(printFileNo, bufP, count, &statusIO);
    FileError(statusIO, &printFileName[1], TRUE);
} /* WriteBytes() */

void WriteCRLF()
{
    WriteBytes(CRLF, 2);
} /* WriteCRLF() */

void WriteAndEcho(pointer buffP, word count)
{
    
    WriteBytes(buffP, count);
    if (printFileNo > 0 )
        ConOutStr(buffP, count);
} /* WriteAndEcho() */

void WAEFnAndMod(pointer buffP, word count)
{
    WriteAndEcho(buffP, count);
    WriteAndEcho(&inFileName[1], inFileName[0]);
    WriteAndEcho("(", 1);
    WriteAndEcho(&curModule->name[1], curModule->name[0]);
    WriteAndEcho(")\r\n", 3);
} /* WAEFnAndMod */

void Start()
{
    ParseCmdLine();
    Phase1();
//    Load(&filePath[1], 0, 0, &actRead, &statusIO);  /* Load() the overlay */
//    FileError(statusIO, &filePath[1], TRUE);
//    if (! Strequ(VERSION, overlayVersion, 4) )    /* make sure it is valid */
//        FileError(ERR219, &filePath[1], TRUE);  /* phase Error() */
    Phase2();
    Close(printFileNo, &statusIO);
    Exit();

} /* Start */

