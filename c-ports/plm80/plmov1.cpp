// $Id: plmov1.cpp,v 1.2 2005/07/11 20:25:34 Mark Exp $
// vim:ts=4
#include "plm.hpp"
#include "common.hpp"
#include <stdio.h>


byte byte_3EBF;

word markedStSP;
word T2CntForStmt;
static word curProcInfo_p;
word curStmtNum;
#pragma pack(push, 1)
struct {
    word varInfoOffset;
    word varArrayIndex;
    word varNestedArrayIndex;
    word val;
} atData;

byte regetTx1Item;
byte byte_88B3;
struct {
    byte type;
    word data[129];
} tx1Item;

#pragma pack(pop)
byte tx1Aux2;
byte tx1Aux1;
byte tx1RdBuf[512];
byte tx2Buf[512];
byte xrfBuf[512];
byte atBuf[512];
byte byte_91C0;
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
word rValue;
byte useByteArith;
word word_99BF;
word word_99C1;
byte byte_99FF[20];
byte byte_9A13[20];
word procInfoStack[20];
word hNodes[20];
word eNodes[20];
word word_9A9F[20];
word word_9AC7[20];		// junk
word controlSP;
word atStmtNum;
byte byte_9B40;
static union {
		atFData_t atFData;
		byte pad[256];	// to allow string to be read in
};

word atOffset;


void sub_7D88();
void sub_76D9();
void sub_7695();
void sub_75F7();
void sub_7486();
void sub_73DC();
void sub_7323();
void sub_72EF(word *arg1w);
void sub_719D();
void sub_711F();
void sub_7049();
void sub_701C();
void sub_6F94(address arg1w, address arg2w, word arg3w);
void sub_6EF6(word arg1w);
void sub_6EE0();
void sub_6923();
word sub_6917();
void sub_67E3();
void chkEndOfStmt();
word sub_677C(word arg1w);
word sub_671D(word arg1w);
static word genLocalLabel();
void sub_6523();
void sub_5BF5(word arg1w);
void flgVisited(word arg1w, word arg2w);
void sub_5AD8();
byte sub_59D4();
byte sub_5945();
void sub_521B();
byte sub_512E(word arg1w);
void sub_50D5();
void mkIndexNode();
void sub_4DCF(byte arg1b);
byte getCallArgCnt();
void sub_4D38();
void sub_4D2C();
void sub_4CFD(word arg1w);
void sub_4CC2();
void acceptOpAndArgs();
void resetStacks();
word parseDataItems(word arg1w);
void getRestictedVar();
void getRestrictedArrayIndex();
void evalSimpleExpr();
void simpleExprNumber();
void simpleExprUnsigned();
void getRestrictedExpr();
void sub_467D();
void sub_4631();
void sub_45E0();
void sub_4599();
word sub_42EF(word arg1w);
void sub_4251(byte *arg1w, byte arg2b);
void sub_3F8B();
void sub_3F19();


void deleteFile(file_t *fp);
void procAtFile();
word getVarSize();
word getElementSize();
void parseDATA_INITIAL();
void parseAT();
void parseGO();
void parseGOTO();
void parseEIDIHLT();
void parseRETURN();
void parseCALL();
void parseSTATEMENT();
void parseEND();
void parseDOLOOP();
void parseCASE();
void parseWHILE();
void parsePROCEDURE();
void parseIF();
void parseSTMTCNT();
void popControl();
void pushControl(byte arg1b);
void popScope();
void pushScope(word arg1w);
void stmtParse15();
void stmtParse14();
void stmtParse13();
void stmtParse12();
void stmtParse11();
void stmtParse10();
void stmtParse9();
void stmtParse8();
void stmtParse7();
void stmtParse6();
void stmtParse5();
void stmtParse4();
void stmtParse3();
void stmtParse2();
void stmtParse1();
void stmtParse0();
word stmtParse(word arg1w);
void exprParse();
void exprParse20();
void exprParse19();
void exprParse18();
void exprParse17();
void exprParse16();
void exprParse15();
void exprParse14();
void exprParse13();
void exprParse12();
void exprParse11();
void exprParse10();
void exprParse9();
void exprParse8();
void exprParse7();
void exprParse6();
void exprParse5();
void exprParse4();
void exprParse3();
void exprParse2();
void exprParse1();
void exprParse0();
void parsePortNum(byte arg1b);
void exprMakeNode(byte arg1b, byte arg2b);
void popOperatorStack();
void pushOperator(byte arg1b);
void moveExpr2Stmt();
void stmtPush3(byte arg1b, byte arg2b, word arg3w);
void exprPush2(byte arg1b, word arg2w);
void swapOperandStack();
void exprPop();
void exprPush3(byte arg1b, byte arg2b, byte arg3w);
void pushParseByte(byte arg1b);
void popParseStack();
void pushParseWord(word arg1w);
void wrAtFileWord(word arg1w);
void wrAtFileByte(byte arg1b);
void wrAtFile(void *buf, word cnt);
void expectRPAREN(byte arg1b);
void resyncRPAREN();
byte matchTx2AuxFlag(byte arg1b);
byte notMatchTx2Item(byte arg1b);
byte matchTx2Item(byte arg1b);
void getTx1Item();
void rdTx1Item();
void setRegetTx1Item();
void wrTx2ExtError(byte arg1b);
void wrTx2Error(byte arg1b);
void mapLtoT2();
word wrTx2Item3Arg(byte arg1b, word arg2w, word arg3w, word arg4w);
word wrTx2Item2Arg(byte arg1b, word arg2w, word arg3w);
word wrTx2Item1Arg(byte arg1b, word arg2w);
word wrTx2Item(byte arg1b);
void wrTx2File(byte *buf, byte cnt);
void optWrXrf();

#ifdef WATCH
void dumpStks()
{
    int i = exSP > stSP ? exSP : stSP;

            printf("    ex Stack           \t     st Stack\n");
    for (; i > 0; i--) {
        if (i <= exSP)
            printf(" %2d: %3d, %3d, 0x%04X", i, ex1Stack[i], ex2Stack[i], ex3Stack[i]);
        else
            printf(" %2d:                 ", i);
        if (i <= stSP)
            printf("\t%3d, %3d, 0x%04X", st1Stack[i], st2Stack[i], st3Stack[i]);
        putchar('\n');
    }
}

#else
#define dumpStks()
#endif

int overlay1() {
	sub_3F19();	// create files and preload tx1
	sub_6523();
	for(;;) {
        try {
			sub_3F8B();
			sub_6EE0();
			if (hasErrors) 
                return 6;
			else
                return 2;
        }
        catch (...) {
        }
		wrTx2ExtError(byte_91C0);
		while (tx1Item.type != L_EOF) {
			if (tx1Item.type == L_STMTCNT) {
				T2CntForStmt = 0;
				mapLtoT2();
				T2CntForStmt = tx1Item.data[0];
			}
			getTx1Item();
		}
	}
}


void sub_3F19() {
//	assignFileBuffer(&tx1File, tx1RdBuf, 512, 1);
	openFile(&tx2File, 3);
//	assignFileBuffer(&tx2File, tx2Buf, 512, 2);
	if (IXREF || XREF || SYMBOLS) {
		openFile(&xrfFile, 2);
//		assignFileBuffer(&xrfFile, xrfBuf, 512, 2);
	}
	openFile(&atFile, 3);
//	assignFileBuffer(&atFile, atBuf, 16, 2);
	blockDepth = 1;
	procChains[1] = 0;
}

void sub_3F8B() {
	if (byte_88B3) 
		wrTx2File((byte *)&linfo, 7);
	closeFile(&tx1File);
	deleteFile(&tx1File);
	openFile(&tx1File, 3);
	if (IXREF || XREF || SYMBOLS) {
		ifwrite(&xrfFile, &byte_3EBF, 1);
//		flushFile(&xrfFile);
		closeFile(&xrfFile);
	}
	wrAtFileByte(AT_END);
	wrAtFileByte(AT_EOF);
//	flushFile(&atFile);
	rewindFile(&atFile);
}



// L_LINEINFO, L_SYNTAXERROR, L_TOKENERROR, L_LIST, L_NOLIST, L_CODE, L_NOCODE, L_EJECT,
// L_INCLUDE, L_STMTCNT, L_LABELDEF, L_LOCALLABEL, L_JMP, L_JMPFALSE, L_PROCEDURE, L_SCOPE,
// L_END, L_DO, L_DOLOOP, L_WHILE, L_CASE, L_CASELABEL, L_IF, L_STATEMENT,
// L_CALL, L_RETURN, L_GO, L_GOTO, L_SEMICOLON, L_ENABLE, L_DISABLE, L_HALT,
// L_EOF, L_AT, L_INITIAL, L_DATA, L_VARIABLE, L_NUMBER, L_STRING, L_PLUSSIGN,
// L_MINUSSIGN, L_PLUS, L_MINUS, L_STAR, L_SLASH, L_MOD, L_COLONEQUALS, L_AND,
// L_OR, L_XOR, L_NOT, L_LT, L_LE, L_EQ, L_NE, L_GE,
// L_GT, L_COMMA, L_LPAREN, L_RPAREN, L_PERIOD, L_TO, L_BY, L_INVALID,
// L_MODULE, L_XREFINFO, L_XREF, L_EXTERNAL


// index into precedence table
byte tx1Aux1Map[] = { 0, 0, 0, 0, 0, 0, 0, 0,
		      0, 0, 0, 0, 0, 0, 0, 0,
		      0, 0, 0, 0, 0, 0, 0, 0,
		      0, 0, 0, 0, 0, 0, 0, 0,
		      0, 0, 0, 0, 1, 2, 0, 3,
		      4, 5, 6, 7, 8, 9, 0x1D, 0xA,
		      0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x12,
		      0x13, 0, 0, 0, 0, 0, 0, 0,
		      0, 0, 0, 0};


// 0x80	- expression item
// 0x40 - binary operator
// 0x20 - pass through
// 0x10 - PROCEDURE, AT, DATA, INITIAL or EXTERNAL

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

byte tx1ItemLengths[] = {
	6, 2, 4, 0, 0, 0, 0, 0,
	6, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 2, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 2, 2, 2, 2, 2,0xFF, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 2, 2, 2
};


byte byte_413F[] = {
	// ?, I_VARIABLE, I_NUMBER, I_PLUSSIGN, I_MINUSSIGN, I_PLUS, I_MINUS, I_STAR
	0 , T2_VARIABLE, T2_NUMBER, T2_ADDB, T2_MINUSSIGN, T2_PLUS, T2_MINUS, T2_STAR,
	// I_SLASH, I_MOD, I_AND, I_OR, I_XOR, I_NOT, I_LT, I_LE
	T2_SLASH, T2_MOD, T2_AND, T2_OR, T2_XOR, T2_NOT, T2_LT, T2_LE,
	// I_EQ, I_NE, I_GE, I_GT, I_ADDRESSOF, I_UNARYMINUS, I_STACKPTR, I_INPUT
	T2_EQ, T2_NE, T2_GE, T2_GT, T2_ADDRESSOF, T2_UNARYMINUS, T2_STACKPTR, T2_INPUT,
	// I_OUTPUT, I_CALL, I_CALLVAR, I_BYTEINDEX, I_WORDINDEX, I_COLONEQUALS, I_MEMBER, I_BASED
	T2_OUTPUT, T2_CALL, T2_CALLVAR, T2_BYTEINDEX, T2_WORDINDEX, T2_STORE, T2_MEMBER, T2_BASED,
	// I_CARRY, I_DEC, I_DOUBLE, I_HIGH, I_LAST, I_LENGTH, I_LOW, I_MOVE,
	T2_CARRY, T2_DEC, T2_DOUBLE, T2_HIGH, T2_LAST, T2_LENGTH, T2_LOW, T2_MOVE,
	// I_PARITY, I_ROL, I_ROR, I_SCL, I_SCR, I_SHL, I_SHR, I_SIGN,
	T2_PARITY, T2_ROL, T2_ROR, T2_SCL, T2_SCR, T2_SHL, T2_SHR, T2_SIGN,
	// I_SIZE, I_TIME, I_ZERO,
	T2_SIZE, T2_TIME, T2_ZERO
};

byte byte_4172[] = {
// I_STRING, I_VARIABLE, I_NUMBER, I_PLUSSIGN, I_MINUSSIGN, I_PLUS, I_MINUS, I_STAR
		10, 0, 0, 60, 60, 60, 60, 70,
// I_SLASH, I_MOD, I_AND, I_OR, I_XOR, I_NOT, I_LT, I_LE
	    70, 70, 30, 20, 20, 40, 50, 50,
// I_EQ, I_NE, I_GE, I_GT,_ADDRESSOF,I_UNARYMINUS, I_STACKPTR, I_INPUT
	    50, 50, 50, 50, 0, 80, 0, 0,
// I_OUTPUT, I_CALL, I_CALLVAR, I_BYTEINDEX, I_WORDINDEX, I_COLONEQUALS, I_MEMBER, I_BASED
	    0, 0, 0, 0, 0, 0, 0, 0,
// I_CARRY, I_DEC, I_DOUBLE, I_HIGH, I_LAST, I_LENGTH, I_LOW, I_MOVE 
	    0, 0, 0, 0, 0, 0, 0, 0,
// I_PARITY, I_ROL, I_ROR, I_SCL, I_SCR, I_SHL, I_SHR, I_SIGN
	    0, 0, 0, 0, 0, 0, 0, 0,
//  I_SIZE, I_TIME, I_ZERO
	    0, 0, 0};


byte builtinsMap[] = {
     I_CARRY, I_DEC, I_DOUBLE, I_HIGH, I_INPUT, I_LAST, I_LENGTH, I_LOW,
     I_MOVE, I_OUTPUT, I_PARITY, I_ROL, I_ROR, I_SCL, I_SCR, I_SHL,
     I_SHR, I_SIGN, I_SIZE, I_STACKPTR, I_TIME, I_ZERO};



void fatalError_ov1(byte errcode) {
	hasErrors = 0xff;
	fatalErrorCode = byte_91C0 = errcode;
	throw;
}



void optWrXrf() {
	byte tmp[5];

	if (XREF) {
		tmp[0] = T2_65;
		*(word *)(tmp + 1) = info2Off(curInfo_p);
		*(word *)(tmp + 3) = curStmtNum;
		ifwrite(&xrfFile, tmp, 5);
	}
}

void wrTx2File(byte *buf, byte cnt) {
	static int ocnt;
	ocnt += cnt;
	T2CntForStmt++;
	if (!hasErrors) 
		ifwrite(&tx2File, buf, cnt);
	else if (T2_STMTCNT <= *buf && *buf <= T2_ERROR)
		ifwrite(&tx2File, buf, cnt);
}

void sub_4251(byte *arg1w, byte arg2b) {
	if (byte_88B3 && *arg1w == T2_STMTCNT) {
		wrTx2File((byte *)&linfo, 7);
		byte_88B3 = 0;
		if (tx1Item.data[0] == 0)
			return;
	}
	wrTx2File(arg1w, arg2b);
}

word wrTx2Item(byte arg1b) {
	byte tmp[1];
	tmp[0] = arg1b;
	sub_4251(tmp, 1);
	return T2CntForStmt;
}


word wrTx2Item1Arg(byte arg1b, word arg2w) {
	byte tmp[3];
	tmp[0] = arg1b;
	*(word *)(tmp + 1) = arg2w;
	sub_4251(tmp, 3);
	return T2CntForStmt;
}


word wrTx2Item2Arg(byte arg1b, word arg2w, word arg3w) {
	byte tmp[5];
	tmp[0] = arg1b;
	*(word *)(tmp + 1) = arg2w;
	*(word *)(tmp + 3) = arg3w;
	sub_4251(tmp, 5);
	return T2CntForStmt;
}

word wrTx2Item3Arg(byte arg1b, word arg2w, word arg3w, word arg4w) {
	byte tmp[7];
	tmp[0] = arg1b;
	*(word *)(tmp + 1) = arg2w;
	*(word *)(tmp + 3) = arg3w;
	*(word *)(tmp + 5) = arg4w;
	sub_4251(tmp, 7);
	return T2CntForStmt;
}

word sub_42EF(word arg1w) {
	return (T2CntForStmt + 1 - arg1w);
}


byte byte_402F[] = {
	// L_LINEINFO, L_SYNTAXERROR, L_TOKENERROR, L_LIST, L_NOLIST, L_CODE, L_NOCODE, L_EJECT,
	T2_LINEINFO, T2_SYNTAXERROR, T2_TOKENERROR, T2_LIST, T2_NOLIST, T2_CODE, T2_NOCODE, T2_EJECT,
	// L_INCLUDE, L_STMTCNT, L_LABELDEF, L_LOCALLABEL, L_JMP, L_JMPFALSE, L_PROCEDURE, L_SCOPE,
	T2_INCLUDE, T2_STMTCNT, T2_LABELDEF, T2_LOCALLABEL, T2_JMP, T2_JMPFALSE, T2_PROCEDURE, 0,
	// L_END, L_DO, L_DOLOOP, L_WHILE, L_CASE, L_CASELABEL, L_IF, L_STATEMENT,
	0, 0, 0, 0, T2_CASE, T2_CASELABEL, 0, 0,
	// L_CALL, L_RETURN, L_GO, L_GOTO, L_SEMICOLON, L_ENABLE, L_DISABLE, L_HALT,
	0, T2_RETURN, T2_GO_TO, T2_GO_TO, T2_SEMICOLON, T2_ENABLE, T2_DISABLE, T2_HALT,
	// L_EOF, L_AT, L_INITIAL, L_DATA, L_VARIABLE, L_NUMBER, L_STRING, L_PLUSSIGN,
	0, 0, 0, 0,T2_VARIABLE, 0, 0, T2_ADDB,
	// L_MINUSSIGN, L_PLUS, L_MINUS, L_STAR, L_SLASH, L_MOD, L_COLONEQUALS, L_AND,
	T2_MINUSSIGN, T2_PLUS, T2_MINUS, T2_STAR, T2_SLASH, T2_MOD, T2_STORE, T2_AND,
	// L_OR, L_XOR, L_NOT, L_LT, L_LE, L_EQ, L_NE, L_GE,
	T2_OR, T2_XOR, T2_NOT, T2_LT, T2_LE, T2_EQ, T2_NE, T2_GE,
	// L_GT, L_COMMA, L_LPAREN, L_RPAREN, L_PERIOD, L_TO, L_BY, L_UNUSED,
	T2_GT, 0, 0, 0, 0, 0, 0, 0,
	// L_MODULE, L_XREFINFO, L_XREF, L_EXTERNAL
	T2_MODULE, 0, 0, 0};

void mapLtoT2() {
	byte i;
	i = tx1ItemLengths[tx1Item.type] + 1;
	tx1Item.type = byte_402F[tx1Item.type];
	sub_4251(&tx1Item.type, i);
}

void wrTx2Error(byte arg1b) {
	hasErrors = 0xff;
	word junk = wrTx2Item1Arg(T2_SYNTAXERROR, arg1b);
}


void wrTx2ExtError(byte arg1b) {
	word junk;
	hasErrors = 0xff;
	if (curInfo_p != 0) 
		junk = wrTx2Item2Arg(T2_TOKENERROR, arg1b, info2Off(curInfo_p));
	else
		junk = wrTx2Item1Arg(T2_SYNTAXERROR, arg1b);
}

void setRegetTx1Item() {
	regetTx1Item = 0xff;
}

void rdTx1Item() {
	word tx1ItemLen;
	ifread(&tx1File, &tx1Item, 1);
	if((tx1ItemLen = tx1ItemLengths[tx1Item.type]) != 0) {
        if (tx1ItemLen != 255) {		// i.e. not a string
			ifread(&tx1File, tx1Item.data, tx1ItemLen);
        } else {
			ifread(&tx1File, tx1Item.data, 2);
			tx1ItemLen = tx1Item.data[0];
			ifread(&tx1File, &tx1Item.data[1], tx1ItemLen);
		}
	}
	tx1Aux1 = tx1Aux1Map[tx1Item.type];
	tx1Aux2 = tx1Aux2Map[tx1Item.type];
}


void getTx1Item() {
    byte *p, *q;
    byte i;
	if (regetTx1Item) {
		regetTx1Item = 0;
		return;
	}
	for (;;) {
		rdTx1Item();
		if (tx1Item.type == L_TOKENERROR) {
			if ((curSymbol_p = tx1Item.data[1]) != 0) {
				if (((curSymbol_p->infoChain) >> 8) == 0xff)
					curSymbol_p->infoChain = 0;
				if((curInfo_p = curSymbol_p->infoChain) == 0)
					createInfo(0, UNK_T);
				tx1Item.data[1] = info2Off(curInfo_p);
			}
			mapLtoT2();
		} else if ((tx1Item.type == L_XREFINFO && XREF)
			  || (tx1Item.type == L_XREF && (IXREF || XREF || SYMBOLS))) {
			tx1Item.data[1] = curStmtNum;
			ifwrite(&xrfFile, &tx1Item, 5);
		} else if (tx1Item.type == L_LINEINFO) {
			if (byte_88B3)
				wrTx2File((byte *)&linfo, 7);
			for (i = 7, p = (byte *)&linfo, q = &tx1Item.type; i != 0; i--)
				*p++ = *q++;
			linfo.itemType = T2_LINEINFO;
			byte_88B3 = 0xff;
		} else if (tx1Aux2 & 0x20)
			mapLtoT2();
		else if (tx1Item.type == L_STMTCNT && tx1Item.data[0] == 0)
			mapLtoT2();
		else if (tx1Item.type != L_XREF && tx1Item.type != L_XREFINFO)
			break;
	}
	if (tx1Item.type == L_VARIABLE)
		curSymbol_p = tx1Item.data[0];
	if (tx1Aux2 & 0x10)
		tx1Item.data[0] = off2Info(tx1Item.data[0]);
}

byte matchTx2Item(byte arg1b) {
	getTx1Item();
	if (tx1Item.type == arg1b)
		return 0xff;
	setRegetTx1Item();
	return 0;
}

byte notMatchTx2Item(byte arg1b) {
	return !matchTx2Item(arg1b);
}

byte matchTx2AuxFlag(byte arg1b) {
	getTx1Item();
	if (tx1Aux2 & arg1b)
		return 0xff;
	setRegetTx1Item();
	return 0;
}


void sub_4599() {
	do {
		getTx1Item();
	} while ((tx1Aux2 & 0x80) && tx1Item.type != L_RPAREN);
	setRegetTx1Item();
}

void resyncRPAREN() {
	sub_4599();
	if (matchTx2Item(L_RPAREN))
#pragma warning(suppress:4390)
		;
}

void expectRPAREN(byte arg1b) {
	if (notMatchTx2Item(L_RPAREN)) {
		wrTx2ExtError(arg1b);
		resyncRPAREN();
	}
}


void sub_45E0() {
	findInfo();
	if (curInfo_p == 0 || getInfoType() == LIT_T)
		createInfo(256, BYTE_T);
	optWrXrf();
	if (getInfoType() != BUILTIN_T && !testInfoFlag(F_LABEL) && !testInfoFlag(F_DECLARED)) {
		wrTx2ExtError(ERR105);	// UNDECLARED IDENTIFIER
		setInfoFlag(F_DECLARED);
	}
}


void sub_4631() {
	word tmp = curInfo_p;
	sub_7D88();
	if (curInfo_p == 0) {
		createInfo(0, BYTE_T);
		setOwningStructure(tmp);
		setInfoFlag(F_MEMBER);
	}
	if (!testInfoFlag(F_LABEL) && !testInfoFlag(F_DECLARED)) {
		wrTx2ExtError(ERR112);	// UNDECLARED STRUCTURE MEMBER
		setInfoFlag(F_DECLARED);
	}
	optWrXrf();
}


void sub_467D() {
	sub_45E0();
	if(matchTx2Item(L_PERIOD))
		if (getInfoType() != STRUCT_T)
			wrTx2ExtError(ERR110);	// INVALID LEFT OPERAND OF QUALIFICATION, NOT A STRUCTURE
		else if (notMatchTx2Item(L_VARIABLE))
			wrTx2ExtError(ERR111);	// INVALID RIGHT OPERAND OF QUALIFICATION, NOT IDENTIFIER
		else
			sub_4631();
}

void wrAtFile(void *buf, word cnt) {
	ifwrite(&atFile, buf, cnt);
}

void wrAtFileByte(byte arg1b) {
	wrAtFile(&arg1b, 1);
}

void wrAtFileWord(word arg1w) {
	wrAtFile(&arg1w, 2);
}

void getRestrictedExpr() {
	atData.varInfoOffset = 0;
	atData.varArrayIndex = 0;
	atData.varNestedArrayIndex = 0;
	atData.val = 0;
	if (matchTx2Item(L_PERIOD)) {
		getRestictedVar();
		if (matchTx2Item(L_PLUSSIGN))
			;
		else if (matchTx2Item(L_MINUSSIGN))
			setRegetTx1Item();
		else
			return;
		useByteArith = 0;
		evalSimpleExpr();
		atData.val = rValue;
	} else if (matchTx2Item(L_RPAREN))
		setRegetTx1Item();
	else {
		useByteArith = 0xff;
		evalSimpleExpr();
		atData.val = rValue;
	}
}


void simpleExprUnsigned() {
	if (matchTx2Item(L_NUMBER))
		rValue = tx1Item.data[0];
	else {
		if (matchTx2Item(L_VARIABLE))
			sub_45E0();
		wrTx2ExtError(ERR151);	// INVALID OPERAND IN RESTRICTED EXPRESSION
		rValue = 0;
	}
	useByteArith &= ((rValue >> 8) == 0);
}

void simpleExprNumber() {
	if (matchTx2Item(L_MINUSSIGN)) {
		simpleExprUnsigned();
		if (useByteArith) 
			rValue = - byte(rValue);
		else
			rValue = - rValue;
	} else
		simpleExprUnsigned();
}


void evalSimpleExpr() {
	word p;
	simpleExprNumber();
	p = rValue;
	for (;;) {
		if (matchTx2Item(L_PLUSSIGN)) {
			simpleExprNumber();
			if (useByteArith) {
				p =  (p + rValue) & 0xff;
			} else 
				p +=  rValue;
		} else if (matchTx2Item(L_MINUSSIGN)) {
			simpleExprNumber();
			if (useByteArith)
				p = (p - rValue) & 0xff;
			else
				p -= rValue;
		} else
			break;
	}
	rValue = p;
}


void getRestrictedArrayIndex() {
	useByteArith = 0xff;
	if (!testInfoFlag(F_ARRAY))
		wrTx2ExtError(ERR149);	// INVALID SUBSCRIPTING IN RESTRICTED REFERENCE
	evalSimpleExpr();
	expectRPAREN(ERR150);	// MISSING ') ' AT END OF RESTRICTED SUBSCRIPT
}



void getRestictedVar() {
	if (notMatchTx2Item(L_VARIABLE)) {
		wrTx2Error(ERR147);	// MISSING IDENTIFIER FOLLOWING DOT OPERATOR
		sub_4599();
		return;
	}
	sub_45E0();
	atData.varInfoOffset = curInfo_p;
    if (getInfoType() == BUILTIN_T) {
		wrTx2Error(ERR123);	// INVALID DOT OPERAND, BUILT-IN PROCEDURE ILLEGAL
		sub_4599();
		return;
	}
	if (matchTx2Item(L_LPAREN)) {
		getRestrictedArrayIndex();
		atData.varArrayIndex = rValue;
	}
	if (matchTx2Item(L_PERIOD)) {
		curInfo_p = atData.varInfoOffset;
		if (getInfoType() != STRUCT_T) {
			wrTx2ExtError(ERR148);	// INVALID QUALIFICATION IN RESTRICTED REFERENCE
			sub_4599();
			return;
		}
		if (notMatchTx2Item(L_VARIABLE)) {
			wrTx2Error(ERR147);	// MISSING IDENTIFIER FOLLOWING DOT OPERATOR
			atData.varInfoOffset = info2Off(atData.varInfoOffset);
			return;
		}
		sub_4631();
		atData.varInfoOffset = curInfo_p;
		if (matchTx2Item(L_LPAREN)) {
			getRestrictedArrayIndex();
			atData.varNestedArrayIndex = rValue;
		}
	}
	atData.varInfoOffset = info2Off(atData.varInfoOffset);
}

word parseDataItems(word arg1w) {
	word p = 0;
	wrAtFileByte(AT_DHDR);
	wrAtFileWord(info2Off(arg1w));
	wrAtFileWord(curStmtNum);
	for (;;) {
		if (matchTx2Item(L_STRING)) {
			wrAtFileByte(AT_STRING);
			wrAtFileWord(tx1Item.data[0]);
			wrAtFile(&tx1Item.data[1], tx1Item.data[0]);
			curInfo_p = arg1w;
			if (getInfoType() == ADDRESS_T) 
				p += (tx1Item.data[0] + 1) / 2;
			else 
				p += tx1Item.data[0];
		} else {
			getRestrictedExpr();
			wrAtFileByte(AT_DATA);
			wrAtFile(&atData, 8);
			p++;
		}
		if (notMatchTx2Item(L_COMMA))
			break;
		if (matchTx2Item(L_RPAREN)) {
			wrTx2Error(ERR151);	// INVALID OPERAND IN RESTRICTED EXPRESSION
			setRegetTx1Item();
			break;
		}
	}
	wrAtFileByte(AT_END);
	expectRPAREN(ERR152);	// MISSING ') ' AFTER CONSTANT LIST
	curInfo_p = arg1w;;
	return p;
}

void resetStacks() {
	parseSP = 0;
	exSP = 0;
	operatorSP = 0;
	stSP = 0;
}

void pushParseWord(word arg1w) {
	if (parseSP == 0x63)
		fatalError(ERR119);	// LIMIT EXCEEDED: PROGRAM TOO COMPLEX
	parseStack[++parseSP] = arg1w;
}


void popParseStack() {
	if (parseSP == 0)
		fatalError(ERR159);	// COMPILER ERROR: PARSE STACK UNDERFLOW
	parseSP--;
}


void pushParseByte(byte arg1b) {
	pushParseWord(arg1b);
}

void exprPush3(byte arg1b, byte arg2b, word arg3w) {
	if (exSP == 0x63)
		fatalError(ERR121);	// LIMIT EXCEEDED: EXPRESSION TOO COMPLEX
	ex1Stack[++exSP] = arg1b;
	ex2Stack[exSP] = arg2b;
	ex3Stack[exSP] = arg3w;
}

void exprPop() {
	if (exSP == 0)
		fatalError(ERR160);	// COMPILER ERROR: OPERAND STACK UNDERFLOW
	exSP--;
}



void swapOperandStack() {
	if (exSP < 2)
		fatalError(ERR161);	// COMPILER ERROR: ILLEGAL OPERAND STACK EXCHANGE
	word i = exSP - 1;
	byte op1 = ex1Stack[exSP];
	byte op2 = ex2Stack[exSP];
	word op3 = ex3Stack[exSP];
	ex1Stack[exSP] = ex1Stack[i];
	ex2Stack[exSP] = ex2Stack[i];
	ex3Stack[exSP] = ex3Stack[i];
	ex1Stack[i] = op1;
	ex2Stack[i] = op2;
	ex3Stack[i] = op3;
}


void exprPush2(byte arg1b, word arg2w) {
	exprPush3(arg1b, 0, arg2w);
}

void stmtPush3(byte arg1b, byte arg2b, word arg3w) {
	if (stSP == 299)
		fatalError(ERR122);	// LIMIT EXCEEDED: PROGRAM TOO COMPLEX	
	st1Stack[++stSP] = arg1b;
	st2Stack[stSP] = arg2b;
	st3Stack[stSP] = arg3w;
}


void moveExpr2Stmt() {
	stmtPush3(ex1Stack[exSP], ex2Stack[exSP], ex3Stack[exSP]);
	exprPop();
}

void pushOperator(byte arg1b) {
	if (operatorSP == 0x31)
		fatalError(ERR120);	// LIMIT EXCEEDED: EXPRESSION TOO COMPLEX
	operatorStack[++operatorSP] = arg1b;
}


void popOperatorStack() {
	if (operatorSP == 0)
		fatalError(ERR162);	// COMPILER ERROR: OPERATOR STACK UNDERFLOW
	operatorSP--;
}


void exprMakeNode(byte arg1b, byte arg2b) {
	byte i, j;
	word w = stSP+1;

	if (exSP < arg2b)
		fatalError(ERR163);	// COMPILER ERROR: GENERATION FAILURE 
	for (j = exSP - arg2b + 1, i = arg2b; i != 0; i--, j++) 
		stmtPush3(ex1Stack[j], ex2Stack[j], ex3Stack[j]);

	exSP -= arg2b;
	if (arg2b == 0)
		w = 0;
	exprPush3(arg1b, arg2b, w);

}



void acceptOpAndArgs() {
	byte i;
	if ((i = (byte)operatorStack[operatorSP]) == I_NOT || i == I_UNARYMINUS)
		exprMakeNode(i, 1);
	else
		exprMakeNode(i, 2);
	popOperatorStack();
}

void sub_4CC2() {
	word p;
	if (testInfoFlag(F_MEMBER)) {
		p = curInfo_p;
		curInfo_p = getOwningStructure();
		exprPush2(I_VARIABLE, curInfo_p);
		exprPush2(I_VARIABLE, p);
		exprMakeNode(I_MEMBER, 2);
	} else
		exprPush2(I_VARIABLE, curInfo_p);
}

void sub_4CFD(word arg1w) {
	curInfo_p = arg1w;
	if (testInfoFlag(F_BASED)) {
		curInfo_p = getBase();
		sub_4CC2();
		curInfo_p = arg1w;
		swapOperandStack();
		exprMakeNode(I_BASED, 2);
	}
}

void sub_4D2C() {
	sub_4CC2();
	sub_4CFD(curInfo_p);
}


void sub_4D38() {
	if (getDataType() == 0)
		wrTx2ExtError(ERR131);	// ILLEGAL REFERENCE TO UNTYPED PROCEDURE
}


byte getCallArgCnt() {
	byte i, j, k;
	i = (byte)parseStack[parseSP];
	j = (byte)operatorStack[operatorSP];
	popParseStack();
	popOperatorStack();
	if (i == j)
		return i;
	if (j > i) {
		wrTx2ExtError(ERR153);	// INVALID NUMBER OF ARGUMENTS IN CALL, TOO MANY
		k = j - i;
		while (k != 0) {
			exprPop();
			k--;
		}
	} else {
		wrTx2ExtError(ERR154);	// INVALID NUMBER OF ARGUMENTS IN CALL, TOO FEW
		k = i - j;
		while (k != 0) {
			exprPush2(I_NUMBER, 0);
			k--;
		}
	}
	return i;
}

void sub_4DCF(byte arg1b) {
	if (notMatchTx2Item(L_LPAREN)) {
		wrTx2ExtError(ERR124);	// MISSING ARGUMENTS FOR BUILT-IN PROCEDURE
		exprPush2(I_NUMBER, 0);
		return;
	}
	if (notMatchTx2Item(L_VARIABLE)) {
		wrTx2Error(ERR125);	// ILLEGAL ARGUMENT FOR BUILT-IN PROCEDURE
		exprPush2(I_NUMBER, 0);
	} else {
		sub_45E0();
		if (matchTx2Item(L_LPAREN)) {
			if (testInfoFlag(F_ARRAY)) {
				resyncRPAREN();
				if (matchTx2Item(L_RPAREN)) {
					if (arg1b == I_LENGTH || arg1b == I_LAST) {
						wrTx2ExtError(ERR125); //  ILLEGAL ARGUMENT FOR BUILT-IN PROCEDURE
						exprPush2(I_NUMBER, 0);
					} else 
						exprPush2(arg1b, curInfo_p);
					return;
				}
			} else 
				wrTx2ExtError(ERR127);	// INVALID SUBSCRIPT ON NON-ARRAY
		} 
		if (matchTx2Item(L_PERIOD)) {
			if (getInfoType() != STRUCT_T)
				wrTx2ExtError(ERR110); // INVALID LEFT OPERAND OF QUALIFICATION, NOT A STRUCTURE
			else if (notMatchTx2Item(L_VARIABLE))
				wrTx2ExtError(ERR111); // INVALID RIGHT OPERAND OF QUALIFICATION, NOT IDENTIFIER
			else
				sub_4631();
		}
		if (matchTx2Item(L_LPAREN))	{
			if (testInfoFlag(F_ARRAY)) {
				resyncRPAREN();
				if (arg1b == I_LENGTH || arg1b == I_LAST) {
					wrTx2ExtError(ERR125);	// ILLEGAL ARGUMENT FOR BUILT-IN PROCEDURE
					exprPush2(I_NUMBER, 0);
				} else 
					exprPush2(arg1b, curInfo_p);
			} else {
				wrTx2ExtError(ERR127);	// INVALID SUBSCRIPT ON NON-ARRAY
				exprPush2(I_NUMBER, 0);
			}
		} else if (arg1b == I_LENGTH || arg1b == I_LAST) {
			if (testInfoFlag(F_ARRAY)) 
				exprPush2(arg1b, curInfo_p);
			else {
				wrTx2ExtError(ERR157);	// INVALID ARGUMENT, ARRAY REQUIRED FOR LENGTH OR LAST
				exprPush2(I_NUMBER, 0);
			}
		} else {
			exprPush2(arg1b, curInfo_p);
			exprPush2(I_LENGTH, curInfo_p);
			exprMakeNode(I_STAR, 2);
		}
	}
	expectRPAREN(ERR126);	// MISSING ') ' AFTER BUILT-IN PROCEDURE ARGUMENT LIST
}

void mkIndexNode() {
	word p, q, r;
	byte i;
	curInfo_p = ex3Stack[exSP-1];	// get var
	if (ex1Stack[exSP] == I_PLUSSIGN) {	// see if index is of form expr + ??
		if (st1Stack[p = ex3Stack[exSP] + 1] == I_NUMBER) {	// expr + number
			ex1Stack[exSP] = st1Stack[q = p - 1];	// pull up expression
			ex2Stack[exSP] = st2Stack[q];
			ex3Stack[exSP] = st3Stack[q];
			exprPush2(I_NUMBER, st3Stack[p]);		// and get the number as an offset
		} else 
			exprPush2(I_NUMBER, 0);		// no simple 0 offset
	} else
		exprPush2(I_NUMBER, 0);			// 0 offset

	if (getInfoType() == ADDRESS_T)		// simple word array
		i = I_WORDINDEX;
	else {
		i = I_BYTEINDEX;
		if (getInfoType() == STRUCT_T) {	// scale structure index
			r = exSP - 1;					// the index expr (ex offset)
			exprPush3(ex1Stack[r], ex2Stack[r], ex3Stack[r]);	// calc dimension
			exprPush2(I_SIZE, curInfo_p);
			exprMakeNode(I_STAR, 2);
			ex1Stack[r] = ex1Stack[exSP];	// replace index expr
			ex2Stack[r] = ex2Stack[exSP];
			ex3Stack[r] = ex3Stack[exSP];
			exprPop();						// waste intermediate
		}
	}
	exprMakeNode(i, 3);
}

void parsePortNum(byte arg1b) {
	word p = 0;
	if (matchTx2Item(L_LPAREN)) {
		if (matchTx2Item(L_NUMBER)) {
			if (tx1Item.data[0] <= 255)
				p = tx1Item.data[0];
			else
				wrTx2ExtError(ERR106);	// INVALID INPUT/OUTPUT PORT NUMBER
		} else
			wrTx2ExtError(ERR107);	// ILLEGAL INPUT/OUTPUT PORT NUMBER, NOT NUMERIC CONSTANT
		expectRPAREN(ERR108);	// MISSING ') ' AFTER INPUT/OUTPUT PORT NUMBER
	} else
		wrTx2ExtError(ERR109);	// MISSING INPUT/OUTPUT PORT NUMBER

	exprPush2(arg1b, p);
}

void sub_50D5() {
	byte i;
	word p = curInfo_p;
	if ((curInfo_p = curProcInfo_p) == 0)
		i = 0;
	else
		i = testInfoFlag(F_REENTRANT);
	curInfo_p = p;
	if (testInfoFlag(F_REENTRANT) && i)
		return;
	if (!testInfoFlag(F_DECLARED)) 
		wrTx2ExtError(ERR169);		// ILLEGAL FORWARD CALL
	else if (!testInfoFlag(F_DEFINED))
		wrTx2ExtError(ERR170);		// ILLEGAL RECURSIVE CALL
}

byte sub_512E(word arg1w) {
	byte c;
	if ((c = ex1Stack[arg1w]) == I_OUTPUT || c == I_STACKPTR || c == I_BASED)
		return 0;
	if (c == I_VARIABLE) 
		curInfo_p = ex3Stack[arg1w];
	else if (c == I_BYTEINDEX || c == I_WORDINDEX)
		curInfo_p = st3Stack[ex3Stack[arg1w]];
	else if (c == I_MEMBER) {
		if (st1Stack[ex3Stack[arg1w]] == I_VARIABLE)
			curInfo_p = st3Stack[ex3Stack[arg1w]];
		else
			curInfo_p = st3Stack[st3Stack[ex3Stack[arg1w]]];
	} else
		return 0xff;
	if (testInfoFlag(F_DATA))
		wrTx2ExtError(ERR173);	//  INVALID LEFT SIDE OF ASSIGNMENT: VARIABLE DECLARED WITH DATA ATTRIBUTE
	return 0;
}

void sub_521B() {
	word p = curSymbol_p;
	curSymbol_p = 0;
	createInfo(256, BYTE_T);
	setInfoFlag(F_DATA);
	curSymbol_p = p;
	exprPush2(I_VARIABLE, curInfo_p);
	setInfoFlag(F_ARRAY);
	setInfoFlag(F_STARDIM);
	setDimension(parseDataItems(curInfo_p));
}



void exprParse0() {
	pushOperator(0);
	if (matchTx2Item(L_VARIABLE)) {
		pushParseByte(1);
		sub_45E0();
		pushParseByte(11);
	} else {
		pushParseByte(3);
		pushParseByte(4);
	}
}



void exprParse1() {
	if (matchTx2Item(L_COLONEQUALS)) {
		if (sub_512E(exSP)) {
			wrTx2ExtError(ERR128);	// INVALID LEFT-HAND OPERAND OF ASSIGNMENT
			exprPop();
		} else
			pushParseByte(2);
		pushParseByte(3);
		pushParseByte(4);
	} else {
		if (ex1Stack[exSP] == I_OUTPUT) {
			wrTx2Error(ERR130);	//  ILLEGAL REFERENCE TO OUTPUT FUNCTION
			exprPop();
			exprPush2(I_NUMBER, 0);
		}
		pushParseByte(3);
	}
}


void exprParse2() {
	exprMakeNode(I_COLONEQUALS, 2);
}


void exprParse3() {
	if (matchTx2AuxFlag(64)) {
		while (byte_4172[tx1Aux1] <= byte_4172[operatorStack[operatorSP]]) {
			if (byte_4172[tx1Aux1] == 50 && byte_4172[operatorStack[operatorSP]] == 50)
				wrTx2ExtError(ERR218);	// ILLEGAL SUCCESSIVE USES OF RELATIONAL OPERATORS
			acceptOpAndArgs();
		}
		pushOperator(tx1Aux1);
		pushParseByte(3);
		pushParseByte(4);
	} else {
		while (operatorStack[operatorSP] != 0)
			acceptOpAndArgs();
		popOperatorStack();
	}
}


void exprParse4() {
	if (matchTx2Item(L_MINUSSIGN))
		pushOperator(I_UNARYMINUS);
	else if (matchTx2Item(L_NOT))
		pushOperator(I_NOT);
	pushParseByte(5);
}


void exprParse5() {
	word p; // *q;
	if (matchTx2Item(L_VARIABLE)) {
		sub_45E0();
		if (getInfoType() == BUILTIN_T && getBuiltinId() == 9) {	 // 9 -> OUTPUT
			wrTx2ExtError(ERR130);	// ILLEGAL REFERENCE TO OUTPUT FUNCTION
			if (matchTx2Item(L_LPAREN))
				resyncRPAREN();
			exprPush2(I_NUMBER, 0);
		} else 
			pushParseByte(11);
	} else if (matchTx2Item(L_NUMBER))
		exprPush2(I_NUMBER, tx1Item.data[0]);
	else if (matchTx2Item(L_STRING)) {
		if (tx1Item.data[0] == 1)
			p = tx1Item.data[1] & 0xff;
		else if (tx1Item.data[0] = 2) {
//			q = &p;
//			*(byte *)q = tx1Item.data[1] >> 8;
//			*(byte *)(q +1) = tx1Item.data[1] & 0xff;
            p = (tx1Item.data[1] >> 8) + (tx1Item.data[1] << 8);
		} else {
			wrTx2ExtError(ERR100);	// INVALID STRING CONSTANT IN EXPRESSION 
			p = 0;
		}
		exprPush2(I_NUMBER, p);
	} else if (matchTx2Item(L_LPAREN)) {
		pushParseByte(6);
		pushParseByte(0);
	} else if (matchTx2Item(L_PERIOD)) {
		if (matchTx2Item(L_VARIABLE)) {
			pushParseByte(7);
			pushParseByte(8);
		} else if (matchTx2Item(L_LPAREN)) {
			sub_521B();
			pushParseByte(7);
		} else {
			wrTx2ExtError(ERR101);	// INVALID ITEM FOLLOWS DOT OPERATOR
			pushParseByte(5);
		}
	} else {
		wrTx2ExtError(ERR102);	// MISSING PRIMARY OPERAND
		exprPush2(I_NUMBER, 0);
	}
}


void exprParse6() {
	expectRPAREN(ERR103);	// MISSING ') ' AT END OF SUBEXPRESSION
}

void exprParse7() {
	exprMakeNode(I_ADDRESSOF, 1);
}


void exprParse8() {
	sub_45E0();
	if (getInfoType() == BUILTIN_T) {
		popParseStack();
		wrTx2ExtError(ERR123);	// INVALID DOT OPERAND, BUILT-IN PROCEDURE ILLEGAL
		if (matchTx2Item(L_LPAREN))
			resyncRPAREN();
		exprPush2(I_NUMBER, 0);
	} else {
		exprPush2(I_VARIABLE, curInfo_p);
		if (getInfoType() == PROC_T) {
			if (matchTx2Item(L_LPAREN)) {
				wrTx2ExtError(ERR104);	// ILLEGAL PROCEDURE INVOCATION WITH DOT OPERATOR
				resyncRPAREN();
			}
		} else {
			if (getInfoType() == LABEL_T)
				wrTx2ExtError(ERR158);	// INVALID DOT OPERAND, LABEL ILLEGAL
			pushParseByte(9);
		}
	}
}


void exprParse9() {
	pushParseWord(curInfo_p);
	pushParseByte(0xa);
	if (matchTx2Item(L_LPAREN)) {
		if (!testInfoFlag(F_ARRAY))
			wrTx2ExtError(ERR127);	// INVALID SUBSCRIPT ON NON-ARRAY
		pushParseByte(0x13);
	}
}


void exprParse10() {
	word p = parseStack[parseSP];
	popParseStack();
	if (matchTx2Item(L_PERIOD)) {
		curInfo_p = p;
		if (getInfoType() != STRUCT_T)
			wrTx2ExtError(ERR110);	// INVALID LEFT OPERAND OF QUALIFICATION, NOT A STRUCTURE
		else if (notMatchTx2Item(L_VARIABLE))
			wrTx2ExtError(ERR111);	// INVALID RIGHT OPERAND OF QUALIFICATION, NOT IDENTIFIER
		else {
			sub_4631();
			exprPush2(I_VARIABLE, curInfo_p);
			pushParseWord(p);
			pushParseByte(14);
			if (matchTx2Item(L_LPAREN))
				pushParseByte(19);
		}
	} else 
		sub_4CFD(p);
}


void exprParse11() {
	word p;
	if (getInfoType() == BUILTIN_T) {
		p = builtinsMap[getBuiltinId()];
		if (p == I_INPUT || p == I_OUTPUT)
			parsePortNum((byte)p);
		else if (p == I_LENGTH || p == I_LAST || p == I_SIZE)
			sub_4DCF((byte)p);
		else {
			pushParseWord(p);
			pushParseWord(getParamCnt());
			pushOperator(0);
			pushParseByte(17);
			sub_4D38();
			if (matchTx2Item(L_LPAREN)) {
				pushParseByte(15);
				pushParseByte(0);
			}
		}
	} else if (getInfoType() == PROC_T) {
		sub_50D5();
		exprPush2(I_VARIABLE, curInfo_p);
		sub_4D38();
		pushParseWord(getParamCnt());
		pushOperator(0);
		pushParseByte(16);
		if (matchTx2Item(L_LPAREN)) {
			pushParseByte(15);
			pushParseByte(0);
		}
	} else
		pushParseByte(12);
}



void exprParse12() {
	exprPush2(I_VARIABLE, curInfo_p);
	pushParseWord(curInfo_p);
	if (getInfoType() == LABEL_T)
		wrTx2ExtError(ERR132);	// ILLEGAL USE OF LABEL
	pushParseByte(13);
	if (testInfoFlag(F_ARRAY)) {
		if (matchTx2Item(L_LPAREN))
			pushParseByte(19);
		else
			wrTx2ExtError(ERR133);	// ILLEGAL REFERENCE TO UNSUBSCRIPTED ARRAY
	} else if (matchTx2Item(L_LPAREN)) {
		wrTx2ExtError(ERR127);	 // INVALID SUBSCRIPT ON NON-ARRAY
		pushParseByte(19);
	}
}


void exprParse13() {
	word p;
	curInfo_p = p = parseStack[parseSP];
	popParseStack();
	if (matchTx2Item(L_PERIOD)) {
		if (getInfoType() != STRUCT_T)
			wrTx2ExtError(ERR110);	// INVALID LEFT OPERAND OF QUALIFICATION, NOT A STRUCTURE
		else if (notMatchTx2Item(L_VARIABLE))
			wrTx2ExtError(ERR111);	// INVALID RIGHT OPERAND OF QUALIFICATION, NOT IDENTIFIER
		else {
			sub_4631();
			exprPush2(I_VARIABLE, curInfo_p);
			pushParseWord(p);
			pushParseByte(14);
			if (testInfoFlag(F_ARRAY)) {
				if (matchTx2Item(L_LPAREN))
					pushParseByte(19);
				else
					wrTx2ExtError(ERR134);	// ILLEGAL REFERENCE TO UNSUBSCRIPTED MEMBER ARRAY
			} else if (matchTx2Item(L_LPAREN))
				wrTx2ExtError(ERR127);	// INVALID SUBSCRIPT ON NON-ARRAY
		}
	} else {
		if (getInfoType() == STRUCT_T)
			wrTx2ExtError(ERR135);	// ILLEGAL REFERENCE TO AN UNQUALIFIED STRUCTURE
		sub_4CFD(p);
	}
}


void exprParse14() {
	word p = parseStack[parseSP];
	popParseStack();
	exprMakeNode(I_MEMBER, 2);
	sub_4CFD(p);
}


void exprParse15() {
	operatorStack[operatorSP]++;
	if (matchTx2Item(L_COMMA)) {
		pushParseByte(0xf);
		pushParseByte(0);
	} else
		expectRPAREN(ERR113);	// MISSING ') ' AT END OF ARGUMENT LIST
}


void exprParse16() {
	exprMakeNode(I_CALL, getCallArgCnt() + 1);
}


void exprParse17() {
	byte i, j;
	j = getCallArgCnt();
	i = (byte)parseStack[parseSP];
	popParseStack();
	exprMakeNode(i, j);
}


void exprParse18() {
	exprMakeNode(I_CALLVAR, operatorStack[operatorSP]+1);
	popOperatorStack();
}


void exprParse19() {
	pushParseByte(20);
	pushParseByte(0);
}

void exprParse20() {
	if (matchTx2Item(L_COMMA)) {
		wrTx2ExtError(ERR114);	// INVALID SUBSCRIPT, MULTIPLE SUBSCRIPTS ILLEGAL
		sub_4599();
	}
	expectRPAREN(ERR115);	// MISSING ') ' AT END OF SUBSCRIPT
	mkIndexNode();
}


void exprParse() {
	while (parseSP != 0) {
#ifdef WATCH
        printf("exprParse - %d\n", parseStack[parseSP]);
#endif
		switch(parseStack[parseSP--]) {
		case 0: exprParse0(); break;
		case 1: exprParse1(); break;
		case 2: exprParse2(); break;
		case 3: exprParse3(); break;
		case 4: exprParse4(); break;
		case 5: exprParse5(); break;
		case 6: exprParse6(); break;
		case 7: exprParse7(); break;
		case 8: exprParse8(); break;
		case 9: exprParse9(); break;
		case 10: exprParse10(); break;
		case 11: exprParse11(); break;
		case 12: exprParse12(); break;
		case 13: exprParse13(); break;
		case 14: exprParse14(); break;
		case 15: exprParse15(); break;
		case 16: exprParse16(); break;
		case 17: exprParse17(); break;
		case 18: exprParse18(); break;
		case 19: exprParse19(); break;
		case 20: exprParse20(); break;
		}
        dumpStks();
    }
}

byte sub_5945() {
	resetStacks();
	byte i = 0;
	do {
		if (notMatchTx2Item(L_VARIABLE)) {
			wrTx2ExtError(ERR128);	// INVALID LEFT-HAND OPERAND OF ASSIGNMENT
			return 0;
		}
		sub_45E0();
		pushParseByte(11);
		exprParse();
		if (sub_512E(exSP)) {
			wrTx2ExtError(ERR128);	// INVALID LEFT-HAND OPERAND OF ASSIGNMENT
			exprPop();
		} else
			i++;
	} while (!notMatchTx2Item(L_COMMA));
	
	if (matchTx2Item(L_EQ)) {
		pushParseByte(0);
		exprParse();
        if (i != 0) {
			exprMakeNode(I_COLONEQUALS, i + 1);
			moveExpr2Stmt();
			markedStSP = stSP;
			return 255;
		}
		return 0;
	} else {
		wrTx2ExtError(ERR116);	// MISSING '=' IN ASSIGNMENT STATEMENT
		return 0;
	}
}

byte sub_59D4() {
	byte i;
	resetStacks();
	if (notMatchTx2Item(L_VARIABLE)) {
		wrTx2ExtError(ERR117);	// MISSING PROCEDURE NAME IN CALL STATEMENT
		return 0;
	}
	sub_467D();
	i = getInfoType();
	if (i == PROC_T) {
		sub_50D5();
		if (getDataType() != 0)
			wrTx2ExtError(ERR129);	// ILLEGAL 'CALL' WITH TYPED PROCEDURE
		exprPush2(I_VARIABLE, curInfo_p);
		pushParseWord(getParamCnt());
		pushOperator(0);
		pushParseByte(16);
		if (matchTx2Item(L_LPAREN)) {
			pushParseByte(15);
			pushParseByte(0);
		}
	} else if (i == BUILTIN_T) {
		if (getDataType() != 0) {
			wrTx2ExtError(ERR129);	// ILLEGAL 'CALL' WITH TYPED PROCEDURE
			return 0;
		}
		pushParseWord(builtinsMap[getBuiltinId()]);
		pushParseWord(getParamCnt());
		pushOperator(0);
		pushParseByte(17);
		if (matchTx2Item(L_LPAREN)) {
			pushParseByte(15);
			pushParseByte(0);
		}
	} else if (i != ADDRESS_T || testInfoFlag(F_ARRAY)) {
		wrTx2ExtError(ERR118);	// INVALID INDIRECT CALL, IDENTIFIER NOT AN ADDRESS SCALAR
		return 0;
	} else {
		sub_4D2C();
		pushOperator(0);
		pushParseByte(18);
		if (matchTx2Item(L_LPAREN)) {
			pushParseByte(15);
			pushParseByte(0);
		}
	}
	exprParse();
	moveExpr2Stmt();
	markedStSP = stSP;
	return 255;
}


void sub_5AD8() {
	resetStacks();
	pushParseByte(0);
	exprParse();
	moveExpr2Stmt();
	markedStSP = stSP;
}


word stmtParse(word arg1w) {
	word p = curInfo_p;
	parseSP = 0;
	pushParseWord(arg1w);
	pushParseByte(0);
	while (parseSP != 0) {
#ifdef WATCH
        printf("stmtParse - %d\n", parseStack[parseSP]);
        dumpStks();
#endif
		switch(parseStack[parseSP--]) {
		case 0: stmtParse0(); break;
		case 1: stmtParse1(); break;
		case 2: stmtParse2(); break;
		case 3: stmtParse3(); break;
		case 4: stmtParse4(); break;
		case 5: stmtParse5(); break;
		case 6: stmtParse6(); break;
		case 7: stmtParse7(); break;
		case 8: stmtParse8(); break;
		case 9: stmtParse9(); break;
		case 10: stmtParse10(); break;
		case 11: stmtParse11(); break;
		case 12: stmtParse12(); break;
		case 13: stmtParse13(); break;
		case 14: stmtParse14(); break;
		case 15: stmtParse15(); break;
		}
        
	}
    dumpStks();
	curInfo_p = p;
	return st3Stack[arg1w];
}

void flgVisited(word arg1w, word arg2w) {
	st1Stack[arg1w] = 0xff;
	st3Stack[arg1w] = arg2w;
}

void sub_5BF5(word arg1w) {
    byte i, j;
    word p = st3Stack[arg1w];
    if ((i = st1Stack[arg1w]) == I_VARIABLE) 
        word_99C1 = wrTx2Item1Arg(T2_VARIABLE, info2Off(p));
    else if (i == I_NUMBER) {
        j = ((p >> 8) == 0) ? T2_NUMBER : T2_BIGNUMBER;
        word_99C1 = wrTx2Item1Arg(j, p);
    } else if (i == I_INPUT)
        word_99C1 = wrTx2Item1Arg(T2_INPUT, p);
    else if (i == I_LENGTH || i == I_LAST || i == I_SIZE)
        word_99C1 = wrTx2Item1Arg(byte_413F[i], info2Off(p));
    else
        word_99C1 = wrTx2Item(byte_413F[i]);
    flgVisited(arg1w, word_99C1);
}

void stmtParse0() {
	byte i;
	word_99BF = parseStack[parseSP];
	popParseStack();
	if ((i = st1Stack[word_99BF]) == I_OUTPUT)
		return;
	if (st2Stack[word_99BF] == 0)
		sub_5BF5(word_99BF);
	else {
		pushParseWord(word_99BF);
		if (i == I_CALL)
			pushParseByte(3);
		else if (i == I_CALLVAR) {
			pushParseByte(6);
			pushParseWord(st3Stack[word_99BF]);
			pushParseByte(0);
		} else if (i == I_COLONEQUALS)
			pushParseByte(9);
		else if (i == I_MOVE) {
			pushParseByte(14);
			pushParseWord(st3Stack[word_99BF]);
			pushParseByte(0);
		} else if (i == I_BYTEINDEX || i == I_WORDINDEX) {
			pushParseByte(8);
			pushParseWord(2);		// serialise 2 leaves
			pushParseWord(st3Stack[word_99BF]);
			pushParseByte(1);
		} else {
			pushParseByte(13);					// post serialisation
			pushParseWord(st2Stack[word_99BF]);	// num args
			pushParseWord(st3Stack[word_99BF]);	// loc of args
			pushParseByte(1);
		}
	}
}


void stmtParse1() {	// serialise 1 leaf then check
	word_99BF = parseStack[parseSP];
	pushParseByte(2);	// flag to check for more leaves
	pushParseWord(word_99BF);	// serialise this leaf
	pushParseByte(0);
}

void stmtParse2() {	// check for any more leaves
	word p;
	parseStack[p = parseSP - 1]--;
	if (parseStack[p] == 0) {
		popParseStack();
		popParseStack();
	} else {
		parseStack[parseSP]++;
		pushParseByte(1);
	}
}

void stmtParse3() {	// parse call args
	word_99BF = parseStack[parseSP];
	pushParseByte(5);	// final call wrap up
	if (st2Stack[word_99BF] > 1) {	// any args
		pushParseWord(st2Stack[word_99BF] - 1);	// num args 
		pushParseWord(st3Stack[word_99BF] + 1);	// index of arg info
		curInfo_p = st3Stack[st3Stack[word_99BF]];	// info of procedure
		advNextInfo();							// adv to arginfo
		pushParseWord(curInfo_p);
		pushParseByte(4);
		pushParseWord(st3Stack[word_99BF] + 1);	// index of arg info
		pushParseByte(0);						// serialise the arg
	}
	word_99C1 = wrTx2Item(T2_BEGCALL);
}

void stmtParse4() {
	word p, q;
	byte i, j;
	q = (p = parseSP - 1) - 1;
	curInfo_p = parseStack[parseSP];
	word_99BF = parseStack[p];
    if ((i = (byte)parseStack[q]) > 2) {	// all bar first 2 args to stack
		if (curInfo_p == 0) 
			word_99C1 = wrTx2Item1Arg(T2_STKARG, sub_42EF(st3Stack[word_99BF]));
		else {
			j = getInfoType() == BYTE_T ? T2_STKBARG : T2_STKWARG;
			word_99C1 = wrTx2Item1Arg(j, sub_42EF(st3Stack[word_99BF]));
			advNextInfo();
			parseStack[parseSP] = curInfo_p;
		}
		flgVisited(word_99BF, word_99C1);
	}
	if (--i == 0) {			// all done, clear working data
		popParseStack();
		popParseStack();
		popParseStack();
	} else {
		parseStack[q] = i;
		parseStack[p] = ++word_99BF;
		pushParseByte(4);
		pushParseWord(word_99BF);	// serialise the arg
		pushParseByte(0);
	}
}


void stmtParse5() {
	word p, q, r;
	byte i;
	word_99BF = parseStack[parseSP];
	popParseStack();
	r = st3Stack[word_99BF];
	i = st2Stack[word_99BF] - 1;
	q = p = 0;
	if (i > 1) {
		p = sub_42EF(st3Stack[r + i - 1]);
		q = sub_42EF(st3Stack[r + i]);
	} else if (i > 0)
		p = sub_42EF(st3Stack[r + i]);
	word_99C1 = wrTx2Item3Arg(T2_CALL, p, q, info2Off(st3Stack[r]));
	flgVisited(word_99BF, word_99C1);
}


void stmtParse6() {
	byte i;
	word p;
	word_99C1 = wrTx2Item(T2_BEGCALL);
	word_99BF = parseStack[parseSP];
	pushParseByte(7);
	if ((i = st2Stack[word_99BF] - 1) != 0) {
		pushParseWord(i);
		p = st3Stack[word_99BF] + 1;
		pushParseWord(p);
		pushParseWord(0);	// no arg info
		pushParseByte(4);
		pushParseWord(p);
		pushParseByte(0);
	}
}


void stmtParse7() {
	byte i;
	word p, q, r;
	word_99BF = parseStack[parseSP];
	popParseStack();
	i = st2Stack[word_99BF] - 1;
	p = st3Stack[word_99BF];
	r = q = 0;
	if (i > 1) {
		q = sub_42EF(st3Stack[p + i - 1]);
		r = sub_42EF(st3Stack[p + i]);
	} else if (i > 0)
		q = sub_42EF(st3Stack[p + i]);
	word_99C1 = wrTx2Item3Arg(T2_CALLVAR, q, r, sub_42EF(st3Stack[p]));
	flgVisited(word_99BF, word_99C1);
}


void stmtParse8() {
	byte i;
	word p;
	word_99BF = parseStack[parseSP];
	popParseStack();
	i = st1Stack[word_99BF];
	p = st3Stack[word_99BF];
	word_99C1 = wrTx2Item3Arg(byte_413F[i], sub_42EF(st3Stack[p]),
				sub_42EF(st3Stack[p+1]), st3Stack[p+2]);
	flgVisited(word_99BF, word_99C1);
}



void stmtParse9() {
	word_99BF = parseStack[parseSP];
	pushParseByte(10);	// post serialise LHS
	pushParseWord(st3Stack[word_99BF] + st2Stack[word_99BF] - 1);
	pushParseByte(0);
}


void stmtParse10() {
	byte i;
	word p;
	word_99BF = parseStack[parseSP];
	pushParseByte(12);		// mark LHS as used at end
	i = st2Stack[word_99BF] - 1;	// num RHS
	p = st3Stack[word_99BF];		// base RHS
	pushParseWord(i);
	pushParseWord(p);
	pushParseWord(p + i);			// LHS
	pushParseByte(11);				// after serialised leaf
	pushParseWord(p);				// do the leaf
	pushParseByte(0);
}


void stmtParse11() {		// do one RHS assignment
	byte i;
	word p, q, r, s;
	i = (byte)parseStack[q = (p = parseSP - 1) - 1];
	s = parseStack[parseSP];
	if (st1Stack[r = parseStack[p]] == I_OUTPUT) {
		word_99C1 = wrTx2Item1Arg(T2_NUMBER, st3Stack[r]);
		word_99C1 = wrTx2Item2Arg(T2_OUTPUT, sub_42EF(word_99C1), sub_42EF(st3Stack[s]));
	} else 
		word_99C1 = wrTx2Item2Arg(T2_STORE, sub_42EF(st3Stack[r]), sub_42EF(st3Stack[s]));
	if (--i == 0) {		// all done
		popParseStack();
		popParseStack();
		popParseStack();
	} else {			// no so do another
		parseStack[q] = i;
		parseStack[p] = ++r;
		pushParseByte(11);	// state 11 after serialise
		pushParseWord(r);	// serialise leaf
		pushParseByte(0);
	}
}

void stmtParse12() {		// mark LHS as used
	word_99BF = parseStack[parseSP];
	popParseStack();
	word_99C1 = st3Stack[st3Stack[word_99BF] + st2Stack[word_99BF] - 1];
	flgVisited(word_99BF, word_99C1);
}


void stmtParse13() {	// binary or unary op
	word p;
	byte i;
	word_99BF = parseStack[parseSP];
	popParseStack();
	p = st3Stack[word_99BF];
	i = byte_413F[st1Stack[word_99BF]];
	if (st2Stack[word_99BF] == 1) 
		word_99C1 = wrTx2Item1Arg(i, sub_42EF(st3Stack[p]));
	else 
		word_99C1 = wrTx2Item2Arg(i, sub_42EF(st3Stack[p]), sub_42EF(st3Stack[p+1]));
	flgVisited(word_99BF, word_99C1);
}


void stmtParse14() {
	word p;
	p = st3Stack[word_99BF = parseStack[parseSP]];
	// emit the count leaf
	word_99C1 = wrTx2Item1Arg(T2_BEGMOVE, sub_42EF(st3Stack[p]));
	flgVisited(p, word_99C1);
	pushParseByte(15);		// MOVE post serialise
	pushParseWord(2);		// serialise the address leaves
	pushParseWord(p + 1);
	pushParseByte(1);
}

void stmtParse15() {		// rest of MOVE
	word p;
	word_99BF = parseStack[parseSP];
	popParseStack();
	p = st3Stack[word_99BF];
	word_99C1 = wrTx2Item3Arg(T2_MOVE, sub_42EF(st3Stack[p+1]),
			sub_42EF(st3Stack[p+2]), sub_42EF(st3Stack[p]));
	flgVisited(word_99BF, word_99C1);
}


byte byte_40B7[] = {
	// L_LINEINFO, L_SYNTAXERROR, L_TOKENERROR, L_LIST, L_NOLIST, L_CODE, L_NOCODE, L_EJECT,
	0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
	// L_INCLUDE, L_STMTCNT, L_LABELDEF, L_LOCALLABEL, L_JMP, L_JMPFALSE, L_PROCEDURE, L_SCOPE, 
	0x12, 0, 0x13, 0x13, 0x13, 0x13, 7, 1,
	// L_END, L_DO, L_DOLOOP, L_WHILE, L_CASE, L_CASELABEL, L_IF, L_STATEMENT, 
	2, 6, 4, 5, 8, 0x13, 3, 9,
	// L_CALL, L_RETURN, L_GO, L_GOTO, L_SEMICOLON, L_ENABLE, L_DISABLE, L_HALT, 
	0x0A, 0x0B, 0x0D, 0x0C, 0x0E, 0x0F, 0x0F, 0x0F,
	// L_EOF, L_AT, L_INITIAL, L_DATA, L_VARIABLE, L_NUMBER, L_STRING, L_PLUSSIGN, 
	0x12, 0x10, 0x11, 0x11, 0x12, 0x12, 0x12, 0x12,
	// L_MINUSSIGN, L_PLUS, L_MINUS, L_STAR, L_SLASH, L_MOD, L_COLONEQUALS, L_AND, 
	0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
	// L_OR, L_XOR, L_NOT, L_LT, L_LE, L_EQ, L_NE, L_GE, 
	0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
	// L_GT, L_COMMA, L_LPAREN, L_RPAREN, L_PERIOD, L_TO, L_BY, L_INVALID, 
	0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
	// L_MODULE, L_XREFINFO, L_XREF, L_EXTERNAL
	0x12, 0x12, 0x12, 0x14};

void sub_6523() {
	controlSP = 0;
	getTx1Item();
	while (tx1Item.type != L_EOF) {
        curInfo_p = 0;
		switch(byte_40B7[tx1Item.type]) {
		case 0: parseSTMTCNT(); break;	// L_STMTCNT
		case 1: pushScope(tx1Item.data[0]); break; // L_SCOPE
		case 2: parseEND(); break;	// L_END
		case 3: parseIF(); break;	// L_IF
		case 4: parseDOLOOP();		// L_DOLOOP
			chkEndOfStmt();
			break;
		case 5: parseWHILE(); break;	// L_WHILE
		case 6: pushControl(1); break;	// L_DO
		case 7: parsePROCEDURE(); break;	// L_PROCEDURE
		case 8: parseCASE(); break;	// L_CASE
		case 9: parseSTATEMENT(); break;	// L_STATEMENT
		case 10: parseCALL(); break;	// L_CALL
		case 11: parseRETURN(); break;	// L_RETURN
		case 12: parseGOTO(); break;	// L_GOTO
		case 13: parseGO(); break;	// L_GO
		case 14: break;			// L_SEMICOLON
		case 15: parseEIDIHLT(); break;	// L_ENABLE, L_DISABLE, L_HALT
		case 16: parseAT(); break;	// L_AT
		case 17: parseDATA_INITIAL(); break;	// L_DATA, L_INITIAL
		case 18: break;			// expression items
		case 19: mapLtoT2(); break;	// L_LABELDEF, L_LOCALLABEL, L_JMP, L_JMPFALSE, L_CASELABEL
		case 20: curInfo_p = tx1Item.data[0];	// L_EXTERNAL
			setInfoFlag(F_DECLARED);
			setInfoFlag(F_DEFINED);
			break;
		}
		getTx1Item();
	}
}



void pushScope(word arg1w) {
	if (blockDepth == 0x22)
		fatalError(ERR164);	// COMPILER ERROR: SCOPE STACK OVERFLOW
	else 
		procChains[++blockDepth] = arg1w;
}

void popScope() {
	if (blockDepth == 0)
		fatalError(ERR165);	// COMPILER ERROR: SCOPE STACK UNDERFLOW
	blockDepth--;
}

void pushControl(byte arg1b) {
	if (controlSP == 0x13)
		fatalError(ERR84);	//  LIMIT EXCEEDED: BLOCK NESTING
	else {
		byte_99FF[++controlSP] = arg1b;
		byte_9A13[controlSP] = 0;
		procInfoStack[controlSP] = 0;
		hNodes[controlSP] = 0;
		eNodes[controlSP] = 0;
		word_9A9F[controlSP] = 0;
		word_9A9F[controlSP] = 0;
	}
}

void popControl() {
	if (controlSP == 0)
		fatalError(ERR167);	// COMPILER ERROR: CONTROL STACK UNDERFLOW
	else {
		if (byte_99FF[controlSP] != 0) 
			byte_9A13[controlSP-1] |= byte_9A13[controlSP];
		controlSP--;
	}
}


static word genLocalLabel() {
	alloc(3, 3);
	return ++localLabelCnt;
}

word sub_671D(word arg1w) {
	word p, q;
	curInfo_p = arg1w;
	q = wrTx2Item1Arg(T2_VARIABLE, info2Off(arg1w));
	if (testInfoFlag(F_MEMBER)) {
		curInfo_p = getOwningStructure();
		p = wrTx2Item1Arg(T2_VARIABLE, info2Off(curInfo_p));
		q = wrTx2Item2Arg(T2_MEMBER, sub_42EF(p), sub_42EF(q));
	}
	return q;
}

word sub_677C(word arg1w) {
	word p, q;
	q = sub_671D(arg1w);
	if (testInfoFlag(F_BASED)) {
			p = sub_671D(getBase());
			q = wrTx2Item2Arg(T2_BASED, sub_42EF(p), sub_42EF(q));
	}
	return q;
}

void chkEndOfStmt() {
	if (matchTx2AuxFlag(128)) {
		wrTx2ExtError(ERR32);	// INVALID SYNTAX, TEXT IGNORED UNTIL ';'
		while (tx1Aux2 & 0x80)
			getTx1Item();
		setRegetTx1Item();
	}
}


void sub_67E3() {
	word p, q, r, s, t, u ,v, w;
	p = procInfoStack[controlSP];
	u = hNodes[controlSP];
	v = eNodes[controlSP];
	if((w = word_9A9F[controlSP]) == 0) {
		curInfo_p = p;
		if (getInfoType() == BYTE_T)
			t = wrTx2Item1Arg(T2_LOCALLABEL, genLocalLabel());
		q = sub_677C(p);
		r = sub_677C(p);
		s = wrTx2Item1Arg(T2_NUMBER, 1);
		if (getInfoType() == BYTE_T) {
			r = wrTx2Item2Arg(T2_ADDB, sub_42EF(r), sub_42EF(s));
			q = wrTx2Item2Arg(T2_STORE, sub_42EF(q), sub_42EF(r));
			q = wrTx2Item2Arg(T2_JNZ, u, 3);
		} else {
			r = wrTx2Item2Arg(T2_ADDW, sub_42EF(r), sub_42EF(s));
			q = wrTx2Item2Arg(T2_STORE, sub_42EF(q), sub_42EF(r));
			q = wrTx2Item1Arg(T2_JNC, u);
		}
	} else 
		q = wrTx2Item1Arg(T2_JMP, w);
	q = wrTx2Item1Arg(T2_LOCALLABEL, v);
}



word sub_6917() {
	sub_5AD8();
	return stmtParse(markedStSP);
}



void sub_6923() {
	word p = sub_6917();
	chkEndOfStmt();
	if (matchTx2Item(L_JMPFALSE))
		p = wrTx2Item2Arg(T2_JMPFALSE, tx1Item.data[0], sub_42EF(p));
	else
		fatalError(ERR168);  // COMPILER ERROR: BRANCH MISSING IN 'IF' STATEMENT
}

void parseSTMTCNT() {
	T2CntForStmt = 0;
	mapLtoT2();
	curStmtNum = tx1Item.data[0];
}


void parseIF() {
	sub_6923();
}


void parsePROCEDURE() {
	tx1Item.data[0] = info2Off(tx1Item.data[0]);
	mapLtoT2();
	pushControl(0);
	procInfoStack[controlSP] = curProcInfo_p;
	curInfo_p = curProcInfo_p = off2Info(tx1Item.data[0]);
	setInfoFlag(F_DECLARED);
}

void parseWHILE() {
	word p, q, r;
	pushControl(2);
	p = genLocalLabel();
	q = genLocalLabel();
	hNodes[controlSP] = p;
	eNodes[controlSP] = q;
	r = wrTx2Item1Arg(T2_LOCALLABEL, p);
	r = sub_6917();
	r = wrTx2Item2Arg(T2_JMPFALSE, q, sub_42EF(r));
	chkEndOfStmt();
}


void parseCASE() {
	word p, q;
	pushControl(3);
	mapLtoT2();
	q = sub_6917();
	p = genLocalLabel();
	q = wrTx2Item2Arg(T2_63, p, sub_42EF(q));
	hNodes[controlSP] = p;
	chkEndOfStmt();
}

void parseDOLOOP() {
	word p, q, r, s, t, u, v, w;
	pushControl(1);
	if (notMatchTx2Item(L_VARIABLE)) {
		wrTx2Error(ERR138);	// MISSING INDEX VARIABLE
		return;
	}
	sub_467D();
	w = curInfo_p;
	procInfoStack[controlSP] = w;
	if ((getInfoType() != BYTE_T && getInfoType() != ADDRESS_T) || testInfoFlag(F_ARRAY)) {
		wrTx2ExtError(ERR139);	// INVALID INDEX VARIABLE TYPE, NOT BYTE OR ADDRESS
		return;
	}
	if (notMatchTx2Item(L_EQ)) {
		wrTx2ExtError(ERR140);	// MISSING '=' FOLLOWING INDEX VARIABLE
		return;
	}
	byte_99FF[controlSP] = 4;
	p = sub_677C(w);
	q = sub_6917();
	p = wrTx2Item2Arg(T2_STORE, sub_42EF(p), sub_42EF(q));
	s = genLocalLabel();
	hNodes[controlSP] = s;
	p = wrTx2Item1Arg(T2_LOCALLABEL, s);
	p = sub_677C(w);

	if (matchTx2Item(L_TO))
		q = sub_6917();
	else {
		wrTx2ExtError(ERR141);	// MISSING 'TO' CLAUSE
		q = wrTx2Item1Arg(T2_NUMBER, 0);
	}

	p = wrTx2Item2Arg(T2_LE, sub_42EF(p), sub_42EF(q));
	t = genLocalLabel();
	eNodes[controlSP] = t;
	p = wrTx2Item2Arg(T2_JMPFALSE, t, sub_42EF(p));

	if (notMatchTx2Item(L_BY))
		return;
	v = genLocalLabel();
	word_9AC7[controlSP] = v;
	p = wrTx2Item1Arg(T2_JMP, v);
	u = genLocalLabel();
	word_9A9F[controlSP] = u;
	p = wrTx2Item1Arg(T2_LOCALLABEL, u);
	p = sub_677C(w);
	q = sub_677C(w);
	r = sub_6917();
	q = wrTx2Item2Arg(T2_ADDW, sub_42EF(q), sub_42EF(r));
	p = wrTx2Item2Arg(T2_STORE, sub_42EF(p), sub_42EF(q));
	p = wrTx2Item1Arg(T2_JNC, s);
	p = wrTx2Item1Arg(T2_JMP, t);
	p = wrTx2Item1Arg(T2_LOCALLABEL, v);
}



void parseEND() {
	word p;
	switch(byte_99FF[controlSP]) {
	case 0: curInfo_p = curProcInfo_p;
		setInfoFlag(F_DEFINED);
		if (getDataType() != 0 && ! byte_9A13[controlSP])
			wrTx2Error(ERR156);	// MISSING RETURN STATEMENT IN TYPED PROCEDURE
		p = wrTx2Item(T2_ENDPROC);
		curProcInfo_p = procInfoStack[controlSP];
		break;
	case 1: break;
	case 2: p = wrTx2Item1Arg(T2_JMP, hNodes[controlSP]);
		p = wrTx2Item1Arg(T2_LOCALLABEL, eNodes[controlSP]);
		break;
	case 3: p = wrTx2Item1Arg(T2_LOCALLABEL, hNodes[controlSP]);
		p = wrTx2Item(T2_ENDCASE);
		break;
	case 4: sub_67E3(); break;
	}
	popControl();
	popScope();
}

void parseSTATEMENT() {
	word p;
	if (sub_5945()) 
		p = stmtParse(markedStSP);
	chkEndOfStmt();
}


void parseCALL() {
	word p;
	if (sub_59D4())
		p = stmtParse(markedStSP);
	chkEndOfStmt();
}


void parseRETURN() {
	word p;
	byte i;
	if (curProcInfo_p == 0) {
		wrTx2Error(ERR155);	// INVALID RETURN IN MAIN PROGRAM
		return;
	}
	curInfo_p = curProcInfo_p;
	i = getDataType();
	if(matchTx2AuxFlag(128)) {	// there is an expression
		setRegetTx1Item();
		if (i == 0)
			wrTx2Error(ERR136);	// INVALID RETURN FOR UNTYPED PROCEDURE, VALUE ILLEGAL
		p = sub_6917();
		if (i == 2) 
			p = wrTx2Item1Arg(T2_RETURNBYTE, sub_42EF(p));
		else
			p = wrTx2Item1Arg(T2_RETURNWORD, sub_42EF(p));
	} else {
		if (i != 0)
			wrTx2Error(ERR137);	// MISSING VALUE IN RETURN FOR TYPED PROCEDURE
		p = wrTx2Item(T2_RETURN);
	}
	byte_9A13[controlSP] = 0xff;
	chkEndOfStmt();
}

void parseEIDIHLT() {
	mapLtoT2();
	chkEndOfStmt();
}

void parseGOTO() {
	word p;
	if (notMatchTx2Item(L_VARIABLE)) 
		wrTx2Error(ERR142);	// MISSING IDENTIFIER FOLLOWING GOTO
	else {
		sub_45E0();
		if (getInfoType() != LABEL_T) 
			wrTx2ExtError(ERR143);	// INVALID REFERENCE FOLLOWING GOTO, NOT A LABEL
		else {
			if ((getInfoScope() >> 8) == 1 && (procChains[blockDepth] >> 8) != 1)
				setInfoFlag(F_MODGOTO);
			if ((getInfoScope() >> 8) == 1 || (getInfoScope() >> 8) == (procChains[blockDepth] >> 8)) {
				p = wrTx2Item1Arg(T2_GO_TO, info2Off(curInfo_p));
				chkEndOfStmt();
			} else 
				wrTx2ExtError(ERR144); // INVALID GOTO LABEL, NOT AT LOCAL OR MODULE LEVEL
		}
	}
}



void parseGO() {
	if (notMatchTx2Item(L_TO))
		wrTx2Error(ERR145); 	// MISSING 'TO' FOLLOWING 'GO'
	parseGOTO();
}


void parseAT() {
	word p = tx1Item.data[0];
	getTx1Item();
	getRestrictedExpr();
	wrAtFileByte(AT_AHDR);
	wrAtFileWord(info2Off(p));
	wrAtFileWord(curStmtNum);
	wrAtFileWord(atData.varInfoOffset);
	wrAtFileWord(atData.varArrayIndex);
	wrAtFileWord(atData.varNestedArrayIndex);
	wrAtFileWord(atData.val);
	expectRPAREN(ERR146);	// MISSING ') ' AFTER 'AT' RESTRICTED EXPRESSION
}


void parseDATA_INITIAL() {
	word p = tx1Item.data[0];
	getTx1Item();
	word q = parseDataItems(p);
	if((curInfo_p = p) != off2Info(0) && testInfoFlag(F_STARDIM))
		setDimension(q);
}

void sub_6EE0() {
	sub_7695();
	sub_7049();
	sub_711F();
	sub_719D();
	procAtFile();
	sub_75F7();
	sub_76D9();
}

void sub_6EF6(word arg1w) {
	static byte i = T2_ERROR;

	hasErrors = 0xff;
	word p = info2Off(curInfo_p);
	ifwrite(&tx2File, &i, 1);
	ifwrite(&tx2File, &arg1w, 2);
	ifwrite(&tx2File, &p, 2);
	ifwrite(&tx2File, &atStmtNum, 2);
}


word getElementSize() {
	switch (getInfoType()) {
	case BYTE_T: return 1;
	case ADDRESS_T: return 2;
	case STRUCT_T: return getOwningStructure();
	}
    return 0;
}


word getVarSize() {
	return testInfoFlag(F_ARRAY) ? getDimension() * getElementSize() : getElementSize();
}


void sub_6F94(address arg1w, address arg2w, word arg3w) {
	arg1w += arg3w - 2;
	arg2w += arg3w - 2;
	while (arg3w > 1) {
		*(word *)arg2w = *(word *)arg1w;
		arg3w -= 2;
		arg2w -= 2;
		arg1w -= 2;
	}
	if (arg3w == 1) 
		*(word *)(arg2w) = (*(word *)arg1w & 0xff00) | (*(word *)arg2w & 0xff);
}


void sub_701C() {
	do {
		advNextInfo();
		if (curInfo_p == 0)
			return;
	} while (!(getInfoType() >= BYTE_T && getInfoType() <= STRUCT_T));
}

void sub_7049() {
	static byte j = 0;
	word p;
	byte i;
	if (!byte_9B40 && !IXREF)
		return;
	/*
		dump all of the symbol table in pascal string format, terminate with 0 length string
		In doing so adjust all the offsets stored in the info table so that when the strings
		are reloaded the offsets are correct because chain and links from the symbol table to
		the information data will have been removed
	*/
	p = topSymbol - 1;
	for (i = 0; i <= 63; i++) {
		for (curSymbol_p = hashChains_p[i]; curSymbol_p != 0; curSymbol_p = curSymbol_p->link) {
			if ((curInfo_p = curSymbol_p->infoChain) != 0 && (curInfo_p >> 8) != 255) {
				p -= curSymbol_p->name[0] + 1;
				while (curInfo_p != 0) {
					setInfoSymbol(p);
					curInfo_p = getNextInfo();
				}
				ifwrite(&nmsFile, curSymbol_p->name, curSymbol_p->name[0] + 1);
			}
		}
	}
	ifwrite(&nmsFile, &j, 1);
}


void sub_711F() {
	word p, q, r, s;
	curInfo_p = off2Info(2);
	sub_701C();
	while (curInfo_p != 0) {
		if (getInfoType() == STRUCT_T)
			setOwningStructure(0);
		else if (testInfoFlag(F_MEMBER)) {
			r = getVarSize();
			p = curInfo_p;
			curInfo_p = getOwningStructure();
			q = getOwningStructure();
			if ((s = r + q) < r)
				sub_6EF6(ERR208);	// LIMIT EXCEEDED: STRUCTURE SIZE
			setOwningStructure(s);
			curInfo_p = p;
			setAddr(q);
		}
		sub_701C();
	}
}

void sub_719D() {
    byte i;
    word p;
    static byte externalsCnt;
    curInfo_p = off2Info(2);
    byte_3C3B = haveModule;
    
    while (curInfo_p != 0) {
        if ((getInfoType() >= BYTE_T && getInfoType() <= STRUCT_T)
            || getInfoType() == PROC_T || getInfoType() == LABEL_T) {
            if (testInfoFlag(F_EXTERNAL)) {
                setInfoExternId(externalsCnt);
                if (++externalsCnt == 0)
                    sub_6EF6(ERR219);	// LIMIT EXCEEDED: NUMBER OF EXTERNALS > 255
                setAddr(0);
                byte_3C3B = 0;
            } else if (testInfoFlag(F_PUBLIC))
                byte_3C3B = 0;
        }
        advNextInfo();
    }
    
    curInfo_p = off2Info(2);
    sub_701C();
    
    while (curInfo_p != 0) {
		if (!(testInfoFlag(F_MEMBER) || testInfoFlag(F_AT) || testInfoFlag(F_EXTERNAL))) {
            if (testInfoFlag(F_PARAMETER)) {
                p = curInfo_p;
                curInfo_p = procInfo[getInfoScope() >> 8];
                i = testInfoFlag(F_EXTERNAL);
                curInfo_p = p;
            } else 
                i = 0;
            if (i || testInfoFlag(F_BASED))
                setAddr(0);
            else if (testInfoFlag(F_DATA))
                sub_72EF(&initDataSize);
            else if (testInfoFlag(F_MEMORY))
                ;
            else if (testInfoFlag(F_AUTOMATIC))
                sub_7323();
            else
                sub_72EF(&uninitDataSize);
        }
        sub_701C();
    }
}


void sub_72EF(word *arg1w) {
	setAddr(*arg1w);
	*arg1w += getVarSize();
	if (*arg1w < getVarSize())
		sub_6EF6(ERR207);	// LIMIT EXCEEDED: SEGMENT SIZE
}

void sub_7323() {
	word p, *q;
	p = curInfo_p;
	curInfo_p = procInfo[getInfoScope() >> 8];
	q = &curInfo_p->parentOffset;
	curInfo_p = p;
	sub_72EF(q);
}


void procAtFile() {
	
	for (;;) {
		ifread(&atFile, &atFData.type, 1);
		switch(atFData.type) {
		case AT_AHDR: sub_73DC(); break;
		case AT_DHDR: ifread(&atFile, &atFData.info_p, 4); break;
		case AT_2: ifread(&atFile, &atFData.val, 2); break;
		case AT_STRING:	ifread(&atFile, &atFData.val, 2);
			ifread(&atFile, &atFData.type, atFData.val);
			break;
		case AT_DATA:	ifread(&atFile, &atFData.varInfoOffset, 8); break;
		case AT_END:	break;
		case AT_EOF:	return;
		}
	}
}


void sub_73DC() {
	ifread(&atFile, &atFData.info_p, 12);
	if (atFData.info_p == 0)
		return;
	atFData.info_p = off2Info(atFData.info_p);
	atStmtNum = atFData.stmtNum;
	atOffset = atFData.val;
	if (atFData.varInfoOffset != 0) {
		curInfo_p = off2Info(atFData.varInfoOffset);
		if (testInfoFlag(F_MEMBER)) {
			atOffset += getElementSize() * atFData.varNestedArrayIndex + getAddr();
			curInfo_p = getOwningStructure();
		}
		atOffset += getAddr() + getElementSize() * atFData.varArrayIndex;
		if (testInfoFlag(F_AT) && curInfo_p >= atFData.info_p)
			sub_6EF6(ERR213);	// UNDEFINED RESTRICTED REFERENCE IN 'AT'
	}
	sub_7486();
}


void sub_7486() {
	byte i, j;
	if (atFData.varInfoOffset == 0)
		i = 0;
	else if (getInfoType() > STRUCT_T || getInfoType() < BYTE_T) {
		i = 0;
		sub_6EF6(ERR211);			// INVALID IDENTIFIER IN 'AT' RESTRICTED REFERENCE
	} else if (testInfoFlag(F_EXTERNAL)) {
		i = 1;
		j = getInfoExternId();
	} else if (testInfoFlag(F_DATA))
		i = 2;
	else if (testInfoFlag(F_AUTOMATIC))
		i = 3;
	else if (testInfoFlag(F_MEMORY))
		i = 4;
	else if (testInfoFlag(F_BASED)) {
		i = 0;
		sub_6EF6(ERR212);			// INVALID RESTRICTED REFERENCE IN 'AT' , BASE ILLEGAL
	} else if (testInfoFlag(F_ABSOLUTE))
		i = 0;
	else
		i = 5;
	curInfo_p = atFData.info_p;

	do {
		if (!testInfoFlag(F_MEMBER)) {
			if (testInfoFlag(F_DATA)) {
				clrFlag(curInfo_p->flags, F_DATA);
				setInfoFlag(F_INITIAL);
			}
			setAddr(atOffset);
			atOffset += getVarSize();
			switch(i) {
			case 0: setInfoFlag(F_ABSOLUTE); break;
			case 1: if (testInfoFlag(F_PUBLIC))
					sub_6EF6(ERR178); // INVALID 'AT' RESTRICTED REFERENCE, EXTERNAL ATTRIBUTE CONFLICTS WITH PUBLIC ATTRIBUTE
				setInfoFlag(F_EXTERNAL);
				setInfoExternId(j);
				break;
			case 2: setInfoFlag(F_DATA); break;
			case 3: setInfoFlag(F_AUTOMATIC); break;
			case 4: setInfoFlag(F_MEMORY); break;
			case 5: break;
			}
		}
		sub_701C();
		if (curInfo_p == 0)
			return;
	} while (testInfoFlag(F_PACKED) || testInfoFlag(F_MEMBER));
}

void sub_75F7() {
	botInfo = (topMem + botMem) - topInfo;
	topInfo = topMem;
	sub_6F94(botMem, off2Info(0), info2Off(topInfo) + 1);
	helpers_p = botInfo - (117 * 2);
	localLabels_p = helpers_p - (localLabelCnt+1)*2;
	word_381E = localLabels_p - (localLabelCnt+1);
	word_3822 = botInfo - 2;
	// zero out the memory
	while (word_381E <= word_3822) {
		*(word *)word_3822 = 0;
		word_3822 -= 2;
	}
	// handle the case where the fill is odd number of bytes
	if (word_3822 == word_381E - 1)
		*(word *)word_3822 &= 0xff;
	word_3822 = word_381E - 2;
}


void sub_7695() {
	if ((byte_9B40 = PRINT | OBJECT) | IXREF) {
		openFile(&nmsFile, 3);
//		assignFileBuffer(&nmsFile, xrfBuf, 512, 2);
	}
//	assignFileBuffer(&atFile, atBuf, 512, 1);
	uninitDataSize = initDataSize = 0;
}



void sub_76D9() {
    byte i;
    static byte j = T2_EOF;
    
    if (IXREF || byte_9B40) {
//        flushFile(&nmsFile);
        rewindFile(&nmsFile);
    }
    rewindFile(&atFile);		// used for string data
    ifwrite(&tx2File, &j, 1);
//    flushFile(&tx2File);
    rewindFile(&tx2File);
    for (i = 1; procCnt >= i; i++) {
        procInfo[i] -= botMem;
    }
}


void sub_7D88() {
    word tmp = curInfo_p;
    curInfo_p = curSymbol_p->infoChain;	// get the symbol's info

    while (curInfo_p != 0) {
	if (testInfoFlag(F_MEMBER)) 	// ? structure member
	    if (tmp == getOwningStructure())
		return;
	curInfo_p = getNextInfo();
    }
}










