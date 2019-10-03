// $Id: plmov0.cpp,v 1.4 2004/12/13 21:48:34 Mark Exp $
#include <iostream>
#include <assert.h>
using namespace std;
#include "plm.hpp"
#include "common.hpp"



macroStk_t macroPtrs[6];
word macroDepth;
word tokenVal;
char *inChr_p;
word stateStack[100];
word stateIdx;
symbol_pt stmtLabels[10];
word stmtLabelCnt;
info_pt curProcInfo_p;
word inOffCurCh;
word inOffLastCh;
word curStmtCnt;
word curBlkCnt;
info_pt curMacroInfo_p;
symbol_pt markedSymbol_p;
char lineBuf[128];
byte inbuf[1280];
byte tokenType;
unsigned char tokenStr[256];
byte savedCh;
byte stmtStartCode;
byte labelBrkToken;
byte labelBrkTxiCode;
word doBlkCnt;
byte tx1Buf[1280];
symbol_pt labelBrkSymbol;
byte lineInfoToWrite;
byte isNonCtrlLine;
byte yyAgain;
struct _linfo linfo;

#pragma pack(push, 1)
union {
    word Wrd;
    struct {
        byte curDoBlkCnt;
        byte curProcId;
    } Byt;
} procData;

word *curProcData;

byte skippingCOND;
word ifDepth;
byte primaryCtrlSeen[14];
struct {
    byte code;
    byte list;
    byte cond;
    word leftmargin;
} saveStack[5];
#pragma pack(pop)
byte saveDepth;
byte CODE;
byte LIST = 255;
byte COND = 255;
char *curCh_p;
byte chrClass;
byte tknLen;
char *optStrVal_p;
word optNumValue;
char optFileName[16];
byte ixiGiven;
byte objGiven;
byte lstGiven;
byte inIFpart;
word skippingCONDDepth;
control_t *tknFlags_p;
char wrkFiles1[5];
char wrkFiles2[5];
byte l59e6_truncated;
char *startOfLine;
byte endToggle;
byte saveClass;
symbol_pt declNames[33];
info_pt declBasedNames[33];
symbol_pt declaringName;
info_pt declaringBase;
info_pt basedInfo;
byte dclFlags[3];
byte dclType;
address lastLit;
word arrayDim;
symbol_pt structMembers[33];
word structMemDim[33];
byte structMemType[33];
word structMCnt;
byte byte_9D7B;
byte hasParams;
word paramCnt;
word curState;
byte endSeen;


word declNameCnt;
byte l6a68_flag;
info_pt parentStructure;

byte setTITLE();
void optInclude();
info_pt getNextInfo();

void start(void);
void sub_3EDF(void);
void sub_3F23(void);
void sub_3FAD(void);
void createTx1File(void);
void writeTx1(void *buf, word len);
void sub_4119(void);
void writeLineInfo(void);
void wrTx1R0user(void *buf, word len);
void wrByte(byte val);
void wrTx1R0user2(word val);
void wrTx1R0InfoOffset(address  val);
void syntaxError(byte errcode);
void tokenErrorAt(byte errcode);
void tokenError(byte errCode, symbol_pt  sym);
void pushBlock(word idAndLevel);
void popDO(void);
void wrOprAndValue(void);
void wrTx1XrefInfo(void);
void wrTx1R0Xref(void);
void nxtCh(void);
void badCmdTail(byte errCode);
void unknownCtrl(void);
void skipWhite(void);
void skipToRPARorEOL(void);
void acceptRPAR(void);
void accessOptStrVal(void);
void acceptFileName(void);
word asc2Num(char *firstch, char *lastch, byte radix);
byte chkRadix(char **plastch);
word parseNum(void);
void getOptNumVal(void);
void getToken(void);
void parseWord(byte maxlen);
void getval(void);
byte getlogical(void);
byte getTest(void);
byte chkNot(void);
word getIFVal(void);
byte parseIfCond(void);
void optPageLen(void);
void optPageWidth(void);
void optDate(void);
void optTitle(void);
byte setTITLE(void);
void optLeftMargin(void);
void optIXRef(void);
void optObject(void);
void optInclude(void);
void optPrint(void);
void optIntVector(void);
byte acceptDrive(char *filename, byte follow);
void acceptRangeChrs(byte low, byte high);
void optWorkFiles(void);
void optSave(void);
void optRestore(void);
void optSetReset(byte isSet);
void optIf(void);
void optElseElseIf(void);
void optEndIf(void);
void sub_550C(void);
void findOption(void);
void skipControlParam(void);
void doControls(void);
void do_Control(char *p);
void doCONDcompile(char *p);
void getNextChr(void);
byte inSrcCh(void);
void readSrcLine(void);
void getNewLine(void);
void getSrcLine(void);
void getCodeLine(void);
void yylex(void);
byte toupper(byte c);
void token2Num(void);
void _token2num(byte radix, byte validate[]);
void nestMacro(void);
byte chkMacro(void);
void getName(word maxlen);
void parseString(void);
void _yylex(void);
void setYyAgain(void);
byte yylexMatch(byte token);
byte yylexNotMatch(byte token);
void sub_60F9(void);
void parseExpresion(byte endTok);
void sub_61CF(void);
void sub_621A(void);
void parseDcl(void);
void sub_62B0(word errcode);
void chkModuleLevel(void);
void chkNotArray(void);
void chkNotStarDim(void);
void createStructMemberInfo(void);
void sub_63B7(word val);
void sub_65AF(void);
void sub_65FA(byte lexItem, byte flag);
void parseDclInitial(void);
void parseDclAt(void);
void parseDclScope(void);
void parseStructMType(void);
void parseStructMDim(void);
void _parseStructMem(void);
void parseStructMem(void);
void parseDclDataType(void);
void parseArraySize(void);
void parseDeclType(void);
void parseLiterally(void);
void parseLitOrType(void);
void getBaseInfo(void);
void sub_6A68(void);
void getDclNames(void);
void parseProcDcl(void);
void setPublic(void);
void setExternal(void);
void setReentrant(void);
void setInterruptNo(void);
void parseProcAttrib(void);
void parseRetType(void);
void sub_6E4B(void);
void parseParams(void);
void sub_6F00(void);
void skipToSemiColon(void);
void expectSemiColon(void);
void errorSkip(void);
void popStateWord(word * p);
void genLocalLabel(void);
void parseStmtLabels(void);
void parseStartStmt(void);
void sub_717F(void);
byte sub_723A(void);
void sub_7296(void);
void pushStateWord(word val);
void pushStateByte(byte state);
void createModuleInfo(symbol_pt  symptr);
void state0(void);
void state1(void);
void state2(void);
void state3(void);
void state19(void);
void state4(void);
void state5(void);
void state21(void);
void state6(void);
void state7(void);
void state8(void);
void state9(void);
void state10(void);
void state11(void);
void state20(void);
void state12(void);
void state13(void);
void state14(void);
void state15(void);
void state16(void);
void state17(void);
void state18(void);
void tellFile(file_t * fp, loc_t * loc);
void rewindFile(file_t * fp);
void backupPos(loc_t * lp, word cnt);
void closeFile(file_t * fp);
void assignFileBuffer(file_t * fp, void *buf, word size, byte mode);
//void flushFile(file_t * fp);
void ifwrite(file_t * fp, void *buf, word len);
byte getInfoType(void);
symbol_pt getInfoSymbol(void);
word getInfoScope(void);
address getLitaddr(void);
void setLitaddr(address  val);
byte testFlag(byte *base, byte flagId);
void clrFlags(byte *base);
void clrFlag(byte *base, byte flagId);
void cpyFlags(byte *base);
byte testInfoFlag(byte flagId);
void setDimension(word val);
void setBase(info_pt  val);
word getOwningStructure(void);
void setOwningStructure(info_pt  val);
byte getDataType(void);
void setIntrNo(byte intNo);
byte getParamCnt(void);
void setProcId(byte procId);
void findMemberInfo(void);
byte *createLit(unsigned char *pstr);
void sub_82D7(void);
void findScopedInfo(word val);
void findInfo(void);
void writeFile(file_t * fp, void *buf, word count);
word getInfoLen(void);

int ov0State;

int overlay0()
{
    ov0State = 20;
    try {
        sub_3FAD();
    }
    catch (...) {
    }
  sub_3EDF();
  return 1;
}


void sub_3EDF()
{
    if (unexpectedEOF)
        syntaxError(ERR87);	// MISSING 'END' , END-OF-FILE ENCOUNTERED
    writeLineInfo();
    wrByte(L_EOF);
    sub_4119();
    tellFile(&srcFile, (loc_t *) & srcFileTable[srcFileIdx].block);
    backupPos((loc_t *) & srcFileTable[srcFileIdx].block,
        (word) (inOffLastCh - inOffCurCh));
    closeFile(&srcFile);
}


void sub_3F23()
{
    static char nl = '\n';
    
    initFile(&srcFile, "SOURCE", srcFileTable[srcFileIdx].filename);
    openFile(&srcFile, 1);
    seekFile(&srcFile, (loc_t *) & srcFileTable[srcFileIdx].block);	// seek to position
    inOffCurCh = inOffLastCh;	// empty buffer
    if (offNextCmdChM1 != 0) {	// check for command line options
        while (cmdLine_p != 0) {
            do_Control(&cmdLine_p->text[offNextCmdChM1 - 3]);
            offNextCmdChM1 = 2;	// reset offset for next line
            cmdLine_p = cmdLine_p->link;	// chase link
        }
    }
    offNextCmdChM1 = 0;		// no more command line
    curProcData = &procData.Wrd;
    inChr_p = &nl - 1;		// point to just before \n
    blockDepth = 1;
    procChains[1] = 0;
    getNextChr();		// forces read of file
}

void sub_3FAD()
{
    sub_3F23();
    sub_6F00();
}

enum { CC_BINDIGIT = 0, CC_OCTDIGIT, CC_DECDIGIT, CC_HEXCHAR, CC_ALPHA, CC_PLUS, CC_MINUS, CC_STAR,
    CC_SLASH, CC_LPAREN, CC_RPAREN, CC_COMMA, CC_COLON, CC_SEMICOLON, CC_QUOTE, CC_PERIOD,
    CC_EQUALS, CC_LESS, CC_GREATER, CC_WSPACE, CC_DOLLAR, CC_INVALID, CC_NONPRINT, CC_NEWLINE };

byte cClass[] = {
    CC_NONPRINT, CC_NONPRINT, CC_NONPRINT, CC_NONPRINT,	// 0
    CC_NONPRINT, CC_NONPRINT, CC_NONPRINT, CC_NONPRINT,
    CC_NONPRINT, CC_WSPACE, CC_NEWLINE, CC_NONPRINT,
    CC_NONPRINT, CC_WSPACE, CC_NONPRINT, CC_NONPRINT,
    CC_NONPRINT, CC_NONPRINT, CC_NONPRINT, CC_NONPRINT,	// 10
    CC_NONPRINT, CC_NONPRINT, CC_NONPRINT, CC_NONPRINT,
    CC_NONPRINT, CC_NONPRINT, CC_NONPRINT, CC_NONPRINT,
    CC_NONPRINT, CC_NONPRINT, CC_NONPRINT, CC_NONPRINT,
    CC_WSPACE, CC_INVALID, CC_INVALID, CC_INVALID, 		// 20
    CC_DOLLAR, CC_INVALID, CC_INVALID, CC_QUOTE,
    CC_LPAREN,  CC_RPAREN,  CC_STAR,  CC_PLUS,
    CC_COMMA,  CC_MINUS, CC_PERIOD,  CC_SLASH,
    CC_BINDIGIT, CC_BINDIGIT, CC_OCTDIGIT, CC_OCTDIGIT,	// 30
    CC_OCTDIGIT, CC_OCTDIGIT, CC_OCTDIGIT, CC_OCTDIGIT,
    CC_DECDIGIT, CC_DECDIGIT, CC_COLON, CC_SEMICOLON,
    CC_LESS, CC_EQUALS, CC_GREATER, CC_INVALID,
    CC_INVALID, CC_HEXCHAR, CC_HEXCHAR, CC_HEXCHAR,	// 40
    CC_HEXCHAR, CC_HEXCHAR, CC_HEXCHAR, CC_ALPHA,
    CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_ALPHA,
    CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_ALPHA,
    CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_ALPHA,			// 50
    CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_ALPHA,	
    CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_INVALID,
    CC_INVALID, CC_INVALID, CC_INVALID, CC_INVALID,		
    CC_INVALID, CC_HEXCHAR, CC_HEXCHAR, CC_HEXCHAR,	// 60
    CC_HEXCHAR, CC_HEXCHAR, CC_HEXCHAR, CC_ALPHA,
    CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_ALPHA,
    CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_ALPHA,
    CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_ALPHA,			// 70
    CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_ALPHA,
    CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_INVALID,
    CC_INVALID, CC_INVALID, CC_INVALID, CC_NONPRINT
};

byte tok2oprMap[] =
    { L_VARIABLE, L_NUMBER, L_STRING, L_PLUSSIGN,
    L_MINUSSIGN, L_STAR, L_SLASH, L_MOD,
    L_PLUS, L_MINUS, L_AND, L_OR,
    L_XOR, L_NOT, L_XREF, L_LT,
    L_LE, L_EQ, L_NE, L_GE,
    L_GT, L_COLONEQUALS, L_INVALID, L_INVALID,
    L_PERIOD, L_LPAREN, L_RPAREN, L_COMMA,
    L_CALL, L_INVALID, L_DISABLE, L_INVALID,
    L_ENABLE, L_END, L_GO, L_GOTO,
    L_HALT, L_IF, L_PROCEDURE, L_RETURN,
    L_INVALID, L_INVALID, L_INVALID, L_INVALID,
    L_INVALID, L_INVALID, L_INVALID, L_INVALID,
    L_INVALID, L_INVALID, L_INVALID, L_INVALID,
    L_INVALID, L_BY, L_INVALID, L_INVALID,
    L_INVALID, L_INVALID, L_TO, L_INVALID
};

void createTx1File()
{
//    word tmp = tx1File.curoff;
    openFile(&tx1File, 3);
//    assignFileBuffer(&tx1File, tx1Buf, 1280, 2);	// assign buffers but don't read
//    tx1File.curoff = tmp;
}


void writeTx1(void *buf, word len)
{
    if (tx1File.aftn == 0)
        createTx1File();
    ifwrite(&tx1File, buf, len);

    //if (tx1File.aftn == 0) {	// no file created
    //    if (tx1File.curoff > 1024)
    //        createTx1File();
    //    else {
    //        movemem(len, buf, &tx1Buf[tx1File.curoff]);
    //        tx1File.curoff += len;
    //        return;
    //    }
    //}
    //ifwrite(&tx1File, buf, len);
}


void sub_4119()
{
    if (tx1File.aftn == 0)
        createTx1File();
//    flushFile(&tx1File);
    rewindFile(&tx1File);
}


void writeLineInfo()
{
    if (lineInfoToWrite) {
        writeTx1(&linfo, 7);
        lineInfoToWrite = 0;
    }
}

// write    recordtype 0 + user data to tx1

void wrTx1R0user(void *buf, word len)
{
    writeLineInfo();
    writeTx1(buf, len);
}

// write    record 0 to tx1 + 1 user byte

void wrByte(byte val)
{
    wrTx1R0user(&val, 1);
}



void wrTx1R0user2(word val)
{
    wrTx1R0user(&val, 2);
}

void wrTx1R0InfoOffset(address  val)
{
    wrTx1R0user2(info2Off(val));
}

void syntaxError(byte errcode)
{
    hasErrors = 0xff;
    wrByte(L_SYNTAXERROR);
    wrTx1R0user2(errcode);
}

void tokenErrorAt(byte errcode)
{
    hasErrors = 0xff;
    wrByte(L_TOKENERROR);
    wrTx1R0user2(errcode);
    wrTx1R0user2(markedSymbol_p);
}

void tokenError(byte errCode, symbol_pt  sym)
{
    hasErrors = 0xff;
    wrByte(L_TOKENERROR);
    wrTx1R0user2(errCode);
    wrTx1R0user2(address(sym));
}



void pushBlock(word idAndLevel)
{
    if (blockDepth == 0x22)
        fatalError(ERR84);	// LIMIT EXCEEDED: BLOCK NESTING
    else {
        procChains[++blockDepth] = idAndLevel;
        curBlkCnt++;
    }
}


void popDO()
{
    if (blockDepth == 0)
        fatalError(ERR96);	// COMPILER ERROR: SCOPE STACK UNDERFLOW
    else {
        curBlkCnt--;
        *curProcData = procChains[--blockDepth];
    }
}


void wrOprAndValue()
{
    if (tok2oprMap[tokenType] == L_INVALID) {
        tokenErrorAt(ERR171);	// INVALID USE OF DELIMITER OR RESERVED WORD IN EXPRESSION 
        return;
    }
    wrByte(tok2oprMap[tokenType]);
    if (tokenType == T_VARIABLE)
        wrTx1R0user2(curSymbol_p);
    else if (tokenType == T_NUMBER)
        wrTx1R0user2(tokenVal);
    else if (tokenType == T_STRING) {
        wrTx1R0user2(tokenStr[0]);
        wrTx1R0user(&tokenStr[1], tokenStr[0]);
    }
}


void wrTx1XrefInfo()
{
    if (XREF) {
        wrByte(L_XREFINFO);
        wrTx1R0InfoOffset(curInfo_p);
    }
}


void wrTx1R0Xref()
{
    if (IXREF || XREF || SYMBOLS) {
        wrByte(L_XREF);
        wrTx1R0InfoOffset(curInfo_p);
    }
}

byte controls[] = {
            5, 'P', 'R', 'I', 'N', 'T', 0, 0, 0xFF, 7, 0,
        7, 'N', 'O', 'P', 'R', 'I', 'N', 'T', 0, 0, 0xFF, 8, 0,
        4, 'L', 'I', 'S', 'T', 3, 1, 0, 0, 0,
        6, 'N', 'O', 'L', 'I', 'S', 'T', 4, 1, 0, 0, 0,
        4, 'C', 'O', 'D', 'E', 5, 1, 0, 0, 0,
        6, 'N', 'O', 'C', 'O', 'D', 'E', 6, 1, 0, 0, 0,
        4, 'X', 'R', 'E', 'F', 0, 0, 1, 1, 1,
        6, 'N', 'O', 'X', 'R', 'E', 'F', 0, 0, 1, 0, 1,
        7, 'S', 'Y', 'M', 'B', 'O', 'L', 'S', 0, 0, 2, 1, 2,
        9, 'N', 'O', 'S', 'Y', 'M', 'B', 'O', 'L', 'S', 0, 0, 2, 0, 2,
        5, 'D', 'E', 'B', 'U', 'G', 0, 0, 3, 1, 3,
        7, 'N', 'O', 'D', 'E', 'B', 'U', 'G', 0, 0, 3, 0, 3,
        6, 'P', 'A', 'G', 'I', 'N', 'G', 0, 0, 4, 1, 4,
        8, 'N', 'O', 'P', 'A', 'G', 'I', 'N', 'G', 0, 0, 4, 0, 4,
        0xA, 'P', 'A', 'G', 'E', 'L', 'E', 'N', 'G', 'T', 'H', 0, 0, 0xFF, 0,
        5,
        9, 'P', 'A', 'G', 'E', 'W', 'I', 'D', 'T', 'H', 0, 0, 0xFF, 1, 6,
        4, 'D', 'A', 'T', 'E', 0, 0, 0xFF, 2, 7,
        5, 'T', 'I', 'T', 'L', 'E', 0, 0, 0xFF, 3, 8,
        5, 'E', 'J', 'E', 'C', 'T', 7, 1, 0, 0, 0,
        0xA, 'L', 'E', 'F', 'T', 'M', 'A', 'R', 'G', 'I', 'N', 0, 1, 0xFF, 4,
        0,
        6, 'O', 'B', 'J', 'E', 'C', 'T', 0, 0, 0xFF, 5, 9,
        8, 'N', 'O', 'O', 'B', 'J', 'E', 'C', 'T', 0, 0, 0xFF, 9, 9,
        8, 'O', 'P', 'T', 'I', 'M', 'I', 'Z', 'E', 0, 0, 6, 1, 0xA,
        0xA, 'N', 'O', 'O', 'P', 'T', 'I', 'M', 'I', 'Z', 'E', 0, 0, 6, 0, 0xA,
        7, 'I', 'N', 'C', 'L', 'U', 'D', 'E', 0, 1, 0xFF, 6, 0,
        9, 'W', 'O', 'R', 'K', 'F', 'I', 'L', 'E', 'S', 0, 0, 0xFF, 0xA, 0xB,
        9, 'I', 'N', 'T', 'V', 'E', 'C', 'T', 'O', 'R', 0, 0, 0xFF, 0xB, 0xC,
        0xB, 'N', 'O', 'I', 'N', 'T', 'V', 'E', 'C', 'T', 'O', 'R', 0, 0, 0xFF,
        0xC,
        0xC,
        5, 'I', 'X', 'R', 'E', 'F', 0, 0, 0xFF, 0xD, 0xD,
        7, 'N', 'O', 'I', 'X', 'R', 'E', 'F', 0, 0, 0xFF, 0xE, 0xD,
        4, 'S', 'A', 'V', 'E', 0, 1, 0xFF, 0xF, 0,
        7, 'R', 'E', 'S', 'T', 'O', 'R', 'E', 0, 1, 0xFF, 0x10, 0,
        3, 'S', 'E', 'T', 0, 1, 0xFF, 0x11, 0,
        5, 'R', 'E', 'S', 'E', 'T', 0, 1, 0xFF, 0x12, 0,
        2, 'I', 'F', 0, 1, 0xFF, 0x13, 0,
        6, 'E', 'L', 'S', 'E', 'I', 'F', 0, 1, 0xFF, 0x14, 0,
        4, 'E', 'L', 'S', 'E', 0, 1, 0xFF, 0x14, 0,
        5, 'E', 'N', 'D', 'I', 'F', 0, 1, 0xFF, 0x15, 0,
        4, 'C', 'O', 'N', 'D', 0, 1, 0xFF, 0x16, 0,
        6, 'N', 'O', 'C', 'O', 'N', 'D', 0, 1, 0xFF, 0x17, 0,
        0
};

void nxtCh()
{
    if (chrClass == CC_NEWLINE)
        return;
    if (*++curCh_p == '\r' || (offNextCmdChM1 != 0 && *curCh_p == '&')) {
        chrClass = CC_NEWLINE;
        return;
    }
    
    if ((chrClass = cClass[*curCh_p]) == CC_NONPRINT)
        *curCh_p = ' ';
    
    if (*curCh_p >= 'a')
        *curCh_p &= 0x5f;
}


void badCmdTail(byte errCode)
{
    if (offNextCmdChM1 != 0)	// parsing command line
        fatal("ILLEGAL COMMAND TAIL SYNTAX OR VALUE", 0x24);
    else
        syntaxError(errCode);
}

void unknownCtrl()
{
    if (offNextCmdChM1 != 0)	// parsing command line
        fatal("UNRECOGNIZED CONTROL IN COMMAND TAIL", 0x24);
    else
        syntaxError(ERR9);	// INVALID CONTROL
}


void skipWhite()
{
    while (*curCh_p == ' ')
        nxtCh();
}


void skipToRPARorEOL()
{
    while (*curCh_p != '\r' && *curCh_p != ')')
        nxtCh();
    if (*curCh_p == ')')
        nxtCh();
}


void acceptRPAR()
{
    skipWhite();
    if (*curCh_p != ')') {
        badCmdTail(ERR12);	// INVALID CONTROL PARAMETER
        skipToRPARorEOL();
    } else
        nxtCh();
}



void accessOptStrVal()
{
    byte nesting = 0;
    skipWhite();
    if (*curCh_p != '(') {
        tknLen = 0;
        return;
    }
    nxtCh();
    optStrVal_p = curCh_p;
    
    while (chrClass != CC_NEWLINE && *curCh_p != '\'') {
        if (*curCh_p == '(')
            nesting++;
        else if (*curCh_p == ')') {
            if (nesting == 0)
                break;
            nesting--;
        }
        nxtCh();
    }
    tknLen = (byte)(curCh_p - optStrVal_p);
    acceptRPAR();
}



void acceptFileName()
{
    skipWhite();
    if (*curCh_p != '(')
        tknLen = 0;		// no parameter
    else {
        nxtCh();
        skipWhite();
        optStrVal_p = curCh_p;	// save start of token
        while (*curCh_p != ' ' && *curCh_p != ')' && chrClass != CC_NEWLINE)
            nxtCh();		// collect until space, ) or EOL
        if ((tknLen = (byte)(curCh_p - optStrVal_p)) > 14) {	// allow  max 14 chars
            badCmdTail(ERR17);	// INVALID PATH-NAME
            tknLen = 0;
        } else {
            FILL(16, optFileName, ' ');	// clear filename
            movemem(tknLen, optStrVal_p, optFileName);	// copy filename
            optStrVal_p = optFileName;
            tknLen++;		// allow for a space at the end
        }
        acceptRPAR();
    }
}



word asc2Num(char *firstch, char *lastch, byte radix)
{
    word num, trial;
    byte digit;
    
    if (firstch > lastch || radix == 0)
        return 0xffff;
    
    num = 0;
    while (firstch <= lastch) {
        if (cClass[*firstch] <= CC_DECDIGIT)	// digits
            digit = *firstch - '0';
        else if (cClass[*firstch] < CC_ALPHA)	// hex digits
            digit = *firstch - 'A' + 10;
        else
            return 0xffff;
        if (digit >= radix)
            return 0xffff;
        trial = num * radix + digit;
        if (trial < digit || (trial - digit) / radix != num)
            return 0xffff;
        num = trial;
        firstch++;
    }
    return num;
}


byte chkRadix(char **plastch)
{
    char *p = *plastch;
    
    if (cClass[*p] <= CC_DECDIGIT)		// digit
        return 10;		// assume decimal no need to remove last char
    --*plastch;
    if (*p == 'B')
        return 2;
    if (*p == 'Q' || *p == 'O')
        return 8;
    if (*p == 'H')
        return 16;
    if (*p == 'D')
        return 10;
    return 0;
}


word parseNum()
{
    char *firstch, *lastch;
    byte radix;
    
    nxtCh();
    skipWhite();
    firstch = curCh_p;
    while (chrClass <= CC_ALPHA)	// alphanumeric
        nxtCh();
    lastch = curCh_p - 1;
    skipWhite();
    radix = chkRadix(&lastch);
    return asc2Num(firstch, lastch, radix);
}


void getOptNumVal()
{
    skipWhite();
    optNumValue = 0;
    if (*curCh_p != '(')
        badCmdTail(ERR11);	// MISSING CONTROL PARAMETER
    else {
        optNumValue = parseNum();
        acceptRPAR();
    }
}



void getToken()
{
    optStrVal_p = curCh_p;
    while (*curCh_p != ' ' && *curCh_p != '(' && chrClass != CC_NEWLINE)
        nxtCh();
    
    tknLen = (byte)(curCh_p - optStrVal_p);
}



void parseWord(byte maxlen)
{
    static char pstr[33];
    
    optStrVal_p = &pstr[1];
    tknLen = 0;
    skipWhite();
    if (chrClass == CC_HEXCHAR || chrClass == CC_ALPHA) {	// A-Z or a-z
        while (chrClass <= CC_ALPHA || chrClass == CC_DOLLAR /* $ */ ) {
            if (chrClass != CC_DOLLAR && maxlen >= tknLen) {
                pstr[1 + tknLen] = *curCh_p;
                tknLen++;
            }
            nxtCh();
        }
    }
    pstr[0] = (maxlen < tknLen) ? maxlen : tknLen;
}

void getval()
{
    char *tmp = curCh_p - 1;
    parseWord(31);
    if (tknLen == 0) {		// oops no word
        curInfo_p = 0xffff;
        curCh_p = tmp;
        chrClass = 0;
        nxtCh();
        return;
    }
    if (tknLen > 31) {
        tknLen--;		// reduce back to 31
        badCmdTail(ERR184);	// CONDITIONAL COMPILATION PARAMETER NAME TOO LONG
    }
    lookup((unsigned char *)optStrVal_p - 1);	// -1 to point to length
    /// TO FIX use union
    if (((curSymbol_p->infoChain) >> 8) == 0xff) {	// reserved word highbyte == 0xff
        curInfo_p = 0xffff;
        curCh_p = tmp;
        chrClass = 0;
        nxtCh();
        return;
    }
    findScopedInfo(1);
}


// 0 none, 1 OR, 2 AND, 3 XOR, 4 error

byte getlogical()
{
    parseWord(3);
    if (tknLen == 0 && chrClass == CC_NEWLINE /* EOL */ )
        return 0;
    
    if (tknLen == 2) {
        if (strncmp(optStrVal_p, "OR", 2) == 0)
            return 1;
    } else if (tknLen == 3) {
        if (strncmp(optStrVal_p, "AND", 3) == 0)
            return 2;
        else if (strncmp(optStrVal_p, "XOR", 3) == 0)
            return 3;
    }
    badCmdTail(ERR185);		// MISSING OPERATOR IN CONDITIONAL COMPILATION EXPRESSION 
    skipToRPARorEOL();
    return 4;
}


// 0 non, 1 =, 2 <. 3 <=, 4 >, 5 >=, 6 <>

byte getTest()
{
    byte test = 0;
    
    skipWhite();
    if (*curCh_p == '<') {
        nxtCh();
        if (*curCh_p == '>') {
            nxtCh();
            return 6;
        } else
            test = 2;
    } else if (*curCh_p == '>') {
        nxtCh();
        test = 4;
    }
    if (*curCh_p == '=') {
        nxtCh();
        test++;
    }
    return test;
}


byte chkNot()
{
    byte notStatus = 0;
    
    for (;;) {
        char *tmp = curCh_p - 1;
        parseWord(3);
        if (tknLen != 3 || strncmp(optStrVal_p, "NOT", 3) != 0) {
            curCh_p = tmp;
            return notStatus;	// no more NOTs backup and return status;
        }
        notStatus = !notStatus;
    }
}

word getIFVal()
{
    word val;
    
    chrClass = 0;
    nxtCh();
    skipWhite();
    if (chrClass <= CC_HEXCHAR) {	// not a digit
        curCh_p--;		// backup to redo digit
        if ((val = parseNum()) > 255)
            badCmdTail(ERR186);	// INVALID CONDITIONAL COMPILATION CONSTANT, TOO LARGE 
        return val & 0xff;
    }
    getval();
    if (curInfo_p == 0xffff) {
        badCmdTail(ERR180);	// MISSING OR INVALID CONDITIONAL COMPILATION PARAMETER 
        skipToRPARorEOL();
        return 256;
    }
    if (curInfo_p == 0)
        return 0;
    return getCondFlag();
}


byte parseIfCond()
{
    byte andFactor = 0xff, orFactor = 0, xorFactor = 0;
    byte not1, not2, relOp;
    word val1, val2;
    
    for (;;) {
        not1 = chkNot();
        if ((val1 = getIFVal()) > 255)
            return 255;
        
        relOp = getTest();	// 0 non, 1 =, 2 <, 3 <=, 4 >, 5 >=, 6 <>
        if (relOp > 0) {	// check we need a 2nd value
            not2 = chkNot();
            if ((val2 = getIFVal()) > 255)
                return 255;
            val2 ^= not2;
            switch (relOp - 1) {
            case 0:
                val1 = (val1 == val2);
                break;
            case 1:
                val1 = (val1 < val2);
                break;
            case 2:
                val1 = (val1 <= val2);
                break;
            case 3:
                val1 = (val1 > val2);
                break;
            case 4:
                val1 = (val1 >= val2);
                break;
            case 5:
                val1 = (val1 != val2);
                break;
            }
        }
        val1 = andFactor & (not1 ^ val1);
        andFactor = 0xff;	// reset mask
        switch (getlogical()) {	// 0 none, 1 OR, 2 AND, 3 XOR, 4 error 
        case 0:
            return xorFactor ^ (orFactor | val1);
        case 1:
            orFactor = xorFactor ^ (orFactor | val1);
            break;
        case 2:
            andFactor = (byte) val1;
            break;
        case 3:
            xorFactor = xorFactor ^ (orFactor | val1);
            break;
        case 4:
            return 0xff;
        }
    }
}


void optPageLen()
{
    getOptNumVal();
    if (optNumValue < 4 || optNumValue == 0xffff)
        badCmdTail(ERR91);	// ILLEGAL PAGELEGTH CONTROL VALUE
    else
        setPAGELEN((word) (optNumValue - 3));
}


void optPageWidth()
{
    getOptNumVal();
    if (optNumValue < 60 || optNumValue == 0xffff || optNumValue > 132)
        badCmdTail(ERR92);	// ILLEGAL PAGEWIDTH CONTROL VALUE
    else
        setPAGEWIDTH(optNumValue);
}


void optDate()
{
    accessOptStrVal();
    setDATE(optStrVal_p, tknLen);
}

void optTitle()
{
    skipWhite();
    if (*curCh_p != '(')
        badCmdTail(ERR11);	// MISSING CONTROL PARAMETER
    else if (!setTITLE()) {
        badCmdTail(ERR12);	// INVALID CONTROL PARAMETER
        skipToRPARorEOL();
    }
}


byte setTITLE()
{
    byte len;
    nxtCh();
    if (*curCh_p != '\'')	// should start with a quote
        return 0;
    
    for (len = 0; nxtCh(), *curCh_p != '\r';) {
        if (*curCh_p == '\'' && (nxtCh(), *curCh_p != '\''))	// check for double quotes
            break;
        if (len <= 59)
            TITLE[len++] = *curCh_p;
    }
    if (len != 0)
        TITLELEN = len;
    else {
        TITLELEN = 1;
        TITLE[0] = ' ';
    }
    if (*curCh_p != ')')
        return 0;
    nxtCh();
    return 0xff;
}



void optLeftMargin()
{
    getOptNumVal();
    if (optNumValue == 0) {
        badCmdTail(ERR12);	// INVALID CONTROL PARAMETER
        optNumValue = 1;
    } else if (optNumValue == 0xffff) {
        badCmdTail(ERR12);	// INVALID CONTROL PARAMETER
        return;
    }
    LEFTMARGIN = optNumValue;
}


void optIXRef()
{
    acceptFileName();
    // use user's file name if given else default
    initFile(&ixiFile, "IXREF ", tknLen ? optStrVal_p : ixiFileName);
    IXREFSet = 0xff;
    IXREF = 0xff;
    openFile(&ixiFile, 2);
    ixiGiven = 0xff;
}



void optObject()
{
    acceptFileName();
    // use user's file name if given else default
    initFile(&objFile, "OBJECT ", tknLen ? optStrVal_p : objFileName);
    objByte = objBlk = 0;
    OBJECT = 0xff;
    OBJECTSet = 0xff;
    openFile(&objFile, 2);
    objGiven = 0xff;
}


void optInclude()
{
    acceptFileName();
    if (tknLen == 0) {
        badCmdTail(ERR15);	// MISSING INCLUDE CONTROL PARAMETER
        return;
    }
    if (optFileName[0] == ':' && optFileName[1] != 'F') {
        if (offNextCmdChM1 != 0)
            fatal("INCLUDE FILE IS NOT A DISKETTE FILE", 0x23);
        else
            fatalError(ERR98);	// INCLUDE FILE IS NOT A DISKETTE FILE
    }
    if (srcFileIdx >= 5)	// check for room in file list
        syntaxError(ERR13);	// LIMIT EXCEEDED: INCLUDE NESTING
    else {
        tellFile(&srcFile, (loc_t *) & srcFileTable[srcFileIdx].block);
        backupPos((loc_t *) & srcFileTable[srcFileIdx].block,
            (word) (inOffLastCh - inOffCurCh));
        srcFileIdx++;
        movemem(tknLen, optStrVal_p, srcFileTable[srcFileIdx].filename);
        closeFile(&srcFile);
        initFile(&srcFile, "SOURCE", optStrVal_p);
        openFile(&srcFile, 1);
        inOffCurCh = inOffLastCh;	// force read next time
        wrByte(L_INCLUDE);
        wrTx1R0user(& optStrVal_p[12], 6);	// write name backwards in blocks of 6 chars!!
        wrByte(L_INCLUDE);
        wrTx1R0user(& optStrVal_p[6], 6);
        wrByte(L_INCLUDE);
        wrTx1R0user(& optStrVal_p[0], 6);
    }
    skipWhite();
    if (*curCh_p != '\r')
        badCmdTail(ERR14);	// INVALID CONTROL FORMAT, INCLUDE NOT LAST CONTROL
}


void optPrint()
{
    acceptFileName();
    if (lstFileOpen)
        badCmdTail(ERR16);	// ILLEGAL PRINT CONTROL
    else {
        if (tknLen != 0)	// check if file name
            initFile(&lstFile, "LIST ", optStrVal_p);
        else
            initFile(&lstFile, "PRINT ", lstFileName);
        PRINTSet = 0xff;
    }
    PRINT = 0xff;		// flag PRINT seen
    openFile(&lstFile, 2);
    lstGiven = 0xff;
}


void optIntVector()
{
    word vecInterval, vecLoc;
    skipWhite();
    if (*curCh_p != '(')
        badCmdTail(ERR11);	// MISSING CONTROL PARAMETER
    else {
        vecInterval = parseNum();
        if (vecInterval != 4 && vecInterval != 8) {
            badCmdTail(ERR176);	// INVALID INTVECTOR INTERVAL VALUE
            skipToRPARorEOL();
        } else if (*curCh_p != ',') {
            badCmdTail(ERR177);	// INVALID INTVECTOR LOCATION VALUE
            skipToRPARorEOL();
        }
        else if ((vecLoc = parseNum()) > 0xffe0
            || vecLoc % (vecInterval * 8) != 0 || *curCh_p != ')') {
            badCmdTail(ERR177);	// INVALID INTVECTOR LOCATION VALUE
            skipToRPARorEOL();
        } else {
            nxtCh();
            intVecNum = (byte) vecInterval;
            intVecLoc = vecLoc;
        }
    }
}


byte l51dd_isOK;
char *l51dd_filename;

byte acceptDrive(char *filename, byte follow)
{
    l51dd_filename = filename;
    l51dd_isOK = 0xff;
    skipWhite();
    acceptRangeChrs(':', ':');
    acceptRangeChrs('F', 'F');
    acceptRangeChrs('0', '9');
    acceptRangeChrs(':', ':');
    skipWhite();
    acceptRangeChrs(follow, follow);
    return l51dd_isOK;
}

void acceptRangeChrs(byte low, byte high)
{
    if (l51dd_isOK) {
        if (*curCh_p < low || high < *curCh_p) {
            badCmdTail(ERR12);	// INVALID CONTROL PARAMETER
            skipToRPARorEOL();
            l51dd_isOK = 0;
        } else {
            *l51dd_filename++ = *curCh_p;
            nxtCh();
        }
    }
}


void optWorkFiles()
{
    skipWhite();
    if (*curCh_p != '(')
        badCmdTail(ERR11);	// MISSING CONTROL PARAMETER
    else {
        nxtCh();
        if (!acceptDrive(wrkFiles1, ',') || !acceptDrive(wrkFiles2, ')'))
            return;
        if (tx1File.aftn != 0)	// already opened the tx1 file
            fatal("MISPLACED CONTROL: WORKFILES ALREADY OPEN", 0x29);
        movemem(4, wrkFiles1, atFile.fullName);	// replace the drive names
        movemem(4, wrkFiles1, nmsFile.fullName);
        movemem(4, wrkFiles1, tx1File.fullName);
        movemem(4, wrkFiles1, xrfFile.fullName);
        movemem(4, wrkFiles1, tx2File.fullName);
    }
}



void optSave()
{
    if (saveDepth >= 5)
        badCmdTail(ERR187);	// LIMIT EXCEEDED: NUMBER OF SAVE LEVELS > 5
    else {
        saveStack[saveDepth].code = CODE;
        saveStack[saveDepth].list = LIST;
        saveStack[saveDepth].cond = COND;
        saveStack[saveDepth].leftmargin = LEFTMARGIN;
    }
}



void optRestore()
{
    if (saveDepth == 0)
        badCmdTail(ERR188);	// MISPLACED RESTORE OPTION
    else {
        CODE = saveStack[--saveDepth].code;
        LIST = saveStack[saveDepth].list;
        COND = saveStack[saveDepth].cond;
        LEFTMARGIN = saveStack[saveDepth].leftmargin;
        wrByte((byte) (CODE ? L_CODE : L_NOCODE));	// flag the new values in workfile
        wrByte((byte) (LIST ? L_LIST : L_NOLIST));
    }
}


void optSetReset(byte isSet)
{
    word val;
    
    skipWhite();
    if (*curCh_p != '(')
        badCmdTail(ERR11);	// MISSING CONTROL PARAMETER
    else {
        do {
            nxtCh();
            getval();		// get parameter and locate in symbol table
            if (curInfo_p == 0xffff) {
                badCmdTail(ERR180);	// MISSING OR INVALID CONDITIONAL COMPILATION PARAMETER 
                skipToRPARorEOL();
                return;
            }
            if (curInfo_p == 0)	// create a temp entry
                createInfo(1, TEMP_T);
            skipWhite();
            if (*curCh_p == '=' && isSet) {	// seen = and we are setting
                val = parseNum();
                if (val > 255) {
                    badCmdTail(ERR181);	// MISSING OR INVALID CONDITIONAL COMPILATION CONSTANT 
                    skipToRPARorEOL();
                    return;
                }
                setCondFlag((byte) val);
            } else
                setCondFlag(isSet);
        }
        while (*curCh_p == ',');
        acceptRPAR();
    }
}


void optIf()
{
    ifDepth++;
    if (!parseIfCond()) {
        skippingCOND = 0xff;
        inIFpart = 0xff;
        skippingCONDDepth = ifDepth;
        if (!COND && LIST)
            wrByte(L_NOLIST);
    }
}


void optElseElseIf()
{
    if (ifDepth == 0)		// no if !!
        badCmdTail(ERR182);	// MISPLACED ELSE OR ELSEIF OPTION
    else {
        skippingCOND = 0xff;
        inIFpart = 0;
        skippingCONDDepth = ifDepth;
        if (!COND && LIST)
            wrByte(L_NOLIST);
    }
    while (*curCh_p != '\r')	// junk rest of line
        nxtCh();
}


void optEndIf()
{
    if (ifDepth == 0)
        badCmdTail(ERR183);	// MISPLACED ENDIF OPTION
    else
        ifDepth--;
}

void sub_550C()
{
    switch (tknFlags_p->controlVal) {
    case 0:
        optPageLen();		// pagelen
        break;
    case 1:
        optPageWidth();		// pagewidth
        break;
    case 2:
        optDate();		// date
        break;
    case 3:
        optTitle();		// title
        break;
    case 4:
        optLeftMargin();	// leftmargin
        break;
    case 5:
        optObject();		// object
        break;
    case 6:
        optInclude();		// include
        break;
    case 7:
        optPrint();		// print
        break;
    case 8:
        PRINT = 0;		// noprint
        PRINTSet = 0;
        break;
    case 9:
        OBJECT = 0;		// noobject
        OBJECTSet = 0;
        break;
    case 10:
        optWorkFiles();		// workfiles
        break;
    case 11:
        optIntVector();		// intvector
        break;
    case 12:			// nointvector
        intVecNum = 0;
        break;
    case 13:
        optIXRef();		// ixref
        break;
    case 14:			// noixref
        IXREF = 0;
        IXREFSet = 0;
        break;
    case 15:
        optSave();		// save;
        break;
    case 16:
        optRestore();		// restore
        break;
    case 17:
        optSetReset(0xff);	// set
        break;
    case 18:
        optSetReset(0);		// reset
        break;
    case 19:
        optIf();		// if
        break;
    case 20:
        optElseElseIf();	// else or elseif
        break;
    case 21:
        optEndIf();		// endif
        break;
    case 22:
        COND = 0xff;		// cond
        break;
    case 23:
        COND = 0;		// nocond
        break;
    }
}


void findOption()
{
    word off = 0;
    word clen;
    
    for (clen = controls[0]; clen; clen = controls[off]) {
        if (tknLen == clen
            && strncmp(optStrVal_p, (char *) &controls[off + 1],
            (byte) clen) == 0) {
            tknFlags_p = (control_t *) & controls[off + clen + 1];
            return;
        }
        off += clen + 6;	// past token string & 5 bytes of data
    }
    tknFlags_p = 0;
    return;
}



void skipControlParam()
{
    skipWhite();
    if (*curCh_p == '(') {
        nxtCh();
        skipToRPARorEOL();
    }
}


void doControls()
{
    getToken();
    if (tknLen == 0) {
        badCmdTail(ERR8);	// INVALID CONTROL FORMAT
        skipControlParam();
        return;
    }
    findOption();
    if (tknFlags_p == 0) {
        unknownCtrl();		// not found
        skipControlParam();
        return;
    }
    
    if (tknFlags_p->primary == 0) {	// primary control ?
        if (isNonCtrlLine) {
            syntaxError(ERR10);	// ILLEGAL USE OF PRIMARY CONTROL AFTER NON-CONTROL LINE
            skipControlParam();
            return;
        }
        if (primaryCtrlSeen[tknFlags_p->PrimaryId]) {
            badCmdTail(ERR95);	// ILLEGAL RESPECIFICATION OF PRIMARY CONTROL IGNORED
            skipControlParam();
            return;
        }
        primaryCtrlSeen[tknFlags_p->PrimaryId] = 0xff;
    }
    if (tknFlags_p->controlId != 0xff) {
        if (tknFlags_p->tokenId == 0) {
            assert(tknFlags_p->controlId <= 7);
            CONTROLS[tknFlags_p->controlId] = tknFlags_p->controlVal;
        }
        else {
            wrByte(tknFlags_p->tokenId);
            if (tknFlags_p->tokenId == 5)
                CODE = 0xff;
            else if (tknFlags_p->tokenId == 6)
                CODE = 0;
            else if (tknFlags_p->tokenId == 3)
                LIST = 0xff;
            else if (tknFlags_p->tokenId == 4)
                LIST = 0;
        }
    } else
        sub_550C();
}


void do_Control(char *p)
{
    curCh_p = p;
    chrClass = 0;
    lstGiven = objGiven = ixiGiven = 0;
    nxtCh();
    
    while (skipWhite(), chrClass != CC_NEWLINE)
        doControls();
    if (ixiGiven)
        closeFile(&ixiFile);
    if (objGiven)
        closeFile(&objFile);
    if (lstGiven)
        closeFile(&lstFile);
}


void doCONDcompile(char *p)
{
    curCh_p = p;
    if (*curCh_p == '$') {	// only look at $ lines
        chrClass = 0;
        nxtCh();
        skipWhite();
        getToken();
        if (tknLen == 2 && strncmp(optStrVal_p, "IF", 2) == 0)
            ifDepth++;
        else if (tknLen == 5 && strncmp(optStrVal_p, "ENDIF", 5) == 0) {
            if (--ifDepth < skippingCONDDepth)
                skippingCOND = 0;
        } else if (ifDepth == skippingCONDDepth && inIFpart)
            if (tknLen == 4 && strncmp(optStrVal_p, "ELSE", 4) == 0)
                skippingCOND = 0;
            else if (tknLen == 6 && strncmp(optStrVal_p, "ELSEIF", 6) == 0)
                skippingCOND = ~parseIfCond();
            if (!skippingCOND && !COND && LIST)
                wrByte(L_LIST);
    }
}


void getNextChr()
{
    inChr_p++;
    while (*inChr_p == '\n')
        getNewLine();
    savedCh = *inChr_p;
}


// get next char into m[inChr$p]
// char is 0x81 on EOF, cr is discarded

byte inSrcCh()
{
    byte c;
    do {
        if (inOffCurCh == inOffLastCh) {
            inOffCurCh = 0;
            readFile(&srcFile, inbuf, 512, &inOffLastCh);
            if (inOffLastCh == 0) {
                return *inChr_p = ISISEOF;	// EOF
            } else
                inOffLastCh--;	// base from 0
        } else
            inOffCurCh++;
    }
    while ((c = inbuf[inOffCurCh] & 0x7f) == '\r');
    return *inChr_p = c;
}

void readSrcLine()
{
    byte i = 127;		// max chars;
    
    for (;;) {
        if (*inChr_p == '\n' || *inChr_p == ISISEOF)
            return;
        if (--i == 0)
            break;
        inChr_p++;
        inSrcCh();
    }
    l59e6_truncated = 0xff;
    while (inSrcCh() != '\n' && *inChr_p != ISISEOF)	// junk rest of long line
        ;
}

void getNewLine()
{
    info_pt tmp;
    
    if (macroDepth != 0) {	// we are expanding a macro
        tmp = curInfo_p;
        curInfo_p = curMacroInfo_p;
        setInfoType(0);		// flag this macro as not expanding anymore
        curMacroInfo_p = macroPtrs[macroDepth].info_p;
        inChr_p = macroPtrs[macroDepth].inChr_p;
        macroDepth--;
        curInfo_p = tmp;
    } else
        getCodeLine();
}



void getSrcLine()
{
    writeLineInfo();
    inChr_p = lineBuf;
    inSrcCh();
    if (*inChr_p == ISISEOF)	// EOF
        return;
    l59e6_truncated = 0;
    readSrcLine();
    inChr_p[0] = '\r';		// append cr lf
    inChr_p[1] = '\n';
    inChr_p = lineBuf;
    linfo.lineCnt++;
    linfo.blkCnt = linfo.stmtCnt = 0;
    lineInfoToWrite = 0xff;
}


void getCodeLine()
{
    static char EOFString[] = "/*'/**/EOF   ";
    for (;;) {
        getSrcLine();
        if (*inChr_p != ISISEOF) {
            startOfLine = inChr_p + LEFTMARGIN - 1;
            while (inChr_p < startOfLine) {
                if (*inChr_p == '\r')	// short line
                    return;
                inChr_p++;
            }
            if (skippingCOND)
                doCONDcompile(inChr_p);
            else if (*inChr_p == '$') {
                wrByte(L_STMTCNT);
                wrTx1R0user2(0);
                if (l59e6_truncated) {
                    syntaxError(ERR86);	// LIMIT EXCEEDED: SOURCE LINE LENGTH
                    l59e6_truncated = 0;
                }
                do_Control(inChr_p);
            } else {
                isNonCtrlLine = 0xff;
                return;
            }
        } else if (srcFileIdx == 0) {	// end of last file
            if (ifDepth != 0)
                syntaxError(ERR188);	// MISPLACED RESTORE OPTION
            inChr_p = EOFString;	// should close all strings and quotes
            return;
        } else {
            closeFile(&srcFile);
            srcFileIdx--;	// back up to previous entry
            initFile(&srcFile, "SOURCE",
                srcFileTable[srcFileIdx].filename);
            openFile(&srcFile, 1);
            seekFile(&srcFile, (loc_t *) & srcFileTable[srcFileIdx].block);
            inOffCurCh = inOffLastCh;	// force read
        }
    }
}

byte ENDorSEMICOLON[] = { T_END, T_SEMICOLON };
byte tokenTypeTable[] = {
    T_NUMBER, T_NUMBER, T_NUMBER, T_VARIABLE,
    T_VARIABLE, T_PLUSSIGN, T_MINUSSIGN, T_STAR,
    T_SLASH, T_LPAREN, T_RPAREN, T_COMMA,
    T_COLON, T_SEMICOLON, T_STRING, T_PERIOD,
    T_EQ, T_LT, T_GT, T_VARIABLE,
    T_VARIABLE, T_VARIABLE, T_VARIABLE, T_VARIABLE
};

byte binValidate[] = { 0, 1, 1, 1 };
byte octValidate[] = { 0, 0, 1, 1 };
byte decValidate[] = { 0, 0, 0, 1 };
byte hexValidate[] = { 0, 0, 0, 2 };



void yylex()
{
    if (yyAgain)
        yyAgain = 0;
    else if (unexpectedEOF)
        tokenType = ENDorSEMICOLON[endToggle = (endToggle + 1) & 1];
    else {
        _yylex();
        if (tokenType == T_EOF) {
            unexpectedEOF = 0xff;
            tokenType = T_END;
        }
    }
    
}


byte toupper(byte c)
{
    return ('a' <= c && c <= 'z') ? c & 0x5f : c;
}


byte l5bcd_toklen;
byte l5bcd_ch;

void token2Num()
{
    if (cClass[l5bcd_ch = tokenStr[l5bcd_toklen = *tokenStr]] <= CC_DECDIGIT)	// last char is a digit
        _token2num(10, decValidate);
    else {
        l5bcd_toklen--;
        if (l5bcd_ch == 'H')
            _token2num(16, hexValidate);
        else if (l5bcd_ch == 'Q' || l5bcd_ch == 'O')
            _token2num(8, octValidate);
        else if (l5bcd_ch == 'B')
            _token2num(2, binValidate);
        else if (l5bcd_ch == 'D')
            _token2num(10, decValidate);
        else {
            tokenErrorAt(ERR4);	// ILLEGAL NUMERIC CONSTANT TYPE
            tokenVal = 0;
        }
    }
}


void _token2num(byte radix, byte validate[])
{
    word trial;
    byte curoff;
    byte ct;
    byte errored;
    byte valId;
    
    tokenVal = 0;
    trial = 0;
    errored = 0;
    for (curoff = 1; curoff <= l5bcd_toklen; curoff++) {
        if ((ct = cClass[l5bcd_ch = tokenStr[curoff]]) > CC_HEXCHAR)	// not 0-9A-F
            valId = 1;
        else
            valId = validate[ct];
        switch (valId) {
        case 0:
            l5bcd_ch -= '0';	// convert to binary;
            break;
        case 1:
            if (!errored) {
                tokenErrorAt(ERR5);	// INVALID CHARACTER IN NUMERIC CONSTANT
                errored = 0xff;
            }
            l5bcd_ch = 0;
            break;
        case 2:
            l5bcd_ch = l5bcd_ch - 'A' + 10;
        }
        // not a particularly good test for overflow
        if ((trial = tokenVal * radix + l5bcd_ch) < tokenVal) {
            tokenErrorAt(ERR94);	// ILLEGAL CONSTANT, VALUE > 65535
            return;
        }
        tokenVal = trial;
    }
    
}



void nestMacro()
{
    char *tmp = (char *)getLitaddr() + 2;
    wrTx1XrefInfo();
    if (macroDepth == 5)
        tokenErrorAt(ERR7);	// LIMIT EXCEEDED: MACROS NESTED TOO DEEPLY
    else {
        setInfoType(7);		// set this macro to expanding
        macroPtrs[++macroDepth].inChr_p = inChr_p;
        macroPtrs[macroDepth].info_p = curMacroInfo_p;
        inChr_p = tmp - 1;
        curMacroInfo_p = curInfo_p;
    }
}

byte chkMacro()
{
    lookup(tokenStr);
    markedSymbol_p = curSymbol_p;
    // TO FIX use unions
    if (((curSymbol_p->infoChain) >> 8) == 0xff)	// simple key word ?
        tokenType = curSymbol_p->infoChain & 0xff;
    else {
        findInfo();
        if (curInfo_p != 0) {
            if (getInfoType() == LIT_T) {
                nestMacro();
                return 0;
            }
            if (getInfoType() == MACRO_T) {
                tokenErrorAt(ERR6);	// ILLEGAL MACRO REFERENCE, RECURSIVE EXPANSION
                return 0;
            }
        }
    }
    return 0xff;
}


void getName(word maxlen)
{
    word curOff = 1;
    byte ct;
    
    while ((ct = cClass[savedCh]) <= CC_ALPHA || savedCh == '$') {	// letter digit or $
        if (savedCh == '$')
            getNextChr();	// ignore $
        else if (curOff > maxlen) {
            tokenErrorAt(ERR3);	// IDENTIFIER, STRING, OR NUMBER TOO LONG, TRUNCATED
            while (ct <= CC_ALPHA || savedCh == '$') {
                getNextChr();
                ct = cClass[savedCh];
            }
            curOff = maxlen + 1;
        } else {
            tokenStr[curOff++] = toupper(savedCh);
            getNextChr();
        }
    }
    
    tokenStr[0] = curOff - 1;
}


void parseString()
{
    byte toolong = 0;
    word curOff = 1;
    
    for (;;) {
        getNextChr();
        while (savedCh != '\'') {
            while (savedCh == '\r' || savedCh == '\n')	// ignore cr and lf
                getNextChr();
            if (savedCh == '\'')
                continue;
            if (curOff != 256)
                tokenStr[curOff++] = savedCh;
            else {
                toolong = 0xff;
                if (savedCh == ';') {
                    tokenErrorAt(ERR85);	// LONG STRING ASSUMED CLOSED AT NEXT SEMICOLON OR QUOTE
                    goto l5F4E;
                }
            }
            getNextChr();
        }
        getNextChr();		// check for double quote
        if (savedCh != '\'')
            break;		// all done
        if (curOff != 256)
            tokenStr[curOff++] = savedCh;
        else
            toolong = 0xff;
    }
l5F4E:
    if ((tokenStr[0] = curOff - 1) == 0)	// save length read
        tokenErrorAt(ERR189);	// NULL STRING NOT ALLOWED
    if (toolong)
        tokenErrorAt(ERR3);	// IDENTIFIER, STRING, OR NUMBER TOO LONG, TRUNCATED 
}

byte typeProcIdx[] = {
    1, 1, 1, 2, 2, 3, 3, 3,
    4, 3, 3, 3, 5, 3, 6, 3,
    3, 7, 8, 0, 9, 9, 10, 0
};

void _yylex()
{
    info_pt tmp;
    byte inComment;
    
    while (1) {
        saveClass = cClass[savedCh];
        tokenType = tokenTypeTable[saveClass];
        switch (typeProcIdx[saveClass]) {
        case 0:		// white space
            getNextChr();
            saveClass = cClass[savedCh];
            while (saveClass == CC_WSPACE) {	// space, tab or cr
                getNextChr();	// gobble up the white space
                saveClass = cClass[savedCh];
            }
            break;
        case 1:		// digits
            getName(31);
            token2Num();
            return;
        case 2:		// letters
            tmp = curInfo_p;
            getName(255);
            if (chkMacro())	// return if not a macro
                return;
            curInfo_p = tmp;
            getNextChr();
            break;
        case 3:		// -, +, *, (, ), ,, ;, = 
            getNextChr();
            return;
        case 4:		// slash
            getNextChr();
            inComment = 0xff;
            if (savedCh != '*')
                return;
            
            getNextChr();
            while (inComment) {
                while (savedCh != '*')
                    getNextChr();
                getNextChr();
                if (savedCh == '/') {
                    inComment = 0;
                    getNextChr();
                }
            }
            break;
        case 5:		// :
            getNextChr();
            if (savedCh == '=') {
                tokenType = T_COLON_EQUALS;
                getNextChr();
            }
            return;
        case 6:		// quote
            parseString();
            return;
        case 7:		// <
            getNextChr();
            if (savedCh == '>') {
                tokenType = T_NE;
                getNextChr();
            } else if (savedCh == '=') {
                tokenType = T_LE;
                getNextChr();
            }
            return;
        case 8:		// >
            getNextChr();
            if (savedCh == '=') {
                tokenType = T_GE;
                getNextChr();
            }
            return;
        case 9:		// $, !, ", #, %, &, ?,         , [, \, ], ^, _, `, {, |, }, ~
            tokenErrorAt(ERR1);	// INVALID PL/M-80 CHARACTER
            getNextChr();
            break;
        case 10:		// non white space control chars and DEL 
            tokenErrorAt(ERR2);	// UNPRINTABLE ASCII CHARACTER 
            getNextChr();
            break;
        }
    }
}


void setYyAgain()
{
    yyAgain = 0xff;		// causes yylext to return previous values. Oneshow action
}

// does yylex and returns true if token type match
// else resets yylex for    next time

byte yylexMatch(byte token)
{
    yylex();
    if (tokenType == token)
        return 0xff;
    setYyAgain();
    return 0;
}

byte yylexNotMatch(byte token)
{
    return !yylexMatch(token);
}

void sub_60F9()
{
    word i = 1;
    
    for (;;) {
        wrOprAndValue();
        yylex();
        
        if (tokenType == T_LPAREN)
            i++;
        else if (tokenType == T_RPAREN) {
            if (--i == 0) {
                wrOprAndValue();
                break;
            }
        } else if (tokenType == T_SEMICOLON
            || (tokenType >= T_CALL && tokenType <= T_RETURN)) {
            tokenErrorAt(ERR82);	// INVALID SYNTAX, MISMATCHED '('
            while (i != 0) {
                wrByte(L_RPAREN);
                i--;
            }
            setYyAgain();
            break;
        }
    }
}

void parseExpresion(byte endTok)
{
    yylex();
    while (tokenType != endTok && tokenType != T_SEMICOLON) {
        if (T_CALL <= tokenType && tokenType <= T_RETURN)
            break;
        wrOprAndValue();
        yylex();
    }
    setYyAgain();
}

void sub_61CF()
{
    word i;
    
    for (i = 0; tokenType != T_SEMICOLON;) {
        if (tokenType == T_LPAREN)
            i++;
        else if (tokenType == T_RPAREN)
            if (i == 0)
                break;
            else
                i--;
            yylex();
    }
    setYyAgain();
}


void sub_621A()
{
    word i;
    for (i = 0; tokenType != T_SEMICOLON;) {
        if (i == 0 && tokenType == T_COMMA)
            break;
        if (tokenType == T_LPAREN)
            i++;
        else if (tokenType == T_RPAREN)
            if (i == 0)
                break;
            else
                i--;
            yylex();
    }
    setYyAgain();
}


void parseDcl()
{
    if (stmtLabelCnt != 0)	// declares do not have labels
        syntaxError(ERR46);	// ILLEGAL USE OF LABEL
    do {
        getDclNames();
        if (declNameCnt != 0) {	// process any names
            parseLitOrType();
            sub_65AF();
        }
    }
    while (!yylexNotMatch(T_COMMA));
}

void sub_62B0(word errcode)
{
    tokenError((byte) errcode, declaringName);
}


void chkModuleLevel()
{
    if (*curProcData != 0x100)
        tokenErrorAt(ERR73);	// INVALID ATTRIBUTE OR INITIALIZATION, NOT AT MODULE LEVEL
}


void chkNotArray()
{
    if (testFlag(dclFlags, F_ARRAY)) {
        tokenErrorAt(ERR63);	// INVALID DIMENSION WITH THIS ATTRIBUTE
        clrFlag(dclFlags, F_ARRAY);
        clrFlag(dclFlags, F_STARDIM);
        arrayDim = 0;
    }
}


void chkNotStarDim()
{
    if (testFlag(dclFlags, F_STARDIM)) {	// check we don't have a * dimension
        tokenErrorAt(ERR62);	// INVALID STAR DIMENSION WITH 'STRUCTURE' OR 'EXTERNAL'
        clrFlag(dclFlags, F_STARDIM);
        arrayDim = 1;
    }
}

void createStructMemberInfo()
{
    word i;
    byte memType;
    word memDim;
    
    if (structMCnt == 0)
        return;
    
    for (i = 1; i <= structMCnt; i++) {
        curSymbol_p = structMembers[i];
        memType = structMemType[i];
        memDim = structMemDim[i];
        createInfo(*curProcData, memType);
        wrTx1R0Xref();
        setInfoFlag(F_MEMBER);
        setInfoFlag(F_LABEL);
        if (memDim != 0) {
            setInfoFlag(F_ARRAY);	// has dimension
            setDimension(memDim);
        } else
            setDimension(1);
        setOwningStructure(parentStructure);
    }
}


void sub_63B7(word val)
{
    byte cFlags, i;
    
    curSymbol_p = declaringName;
    findScopedInfo(*curProcData);
    if (curInfo_p != 0) {
        wrTx1XrefInfo();
        if (testInfoFlag(F_PARAMETER) && !testInfoFlag(F_LABEL)) {
            cFlags = 0;
            for (i = 0; i <= 2; i++)
                cFlags |= dclFlags[i];
            if (cFlags)
                sub_62B0(ERR76);	// CONFLICTING ATTRIBUTE WITH PARAMETER
            
            if (dclType != 2 && dclType != 3)	/* BYTE or ADDRESS */
                sub_62B0(ERR79);	// ILLEGAL PARAMETER TYPE, NOT BYTE OR ADDRESS
            else
                setInfoType(dclType);
            if (declaringBase != 0) {
                sub_62B0(ERR77);	// INVALID PARAMETER DECLARATION, BASE ILLEGAL 
                declaringBase = 0;
            }
        } else {
            sub_62B0(ERR78);	// DUPLICATE DECLARATION 
            return;
        }
    } else {
        createInfo(*curProcData, dclType);
        wrTx1R0Xref();
        cpyFlags(dclFlags);
    }
    parentStructure = curInfo_p;
    if (dclType == 0) {		// Literally
        if (declaringBase != 0)
            sub_62B0(ERR81);	// CONFLICTING ATTRIBUTE WITH 'BASE'
        setLitaddr(lastLit);
        return;
    }
    if (dclType == 1) {		// Label
        if (declaringBase != 0)
            sub_62B0(ERR80);	// INVALID DECLARATION, LABEL MAY NOT BE BASED 
        if (testInfoFlag(F_EXTERNAL))	// External
            setInfoFlag(F_LABEL);
        return;
    }
    
    if (declaringBase != 0) {
        if (testInfoFlag(F_PUBLIC) || testInfoFlag(F_EXTERNAL)
            || testInfoFlag(F_AT) || testInfoFlag(F_INITIAL)
            || testInfoFlag(F_DATA)) {
            sub_62B0(ERR81);	// CONFLICTING ATTRIBUTE WITH 'BASE'    
            declaringBase = 0;
        } else
            setInfoFlag(F_BASED);
    }
    setDimension(arrayDim);
    setBase(declaringBase);
    curInfo_p = curProcInfo_p;
    if (testInfoFlag(F_REENTRANT)) {	// reentrant
        curInfo_p = parentStructure;
        if (!testInfoFlag(F_DATA) && !testInfoFlag(F_BASED)
            && !testInfoFlag(F_AT))
            setInfoFlag(F_AUTOMATIC);
    }
    curInfo_p = parentStructure;
    if (testInfoFlag(F_PARAMETER))
        byte_9D7B++;
    else if (val - byte_9D7B != 1)
        setInfoFlag(F_PACKED);
    setInfoFlag(F_LABEL);
    if (dclType == 4)		// structure
        createStructMemberInfo();
}

void sub_65AF()
{
    word i;
    byte_9D7B = 0;
    for (i = 1; i <= declNameCnt; i++) {
        declaringName = declNames[i];
        declaringBase = declBasedNames[i];
        sub_63B7(i);
    }
}



void sub_65FA(byte lexItem, byte flag)
{
    if (testFlag(dclFlags, F_EXTERNAL))
        tokenErrorAt(ERR41);	// CONFLICTING ATTRIBUTE
    if (yylexMatch(T_LPAREN)) {
        wrByte(lexItem);
        if (l6a68_flag)
            wrTx1R0InfoOffset(topInfo + 1);
        else
            wrTx1R0InfoOffset(off2Info(0));
        setFlag(dclFlags, flag);
        sub_60F9();
    } else
        tokenErrorAt(ERR75);	// MISSING ARGUMENT OF 'AT' , 'DATA' , OR 'INITIAL'
}



void parseDclInitial()
{
    if (yylexMatch(T_INITIAL))
        sub_65FA(L_INITIAL, F_INITIAL);
    else if (yylexMatch(T_DATA))
        sub_65FA(L_DATA, F_DATA);
    else if (testFlag(dclFlags, F_STARDIM)) {
        tokenErrorAt(ERR74);	// INVALID STAR DIMENSION, NOT WITH 'DATA' OR 'INITIAL'
        clrFlag(dclFlags, F_STARDIM);
        arrayDim = 1;
    }
}


void parseDclAt()
{
    if (yylexMatch(T_AT))
        sub_65FA(L_AT, F_AT);
}

void parseDclScope()
{
    if (yylexMatch(T_PUBLIC)) {
        chkModuleLevel();
        setFlag(dclFlags, F_PUBLIC);
    } else if (yylexMatch(T_EXTERNAL)) {
        chkNotStarDim();
        chkModuleLevel();
        setFlag(dclFlags, F_EXTERNAL);
    }
}

void parseStructMType()
{
    word type;
    
    if (yylexMatch(T_BYTE))
        type = 2;
    else if (yylexMatch(T_ADDRESS))
        type = 3;
    else {
        type = 2;
        if (yylexMatch(T_STRUCTURE)) {
            tokenErrorAt(ERR70);	// INVALID MEMBER TYPE, 'STRUCTURE' ILLEGAL
            if (yylexMatch(L_LPAREN)) {
                sub_61CF();
                yylex();
            }
        } else if (yylexMatch(T_LABEL))
            tokenErrorAt(ERR71);	// INVALID MEMBER TYPE, 'LABEL' ILLEGAL
        else
            tokenErrorAt(ERR72);	// MISSING TYPE FOR STRUCTURE MEMBER
    }
    structMemType[structMCnt] = (byte) type;
}


void parseStructMDim()
{
    word dim;
    
    if (yylexMatch(T_LPAREN)) {
        if (yylexMatch(T_NUMBER))
            dim = tokenVal;
        else if (yylexMatch(T_STAR)) {
            dim = 1;
            tokenErrorAt(ERR69);	// INVALID STAR DIMENSION WITH STRUCTURE MEMBER
        } else {
            dim = 1;
            tokenErrorAt(ERR59);	// ILLEGAL DIMENSION ATTRIBUTE
        }
        if (dim == 0) {
            dim = 1;
            tokenErrorAt(ERR57);	// INVALID DIMENSION, ZERO ILLEGAL
        }
        if (yylexNotMatch(T_RPAREN)) {
            tokenErrorAt(ERR60);	// MISSING ') ' AT END OF DIMENSION
            sub_61CF();
            yylex();
        }
        structMemDim[structMCnt] = dim;
    }
}



void _parseStructMem()
{
    byte mcnt;
    if (yylexNotMatch(T_VARIABLE))
        tokenErrorAt(ERR66);	// INVALID STRUCTURE MEMBER, NOT AN IDENTIFIER
    else {
        for (mcnt = 1; mcnt <= structMCnt; mcnt++)
            if (curSymbol_p == structMembers[mcnt])
                tokenErrorAt(ERR67);	// DUPLICATE STRUCTURE MEMBER NAME
        if (structMCnt == 32)
            tokenErrorAt(ERR68);	// LIMIT EXCEEDED: NUMBER OF STRUCTURE MEMBERS
        else
            structMCnt++;
        structMembers[structMCnt] = curSymbol_p;
        structMemType[structMCnt] = 0;
        structMemDim[structMCnt] = 0;
        parseStructMDim();
        parseStructMType();
    }
}


void parseStructMem()
{
    structMCnt = 0;
    if (yylexNotMatch(T_LPAREN))
        tokenErrorAt(ERR64);	// MISSING STRUCTURE MEMBERS
    else {
        do {
            _parseStructMem();
        }
        while (!yylexNotMatch(T_COMMA));
        if (yylexNotMatch(T_RPAREN)) {
            tokenErrorAt(ERR65);	// MISSING ') ' AT END OF STRUCTURE MEMBER LIST
            sub_61CF();
            yylex();
        }
    }
}

void parseDclDataType()
{
    if (yylexMatch(T_BYTE))
        dclType = 2;
    else if (yylexMatch(T_ADDRESS))
        dclType = 3;
    else if (yylexMatch(T_STRUCTURE)) {
        dclType = 4;
        chkNotStarDim();
        parseStructMem();
    } else if (yylexMatch(T_LABEL)) {
        dclType = 1;
        chkNotArray();
    } else {
        tokenErrorAt(ERR61);	// MISSING TYPE
        dclType = 2;
    }
}

void parseArraySize()
{
    if (yylexMatch(T_LPAREN)) {
        setFlag(dclFlags, F_ARRAY);
        if (yylexMatch(T_NUMBER)) {
            if (tokenVal == 0) {
                tokenErrorAt(ERR57);	// INVALID DIMENSION, ZERO ILLEGAL
                arrayDim = 1;
            } else
                arrayDim = tokenVal;
        } else if (yylexMatch(T_STAR)) {
            if (declNameCnt > 1) {
                tokenErrorAt(ERR58);	// INVALID STAR DIMENSION IN FACTORED DECLARATION
                arrayDim = 1;
            } else
                setFlag(dclFlags, F_STARDIM);
        } else {
            tokenErrorAt(ERR59);	// ILLEGAL DIMENSION ATTRIBUTE
            arrayDim = 1;
        }
        if (yylexNotMatch(T_RPAREN)) {
            tokenErrorAt(ERR60);	// MISSING ') ' AT END OF DIMENSION
            sub_61CF();
            yylex();
        }
    }
}


void parseDeclType()
{
    parseArraySize();
    parseDclDataType();
    parseDclScope();
    parseDclAt();
    parseDclInitial();
}

void parseLiterally()
{
    if (yylexNotMatch(T_STRING)) {
        tokenErrorAt(ERR56);	// INVALID MACRO TEXT, NOT A STRING CONSTANT
        tokenStr[0] = 1;	// make into a space
        tokenStr[1] = ' ';
    }
    lastLit = createLit(tokenStr);
    dclType = 0;
}

//  parse for literally or the appropriate data type

void parseLitOrType()
{
    arrayDim = 1;
    clrFlags(dclFlags);
    dclType = 0;
    if (yylexMatch(T_LITERALLY))
        parseLiterally();
    else
        parseDeclType();
}

void getBaseInfo()
{
    symbol_pt base1Name, base2Name;
    basedInfo = 0;
    if (yylexNotMatch(T_VARIABLE))
        tokenErrorAt(ERR52);	// INVALID BASE, MEMBER OF BASED STRUCTURE OR ARRAY OF STRUCTURES
    else {
        base1Name = curSymbol_p;
        if (yylexMatch(T_PERIOD)) {	// see if based on struct.var
            if (yylexMatch(T_VARIABLE))
                base2Name = curSymbol_p;
            else {
                tokenErrorAt(ERR53);	// INVALID STRUCTURE MEMBER IN BASE
                return;
            }
        } else
            base2Name = 0;
        curSymbol_p = base1Name;
        findInfo();
        if (curInfo_p == 0) {
            tokenErrorAt(ERR54);	// UNDECLARED BASE
            return;
        }
        wrTx1XrefInfo();
        if (base2Name == 0)
            basedInfo = curInfo_p;
        else {
            curSymbol_p = base2Name;
            findMemberInfo();
            if (curInfo_p == 0) {
                tokenErrorAt(ERR55);	// UNDECLARED STRUCTURE MEMBER IN BASE
                return;
            }
            wrTx1XrefInfo();
            basedInfo = curInfo_p;
        }
    }
}



void sub_6A68()
{
    if (yylexNotMatch(T_VARIABLE)) {
        tokenErrorAt(ERR48);	// ILLEGAL DECLARATION STATEMENT SYNTAX
        return;
    }
    if (declNameCnt == 32)
        tokenErrorAt(ERR49);	// LIMIT EXCEEDED: NUMBER OF ITEMS IN FACTORED DECLARE
    else
        declNameCnt++;
    declNames[declNameCnt] = curSymbol_p;
    declBasedNames[declNameCnt] = 0;	// no based name yet
    if (!l6a68_flag) {
        findScopedInfo(*curProcData);
        if (curInfo_p == 0)
            l6a68_flag = 0xff;
    }
    if (!yylexMatch(T_BASED))
        return;
    getBaseInfo();
    if (basedInfo == 0)
        return;
    curInfo_p = basedInfo;
    if (testInfoFlag(F_BASED) || testInfoFlag(F_ARRAY)
        || getInfoType() != ADDRESS_T) {
        tokenErrorAt(ERR50);	// INVALID ATTRIBUTES FOR BASE
        basedInfo = 0;
    } else if (testInfoFlag(F_MEMBER)) {
        curInfo_p = getOwningStructure();
        if (testInfoFlag(F_ARRAY) || testInfoFlag(F_BASED)) {
            tokenErrorAt(ERR52);	// INVALID BASE, MEMBER OF BASED STRUCTURE OR ARRAY OF STRUCTURES
            basedInfo = 0;
        }
    }
    declBasedNames[declNameCnt] = basedInfo;
}


void getDclNames()
{
    declNameCnt = 0;
    l6a68_flag = 0;
    if (yylexMatch(T_LPAREN)) {
        do {
            sub_6A68();
        }
        while (!yylexNotMatch(T_COMMA));
        
        if (yylexNotMatch(T_RPAREN)) {
            tokenErrorAt(ERR47);	// MISSING ') ' AT END OF FACTORED DECLARATION
            sub_61CF();
            yylex();
        }
    } else
        sub_6A68();
}


void parseProcDcl()
{
    info_pt tmp = curProcInfo_p;
    curSymbol_p = stmtLabels[1];	// assumed to be words
    findScopedInfo(*curProcData);
    if (curInfo_p != 0)
        syntaxError(ERR34);	// DUPLICATE PROCEDURE DECLARATION
    createInfo(*curProcData, PROC_T);
    setInfoFlag(F_LABEL);
    wrTx1R0Xref();
    if (procCnt == 254)
        fatalError(ERR35);	// LIMIT EXCEEDED: NUMBER OF PROCEDURES
    procInfo[++procCnt] = curInfo_p;
    procData.Byt.curProcId = (byte) procCnt;
    curProcInfo_p = curInfo_p;
    procData.Byt.curDoBlkCnt = 0;
    doBlkCnt = 0;
    pushBlock(*curProcData);
    parseParams();
    parseRetType();
    parseProcAttrib();
    curInfo_p = curProcInfo_p;
    if (!testInfoFlag(F_EXTERNAL)) {	// external
        wrByte(L_PROCEDURE);
        wrTx1R0InfoOffset(curInfo_p);
        wrByte(L_SCOPE);
        wrTx1R0user2(*curProcData);
    } else {
        wrByte(L_EXTERNAL);
        wrTx1R0InfoOffset(curInfo_p);
    }
    setProcId(procData.Byt.curProcId);
    if (tmp != 0) {		// nested proc
        curInfo_p = tmp;
        if (testInfoFlag(F_REENTRANT))	// reentrant
            syntaxError(ERR88);	// INVALID PROCEDURE NESTING, ILLEGAL IN REENTRANT PROCEDURE
        curInfo_p = curProcInfo_p;
    }
}



void setPublic()
{
    if (getInfoScope() != 0x100)
        tokenErrorAt(ERR39);	// INVALID ATTRIBUTE OR INITIALIZATION, NOT AT MODULE LEVEL
    else if (testInfoFlag(F_PUBLIC))	// public
        tokenErrorAt(ERR40);	// DUPLICATE ATTRIBUTE
    else if (testInfoFlag(F_EXTERNAL))	// external
        tokenErrorAt(ERR41);	// CONFLICTING ATTRIBUTE
    else
        setInfoFlag(F_PUBLIC);
}


void setExternal()
{
    if (getInfoScope() != 0x100)
        tokenErrorAt(ERR39);	// INVALID ATTRIBUTE OR INITIALIZATION, NOT AT MODULE LEVEL
    else if (testInfoFlag(F_EXTERNAL))	// external
        tokenErrorAt(ERR40);	// DUPLICATE ATTRIBUTE
    else if (testInfoFlag(F_REENTRANT) || testInfoFlag(F_PUBLIC))	// reentrant or public
        tokenErrorAt(ERR41);	// CONFLICTING ATTRIBUTE
    else
        setInfoFlag(F_EXTERNAL);
}

void setReentrant()
{
    if (getInfoScope() != 0x100)
        tokenErrorAt(ERR39);	// INVALID ATTRIBUTE OR INITIALIZATION, NOT AT MODULE LEVEL
    else if (testInfoFlag(F_REENTRANT))	// reentrant
        tokenErrorAt(ERR40);	// DUPLICATE ATTRIBUTE
    else if (testInfoFlag(F_EXTERNAL))	// external
        tokenErrorAt(ERR41);	// CONFLICTING ATTRIBUTE
    else
        setInfoFlag(F_REENTRANT);
}



void setInterruptNo()
{
    if (yylexMatch(T_NUMBER)) {
        if (tokenVal > 255) {
            tokenErrorAt(ERR42);	// INVALID INTERRUPT VALUE
            tokenVal = 0;
        }
    } else {
        tokenErrorAt(ERR43);	// MISSING INTERRUPT VALUE
        tokenVal = 0;
    }
    if (getInfoScope() != 0x100)
        tokenErrorAt(ERR39);	// INVALID ATTRIBUTE OR INITIALIZATION, NOT AT MODULE LEVEL
    else if (testInfoFlag(F_INTERRUPT))	// INTERRUPT
        tokenErrorAt(ERR40);	// DUPLICATE ATTRIBUTE
    else if (hasParams)
        tokenErrorAt(ERR44);	// ILLEGAL ATTRIBUTE, 'INTERRUPT' WITH PARAMETERS
    else if (getDataType() != 0)
        tokenErrorAt(ERR45);	// ILLEGAL ATTRIBUTE, 'INTERRUPT' WITH TYPED PROCEDURE
    else if (testInfoFlag(F_EXTERNAL))	// external
        tokenErrorAt(ERR41);	// CONFLICTING ATTRIBUTE
    else {
        setInfoFlag(F_INTERRUPT);
        setIntrNo((byte) tokenVal);
    }
}


void parseProcAttrib()
{
    curInfo_p = curProcInfo_p;
    
    for (;;) {
        if (yylexMatch(T_PUBLIC))
            setPublic();
        else if (yylexMatch(T_EXTERNAL))
            setExternal();
        else if (yylexMatch(T_REENTRANT))
            setReentrant();
        else if (yylexMatch(T_INTERRUPT))
            setInterruptNo();
        else
            break;
    }
}


void parseRetType()
{
    curInfo_p = curProcInfo_p;
    if (yylexMatch(T_BYTE))
        setDataType(2);
    else if (yylexMatch(T_ADDRESS))
        setDataType(3);
}


void sub_6E4B()
{
    findScopedInfo(*curProcData);
    if (curInfo_p != 0)
        tokenErrorAt(ERR38);	// DUPLICATE PARAMETER NAME
    createInfo(*curProcData, BYTE_T);
    wrTx1R0Xref();
    setInfoFlag(F_PARAMETER);
    paramCnt++;
}

void parseParams()
{
    paramCnt = 0;
    if (yylexMatch(T_LPAREN)) {	// check has parameters
        do {
            hasParams = 0xff;
            if (yylexMatch(T_VARIABLE))
                sub_6E4B();
            else
                tokenErrorAt(ERR36);	// MISSING PARAMETER
        }
        while (!yylexNotMatch(T_COMMA));
        if (yylexNotMatch(T_RPAREN)) {
            tokenErrorAt(ERR37);	// MISSING ') ' AT END OF PARAMETER LIST
            sub_61CF();
            yylex();
        }
        curInfo_p = curProcInfo_p;
        setParamCnt((byte) paramCnt);
    } else
        hasParams = 0;
}

byte brkTxiCodes[] = {
    // variable, semicolon, call, declare
    L_STATEMENT, L_SEMICOLON, L_CALL, L_LINEINFO,
    L_DISABLE, L_DO, L_ENABLE, L_END,
    L_GO, L_GOTO, L_HALT, L_IF,
    L_PROCEDURE, L_RETURN
};

void sub_6F00()
{
    stateIdx = 0;
    endSeen = 0;
    pushStateByte(0);
    while (stateIdx != 0) {
        switch (curState = stateStack[stateIdx--]) {
        case 0:
            state0();
            break;
        case 1:
            state1();
            break;
        case 2:
            state2();
            break;
        case 3:
            state3();
            break;
        case 4:
            state4();
            break;
        case 5:
            state5();
            break;
        case 6:
            state6();
            break;
        case 7:
            state7();
            break;
        case 8:
            state8();
            break;
        case 9:
            state9();
            break;
        case 10:
            state10();
            break;
        case 11:
            state11();
            break;
        case 12:
            state12();
            break;
        case 13:
            state13();
            break;
        case 14:
            state14();
            break;
        case 15:
            state15();
            break;
        case 16:
            state16();
            break;
        case 17:
            state17();
            break;
        case 18:
            state18();
            break;
        case 19:
            state19();
            break;
        case 20:
            state20();
            break;
        case 21:
            state21();
            break;
        }
    }
}

void skipToSemiColon()
{
    while (tokenType != T_SEMICOLON)
        yylex();
    
    yyAgain = 0;		// remove any yyAgain flag
}

void expectSemiColon()
{
    if (yylexNotMatch(T_SEMICOLON)) {
        tokenErrorAt(ERR32);	// INVALID SYNTAX, TEXT IGNORED UNTIL ';' 
        skipToSemiColon();
    }
}

void errorSkip()
{
    skipToSemiColon();
    setYyAgain();
}

void popStateWord(word * p)
{
    if (stateIdx == 0)
        fatalError(ERR97);	// COMPILER ERROR: PARSE STACK UNDERFLOW 
    *p = stateStack[stateIdx--];
}


void genLocalLabel()
{
    alloc(3, 3);
    localLabelCnt++;
}



void parseStmtLabels()
{
    stmtLabelCnt = 0;
    
    while (1) {
        labelBrkToken = tokenType;
        labelBrkSymbol = curSymbol_p;
        if (tokenType != T_VARIABLE)
            return;
        
        if (yylexMatch(T_COLON)) {
            if (stmtLabelCnt == 9)
                tokenErrorAt(ERR30);	// LIMIT EXCEEDED: NUMBER OF LABELS ON STATEMENT 
            else
                stmtLabelCnt++;
            stmtLabels[stmtLabelCnt] = curSymbol_p;
        } else
            return;
        yylex();
    }
}

void parseStartStmt()
{
    byte tmp;
    
    if (endSeen) {
        endSeen = 0;
        return;
    }
    tmp = XREF;
    XREF = 0;
    yylex();
    XREF = tmp;
    curStmtCnt++;
    if (linfo.stmtCnt == 0) {
        linfo.stmtCnt = curStmtCnt;
        linfo.blkCnt = curBlkCnt;
    }
    wrByte(L_STMTCNT);
    wrTx1R0user2(curStmtCnt);
    if (l59e6_truncated) {
        syntaxError(ERR86);	// LIMIT EXCEEDED: SOURCE LINE LENGTH
        l59e6_truncated = 0;
    }
    parseStmtLabels();
    if (labelBrkToken == T_SEMICOLON) {
        stmtStartCode = 1;
        setYyAgain();
    } else if (labelBrkToken == T_VARIABLE) {
        stmtStartCode = 0;
    } else if (labelBrkToken >= T_CALL && labelBrkToken <= T_RETURN) {
        stmtStartCode = labelBrkToken - T_CALL + 2;
    } else {
        tokenErrorAt(ERR29);	// ILLEGAL STATEMENT
        stmtStartCode = 1;
        errorSkip();
    }
    labelBrkTxiCode = brkTxiCodes[stmtStartCode];
}


void sub_717F()
{
    word i;
    symbol_pt tmp = curSymbol_p;
    if (stmtLabelCnt != 0) {
        for (i = 1; i <= stmtLabelCnt; i++) {
            curSymbol_p = stmtLabels[i];
            findScopedInfo(*curProcData);	// give the symbol the current proc data
            if (curInfo_p != 0) {
                if (testInfoFlag(F_LABEL))	// already marked as a label !!
                    tokenError(ERR33, curSymbol_p);	// DUPLICATE LABEL DECLARATION
                else {
                    wrByte(L_LABELDEF);
                    wrTx1R0InfoOffset(curInfo_p);
                    setInfoFlag(F_LABEL);
                }
                wrTx1XrefInfo();
            } else {
                createInfo(*curProcData, LABEL_T);
                wrTx1R0Xref();
                wrByte(L_LABELDEF);
                wrTx1R0InfoOffset(curInfo_p);
                setInfoFlag(F_LABEL);
            }
        }
        if (*curProcData == 0x100)	// see if module name
            wrByte(L_MODULE);
    }
    curSymbol_p = tmp;
}



byte sub_723A()
{
    if (yylexMatch(T_VARIABLE)) {
        curInfo_p = procInfo[1];
        if (getInfoSymbol() != curSymbol_p)
            tokenErrorAt(ERR20);	// MISMATCHED IDENTIFIER AT END OF BLOCK 
    }
    expectSemiColon();
    if (unexpectedEOF) {
        wrByte(L_END);
        return 0xff;
    }
    yylex();
    if (unexpectedEOF) {
        wrByte(L_END);
        unexpectedEOF = 0;
        return 0xff;
    }
    syntaxError(ERR93);		// MISSING 'DO' FOR 'END' , 'END' IGNORED
    setYyAgain();
    return 0;
}


void sub_7296()
{
    byte val, i;
    
    popDO();
    if (yylexMatch(T_VARIABLE)) {
        curInfo_p = curProcInfo_p;
        if (getInfoSymbol() != curSymbol_p)
            tokenErrorAt(ERR20);	// MISMATCHED IDENTIFIER AT END OF BLOCK
    }
    curInfo_p = curProcInfo_p;
    val = getParamCnt();
    for (i = 1; i <= val; i++) {
        sub_82D7();
        if (!testInfoFlag(F_LABEL))
            tokenError(ERR25, getInfoSymbol());	// UNDECLARED PARAMETER
    }
    popStateWord(&doBlkCnt);
    popStateWord((word *) & curProcInfo_p);
    expectSemiColon();
}

void pushStateWord(word val)
{
    if (stateIdx != 0x63)
        stateStack[++stateIdx] = val;
    else
        fatalError(ERR31);	// LIMIT EXCEEDED: PROGRAM TOO COMPLEX
}

void pushStateByte(byte state)
{
    pushStateWord(state);
}


void createModuleInfo(symbol_pt  symptr)
{
    curSymbol_p = symptr;
    createInfo(0, PROC_T);
    setInfoFlag(F_LABEL);
    wrTx1R0Xref();
    curProcInfo_p = curInfo_p;
    setProcId(1);
    procCnt = 1;
    procInfo[1] = curInfo_p;
    *curProcData = 0x100;	// proc 1, do level = 0
    wrByte(L_DO);
    wrByte(L_SCOPE);
    wrTx1R0user2(*curProcData);
    pushBlock(*curProcData);
}


void state0()
{
    parseStartStmt();
    pushStateByte(1);
    if (stmtStartCode != (T_DO - T_CALL + 2)) {
        syntaxError(ERR89);	// MISSING 'DO' FOR MODULE
        lookup((unsigned char *)"\6MODULE");
        createModuleInfo(curSymbol_p);
        pushStateByte(19);
    } else {
        if (stmtLabelCnt == 0) {
            syntaxError(ERR90);	// MISSING NAME FOR MODULE
            lookup((unsigned char *)"\6MODULE");
            stmtLabelCnt = 1;
            stmtLabels[1] = curSymbol_p;	// assumes words
        } else if (stmtLabelCnt > 1)
            syntaxError(ERR18);	// INVALID MULTIPLE LABELS AS MODULE NAMES
        createModuleInfo(stmtLabels[1]);
        expectSemiColon();
        pushStateByte(3);
    }
}

void state1()
{
    if (stmtStartCode != (T_END - T_CALL + 2)) {
        haveModule = 0xff;
        wrByte(L_MODULE);
        pushStateByte(2);
    } else {
        if (stmtLabelCnt != 0)
            tokenErrorAt(ERR19);	// INVALID LABEL IN MODULE WITHOUT MAIN PROGRAM
        if (!sub_723A()) {
            pushStateByte(1);
            pushStateByte(10);
        }
    }
}



void state2()
{
    if (stmtStartCode != (T_END - T_CALL + 2)) {
        pushStateByte(2);
        pushStateByte(10);
        pushStateByte(11);
    } else {
        sub_717F();
        if (sub_723A())
            wrByte(L_HALT);
        else {
            pushStateByte(2);
            pushStateByte(10);
        }
    }
}

void state3()
{
    parseStartStmt();
    pushStateByte(19);
}

void state19()
{				// check for DECLARE or PROCEDURE
    if (stmtStartCode == (T_DECLARE - T_CALL + 2)) {
        parseDcl();
        pushStateByte(3);
        expectSemiColon();
    } else if (stmtStartCode == (T_PROCEDURE - T_CALL + 2)) {
        pushStateByte(3);
        pushStateByte(4);
    }
}



void state4()
{
    if (stmtLabelCnt == 0) {
        syntaxError(ERR21);	// MISSING PROCEDURE NAME
        pushStateByte(7);
    } else {
        if (stmtLabelCnt != 1) {
            tokenErrorAt(ERR22);	// INVALID MULTIPLE LABELS AS PROCEDURE NAMES
            stmtLabelCnt = 1;
        }
        pushStateWord((word) curProcInfo_p);
        pushStateWord(doBlkCnt);
        parseProcDcl();
        expectSemiColon();
        curInfo_p = curProcInfo_p;
        if (testInfoFlag(F_EXTERNAL))	// external
            pushStateByte(5);
        else {
            pushStateByte(6);
            pushStateByte(21);
            pushStateByte(3);
        }
    }
}

void state5()
{
    parseStartStmt();
    if (stmtStartCode == (T_END - T_CALL + 2)) {
        if (stmtLabelCnt != 0) {
            tokenErrorAt(ERR23);	// INVALID LABELLED END IN EXTERNAL PROCEDURE
            stmtLabelCnt = 0;
        }
        sub_7296();
    } else {
        pushStateByte(5);
        if (stmtStartCode == (T_DECLARE - T_CALL + 2)) {
            parseDcl();
            expectSemiColon();
        } else {
            tokenErrorAt(ERR24);	// INVALID STATEMENT IN EXTERNAL PROCEDURE
            if (stmtStartCode == (T_PROCEDURE - T_CALL + 2)
                || stmtStartCode == (T_DO - T_CALL + 2))
                pushStateByte(7);
            else
                skipToSemiColon();
        }
    }
}


void state21()
{
    if (stmtStartCode == (T_END - T_CALL + 2))
        tokenErrorAt(ERR174);	// INVALID NULL PROCEDURE
    else
        pushStateByte(9);
}

void state6()
{
    sub_717F();
    sub_7296();
    wrByte(L_END);
}


// states 7 & 8 skip to end of block, handling nested blocks
void state7()
{
    skipToSemiColon();
    pushStateByte(8);
}

void state8()
{
    parseStartStmt();
    if (stmtStartCode == (T_PROCEDURE - T_CALL + 2)	// nested block
        || stmtStartCode == (T_DO - T_CALL + 2)) {
        pushStateByte(8);
        pushStateByte(7);							// proc nested block
    } else {
        skipToSemiColon();							// skip to end of statement
        if (stmtStartCode != (T_END - T_CALL + 2))	// if not an END then go again
            pushStateByte(8);
    }
}

void state9()
{
    if (stmtStartCode != (T_END - T_CALL + 2)) {
        pushStateByte(9);
        pushStateByte(10);
        pushStateByte(11);
    }
}


void state10()
{
    parseStartStmt();
}


void state11()
{
    if (stmtStartCode == (T_DECLARE - T_CALL + 2)
        || stmtStartCode == (T_PROCEDURE - T_CALL + 2)
        || stmtStartCode == (T_END - T_CALL + 2)) {
        tokenErrorAt(ERR26);	// INVALID DECLARATION, STATEMENT OUT OF PLACE
        pushStateByte(20);
    } else if (stmtStartCode == (T_DO - T_CALL + 2))
        pushStateByte(12);
    else if (stmtStartCode == (T_IF - T_CALL + 2))
        pushStateByte(16);
    else {
        sub_717F();
        wrByte(labelBrkTxiCode);
        if (stmtStartCode != 1) {	// Semicolon
            if (stmtStartCode == 0) {	// Variable
                wrByte(L_VARIABLE);
                wrTx1R0user2((word) labelBrkSymbol);
            }
            parseExpresion(T_SEMICOLON);
        }
        expectSemiColon();
    }
}

void state20()
{
    if (stmtStartCode == (T_DECLARE - T_CALL + 2)) {
        parseDcl();
        expectSemiColon();
        pushStateByte(20);
        pushStateByte(10);
    } else if (stmtStartCode == (T_PROCEDURE - T_CALL + 2)) {
        pushStateByte(20);
        pushStateByte(10);
        pushStateByte(4);
    } else if (stmtStartCode == (T_END - T_CALL + 2))
        endSeen = 0xff;
    else
        pushStateByte(11);
}


void state12()
{				// process do
    sub_717F();
    if (stmtLabelCnt != 0)
        pushStateWord((word) stmtLabels[stmtLabelCnt]);
    else
        pushStateWord(0);
    if (yylexMatch(T_VARIABLE)) {
        wrByte(L_DOLOOP);	// start of simple assignment etc
        wrOprAndValue();
        parseExpresion(T_SEMICOLON);
        pushStateByte(13);
        pushStateByte(9);
        pushStateByte(10);
    } else if (yylexMatch(T_WHILE)) {
        wrByte(L_WHILE);	// start of while
        parseExpresion(T_SEMICOLON);
        pushStateByte(13);
        pushStateByte(9);
        pushStateByte(10);
    } else if (yylexMatch(T_CASE)) {
        wrByte(L_CASE);	// start of case
        parseExpresion(T_SEMICOLON);
        genLocalLabel();
        pushStateWord(localLabelCnt);
        pushStateByte(14);
    } else {
        wrByte(L_DO);	// simple do end block
        pushStateByte(13);
        pushStateByte(9);
        pushStateByte(3);
    }
    expectSemiColon();		// we should now see a semicolon
    if (doBlkCnt >= 255)
        syntaxError(ERR27);	// LIMIT EXCEEDED: NUMBER OF DO BLOCKS
    else
        doBlkCnt++;
    
    procData.Byt.curDoBlkCnt = (byte) doBlkCnt;
    pushBlock(*curProcData);
    wrByte(L_SCOPE);
    wrTx1R0user2(*curProcData);
}


void state13()
{
    symbol_pt labelPtr;
    
    sub_717F();
    popDO();
    popStateWord((word *) & labelPtr);
    if (yylexMatch(T_VARIABLE) && labelPtr != curSymbol_p)
        tokenErrorAt(ERR20);	// MISMATCHED IDENTIFIER AT END OF BLOCK
    wrByte(L_END);
    expectSemiColon();
}



void state14()
{				// process CASE statements
    word stateWord;
    symbol_pt labelptr;
    
    parseStartStmt();
    if (stmtStartCode != (T_END - T_CALL + 2)) {
        genLocalLabel();
        wrByte(L_CASELABEL);
        wrTx1R0user2(localLabelCnt);
        pushStateByte(15);
        pushStateByte(11);
    } else {
        popStateWord(&stateWord);	// get the head jump
        sub_717F();
        if (stmtLabelCnt != 0) {
            wrByte(L_JMP);
            wrTx1R0user2(stateWord);
        }
        popDO();
        popStateWord((word *) & labelptr);
        wrByte(L_END);
        wrByte(L_LOCALLABEL);
        wrTx1R0user2(stateWord);
        // check end label if present
        if (yylexMatch(T_VARIABLE) && labelptr != curSymbol_p)
            syntaxError(ERR20);	// MISMATCH IDENTIFIER AT END OF BLOCK
        expectSemiColon();
    }
}


void state15()
{
    word stateWord;
    
    popStateWord(&stateWord);
    wrByte(L_JMP);
    wrTx1R0user2(stateWord);
    pushStateWord(stateWord);
    pushStateByte(14);
}


void state16()
{
    sub_717F();			// process if condition then
    wrByte(L_IF);
    parseExpresion(T_THEN);
    if (yylexNotMatch(T_THEN)) {
        tokenErrorAt(ERR28);	// MISSING 'THEN'
        wrByte(L_JMPFALSE);
        wrTx1R0user2(0);
    } else {
        genLocalLabel();
        pushStateWord(localLabelCnt);
        wrByte(L_JMPFALSE);
        wrTx1R0user2(localLabelCnt);
        pushStateByte(17);
        pushStateByte(11);
        pushStateByte(10);
    }
}

void state17()
{				// process optional ELSE
    word stateWord;
    byte tmp;
    
    popStateWord(&stateWord);
    yylex();
    tmp = lineInfoToWrite;
    lineInfoToWrite = 0;
    if (tokenType == T_ELSE) {	// do we have an else clause
        genLocalLabel();	// generate the target label
        wrByte(L_JMP);
        wrTx1R0user2(localLabelCnt);
        pushStateWord(localLabelCnt);
        pushStateByte(18);
        pushStateByte(11);
        pushStateByte(10);
    } else
        setYyAgain();
    
    wrByte(L_LOCALLABEL);
    wrTx1R0user2(stateWord);
    lineInfoToWrite = tmp;
}


void state18()
{
    byte tmp = lineInfoToWrite;
    word stateWord;
    
    lineInfoToWrite = 0;
    popStateWord(&stateWord);
    wrByte(L_LOCALLABEL);
    wrTx1R0user2(stateWord);
    lineInfoToWrite = tmp;
}


address getLitaddr()
{
    return curInfo_p->litaddr;
}

void setLitaddr(address  val)
{
    curInfo_p->litaddr = val;
}


void setIntrNo(byte intNo)
{
    curInfo_p->intrNo = intNo;
}



void setProcId(byte procId)
{
    curInfo_p->procID = procId;
}



byte *createLit(unsigned char *pstr)
{
    word litLen = pstr[0] + 1;
    byte *litSymbol = allocSymbolSpc((word) (litLen + 3));
    movemem(litLen, pstr, litSymbol + 1);
    movemem(2, " \n", litSymbol + litLen + 1);	// add " \n"
    litSymbol[0] = 255;		// put max size \n will terminate;
    return litSymbol;
}

void sub_82D7()
{
    while (1) {
        curInfo_p = curInfo_p + getInfoLen();
        if (curInfo_p >= topInfo) {
            curInfo_p = 0;
            return;
        }
        if (getInfoType() != TEMP_T)
            return;
    }
}



void fatalError_ov0(byte errcode)
{
    hasErrors = 0xff;
    if (ov0State != 0x14) {
        if (errcode == ERR83)	// LIMIT EXCEEDED: DYNAMIC STORAGE
            fatal("DYNAMIC STORAGE OVERFLOW", 0x18);
        fatal("UNKNOWN FATAL ERROR", 0x13);
    }
    tokenErrorAt(errcode);
    fatalErrorCode = errcode;
    throw "Bad";
}
