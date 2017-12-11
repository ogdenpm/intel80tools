/*
    vim:ts=4:shiftwidth=4:expandtab:
*/
#include "lib.h"
#include <setjmp.h>

extern jmp_buf reset;

static byte errStrs[] = {
    "\0\0"
    "\x02" "ILLEGAL AFTN ARGUMENT\0"
    "\x04" "INCORRECTLY SPECIFIED FILE\0"
    "\x05" "UNRECOGNIZED DEVICE NAME\0"
    "\x06" "ATTEMPT TO WRITE TO INPUT DEVICE\0"
    "\x08" "ATTEMPT TO READ FROM OUTPUT DEVICE\0"
    "\x0A" "NOT ON SAME DISK\0"
    "\x0B" "FILE ALREADY EXISTS\0"
    "\x0C" "FILE IS ALREADY OPEN\0"
    "\x0D" "NO SUCH FILE\0"
    "\x0E" "WRITE PROTECTED\0"
    "\x11" "NOT A DISK FILE\0"
    "\x12" "ILLEGAL ISIS COMMAND\0"
    "\x13" "ATTEMPTED SEEK ON NON-DISK FILE\0"
    "\x14" "ATTEMPTED BACK SEEK TOO FAR\0"
    "\x15" "CAN'T RESCAN\0"
    "\x16" "ILLEGAL ACCESS MODE TO OPEN\0"
    "\x17" "MISSING FILENAME\0"
    "\x19" "ILLEGAL ECHO FILE\0"
    "\x1A" "ILLEGAL ATTRIBUTE IDENTIFIER\0"
    "\x1B" "ILLEGAL SEEK COMMAND\0"
    "\x1C" "MISSING EXTENSION\0"
    "\x1F" "CAN'T SEEK ON WRITE ONLY FILE\0"
    "\x20" "CAN'T DELETE OPEN FILE\0"
    "\x23" "SEEK PAST EOF\0"
    "\xC9" "UNRECOGNIZED CONTROL\0"
    "\xCA" "UNRECOGNIZED DELIMITER\0"
    "\xCB" "INVALID SYNTAX\0"
    "\xCC" "PREMATURE EOF\0"
    "\xCE" "ILLEGAL DISKETTE LABEL\0"
    "\xD0" "CHECKSUM ERROR\0"
    "\xD1" "RELO FILE SEQUENCE ERROR\0"
    "\xD2" "INSUFFICIENT MEMORY\0"
    "\xD3" "RECORD TOO LONG\0"
    "\xD4" "ILLEGAL RELO RECORD\0"
    "\xD5" "FIXUP BOUNDS ERROR\0"
    "\xD6" "ILLEGAL SUBMIT PARAMETER\0"
    "\xD7" "ARGUMENT TOO LONG\0"
    "\xD8" "TOO MANY PARAMETERS\0"
    "\xD9" "OBJECT RECORD TOO SHORT\0"
    "\xDA" "ILLEGAL RECORD FORMAT\0"
    "\xDB" "PHASE ERROR\0"
    "\xDC" "NO EOF\0"
    "\xDD" "SEGMENT TOO LARGE\0"
    "\xDE" "UNRECOGNIZED RECORD TYPE\0"
    "\xDF" "BAD FIXUP RECORD\0"
    "\xE0" "BAD RECORD SEQUENCE\0"
    "\xE1" "INVALID MODULE NAME\0"
    "\xE2" "MODULE NAME TOO LONG\0"
    "\xE3" "LEFT PARENTHESIS EXPECTED\0"
    "\xE4" "RIGHT PARENTHESIS EXPECTED\0"
    "\xE5" "UNRECOGNIZED CONTROL\0"
    "\xE6" "DUPLICATE SYMBOL IN INPUT\0"
    "\xE9" "'TO' EXPECTED\0"
    "\xEB" "NOT LIBRARY\0"
    "\xE8" "UNRECOGNIZED COMMAND\0"
    "\xEE" "ILLEGAL STACK CONTENT RECORD\0"
    "\xEF" "NO MODULE HEADER RECORD\0"
    "\xF0" "PROGRAM EXCEEDS 64K"};


bool debugLog = true;
word debugConn = 0;

static pointer SkipAfn(pointer path)
{

    while (1) {
        path = PastFileName(path);
        if (*path == '*' || *path == '?')
            path = path + 1;
        else
            return path;
    }
}

void WriteErrStr(word errCode)
{
    word i;
    word status;

    if (errCode != 0)
    {
        i = 0;
        while (i < sizeof(errStrs)) { 
            if (errCode == errStrs[i] && errStrs[i + 1] != 0)
            {
                i = i + 1;
                Write(0, " ", 1, &status);
                while (errStrs[i] != 0) {
                    Write(0, &errStrs[i], 1, &status);
                    i = i + 1;
                }
                Write(0, "\r\n", 2, &status);
                return;
            }
            else {
                while (errStrs[i] != 0) {
                    i = i + 1;
                }
                i = i + 1;
            }
        }
        Error(errCode); /* pass to ISIS */
    }

}



void FileStatusChk(word errCode, pointer pathP, bool isFatal)
{
    word fstatus;

    if (errCode != 0)
    {
        pathP = SkipSpc(pathP);
        Write(0, " ", 1, &fstatus);
        Write(0, pathP, (word)(SkipAfn(pathP) - pathP), &fstatus);
        Write(0, ",", 1, &fstatus);
        WriteErrStr(errCode);
        longjmp(reset, 2); // goto reset;
    }
}

void Log(pointer buf, word cnt)
{
    Write(0, buf, cnt, &status);
    /* code cannot be reached as nothing modifies debugConn
       looks like debug code to Log() the data into another file
       debugConn is the fileid and debugLog is the Log() on /off
    */
    if (debugConn != 0 && debugLog)
        Write(debugConn, buf, cnt, &status);
}

void LogCRLF(pointer buf, word cnt)
{
    Log(buf, cnt);
    Log("\r\n", 2);
}


void Fatal(byte err)
{
    FileStatusChk(err, argFilename, true);
    Exit();
}

void IllegalRecord()
{
    FileStatusChk(ERR218, argFilename, true);   /* ILLEGAL RECORD FORMAT */
}


pointer memTop;
pointer heapTop;

pointer AllocUp(word cnt)
{

    if (memTop - heapTop >= cnt)
    {
        heapTop = heapTop + cnt;
        return heapTop - cnt;
    }
    WriteErrStr(ERR210);    /* INSUFFICIENT MEMEORY */
    longjmp(reset, 3); // goto reset;
}




pointer AllocDown(word cnt)
{
    if (memTop - heapTop >= cnt)
        return (memTop = memTop - cnt);
    WriteErrStr(ERR210);    /* INSUFFICIENT MEMEORY */
    longjmp(reset, 4); // goto reset;
}

file_t *fileHead;
file_t *fileP;

static void StatusChk(byte connB, wpointer statusP)
{
    if (*statusP == 0)
        return;

    fileP = (file_t *)&fileHead;
    while ((fileP = fileP->link) != 0) {
        if (fileP->conn == connB && fileP->isopen)
        {
            FileStatusChk(*statusP, fileP->pathP, false);
            longjmp(reset, 5); // goto reset;
        }
    }
}


void OpenFile(wpointer connP, pointer pathP, word access, word echo, wpointer statusP)
{
    Open(connP, pathP, access, echo, statusP);
    if (*statusP != 0)
    {
        FileStatusChk(*statusP, pathP, 0);
        longjmp(reset, 6); // goto reset;
    }

    fileP = (file_t *)AllocUp(sizeof(file_t));
    fileP->link = fileHead;
    fileHead = fileP;
    fileP->pathP = pathP; 
    fileP->isopen = true;
    fileP->conn = (byte) *connP;
}

void CloseFile(word conn, wpointer statusP)
{
	Close(conn, statusP);   /* close on ISIS */

    /* clear down the connection on the file list */
    fileP = (file_t *)&fileHead;
    while ((fileP = fileP->link) != 0) {
        if (fileP->conn == conn) /* found the file */
        {
            fileP->isopen = false;
            FileStatusChk(*statusP, fileP->pathP, true);    
        }
    }
}


void ReadFile(word conn, pointer buffP, word count, wpointer actualP, wpointer statusP)
{
    Read(conn, buffP, count, actualP, statusP);
    StatusChk((byte)conn, statusP);
}

void WriteFile(word conn, pointer buffP, word count, wpointer statusP)
{

    Write(conn, buffP, count, statusP);
    StatusChk((byte)conn, statusP);
}


void SeekFile(word conn, word mode, wpointer blockP, wpointer byteP, wpointer statusP)
{
    Seek(conn, mode, blockP, byteP, statusP);
    StatusChk((byte)conn, statusP);
}

void DeleteFile(pointer pathP, wpointer statusP)
{
    Delete(pathP, statusP);
    if (*statusP != 0)
    {
        FileStatusChk(status, pathP, 0);
        longjmp(reset, 7); // goto reset;
    }
}

void RenameFile(pointer oldP, pointer newP, wpointer statusP)
{
    Rename(oldP, newP, statusP);
    if (*statusP != 0)
    {   
        FileStatusChk(status, oldP, 0);
        longjmp(reset, 8); // goto reset;
    }
}

librec_t curRec;
librec_t savedRec;

/* EXTERNALS */

void OpenLib()
{
    OpenFile(&inConn, argFilename, 1, 0, &status);
    curRec.bytesLeft = 0;
}

void GetRecordBytes(word count, pointer bufP)
{
    ReadFile(inConn, bufP, count, &actual, &status);
    if (actual != count || count > curRec.bytesLeft)
        FileStatusChk(ERR217, argFilename, 0xFF);
    
    curRec.bytesLeft = curRec.bytesLeft - count;
    while (count != 0) {
        curRec.crc = curRec.crc + bufP[count = count - 1];
    }
}


void ReadChkCrc()
{
    byte recByte; 

    GetRecordBytes(1, &recByte);
    if (curRec.crc != 0)
        FileStatusChk(ERR208, argFilename, true);   /* CHECKSUM ERROR */
}


/* skip current record if one being processed */

void SkipCurRec()
{
    word zblk = 0;

    if (curRec.bytesLeft == 1)
        ReadChkCrc();
    else
        SeekFile(inConn, 3, &zblk, &curRec.bytesLeft, &status);
    curRec.bytesLeft = 0;
}


void PrepRecord()
{
    SkipCurRec();   /* skip current record if (one being processed */
    SeekFile(inConn, 0, &curRec.curBlk, &curRec.curByte, &status);
    curRec.bytesLeft = 3;   /* 3 bytes for rectype and record length */
    curRec.crc = 0;     /* reset crc */
    GetRecordBytes(3, &curRec.type);
    curRec.bytesLeft = curRec.len;  /* correct the length using the record length */
}

void AcceptRecord(byte type)
{
    PrepRecord();
    if (curRec.type != type)
        Fatal(ERR218);  /* ILLEGAL RECORD FORMAT */
}

void GetName(pointer buf)
{
    GetRecordBytes(1, buf);

    if (buf[0] > 31)
        IllegalRecord();

    GetRecordBytes(buf[0], buf + 1);
}


void SkipModule()
{
    while (curRec.type != OBJEND) {
        PrepRecord();
    }
    SkipCurRec();   /* skip the OBJ end as well */
}

void LibSeek(byte modeB, wpointer blkByteP)
{
    SeekFile(inConn, modeB, blkByteP, blkByteP + 1, &status);   // scaled for C pointers
    if (modeB == 2)
        curRec.bytesLeft = 0;
    return;
}


void SaveCurPos()
{
    LibSeek(0, &curRec.posBlk);
    savedRec = curRec;
}


void RestoreCurPos()
{
    LibSeek(2, &savedRec.posBlk);
    curRec = savedRec;
}

void SwapCurSavedRec()
{
    word tmp;
    librec_t tmpRec;

    LibSeek(0, &curRec.posBlk); 
    tmp = curRec.bytesLeft;
    LibSeek(2, &savedRec.posBlk);
    curRec.bytesLeft = tmp;
    tmpRec = curRec;
    curRec = savedRec;
    savedRec = tmpRec;
}


byte Hash(pointer pstr)
{
    byte i, j;

    j = 0;
    for (i = 0; i <= pstr[0]; i++) {
        j = ((j >> 1) | (j << 7)) ^ pstr[i];
    }
    return j % 128;
} /* Hash */

bool LookupSymbol(pointer modNameP, symbol_t **hashP)
{
    symbol_t *curSymP;
    pointer symbol;
    byte i;

    /* using hashing find the start of the symbol chain */
    curSymP = (symbol_t *)&hashTable[Hash(modNameP)];

    /* scan the chain looking for a match */
    while (curSymP->link != 0) {
        curSymP = curSymP->link;
        symbol = curSymP->symbol;

        for (i = 0; i <= modNameP[0]; i++) {
            if (symbol[i] != modNameP[i])
                goto outerloop;
        }
        *hashP = curSymP;  
        return true;    /* found it */
outerloop:
        ;
    }
    *hashP = curSymP;
    return false;       /* not found */
}

line_t *lineHead;
line_t *curLineP ;
pointer lookAheadP;
byte lookAheadLen;
pointer tokenP;
byte tokLen;
// byte junk1;
bool inModuleList;

__declspec(noreturn) void LibError(byte err)
{
    line_t *lineP;
    pointer s;

    s = tokenP + tokLen;    /* assumes these are set up correctly, doesn't always seem to be correct */
    WriteErrStr(err);
    lineP = lineHead;

    /*
        print out lines that are in the chain before this one
        basiy s will not be in the range of the line pointed
        to by lineP
    */
    while (s < (pointer) lineP || lineP->text + lineP->len < s) {
        Write(0, lineP->text, lineP->len, &status);
        lineP = lineP->next;
    }
    /* this logic looks flawed I suspect the else clause is generally taken */
    /* would be reasonable if curLineP->len was used */
    if (s > (pointer)curLineP  && curLineP->text + 1 + curLineP->len > s)	// next changed to len
        Write(0, curLineP->text, (word)(s - curLineP->text), &status);
    else
        Write(0, lineP->text, (word)(s - lineP->text), &status);

    Write(0, "#\r\n", 3, &status);
    longjmp(reset, 9); // goto reset;
}

static word GetTokenLen(pointer str)
{
//    byte junk;
    pointer tmp;
    word wtmp;  // simple var to avoid mixed pointer/word usage

    if (*str == CR)
        return 0;

    if (inModuleList)
    {
        if (*str < '?' || *str > 'Z')
            LibError(ERR225);   /* INVALID MODULE NAME */

        tmp = str;

        while (((*str >= '0' && *str <= '9') || (*str <= 'Z' && *str >= '?' && (str - tmp < 33)))) {
            str = str + 1;
        }

        if (str - tmp > 32)
            LibError(ERR226);   /* MODULE NAME TOO LONG */

        return (word)(str - tmp);
    }
    if (*str == '(')
        return 1;
    else if (*str == ')')
        return 1;
    else if (*str == ',')
        return 1;
    wtmp = (word)(PastFileName(str) - str);
    if (wtmp > 0)
        return wtmp;
    else
        LibError(ERR203);   /* INVALID SYNTAX */
}




static void GetLine()
{
    byte i;

    while (1) {     // goto loop construct removed
        /* allocate a line buffer and insert into the chain */
        curLineP->next = (line_t *)AllocDown(sizeof(line_t) + 121);
        curLineP = curLineP->next;
        curLineP->next = 0;
        /* Read() line and convert to upper case */
        Read(1, curLineP->text, 122, &curLineP->len, &status);
        for (i = 0; i <= 121; i++) {    /* modified to use structure access to text */
            if (curLineP->text[i] >= 'a' && curLineP->text[i] <= 'z')
                curLineP->text[i] -= 0x20;   /* convert to upper case */
        }

        if (tokLen == 0)           /* initialise token if first one */
            tokenP = curLineP->text;
        lookAheadP = SkipSpc(curLineP->text);
        /* check line with just & on it */
        if (lookAheadP[0] != '&') {      /* test inverted to remove goto loop */
            lookAheadLen = (byte)GetTokenLen(lookAheadP);
            return;
        }
        /* make sure nothing after the & */
        lookAheadP = SkipSpc(lookAheadP + 1);
        if (lookAheadP[0] != CR)
            LibError(ERR203);   /* INVALID SYNTAX */
        /* prompt user and get another line */
        Write(0, "**", 2, &status);
        /* note potentially wasteful on memory as this line buffer could be reused */
    }
}

void GetToken()
{
    tokenP = lookAheadP;        /* record latest token */
    tokLen = lookAheadLen;
    /* track in / out of module list */
    if (lookAheadP[0] == ')')
        inModuleList = 0;   /* no longer in module list */
    lookAheadP = SkipSpc(lookAheadP + lookAheadLen);
    /* continuation line ? */
    if (lookAheadP[0] == '&')
    {
        lookAheadP = SkipSpc(lookAheadP + 1);
        /* can't be anything other than blanks after the & */
        if (lookAheadP[0] != CR)
        {
            tokenP = lookAheadP;    /* point to the Error() character */
            /* this code is flawed as tokLen will be wrong for LibError() */
            LibError(ERR203);   /* INVALID SYNTAX */
        }
        /* get another line */
        Write(0, "**", 2, &status);
		GetLine();
    }
    lookAheadLen = (byte)GetTokenLen(lookAheadP);
}


bool MatchToken(pointer chaP, byte len)
{
    byte i;

    if (tokLen != len)
        return false;
    if (len == 0)
        return true;

    for (i = 0; i <= len - 1; i++) {
        if (chaP[i] != tokenP[i])
            return false;
    }
    return true;
}   


bool MatchLookAhead(pointer chaP, byte len)
{
    byte i;

    if (lookAheadLen != len)
        return false;
    if (len == 0)
        return true;
    for (i = 0; i <= len - 1; i++) {
        if (chaP[i] != lookAheadP[i])
            return false;
    }
    return true;
}

void GetCmd()
{
        inModuleList /* = junk1 */ = tokLen = 0;
        lineHead = 0;       /* no line chain */
        curLineP = (line_t *)&lineHead;
        GetLine();        /* get first line token and lookAhead token */
        GetToken();
}

