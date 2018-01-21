#include "plm.h"

/* index into precedence table */
static byte tx1Aux1Map[] = {0, 0, 0, 0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0, 0, 0, 0,
	      0, 0, 0, 0, 1, 2, 0, 3,
	      4, 5, 6, 7, 8, 9, 0x1D, 0xA,
	      0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x12,
	      0x13, 0, 0, 0, 0, 0, 0, 0,
	      0, 0, 0, 0};

byte b402F[] = {
    /* L_LINEINFO, L_SYNTAXERROR, L_TOKENERROR, L_LIST, L_NOLIST, L_CODE, L_NOCODE, L_EJECT, */
    T2_LINEINFO, T2_SYNTAXERROR, T2_TOKENERROR, T2_LIST, T2_NOLIST, T2_CODE, T2_NOCODE, T2_EJECT,
    /* L_INCLUDE, L_STMTCNT, L_LABELDEF, L_LOCALLABEL, L_JMP, L_JMPFALSE, L_PROCEDURE, L_SCOPE, */
    T2_INCLUDE, T2_STMTCNT, T2_LABELDEF, T2_LOCALLABEL, T2_JMP, T2_JMPFALSE, T2_PROCEDURE, 0,
    /* L_END, L_DO, L_DOLOOP, L_WHILE, L_CASE, L_CASELABEL, L_IF, L_STATEMENT, */
    0, 0, 0, 0, T2_CASE, T2_CASELABEL, 0, 0,
    /* L_CALL, L_RETURN, L_GO, L_GOTO, L_SEMICOLON, L_ENABLE, L_DISABLE, L_HALT, */
    0, T2_RETURN, T2_GO_TO, T2_GO_TO, T2_SEMICOLON, T2_ENABLE, T2_DISABLE, T2_HALT,
    /* L_EOF, L_AT, L_INITIAL, L_DATA, L_VARIABLE, L_NUMBER, L_STRING, L_PLUSSIGN, */
    0, 0, 0, 0,T2_VARIABLE, 0, 0, T2_ADDB,
    /* L_MINUSSIGN, L_PLUS, L_MINUS, L_STAR, L_SLASH, L_MOD, L_COLONEQUALS, L_AND, */
    T2_MINUSSIGN, T2_PLUS, T2_MINUS, T2_STAR, T2_SLASH, T2_MOD, T2_STORE, T2_AND,
    /* L_OR, L_XOR, L_NOT, L_LT, L_LE, L_EQ, L_NE, L_GE, */
    T2_OR, T2_XOR, T2_NOT, T2_LT, T2_LE, T2_EQ, T2_NE, T2_GE,
    /* L_GT, L_COMMA, L_LPAREN, L_RPAREN, L_PERIOD, L_TO, L_BY, L_UNUSED, */
    T2_GT, 0, 0, 0, 0, 0, 0, 0,
    /* L_MODULE, L_XREFINFO, L_XREF, L_EXTERNAL */
    T2_MODULE, 0, 0, 0};

/* 0x80	- expression item */
/* 0x40 - binary operator */
/* 0x20 - pass through */
/* 0x10 - procedure, at, data, initial or external */

byte tx1Aux2Map[] = {
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0, 0, 0, 0, 0, 0x10, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0x10, 0x10, 0x10, 0x80, 0x80, 0x80,0xC0,
        0xC0,0xC0,0xC0,0xC0,0xC0,0xC0, 0x80,0xC0,
        0xC0,0xC0, 0x80,0xC0,0xC0,0xC0,0xC0,0xC0,
        0xC0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0,
        0x20, 0, 0, 0x10};

byte b40B7[] = {
    /* L_LINEINFO, L_SyntaxError, L_TokenError, L_LIST, L_NOLIST, L_CODE, L_NOCODE, L_EJECT, */
    0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
    /* L_INCLUDE, L_STMTCNT, L_LABELDEF, L_LOCALLABEL, L_JMP, L_JMPFALSE, L_PROCEDURE, L_SCOPE,  */
    0x12, 0, 0x13, 0x13, 0x13, 0x13, 7, 1,
    /* L_END, L_DO, L_DOLOOP, L_WHILE, L_CASE, L_CASELABEL, L_IF, L_STATEMENT,  */
    2, 6, 4, 5, 8, 0x13, 3, 9,
    /* L_CALL, L_RETURN, L_GO, L_GOTO, L_SEMICOLON, L_ENABLE, L_DISABLE, L_HALT,  */
    0xA, 0xB, 0xD, 0xC, 0xE, 0xF, 0xF, 0xF,
    /* L_EOF, L_AT, L_INITIAL, L_DATA, L_VARIABLE, L_NUMBER, L_STRING, L_PLUSSIGN,  */
    0x12, 0x10, 0x11, 0x11, 0x12, 0x12, 0x12, 0x12,
    /* L_MINUSSIGN, L_PLUS, L_MINUS, L_STAR, L_SLASH, L_MOD, L_COLONEQUALS, L_AND,  */
    0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
    /* L_OR, L_XOR, L_NOT, L_LT, L_LE, L_EQ, L_NE, L_GE,  */
    0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
    /* L_GT, L_COMMA, L_LPAREN, L_RPAREN, L_PERIOD, L_TO, L_BY, L_INVALID,  */
    0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
    /* L_MODULE, L_XREFINFO, L_XREF, L_EXTERNAL */
    0x12, 0x12, 0x12, 0x14};


byte tx1ItemLengths[] = {
 /* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */ 
    6, 2, 4, 0, 0, 0, 0, 0, 6, 2, 2, 2, 2, 2, 2, 2,
    0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 2, 2, 2, 2,255,0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 2, 2};


byte b413F[] = {
    /* ?, I_VARIABLE, I_NUMBER, I_PLUSSIGN, I_MINUSSIGN, I_PLUS, I_MINUS, I_STAR */
    0 , T2_VARIABLE, T2_NUMBER, T2_ADDB, T2_MINUSSIGN, T2_PLUS, T2_MINUS, T2_STAR,
    /* I_SLASH, I_MOD, I_AND, I_OR, I_XOR, I_NOT, I_LT, I_LE */
    T2_SLASH, T2_MOD, T2_AND, T2_OR, T2_XOR, T2_NOT, T2_LT, T2_LE,
    /* I_EQ, I_NE, I_GE, I_GT, I_ADDRESSOF, I_UNARYMINUS, I_STACKPTR, I_INPUT */
    T2_EQ, T2_NE, T2_GE, T2_GT, T2_ADDRESSOF, T2_UNARYMINUS, T2_STACKPTR, T2_INPUT,
    /* I_OUTPUT, I_CALL, I_CALLVAR, I_BYTEINDEX, I_WORDINDEX, I_COLONEQUALS, I_MEMBER, I_BASED */
    T2_OUTPUT, T2_CALL, T2_CALLVAR, T2_BYTEINDEX, T2_WORDINDEX, T2_STORE, T2_MEMBER, T2_BASED,
    /* I_CARRY, I_DEC, I_DOUBLE, I_HIGH, I_LAST, I_LENGTH, I_LOW, I_MOVE, */
    T2_CARRY, T2_DEC, T2_DOUBLE, T2_HIGH, T2_LAST, T2_LENGTH, T2_LOW, T2_MOVE,
    /* I_PARITY, I_ROL, I_ROR, I_SCL, I_SCR, I_SHL, I_SHR, I_SIGN, */
    T2_PARITY, T2_ROL, T2_ROR, T2_SCL, T2_SCR, T2_SHL, T2_SHR, T2_SIGN,
    /* I_SIZE, I_TIME, I_ZERO, */
    T2_SIZE, T2_TIME, T2_ZERO};

byte b4172[] = {
/* I_STRING, I_VARIABLE, I_NUMBER, I_PLUSSIGN, I_MINUSSIGN, I_PLUS, I_MINUS, I_STAR */
	10, 0, 0, 60, 60, 60, 60, 70,
/* I_SLASH, I_MOD, I_AND, I_OR, I_XOR, I_NOT, I_LT, I_LE */
        70, 70, 30, 20, 20, 40, 50, 50,
/* I_EQ, I_NE, I_GE, I_GT,_ADDRESSOF,I_UNARYMINUS, I_STACKPTR, I_INPUT */
        50, 50, 50, 50, 0, 80, 0, 0,
/* I_OUTPUT, I_CALL, I_CALLVAR, I_BYTEINDEX, I_WORDINDEX, I_COLONEQUALS, I_MEMBER, I_BASED */
        0, 0, 0, 0, 0, 0, 0, 0,
/* I_CARRY, I_DEC, I_DOUBLE, I_HIGH, I_LAST, I_LENGTH, I_LOW, I_MOVE  */
        0, 0, 0, 0, 0, 0, 0, 0,
/* I_PARITY, I_ROL, I_ROR, I_SCL, I_SCR, I_SHL, I_SHR, I_SIGN */
        0, 0, 0, 0, 0, 0, 0, 0,
/*  I_SIZE, I_TIME, I_ZERO */
        0, 0, 0};


byte builtinsMap[] = {
     I_CARRY, I_DEC, I_DOUBLE, I_HIGH, I_INPUT, I_LAST, I_LENGTH, I_LOW,
     I_MOVE, I_OUTPUT, I_PARITY, I_ROL, I_ROR, I_SCL, I_SCR, I_SHL,
     I_SHR, I_SIGN, I_SIZE, I_STACKPTR, I_TIME, I_ZERO};




word parseSP;
word parseStack[100];
word operatorSP;
word operatorStack[50];
word exSP;
byte ex1Stack[100];
byte ex2Stack[100];
word ex3Stack[100];
word stSP;
byte st1Stack[300];
byte st2Stack[300];
word st3Stack[300];


void FatalError_ov1(byte err)
{
    hasErrors = true;
    b91C0, fatalErrorCode = err;
    longjmp(cleanup, -1);
}


void OptWrXrf()
{
#pragma pack(push, 1)
	struct {
		byte type;
		word info;
		word stmt;
	} tmp;
#pragma pack(pop)
    if (! XREF)
		return;
    tmp.type = T2_65;
    tmp.info = curInfoP - botInfo;
    tmp.stmt = curStmtNum;
    Fwrite(&xrfFile, (pointer)&tmp, 5);
}

void WrTx2File(pointer buf, byte cnt)
{
    t2CntForStmt = t2CntForStmt + 1;
    if (! hasErrors) 
	Fwrite(&tx2File, buf, cnt);
    else if (T2_STMTCNT <= *buf && *buf <= T2_ERROR)
	Fwrite(&tx2File, buf, cnt);
}

static void Sub_4251(pointer typeP, byte cnt)
{
    if (b88B3)
        if (*typeP == T2_STMTCNT) {
            WrTx2File((pointer)&linfo, 7);
            b88B3 = false;
            if (tx1Item.dataw[0] == 0)
                return;
        }
    WrTx2File(typeP, cnt);
}

word WrTx2Item(byte arg1b)
{
    Sub_4251(&arg1b, 1);
    return t2CntForStmt;
}

word WrTx2Item1Arg(byte arg1b, word arg2w)
{
#pragma pack(push, 1)
	struct { byte arg1; word arg2; } tmp = { arg1b, arg2w };
#pragma pack(pop)
    Sub_4251((pointer)&tmp, 3);
    return t2CntForStmt;
}

word WrTx2Item2Arg(byte arg1b, word arg2w, word arg3w)
{
#pragma pack(push, 1)
	struct { byte arg1; word arg2, arg3; } tmp = { arg1b, arg2w, arg3w };
#pragma pack(pop)
    Sub_4251((pointer)&tmp, 5);
    return t2CntForStmt;
}

word WrTx2Item3Arg(byte arg1b, word arg2w, word arg3w, word arg4w)
{
#pragma pack(push, 1)
	struct { byte arg1; word arg2, arg3, arg4; } tmp = { arg1b, arg2w, arg3w, arg4w };
#pragma pack(pop)

    Sub_4251((pointer)&tmp, 7);
    return t2CntForStmt;
}

word Sub_42EF(word arg1w)
{
    return (t2CntForStmt + 1 - arg1w);
}

void MapLToT2()
{
    byte i;
    i = tx1ItemLengths[tx1Item.type] + 1;
    tx1Item.type = b402F[tx1Item.type];
    Sub_4251(&tx1Item.type, i);
}

void WrTx2Error(byte arg1b)
{
    hasErrors = true;
    WrTx2Item1Arg(T2_SYNTAXERROR, arg1b);
}


void WrTx2ExtError(byte arg1b)
{
    hasErrors = true;
    if (curInfoP != 0) 
	    WrTx2Item2Arg(T2_TOKENERROR, arg1b, curInfoP - botInfo);
    else
	    WrTx2Item1Arg(T2_SYNTAXERROR, arg1b);
}

void SetRegetTx1Item()
{
    regetTx1Item = true;
}

void RdTx1Item()
{
    word tx1ItemLen;

    Fread(&tx1File, (pointer)&tx1Item, 1);
    tx1ItemLen = tx1ItemLengths[tx1Item.type];
    if (tx1ItemLen != 0)
        if (tx1ItemLen != 255) {		/* i.e. not a string */
            Fread(&tx1File, (pointer)tx1Item.dataw, tx1ItemLen);
        } else {
            Fread(&tx1File, (pointer)tx1Item.dataw, 2);
            tx1ItemLen = tx1Item.dataw[0];
            Fread(&tx1File, (pointer)&tx1Item.dataw[1], tx1Item.dataw[0]);
        }
    tx1Aux1 = tx1Aux1Map[tx1Item.type];
    tx1Aux2 = tx1Aux2Map[tx1Item.type];
}
