#include "plm.h"


byte cClass[] = {
	CC_NONPRINT, CC_NONPRINT, CC_NONPRINT, CC_NONPRINT,
	CC_NONPRINT, CC_NONPRINT, CC_NONPRINT, CC_NONPRINT,
	CC_NONPRINT, CC_WSPACE, CC_NEWLINE, CC_NONPRINT,
	CC_NONPRINT, CC_WSPACE, CC_NONPRINT, CC_NONPRINT,
	CC_NONPRINT, CC_NONPRINT, CC_NONPRINT, CC_NONPRINT,
	CC_NONPRINT, CC_NONPRINT, CC_NONPRINT, CC_NONPRINT,
	CC_NONPRINT, CC_NONPRINT, CC_NONPRINT, CC_NONPRINT,
	CC_NONPRINT, CC_NONPRINT, CC_NONPRINT, CC_NONPRINT,
	CC_WSPACE, CC_INVALID, CC_INVALID, CC_INVALID,
	CC_DOLLAR, CC_INVALID, CC_INVALID, CC_QUOTE,
	CC_LPAREN,  CC_RPAREN,  CC_STAR,  CC_PLUS,
	CC_COMMA,  CC_MINUS, CC_PERIOD,  CC_SLASH,
	CC_BINDIGIT, CC_BINDIGIT, CC_OCTDIGIT, CC_OCTDIGIT,
	CC_OCTDIGIT, CC_OCTDIGIT, CC_OCTDIGIT, CC_OCTDIGIT,
	CC_DECDIGIT, CC_DECDIGIT, CC_COLON, CC_SEMICOLON,
	CC_LESS, CC_EQUALS, CC_GREATER, CC_INVALID,
	CC_INVALID, CC_HEXCHAR, CC_HEXCHAR, CC_HEXCHAR,
	CC_HEXCHAR, CC_HEXCHAR, CC_HEXCHAR, CC_ALPHA,
	CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_ALPHA,
	CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_ALPHA,
	CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_ALPHA,
	CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_ALPHA,
	CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_INVALID,
	CC_INVALID, CC_INVALID, CC_INVALID, CC_INVALID,
	CC_INVALID, CC_HEXCHAR, CC_HEXCHAR, CC_HEXCHAR,
	CC_HEXCHAR, CC_HEXCHAR, CC_HEXCHAR, CC_ALPHA,
	CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_ALPHA,
	CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_ALPHA,
	CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_ALPHA,
	CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_ALPHA,
	CC_ALPHA, CC_ALPHA, CC_ALPHA, CC_INVALID,
	CC_INVALID, CC_INVALID, CC_INVALID, CC_NONPRINT};

byte tok2oprMap[] = { 
	L_VARIABLE, L_NUMBER, L_STRING, L_PLUSSIGN,
	L_MINUSSIGN, L_STAR, L_SLASH, L_MOD,
	L_PLUS, L_MINUS, L_AND, L_OR,
	L_XOR, L_NOT, 0, L_LT,
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
	L_INVALID, L_INVALID, L_TO, L_INVALID};


/* public variables */
pointer macroPtrs[12]; /* six inChrP, infoP pairs stored as pointer not offset_t */
word macroDepth = 0;
word tokenVal;
pointer inChrP;		/* has to be pointer as it accesses data outside info/symbol space */
word stateStack[100];
word stateIdx;
offset_t stmtLabels[10];
word stmtLabelCnt;
word curProcInfoP;
word ocurch = 0;
word olstch = 0;
word curStmtCnt = 0;
word curBlkCnt = 0;
offset_t curMacroInfoP = 0;
offset_t markedSymbolP = 0;
byte  lineBuf[128];
byte  inbuf[1280];
byte  tokenType;
byte  tokenStr[256];
byte  lastCh;
byte  stmtStartCode;
byte  labelBrkToken;
byte  lblBrkTxiCode;
word doBlkCnt = 0;
byte  tx1Buf[1280];
offset_t labelBrkSymbol;
bool  lineInfoToWrite = false;
bool  isNonCtrlLine = false;
bool  yyAgain = false;
linfo_t linfo = {0, 0};
// byte  curDoBlkCnt, curProcId initial(0, 0);
word_t procData;
wpointer curProcData;
byte  state;
bool  skippingCOND = false;
word ifDepth = 0;
	

void CreateTxi1File()
{
	word tmp;

	tmp = tx1File.curoff;
	OpenF(&tx1File, 3);
	CreatF(&tx1File, tx1Buf, 1280, 2);
	tx1File.curoff = tmp;
} /* CreateTxi1File() */

void WriteTx1(pointer buf, word len)
{
	if (tx1File.aftn == 0 )
		if (tx1File.curoff > 1024 )
			CreateTxi1File();
		else
		{
            memmove(&tx1Buf[tx1File.curoff], buf, len);
			tx1File.curoff = tx1File.curoff + len;
			return;
		}
	Fwrite(&tx1File, buf, len);
} /* WriteTx1() */

void Sub_4119()
{
	if (tx1File.aftn == 0 )
		CreateTxi1File();
	Fflush(&tx1File);
	Rewind(&tx1File);
} /* Sub_4119() */


void WriteLineInfo()
{
	if (lineInfoToWrite )
	{
		WriteTx1((pointer)&linfo, 7);
		lineInfoToWrite = false;
	}
} /* WriteLineInfo() */


void WrBuf(pointer buf, word len)
{
	WriteLineInfo();
	WriteTx1(buf, len);
} /* WrBuf() */

void WrByte(byte v)
{
	WrBuf(&v, 1);
} /* WrByte() */



void WrWord(word v)
{
	WrBuf((pointer)&v, 2);
} /* WrWord() */


void WrOffset(offset_t addr)
{
	WrWord(addr - botInfo);
} /* WrOffset() */ 

void SyntaxError(byte err)
{
	hasErrors = true;
	WrByte(L_SYNTAXERROR);
	WrWord(err);
} /* SyntaxError() */


void TokenErrorAt(byte err)
{
	hasErrors = true;
	WrByte(L_TOKENERROR);
	WrWord(err);
	WrWord(markedSymbolP);
} /* TokenErrorAt() */


void TokenError(byte err, offset_t symP)
{
	hasErrors = true;
	WrByte(L_TOKENERROR);
	WrWord(err);
	WrWord(symP);
} /* TokenError() */

void FatalError_ov0(byte err)
{
    hasErrors = true;
    if (state != 20) {
        if (err == ERR83)	/* LIMIT EXCEEDED: DYNAMIC STORAGE */
            Fatal("DYNAMIC STORAGE OVERFLOW", 24);
        Fatal("UNKNOWN FATAL ERROR", 19);
    }
    TokenErrorAt(err);
    fatalErrorCode = err;
    longjmp(resetPt, -1);
}

void PushBlock(word idAndLevel)
{
	if (blockDepth == 34 )
		FatalError(ERR84);	/* LIMIT EXCEEDED: BLOCK NESTING */
	else
	{
		procChains[blockDepth = blockDepth + 1] = idAndLevel;
		curBlkCnt = curBlkCnt + 1;
	}
} /* PushBlock() */

void PopDo()
{
	if (blockDepth == 0 )
		FatalError(ERR96);	/* COMPILER ERROR: SCOPE STACK UNDERFLOW */
	else
	{
		curBlkCnt = curBlkCnt - 1;
		*curProcData = procChains[blockDepth = blockDepth - 1];
	}
} /* PopDo() */



void WrOprAndValue()
{
	if (tok2oprMap[tokenType] == L_INVALID )
	{
		TokenErrorAt(ERR171);	/* INVALID USE OF DELIMITER or RESERVED WORD IN EXPRESSION */
		return;
	}
	WrByte(tok2oprMap[tokenType]);
	if (tokenType == T_VARIABLE )
		WrWord(curSymbolP);
	else if (tokenType == T_NUMBER )
		WrWord(tokenVal);
	else if (tokenType == T_STRING )
	{
		WrWord(tokenStr[0]);
		WrBuf(&tokenStr[1], tokenStr[0]);
	}
} /* WrOprAndValue() */


void WrXrefUse()
{
	if (XREF )
	{
		WrByte(L_XREFINFO);
		WrOffset(curInfoP);
	}
}


void WrXrefDef()
{
	if (XREF || IXREF || SYMBOLS )
	{
		WrByte(L_XREF);
		WrOffset(curInfoP);
	}
}

