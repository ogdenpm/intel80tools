// $Id: plmov6.cpp,v 1.1 2003/10/04 21:08:48 Mark Ogden Exp $
#include "plm.hpp"
#include "common.hpp"


byte hasPrintOrObjectFile;
byte emittingLine;
byte listOff;
byte codeOn;
word ERRNum;
word ERRInfo;
word ERRStmtNum;
byte moreToDo = 0xff;
word curLineCount;
word sourceLineNum;
word statementDepth;
word curStmtNum6;
word offLastCh;
word offCurCh;
byte skipLinePrefix = 0xff;
byte skipLinePrefixOnError = 0xff;
byte lstLineLen;
char srcLineBuf[130];
byte itemType;
word itemArgs[4];
byte crCnt;


void emitSourceLineNumber();
void setSkipLst(byte arg1b);
void setMarginAndTabW(byte arg1b, byte arg2b);
void setMarkerInfo(byte arg1b, byte arg2b, byte arg3b);
void flushLstBuf();
void printExitMsg();
void lstModuleInfo();
byte getSourceCh6();
void getSourceLine6();
void fatalError_ov6(byte arg1b);
void sub_66F0();
void emitError6();
void emitLinePrefix();
void updateCmdInfo();
void error6();
void tokenError6();
void syntaxError6();
void updateLineInfo();
void execTx2Item();
void sub_41B6();
void loadNames();
void sub_404A();
void sub_3F96();
void xputstrLst(char *arg1w);

int overlay6() {
	try {
		sub_404A();
		if (IXREF || hasPrintOrObjectFile)
			loadNames();
		sub_3F96();
		while (moreToDo)
			execTx2Item();
		emitLinePrefix();
	}
	catch (...) {
	}
	sub_41B6();
	if (IXREF || PRINT) {
		if (SYMBOLS || XREF || IXREF)
			overlay5();
		else
			lstModuleInfo();
	}
	printExitMsg();
	return -1;
}

void sub_3F96() {
	if (PRINT) {
		newPageNextChLst();
		xputstr2cLst("ISIS-II PL/M-80 ", 0);
		xputnstrLst(version, 4);
		xputstr2cLst(" COMPILATION OF MODULE ", 0);
		curInfo_p = off2Info(procInfo[1]);
		if((curSymbol_p = getInfoSymbol()) != 0) 
			xputnstrLst((char *)&curSymbol_p->name[1], curSymbol_p->name[0]);
		newLineLst();
		if (OBJECT) 
			xputstr2cLst("NO OBJECT MODULE GENERATED", 0);
		else
			xputstr2cLst("NO OBJECT MODULE REQUESTED", 0);
		newLineLst();
		xputstr2cLst("COMPILER INVOKED BY:  ", 0);
		cmdLine_p = startCmdLine_p;
		while (cmdLine_p != 0) {
			tabLst(0xe9);
			xputstr2cLst(cmdLine_p->text, '\r');
			cmdLine_p = cmdLine_p->link;
		}
		newLineLst();
		setSkipLst(3);
	}
}



void sub_404A() {
	static byte tx2buf[2048];
	static byte nmsbuf[2048];
	static char lstBuf[2048];
	if (PRINT) {
		lstBufPtr = lstBuf;
		lstBufSize = 0x7ff;
	}
	hasPrintOrObjectFile = OBJECT | PRINT;
	if (OBJECT)
		deleteFile(&objFile);
	if (!lstFileOpen && PRINTSet) {
		deleteFile(&lstFile);
		PRINTSet = 0;
	}
	closeFile(&tx1File);
	deleteFile(&tx1File);
	//assignFileBuffer(&tx2File, tx2buf, 0x800, 1);
	if (IXREF || hasPrintOrObjectFile)
		//assignFileBuffer(&nmsFile, nmsbuf, 0x800, 1);
	curStmtNum6 = 0;
	if (PRINT) {
		srcFileIdx = 0;
		initFile(&srcFile, "SOURCE", srcFileTable[srcFileIdx].filename); // note struct used
		openFile(&srcFile, 1);
	}
	curInfo_p = off2Info(procInfo[1]);
	setSkipLst(3);
	setMarkerInfo(11, '-', 15);
	if (fatalErrorCode > 0) {
		ERRInfo = ERRStmtNum = 0;
		ERRNum = fatalErrorCode;
		emitError6();
		setSkipLst(2);
	}
	emittingLine = PRINT;
	listOff = 0;
	codeOn = 0;
	programErrCnt = 0;
	linesRead = 0;
	initDataSize = 0;
}

void loadNames() {
	byte i;
	curSymbol_p = (topSymbol = localLabels_p - 3) - 1;
	ifread(&nmsFile, &i, 1);
	while (i != 0) {
		curSymbol_p = word(curSymbol_p) - i - 1;
		curSymbol_p->name[0] = i;
		ifread(&nmsFile, &curSymbol_p->name[1], i);
		ifread(&nmsFile, &i, 1);
	}
	botSymbol = address(curSymbol_p->name);
	botMem = (word)botSymbol;
}


void sub_41B6() {
	closeFile(&atFile);
	deleteFile(&atFile);
	closeFile(&tx2File);
	deleteFile(&tx2File);
	if (IXREF || hasPrintOrObjectFile) {
		closeFile(&nmsFile);
		deleteFile(&nmsFile);
	}
	linesRead = curLineCount;
	if (PRINT) {
		tellFile(&srcFile, (loc_t *)&srcFileTable[srcFileIdx].block);
		backupPos((loc_t *)&srcFileTable[srcFileIdx].block, offLastCh - offCurCh);
		closeFile(&srcFile);
		flushLstBuf();
	}
}

byte byte_4222[] = {
     // 0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    0xE,  1,    1,    1,  //00
     	1,    1,    1,    2,    2,    2,    2,    2,    2,    2,    2,    2,    3,    3,    2,    1,  //10
     	1,    1,    1,    1,    2,    2,    2,    2,    2,    2,    2,    2,    0,    0,    0,    0,  //20
     	0,    0,    0,    0,    0,    0,    0,    0,    0,    1,    1,    1,    1,    2,    2,    6,  //30
     	1,    0,    0,    0,    0,    3,    0,    9,    9,    8,    1,    1,    1,    0,    0,    0,  //40
     	0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  //50
     	0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  //60
     	0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    1,  //70
     	1,    1,    2,    9,    0xB,  0xB,  0x81, 0x99, 0x91, 0x91, 0x91, 0x81, 0x89, 0x89, 0x89, 0x8A,  //80
	    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x90, 0xA1, 0xA3, 0xA0, 0xA1, 0xA2, 0xB0, 0xA0, 0xA0, 0xA0,  //90
	    0xA0, 0xA0, 0xA3, 0xA3, 0,    0,    0,    0,    0,    0,    0,    0,    0x41, 0x41, 0x41, 0x40,  //A0
	    0,    0,    0,    0,    0,    0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xD8, 0x41, 0x41, 0x41, 0xC0      //B0
};

void execTx2Item() {
	itemArgs[2] = itemArgs[1] = itemArgs[0] = 0;
	ifread(&tx2File, &itemType, 1);
	if (itemType != T2_INCLUDE)
		ifread(&tx2File, itemArgs, (byte_4222[itemType] & 3) * 2);
	if (itemType == T2_LINEINFO)
		updateLineInfo();
	else if (itemType == T2_STMTCNT)
		curStmtNum6 = itemArgs[0];
	else if (itemType == T2_SYNTAXERROR)
		syntaxError6();
	else if (itemType == T2_TOKENERROR)
		tokenError6();
	else if (itemType == T2_ERROR)
		error6();
	else if (itemType >= T2_LIST && itemType <= T2_INCLUDE)
		updateCmdInfo();
	else if (itemType == T2_EOF)
		moreToDo = 0;
}


void updateLineInfo() {
	word word_9B7F;
	if (itemArgs[1] > 0 || itemArgs[2] == 0)
		itemArgs[3] = itemArgs[0];
	else {
		itemArgs[3] = itemArgs[2];
		itemArgs[2] = 0;
	}
	word_9B7F = itemArgs[0];
	while (itemArgs[3] >= word_9B7F) {
		emitLinePrefix();
		curLineCount = word_9B7F;
		sourceLineNum = itemArgs[1];
		statementDepth = itemArgs[2];
		getSourceLine6();
		word_9B7F++;
	}
}

void syntaxError6() {
	ERRNum = itemArgs[0];
	ERRInfo = 0;
	ERRStmtNum = curStmtNum6;
	emitError6();
}


void tokenError6() {
	ERRNum = itemArgs[0];
	ERRInfo = itemArgs[1];
	ERRStmtNum = curStmtNum6;
	emitError6();
}


void error6() {
	ERRNum = itemArgs[0];
	ERRInfo = itemArgs[1];
	ERRStmtNum = itemArgs[2];
	emitError6();
}


void updateCmdInfo() {
	char name[20];

	switch(itemType) {
	case T2_LIST:	listOff = 0; break;
	case T2_NOLIST: listOff = 0xff; break;
	case T2_CODE:	codeOn = PRINT; break;
	case T2_NOCODE: codeOn = 0; break;
	case T2_EJECT: if (emittingLine)
			newPageNextChLst();
		break;
	case T2_INCLUDE:
		emitLinePrefix();
		tellFile(&srcFile, (loc_t *) & srcFileTable[srcFileIdx].block);
    		backupPos((loc_t *) & srcFileTable[srcFileIdx].block, offLastCh - offCurCh);
		srcFileIdx++;	// 10 words in asm src
		ifread(&tx2File, name+13, 6);	// read in name of include file
		ifread(&tx2File, name+6, 7);
		ifread(&tx2File, name, 7);	// overwrites the type byte
		movemem(16, name+1, srcFileTable[srcFileIdx].filename);
		closeFile(&srcFile);
		initFile(&srcFile, "SOURCE", name+1);
		openFile(&srcFile, 1);
		offCurCh = offLastCh;	// force read next time	
	}
}



char *ERRStrings[] = {
	"UNKNOWN ERROR", // 0
	"INVALID PL/M-80 CHARACTER", // 1
	"UNPRINTABLE ASCII CHARACTER", // 2
	"IDENTIFIER, STRING, OR NUMBER TOO LONG, TRUNCATED", // 3
	"ILLEGAL NUMERIC CONSTANT TYPE", // 4
	"INVALID CHARACTER IN NUMERIC CONSTANT", // 5
	"ILLEGAL MACRO REFERENCE, RECURSIVE EXPANSION", // 6
	"LIMIT EXCEEDED: MACROS NESTED TOO DEEPLY", // 7
	"INVALID CONTROL FORMAT", // 8
	"INVALID CONTROL", // 9
	"ILLEGAL USE OF PRIMARY CONTROL AFTER NON-CONTROL LINE", // 0xA
	"MISSING CONTROL PARAMETER", // 0xB
	"INVALID CONTROL PARAMETER", // 0xC
	"LIMIT EXCEEDED: INCLUDE NESTING", // 0xD
	"INVALID CONTROL FORMAT, INCLUDE NOT LAST CONTROL", // 0xE
	"MISSING INCLUDE CONTROL PARAMETER", // 0xF
	"ILLEGAL PRINT CONTROL", // 0x10
	"INVALID PATH-NAME", // 0x11
	"INVALID MULTIPLE LABELS AS MODULE NAMES", // 0x12
	"INVALID LABEL IN MODULE WITHOUT MAIN PROGRAM", // 0x13
	"MISMATCHED IDENTIFIER AT END OF BLOCK", // 0x14
	"MISSING PROCEDURE NAME", // 0x15
	"INVALID MULTIPLE LABELS AS PROCEDURE NAMES", // 0x16
	"INVALID LABELLED END IN EXTERNAL PROCEDURE", // 0x17
	"INVALID STATEMENT IN EXTERNAL PROCEDURE", // 0x18
	"UNDECLARED PARAMETER", // 0x19
	"INVALID DECLARATION, STATEMENT OUT OF PLACE", // 0x1A
	"LIMIT EXCEEDED: NUMBER OF DO BLOCKS", // 0x1B
	"MISSING 'THEN'", // 0x1C
	"ILLEGAL STATEMENT", // 0x1D
	"LIMIT EXCEEDED: NUMBER OF LABELS ON STATEMENT", // 0x1E
	"LIMIT EXCEEDED: PROGRAM TOO COMPLEX", // 0x1F
	"INVALID SYNTAX, TEXT IGNORED UNTIL ';'", // 0x20
	"DUPLICATE LABEL DECLARATION", // 0x21
	"DUPLICATE PROCEDURE DECLARATION", // 0x22
	"LIMIT EXCEEDED: NUMBER OF PROCEDURES", // 0x23
	"MISSING PARAMETER", // 0x24
	"MISSING ')' AT END OF PARAMETER LIST", // 0x25
	"DUPLICATE PARAMETER NAME", // 0x26
	"INVALID ATTRIBUTE OR INITIALIZATION, NOT AT MODULE LEVEL", // 0x27
	"DUPLICATE ATTRIBUTE", // 0x28
	"CONFLICTING ATTRIBUTE", // 0x29
	"INVALID INTERRUPT VALUE", // 0x2A
	"MISSING INTERRUPT VALUE", // 0x2B
	"ILLEGAL ATTRIBUTE, 'INTERRUPT' WITH PARAMETERS", // 0x2C
	"ILLEGAL ATTRIBUTE, 'INTERRUPT' WITH TYPED PROCEDURE", // 0x2D
	"ILLEGAL USE OF LABEL", // 0x2E
	"MISSING ')' AT END OF FACTORED DECLARATION", // 0x2F
	"ILLEGAL DECLARATION STATEMENT SYNTAX", // 0x30
	"LIMIT EXCEEDED: NUMBER OF ITEMS IN FACTORED DECLARE", // 0x31
	"INVALID ATTRIBUTES FOR BASE", // 0x32
	"INVALID BASE, SUBSCRIPTING ILLEGAL", // 0x33
	"INVALID BASE, MEMBER OF BASED STRUCTURE OR ARRAY OF STRUCTURES", // 0x34
	"INVALID STRUCTURE MEMBER IN BASE", // 0x35
	"UNDECLARED BASE", // 0x36
	"UNDECLARED STRUCTURE MEMBER IN BASE", // 0x37
	"INVALID MACRO TEXT, NOT A STRING CONSTANT", // 0x38
	"INVALID DIMENSION, ZERO ILLEGAL", // 0x39
	"INVALID STAR DIMENSION IN FACTORED DECLARATION", // 0x3A
	"ILLEGAL DIMENSION ATTRIBUTE", // 0x3B
	"MISSING ')' AT END OF DIMENSION", // 0x3C
	"MISSING TYPE", // 0x3D
	"INVALID STAR DIMENSION WITH 'STRUCTURE' OR 'EXTERNAL'", // 0x3E
	"INVALID DIMENSION WITH THIS ATTRIBUTE", // 0x3F
	"MISSING STRUCTURE MEMBERS", // 0x40
	"MISSING ')' AT END OF STRUCTURE MEMBER LIST", // 0x41
	"INVALID STRUCTURE MEMBER, NOT AN IDENTIFIER", // 0x42
	"DUPLICATE STRUCTURE MEMBER NAME", // 0x43
	"LIMIT EXCEEDED: NUMBER OF STRUCTURE MEMBERS", // 0x44
	"INVALID STAR DIMENSION WITH STRUCTURE MEMBER", // 0x45
	"INVALID MEMBER TYPE, 'STRUCTURE' ILLEGAL", // 0x46
	"INVALID MEMBER TYPE, 'LABEL' ILLEGAL", // 0x47
	"MISSING TYPE FOR STRUCTURE MEMBER", // 0x48
	"INVALID ATTRIBUTE OR INITIALIZATION, NOT AT MODULE LEVEL", // 0x49
	"INVALID STAR DIMENSION, NOT WITH 'DATA' OR 'INITIAL'", // 0x4A
	"MISSING ARGUMENT OF 'AT', 'DATA', OR 'INITIAL'", // 0x4B
	"CONFLICTING ATTRIBUTE WITH PARAMETER", // 0x4C
	"INVALID PARAMETER DECLARATION, BASE ILLEGAL", // 0x4D
	"DUPLICATE DECLARATION", // 0x4E
	"ILLEGAL PARAMETER TYPE, NOT BYTE OR ADDRESS", // 0x4F
	"INVALID DECLARATION, LABEL MAY NOT BE BASED", // 0x50
	"CONFLICTING ATTRIBUTE WITH 'BASE'", // 0x51
	"INVALID SYNTAX, MISMATCHED '('", // 0x52
	"LIMIT EXCEEDED: DYNAMIC STORAGE", // 0x53
	"LIMIT EXCEEDED: BLOCK NESTING", // 0x54
	"LONG STRING ASSUMED CLOSED AT NEXT SEMICOLON OR QUOTE", // 0x55
	"LIMIT EXCEEDED: SOURCE LINE LENGTH", // 0x56
	"MISSING 'END', END-OF-FILE ENCOUNTERED", // 0x57
	"INVALID PROCEDURE NESTING, ILLEGAL IN REENTRANT PROCEDURE", // 0x58
	"MISSING 'DO' FOR MODULE", // 0x59
	"MISSING NAME FOR MODULE", // 0x5A
	"ILLEGAL PAGELENGTH CONTROL VALUE", // 0x5B
	"ILLEGAL PAGEWIDTH CONTROL VALUE", // 0x5C
	"MISSING 'DO' FOR 'END', 'END' IGNORED", // 0x5D
	"ILLEGAL CONSTANT, VALUE > 65535", // 0x5E
	"ILLEGAL RESPECIFICATION OF PRIMARY CONTROL IGNORED", // 0x5F
	"COMPILER ERROR: SCOPE STACK UNDERFLOW", // 0x60
	"COMPILER ERROR: PARSE STACK UNDERFLOW", // 0x61
	"INCLUDE FILE IS NOT A DISKETTE FILE", // 0x62
	"UNKNOWN ERROR", // 0x63
	"INVALID STRING CONSTANT IN EXPRESSION", // 0x64
	"INVALID ITEM FOLLOWS DOT OPERATOR", // 0x65
	"MISSING PRIMARY OPERAND", // 0x66
	"MISSING ')' AT END OF SUBEXPRESSION", // 0x67
	"ILLEGAL PROCEDURE INVOCATION WITH DOT OPERATOR", // 0x68
	"UNDECLARED IDENTIFIER", // 0x69
	"INVALID INPUT/OUTPUT PORT NUMBER", // 0x6A
	"ILLEGAL INPUT/OUTPUT PORT NUMBER, NOT NUMERIC CONSTANT", // 0x6B
	"MISSING ')' AFTER INPUT/OUTPUT PORT NUMBER", // 0x6C
	"MISSING INPUT/OUTPUT PORT NUMBER", // 0x6D
	"INVALID LEFT OPERAND OF QUALIFICATION, NOT A STRUCTURE", // 0x6E
	"INVALID RIGHT OPERAND OF QUALIFICATION, NOT IDENTIFIER", // 0x6F
	"UNDECLARED STRUCTURE MEMBER", // 0x70
	"MISSING ')' AT END OF ARGUMENT LIST", // 0x71
	"INVALID SUBSCRIPT, MULTIPLE SUBSCRIPTS ILLEGAL", // 0x72
	"MISSING ')' AT END OF SUBSCRIPT", // 0x73
	"MISSING '=' IN ASSIGNMENT STATEMENT", // 0x74
	"MISSING PROCEDURE NAME IN CALL STATEMENT", // 0x75
	"INVALID INDIRECT CALL, IDENTIFIER NOT AN ADDRESS SCALAR", // 0x76
	"LIMIT EXCEEDED: PROGRAM TOO COMPLEX", // 0x77
	"LIMIT EXCEEDED: EXPRESSION TOO COMPLEX", // 0x78
	"LIMIT EXCEEDED: EXPRESSION TOO COMPLEX", // 0x79
	"LIMIT EXCEEDED: PROGRAM TOO COMPLEX", // 0x7A
	"INVALID DOT OPERAND, BUILT-IN PROCEDURE ILLEGAL", // 0x7B
	"MISSING ARGUMENTS FOR BUILT-IN PROCEDURE", // 0x7C
	"ILLEGAL ARGUMENT FOR BUILT-IN PROCEDURE", // 0x7D
	"MISSING ')' AFTER BUILT-IN PROCEDURE ARGUMENT LIST", // 0x7E
	"INVALID SUBSCRIPT ON NON-ARRAY", // 0x7F
	"INVALID LEFT-HAND OPERAND OF ASSIGNMENT", // 0x80
	"ILLEGAL 'CALL' WITH TYPED PROCEDURE", // 0x81
	"ILLEGAL REFERENCE TO OUTPUT FUNCTION", // 0x82
	"ILLEGAL REFERENCE TO UNTYPED PROCEDURE", // 0x83
	"ILLEGAL USE OF LABEL", // 0x84
	"ILLEGAL REFERENCE TO UNSUBSCRIPTED ARRAY", // 0x85
	"ILLEGAL REFERENCE TO UNSUBSCRIPTED MEMBER ARRAY", // 0x86
	"ILLEGAL REFERENCE TO AN UNQUALIFIED STRUCTURE", // 0x87
	"INVALID RETURN FOR UNTYPED PROCEDURE, VALUE ILLEGAL", // 0x88
	"MISSING VALUE IN RETURN FOR TYPED PROCEDURE", // 0x89
	"MISSING INDEX VARIABLE", // 0x8A
	"INVALID INDEX VARIABLE TYPE, NOT BYTE OR ADDRESS", // 0x8B
	"MISSING '=' FOLLOWING INDEX VARIABLE", // 0x8C
	"MISSING 'TO' CLAUSE", // 0x8D
	"MISSING IDENTIFIER FOLLOWING GOTO", // 0x8E
	"INVALID REFERENCE FOLLOWING GOTO, NOT A LABEL", // 0x8F
	"INVALID GOTO LABEL, NOT AT LOCAL OR MODULE LEVEL", // 0x90
	"MISSING 'TO' FOLLOWING 'GO'", // 0x91
	"MISSING ')' AFTER 'AT' RESTRICTED EXPRESSION", // 0x92
	"MISSING IDENTIFIER FOLLOWING DOT OPERATOR", // 0x93
	"INVALID QUALIFICATION IN RESTRICTED REFERENCE", // 0x94
	"INVALID SUBSCRIPTING IN RESTRICTED REFERENCE", // 0x95
	"MISSING ')' AT END OF RESTRICTED SUBSCRIPT", // 0x96
	"INVALID OPERAND IN RESTRICTED EXPRESSION", // 0x97
	"MISSING ')' AFTER CONSTANT LIST", // 0x98
	"INVALID NUMBER OF ARGUMENTS IN CALL, TOO MANY", // 0x99
	"INVALID NUMBER OF ARGUMENTS IN CALL, TOO FEW", // 0x9A
	"INVALID RETURN IN MAIN PROGRAM", // 0x9B
	"MISSING RETURN STATEMENT IN TYPED PROCEDURE", // 0x9C
	"INVALID ARGUMENT, ARRAY REQUIRED FOR LENGTH OR LAST", // 0x9D
	"INVALID DOT OPERAND, LABEL ILLEGAL", // 0x9E
	"COMPILER ERROR: PARSE STACK UNDERFLOW", // 0x9F
	"COMPILER ERROR: OPERAND STACK UNDERFLOW", // 0xA0
	"COMPILER ERROR: ILLEGAL OPERAND STACK EXCHANGE", // 0xA1
	"COMPILER ERROR: OPERATOR STACK UNDERFLOW", // 0xA2
	"COMPILER ERROR: GENERATION FAILURE", // 0xA3
	"COMPILER ERROR: SCOPE STACK OVERFLOW", // 0xA4
	"COMPILER ERROR: SCOPE STACK UNDERFLOW", // 0xA5
	"COMPILER ERROR: CONTROL STACK OVERFLOW", // 0xA6
	"COMPILER ERROR: CONTROL STACK UNDERFLOW", // 0xA7
	"COMPILER ERROR: BRANCH MISSING IN 'IF' STATEMENT", // 0xA8
	"ILLEGAL FORWARD CALL", // 0xA9
	"ILLEGAL RECURSIVE CALL", // 0xAA
	"INVALID USE OF DELIMITER OR RESERVED WORD IN EXPRESSION", // 0xAB
	"INVALID LABEL: UNDEFINED", // 0xAC
	"INVALID LEFT SIDE OF ASSIGNMENT: VARIABLE DECLARED WITH DATA ATTRIBUTE", // 0xAD
	"INVALID NULL PROCEDURE", // 0xAE
	"UNKNOWN ERROR", // 0xAF
	"INVALID INTVECTOR INTERVAL VALUE", // 0xB0
	"INVALID INTVECTOR LOCATION VALUE", // 0xB1
	"INVALID 'AT' RESTRICTED REFERENCE, EXTERNAL ATTRIBUTE CONFLICTS WITH PUBLIC ATTRIBUTE", // 0xB2
	"OUTER 'IF' MAY NOT HAVE AN 'ELSE' PART", // 0xB3
	"MISSING OR INVALID CONDITIONAL COMPILATION PARAMETER", // 0xB4
	"MISSING OR INVALID CONDITIONAL COMPILATION CONSTANT", // 0xB5
	"MISPLACED ELSE OR ELSEIF OPTION", // 0xB6
	"MISPLACED ENDIF OPTION", // 0xB7
	"CONDITIONAL COMPILATION PARAMETER NAME TOO LONG", // 0xB8
	"MISSING OPERATOR IN CONDITIONAL COMPILATION EXPRESSION", // 0xB9
	"INVALID CONDITIONAL COMPILATION CONSTANT, TOO LARGE", // 0xBA
	"LIMIT EXCEEDED: NUMBER OF SAVE LEVELS > 5", // 0xBB
	"MISPLACED RESTORE OPTION", // 0xBC
	"NULL STRING NOT ALLOWED", // 0xBD
	"UNKNOWN ERROR", // 0xBE
	"UNKNOWN ERROR", // 0xBF
	"UNKNOWN ERROR", // 0xC0
	"UNKNOWN ERROR", // 0xC1
	"UNKNOWN ERROR", // 0xC2
	"UNKNOWN ERROR", // 0xC3
	"UNKNOWN ERROR", // 0xC4
	"UNKNOWN ERROR", // 0xc5
	"UNKNOWN ERROR", // 0xc7
	"LIMIT EXCEEDED: STATEMENT SIZE", // 0xC8
	"INVALID DO CASE BLOCK, AT LEAST ONE CASE REQUIRED", // 0xC9
	"LIMIT EXCEEDED: NUMBER OF ACTIVE CASES", // 0xCA
	"LIMIT EXCEEDED: NESTING OF TYPED PROCEDURE CALLS", // 0xCB
	"LIMIT EXCEEDED: NUMBER OF ACTIVE PROCEDURES AND DO CASE GROUPS", // 0xCC
	"ILLEGAL NESTING OF BLOCKS, ENDS NOT BALANCED", // 0xCD
	"LIMIT EXCEEDED: CODE SEGMENT SIZE", // 0xCE
	"LIMIT EXCEEDED: SEGMENT SIZE", // 0xCF
	"LIMIT EXCEEDED: STRUCTURE SIZE", // 0xD0
	"ILLEGAL INITIALIZATION OF MORE SPACE THAN DECLARED", // 0xD1
	"ILLEGAL INITIALIZATION OF A BYTE TO A VALUE > 255", // 0xD2
	"INVALID IDENTIFIER IN 'AT' RESTRICTED REFERENCE", // 0xD3
	"INVALID RESTRICTED REFERENCE IN 'AT', BASE ILLEGAL", // 0xD4
	"UNDEFINED RESTRICTED REFERENCE IN 'AT'", // 0xD5
	"COMPILER ERROR: OPERAND CANNOT BE TRANSFORMED", // 0xD6
	"COMPILER ERROR: EOF READ IN FINAL ASSEMBLY", // 0xD7
	"COMPILER ERROR: BAD LABEL ADDRESS", // 0xD8
	"ILLEGAL INITIALIZATION OF AN EXTERNAL VARIABLE", // 0xD9
	"ILLEGAL SUCCESSIVE USES OF RELATIONAL OPERATORS", // 0xDA
	"LIMIT EXCEEDED: NUMBER OF EXTERNALS > 255" // 0xDB
};


void emitLinePrefix() {
	if (!skipLinePrefix && emittingLine) {
		setMarginAndTabW(15, 4);
		if (sourceLineNum != 0)
			emitSourceLineNumber();
		tabLst(0xf9);
		if (statementDepth < 10) {
			xputcLst(' ');
			xputcLst(" 123456789"[statementDepth]);
		} else
			xnumLst(statementDepth, 2, 10);
		if (srcFileIdx != 0) {
			tabLst(0xf5);
			xputnstrLst("=", 1);
			if (srcFileIdx != 1)  // 10 in bytes
				xnumLst(srcFileIdx /* /10 */ - 1, 1, 10);
		}
		if (lstLineLen > 0) {
			tabLst(0xf1);
			xputnstrLst(srcLineBuf, lstLineLen);
		}
		newLineLst();
		skipLinePrefixOnError = 0xff;
	}
	skipLinePrefix = 0xff;
	emittingLine = ~listOff & PRINT;
}


void emitError6() {
	programErrCnt++;
	if (PRINT) {
		skipLinePrefix = skipLinePrefixOnError;
		emittingLine = 0xff;
		emitLinePrefix();
		xputnstrLst("*** ERROR #", 0xb);
		xnumLst(ERRNum, 0, 10);
		xputnstrLst(", ", 2);
		if (ERRStmtNum != 0) {
			xputnstrLst("STATEMENT #", 0xb);
			xnumLst(ERRStmtNum, 0, 10);
			xputnstrLst(", ", 2);
		}
		if (ERRInfo != 0) {
			xputnstrLst("NEAR '", 6);
			curInfo_p = off2Info(ERRInfo);
			curSymbol_p = getInfoSymbol();
			if (curSymbol_p != 0)
				xputnstrLst((char *)&curSymbol_p->name[1], curSymbol_p->name[0]);
			else
				xputstr2cLst("<LONG CONSTANT>", 0);
			xputnstrLst("', ", 3);
		}

		tabLst(2);
		
		xputstrLst(ERRStrings[ERRNum <= 0xDB ? ERRNum : 0]);
		newLineLst();
	}
}


void fatalError_ov6(byte arg1b) {
	ERRNum = fatalErrorCode = arg1b;
	ERRInfo = 0;
	ERRStmtNum = curStmtNum6;
	emitError6();
	throw "fatal";
}

void getSourceLine6() {
	lstLineLen = 0;
	crCnt = 0;
	for (;;) {
		srcLineBuf[lstLineLen] = getSourceCh6();
		if (srcLineBuf[lstLineLen] == '\n') {
			skipLinePrefixOnError = skipLinePrefix = 0;
			return;
		}
		if (srcLineBuf[lstLineLen] == '\r')
			crCnt++;
		else if (srcLineBuf[lstLineLen] != '\r' && lstLineLen < 128)
			lstLineLen++;
	}
}


byte getSourceCh6() {
	static byte srcbuf[2048];
	if (offLastCh == offCurCh) {
		for (;;) {
			readFile(&srcFile, srcbuf, 1280, &offLastCh);
			offCurCh = 0;
			if (offLastCh > 0)
				break;
			if (lstLineLen != 0)
				return '\n';
			if (srcFileIdx == 0)
				if (crCnt != 0)
					return '\n';
				else
					fatalError_ov6(ERR215);	// "COMPILER ERROR: EOF READ IN FINAL ASSEMBLY"
			closeFile(&srcFile);
			srcFileIdx--;		// 10 words in assembly
			initFile(&srcFile, "SOURCE", srcFileTable[srcFileIdx].filename);
			openFile(&srcFile, 1);
			seekFile(&srcFile, (loc_t *)&srcFileTable[srcFileIdx].block);
		}
		offLastCh--;
	} else
		offCurCh++;
	return srcbuf[offCurCh] & 0x7f;
}


void lstModuleInfo()
{
	word p, q, r;
	p = 0;
	for (q= 1; q <= procCnt; q++) {
		curInfo_p = off2Info(procInfo[q]);
		r= getBasedOffset();
		if (p < r) 
			p = r;
	}
	setSkipLst(3);
	xputstr2cLst("MODULE INFORMATION:", 0);
	newLineLst();
	setSkipLst(1);
	tabLst(5);
	xputstr2cLst("CODE AREA SIZE     = ", 0);
	xnumLst(initDataSize, 0xFC, 0xF0);
	tabLst(2);
	xnumLst(initDataSize, 5, 0xF6);
	newLineLst();
	tabLst(5);
	xputstr2cLst("VARIABLE AREA SIZE = ", 0);
	xnumLst(uninitDataSize, 0xFC, 0xF0);
	tabLst(2);
	xnumLst(uninitDataSize, 5, 0xF6);
	newLineLst();
	tabLst(5);
	xputstr2cLst("MAXIMUM STACK SIZE = ", 0);
	xnumLst(p, 0xFC, 0xF0);
	tabLst(2);
	xnumLst(p, 5, 0xF6);
	newLineLst();
	tabLst(5);
	xnumLst(linesRead, 0, 0xA);
	xputstr2cLst(" LINES READ", 0);
	newLineLst();
	tabLst(5);
	xnumLst(programErrCnt, 0, 0xA);
	xputstr2cLst(" PROGRAM ERROR", 0);
	if (programErrCnt != 1)
		xputstr2cLst("S", 0);

	newLineLst();
	setSkipLst(1);
	xputstr2cLst("END OF PL/M-80 COMPILATION", 0);
	newLineLst();
	flushLstBuf();
	closeFile(&lstFile);
	lstFileOpen = 0;
}


void printExitMsg() {
	static char msg[] =  "XXXXX PROGRAM ERROR"; 
	byte i = num2Asc(programErrCnt, 5, 10, msg);
	printf("PL/M-80 COMPILATION COMPLETE.  %5d PROGRAM ERROR", programErrCnt);
	if (programErrCnt != 1)
		putchar('S');
	printf("\r\n\n\n");
}


void xputcLst(byte arg1b) {
	byte i;

	if (col == 0) {
		if (linesLeft == 0)
			newPageLst();
		else if (skipCnt >= linesLeft)
			newPageLst();
		else
			while (skipCnt != 0) {
				putcLst('\n');
				linesLeft--;
				skipCnt--;
			}
	} else if (col >= PAGEWIDTH)
		newLineLeaderLst();
	if (arg1b == '\t') {
		if (listingMargin > col)
			return;
		i = tabWidth - (col - listingMargin) % tabWidth;
		if (PAGEWIDTH <= col + i) {
			newLineLeaderLst();
			return;
		}
		while (i != 0) {
			putcLst(' ');
			col++;
			i--;
		}
		return;
	} else {
		putcLst(arg1b);
		if (arg1b == '\r')
			col = 0;
		else
			col++;
	}
}




void flushLstBuf() {
	if (lstChCnt != 0) {
		if (!lstFileOpen) {
			openFile(&lstFile, 2);
			lstFileOpen = 0xff;
		}
		writeFile(&lstFile, lstBufPtr, lstChCnt);
		lstChCnt = 0;
	}
}

void newLineLst() {
	if (col == 0 && linesLeft == 0)
		newPageLst();
	put2cLst(0xd0a);
	linesLeft--;
	col = 0;
}


void tabLst(byte arg1b) {
	if (arg1b > 0x7f) {
		if(col >= (arg1b = -arg1b))
			newLineLst();
		arg1b = arg1b - col - 1;
	}
	while (arg1b != 0) {
		xputcLst(' ');
		arg1b--;
	}
}


void newPageNextChLst() {
	linesLeft = 0;
}







void setSkipLst(byte arg1b) {
	skipCnt = arg1b;
}


void xputstr2cLst(char *arg1w, byte arg2b) {
	while (*arg1w != arg2b) 
		xputcLst(*arg1w++);
}

void  xputnstrLst(char *arg1w, byte arg2b) {
	while (arg2b != 0) {
		xputcLst(*arg1w++);
		arg2b--;
	}
}

void xputstrLst(char *arg1w) {
	while (*arg1w)
		xputcLst(*arg1w++);
}


void xnumLst(word arg1w, byte arg2b, byte arg3b) {
	char buf[7];
	byte i;

	i = num2Asc(arg1w, arg2b, arg3b, buf);
	xputnstrLst(buf, i);
}

void emitSourceLineNumber() {
	char buf[7];
	sprintf(buf, "%6d", sourceLineNum);
	xputnstrLst(&buf[2], 4);
}
	



void putcLst(byte arg1b) {		// 77D2
	lstBufPtr[lstChCnt] = arg1b;
	if (lstChCnt == lstBufSize) {
		if (!lstFileOpen) {
			openFile(&lstFile, 2);
			lstFileOpen = 0xff;
		}
		writeFile(&lstFile, lstBufPtr, lstBufSize + 1);
		lstChCnt = 0;
	} else
		lstChCnt++;
}


void put2cLst(word arg1w) {
	char *p = (char *)&arg1w;
	putcLst(p[1]);
	putcLst(p[0]);
}


void newPageLst() {
	char pnum[3];
	byte i, j, k, m;
	if (!PAGING)
		return;
	putcLst(0xc);
	linesLeft = PAGELEN;
	pageNo++;
	i = num2Asc(pageNo, 3, 10, pnum);
	if (TITLELEN > (j = PAGEWIDTH - 41)) 
		m = j;
	else
		m = TITLELEN;
	putnstrLst(aPlm80Compiler, 20);
	putnstrLst(TITLE, m);
	for (k = j - m + 2; k != 0; k--)
		putcLst(' ');
	putnstrLst(DATE, 9);
	putnstrLst("  PAGE ", 7);
	putnstrLst(pnum, 3);
	putnstrLst("\r\n\n\n", 4);
	skipCnt = 0;
}


void newLineLeaderLst() {
	put2cLst(0xd0a);	// crlf
	col = 0;
	if (--linesLeft == 0)
		newPageLst();
	while (byte_3CFB > col) {
		putcLst(' ');
		col++;
	}
	if (byte_3CFC != 0) {
		putcLst(byte_3CFC);
		col++;
	}
	while (byte_3CFD > col) {
		putcLst(' ');
		col++;
	}
}


void putnstrLst(char *arg1w, word arg2w) {
	while (arg2w != 0) {
		putcLst(*arg1w++);
		arg2w--;
	}
}

