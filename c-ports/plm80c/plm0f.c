#include "plm.h"

static byte brkTxiCodes[] = {
    L_STATEMENT, L_SEMICOLON, L_CALL, L_LINEINFO,
    L_DISABLE, L_DO, L_ENABLE, L_END,
    L_GO, L_GOTO, L_HALT, L_IF,
    L_PROCEDURE, L_RETURN};


word curState;
bool endSeen;

static void SkipToSemiColon()
{
    while (tokenType != T_SEMICOLON)
        Yylex();
    yyAgain = false;
}

static void ExpectSemiColon()
{
    if (YylexNotMatch(T_SEMICOLON)) {
        TokenErrorAt(ERR32);    /* INVALID SYNTAX, TEXT IGNORED UNTIL ';' */
        SkipToSemiColon();
    }
}

static void ErrorSkip()
{
    SkipToSemiColon();
    SetYyAgain();
}

static void PopStateWord(wpointer stateP)
{
    if (stateIdx == 0)
        FatalError(ERR97);      /* COMPILER Error: PARSE STACK UNDERFLOW */
    *stateP = stateStack[stateIdx];
    stateIdx = stateIdx - 1;
}

static void GenLocalLabel()
{
    Alloc(3, 3);
    localLabelCnt = localLabelCnt + 1;
}

/*
    collect labels (max 9)
    looks for Variable() ':'
    sybmol references stored in stmtLabels array
    count is in stmtLabelCnt
*/
static void ParseStmtLabels()
{
    stmtLabelCnt = 0;
    
    while (1) {
        labelBrkToken = tokenType;
        labelBrkSymbol = curSymbolP;
        if (tokenType != T_VARIABLE)
            return;
        
        if (YylexMatch(T_COLON)) {
            if (stmtLabelCnt == 9)
                TokenErrorAt(ERR30);    /* LIMIT EXCEEDED: NUMBER OF LABELS ON STATEMENT */
                    /* LIMIT EXCEEDED: NUMBER OF LABELS ON STATEMENT */
            else
                stmtLabelCnt = stmtLabelCnt + 1;
            stmtLabels[stmtLabelCnt] = curSymbolP;
        }
        else
            return;
        Yylex();
    }
}

static void ParseStartStmt()
{
    bool tmp;
    
    if (endSeen) {
        endSeen = false;
        return;
    }
    tmp = XREF;
    XREF = false;
    Yylex();
    XREF = tmp;
    curStmtCnt = curStmtCnt + 1;
    if (linfo.stmtCnt == 0) {
        linfo.stmtCnt = curStmtCnt;
        linfo.blkCnt = curBlkCnt;
    }
    WrByte(L_STMTCNT);
    WrWord(curStmtCnt);
    if (trunc) {
        SyntaxError(ERR86); /* LIMIT EXCEEDED: SOURCE LINE length() */
        trunc = false;
    }
    ParseStmtLabels();
    if (labelBrkToken == T_SEMICOLON) {
        stmtStartCode = 1;
        SetYyAgain();
    }
    else if (labelBrkToken == T_VARIABLE)
        stmtStartCode = 0;
    else if (labelBrkToken >= T_CALL && labelBrkToken <= T_RETURN)
        stmtStartCode = labelBrkToken - T_CALL + 2;
    else {
        TokenErrorAt(ERR29);    /* ILLEGAL STATEMENT */
        stmtStartCode = 1;
        ErrorSkip();
    }
    lblBrkTxiCode = brkTxiCodes[stmtStartCode];
}

static void WrLabelDefs()
{
   word i;
   offset_t tmp;

    tmp = curSymbolP;
    if (stmtLabelCnt != 0) {
        for (i = 1; i <= stmtLabelCnt; i++) {
            curSymbolP = stmtLabels[i];
            FindScopedInfo(*curProcData);
            if (curInfoP != 0) {    /* already seen at this scope */
                if (TestInfoFlag(F_LABEL))
                    TokenError(ERR33, curSymbolP);  /* DUPLICATE label DECLARATION */
                        /* DUPLICATE label DECLARATION */
                else {
                    WrByte(L_LABELDEF);
                    WrOffset(curInfoP);
                    SetInfoFlag(F_LABEL);
                }
                WrXrefUse();
            } else {
                CreateInfo(*curProcData, LABEL_T);
                WrXrefDef();
                WrByte(L_LABELDEF);
                WrOffset(curInfoP);
                SetInfoFlag(F_LABEL);
            }
        }
        if (*curProcData == 0x100)
            WrByte(L_MODULE);
    }
    curSymbolP = tmp;
} /* WrLabelDefs() */

static bool Sub_723A()
{
    if (YylexMatch(T_VARIABLE)) {
        curInfoP = procInfo[1];
        if (GetSymbol() != curSymbolP)
            TokenErrorAt(ERR20);    /* MISMATCHED IDENTIFIER at end OF BLOCK */
    }
    ExpectSemiColon();
    if (unexpectedEOF) {
        WrByte(L_END);
        return true;
    } else {
        Yylex();
        if (unexpectedEOF) {
            WrByte(L_END);
            unexpectedEOF = false;
            return true;
        } else {
            SyntaxError(ERR93); /* MISSING 'do' FOR 'end' , 'end' IGNORED */
            SetYyAgain();
            return false;
        }
    }
} /* Sub_723A() */

static void Sub_7296()
{
    byte i, v;
	word tmp;

    PopDo();
    if (YylexMatch(T_VARIABLE)) {
        curInfoP = curProcInfoP;
        if (GetSymbol() != curSymbolP)
            TokenErrorAt(ERR20);    /* MISMATCHED IDENTIFIER at end OF BLOCK */
    }
    curInfoP = curProcInfoP;
    v = GetParamCnt();
    for (i = 1; i <= v; i++) {
        AdvNxtInfo();
        if (! TestInfoFlag(F_LABEL))
            TokenError(ERR25, GetSymbol());   /* UNDECLARED PARAMETER */
    }
    PopStateWord(&doBlkCnt);
	
    PopStateWord(&tmp);
	curProcInfoP = tmp;
    ExpectSemiColon();
}

static void PushStateWord(word v)
{
    if (stateIdx != 0x63)
        stateStack[stateIdx = stateIdx + 1] = v;
    else
        FatalError(ERR31);      /* LIMIT EXCEEDED: PROGRAM TOO COMPLEX */
}


static void PushStateByte(byte state)
{
    PushStateWord(state);
}


static void CreateModuleInfo(offset_t symPtr)
{

    curSymbolP = symPtr;
    CreateInfo(0, PROC_T);
    SetInfoFlag(F_LABEL);
    WrXrefDef();
    curProcInfoP = curInfoP;
    SetProcId(1);
    procCnt = 1;
    procInfo[1] = curInfoP;
    *curProcData = 0x100;           /* proc = 1,  do level = 0 */
    WrByte(L_DO);
    WrByte(L_SCOPE);
    WrWord(*curProcData);
    PushBlock(*curProcData);
}


static void State0()
{
    ParseStartStmt();
    PushStateByte(1);
    if (stmtStartCode != (T_DO - T_CALL + 2))
    {
        SyntaxError(ERR89);     /* MISSING 'do' FOR MODULE */
        Lookup("\x6MODULE");
        CreateModuleInfo(curSymbolP);
        PushStateByte(19);
    } else {
        if (stmtLabelCnt == 0) {
            SyntaxError(ERR90); /* MISSING NAME FOR MODULE */
            Lookup("\x6MODULE");
            stmtLabelCnt = 1;
            stmtLabels[1] = curSymbolP;
        }
        else if (stmtLabelCnt > 1)
            SyntaxError(ERR18); /* INVALID MULTIPLE LABELS AS MODULE NAMES */
        CreateModuleInfo(stmtLabels[1]);
        ExpectSemiColon();
        PushStateByte(3);
    }
} /* State0() */



static void State1()
{
    if (stmtStartCode != (T_END - T_CALL + 2)) {
        haveModule = true;
        WrByte(L_MODULE);
        PushStateByte(2);
    } else {
        if (stmtLabelCnt != 0)
            TokenErrorAt(ERR19);    /* INVALID label IN MODULE WITHOUT MAIN PROGRAM */
        if (! Sub_723A()) {
            PushStateByte(1);
            PushStateByte(10);
        }
    }
} /* State1() */


static void State2()
{
    if (stmtStartCode != (T_END - T_CALL + 2)) {
        PushStateByte(2);
        PushStateByte(10);
        PushStateByte(11);
    } else {
        WrLabelDefs();
        if (Sub_723A())
            WrByte(L_HALT);
        else {
            PushStateByte(2);
            PushStateByte(10);
        }
    }
} /* State2() */


static void State3()
{
    ParseStartStmt();
    PushStateByte(19);
}


static void State19()
{   /* check for declare or procedure */
    if (stmtStartCode == (T_DECLARE - T_CALL + 2)) {
        ParseDcl();
        PushStateByte(3);
        ExpectSemiColon();
    } else if (stmtStartCode == (T_PROCEDURE - T_CALL + 2)) {
        PushStateByte(3);
        PushStateByte(4);
    }
}

static void State4()
{
    if (stmtLabelCnt == 0) {
        SyntaxError(ERR21); /* MISSING procedure NAME */
        PushStateByte(7);
    } else {
        if (stmtLabelCnt != 1) {
            TokenErrorAt(ERR22);    /* INVALID MULTIPLE LABELS AS procedure NAMES */
            stmtLabelCnt = 1;
        }
        PushStateWord(curProcInfoP);
        PushStateWord(doBlkCnt);
        ParseProcDcl();
        ExpectSemiColon();
        curInfoP = curProcInfoP;
        if (TestInfoFlag(F_EXTERNAL))
            PushStateByte(5);
        else {
            PushStateByte(6);
            PushStateByte(21);
            PushStateByte(3);
        }
    }
}


static void State5()
{
    ParseStartStmt();
    if (stmtStartCode == (T_END - T_CALL + 2)) {
        if (stmtLabelCnt != 0) {
            TokenErrorAt(ERR23);    /* INVALID LABELLED end IN external procedure */
            stmtLabelCnt = 0;
        }
        Sub_7296();
    } else {
        PushStateByte(5);
        if (stmtStartCode == (T_DECLARE - T_CALL + 2)) {
            ParseDcl();
            ExpectSemiColon();
        } else {
            TokenErrorAt(ERR24);    /* INVALID STATEMENT IN external procedure */
            if (stmtStartCode == (T_PROCEDURE - T_CALL + 2) ||
               stmtStartCode == (T_DO - T_CALL + 2))
                PushStateByte(7);
            else
                SkipToSemiColon();
        }
    }
}


static void State21()
{
    if (stmtStartCode == (T_END - T_CALL + 2))
        TokenErrorAt(ERR174);   /* INVALID NULL procedure */
    else
        PushStateByte(9);
}


static void State6()
{
    WrLabelDefs();
    Sub_7296();
    WrByte(L_END);
}


/* states 7 & 8 skip to end of block, handling nested blocks */
static void State7()
{
    SkipToSemiColon();
    PushStateByte(8);
}


static void State8()
{
    ParseStartStmt();
    if (stmtStartCode == (T_PROCEDURE - T_CALL + 2) ||  /* nested block */
       stmtStartCode == (T_DO - T_CALL + 2)) {
        PushStateByte(8);
        PushStateByte(7);               /* proc nested block */
    } else {
        SkipToSemiColon();              /* skip to end of statement */
        if (stmtStartCode != (T_END - T_CALL + 2))  /* if ! an end then go again */
            PushStateByte(8);
    }
}

static void State9()
{
    if (stmtStartCode != (T_END - T_CALL + 2)) {
        PushStateByte(9);
        PushStateByte(10);
        PushStateByte(11);
    }
}


static void State10()
{
    ParseStartStmt();
}

static void State11()
{
    if (stmtStartCode == (T_DECLARE - T_CALL + 2) ||
        stmtStartCode == (T_PROCEDURE - T_CALL + 2) ||
        stmtStartCode == (T_END - T_CALL + 2)) {

        TokenErrorAt(ERR26);    /* INVALID DECLARATION, STATEMENT OUT OF PLACE */
        PushStateByte(20);
    } else if (stmtStartCode == (T_DO - T_CALL + 2))
        PushStateByte(12);
    else if (stmtStartCode == (T_IF - T_CALL + 2))
        PushStateByte(16);
    else {
        WrLabelDefs();
        WrByte(lblBrkTxiCode);
        if (stmtStartCode != 1) {   /* Semicolon() */
            if (stmtStartCode == 0) {   /* Variable() */
                WrByte(L_VARIABLE);
                WrWord(labelBrkSymbol);
            }
            ParseExpresion(T_SEMICOLON);
        }
        ExpectSemiColon();
    }
}


static void State20()
{
    if (stmtStartCode == (T_DECLARE - T_CALL + 2)) {
        ParseDcl();
        ExpectSemiColon();
        PushStateByte(20);
        PushStateByte(10);
    } else if (stmtStartCode == (T_PROCEDURE - T_CALL + 2)) {
        PushStateByte(20);
        PushStateByte(10);
        PushStateByte(4);
    } else if (stmtStartCode == (T_END - T_CALL + 2))
        endSeen = true;
    else
        PushStateByte(11);
}


static void State12()
{   /* process do */
    WrLabelDefs();
    if (stmtLabelCnt != 0)
        PushStateWord(stmtLabels[stmtLabelCnt]);
    else
        PushStateWord(0);
    if (YylexMatch(T_VARIABLE)) {
        WrByte(L_DOLOOP);   /* start of do loop */
        WrOprAndValue();
        ParseExpresion(T_SEMICOLON);
        PushStateByte(13);
        PushStateByte(9);
        PushStateByte(10);
    } else if (YylexMatch(T_WHILE)) {
        WrByte(L_WHILE);    /* start of while */
        ParseExpresion(T_SEMICOLON);
        PushStateByte(13);
        PushStateByte(9);
        PushStateByte(10);
    } else if (YylexMatch(T_CASE)) {
        WrByte(L_CASE); /* start of case */
        ParseExpresion(T_SEMICOLON);
        GenLocalLabel();
        PushStateWord(localLabelCnt);
        PushStateByte(14);
    } else {
        WrByte(L_DO);   /* simple do end block */
        PushStateByte(13);
        PushStateByte(9);
        PushStateByte(3);
    }
    ExpectSemiColon();      /* we should now see a semicolon */
    if (doBlkCnt >= 255)
        SyntaxError(ERR27); /* LIMIT EXCEEDED: NUMBER OF do BLOCKS */
    else
        doBlkCnt = doBlkCnt + 1;

    procData.lb = (byte)doBlkCnt;
    PushBlock(*curProcData);
    WrByte(L_SCOPE);
    WrWord(*curProcData);
}



static void State13()
{
    offset_t labelPtr;

    WrLabelDefs();
    PopDo();
    PopStateWord(&labelPtr);
    if (YylexMatch(T_VARIABLE))
        if (curSymbolP != labelPtr)
            TokenErrorAt(ERR20);    /* MISMATCHED IDENTIFIER at end OF BLOCK */
    WrByte(L_END);
    ExpectSemiColon();
}


static void State14()
{   /* process CASE statements */
	offset_t labelPtr;
    word stateWord;
    
    ParseStartStmt();
    if (stmtStartCode != (T_END - T_CALL + 2))
    {
        GenLocalLabel();
        WrByte(L_CASELABEL);
        WrWord(localLabelCnt);
        PushStateByte(15);
        PushStateByte(11);
    }
    else
    {
        PopStateWord(&stateWord);   /* get the head jump */
        WrLabelDefs();
        if (stmtLabelCnt != 0) {
            WrByte(L_JMP);
            WrWord(stateWord);
        }
        PopDo();
        PopStateWord(&labelPtr);
        WrByte(L_END);
        WrByte(L_LOCALLABEL);
        WrWord(stateWord);
        /* check end label if (present */
        if (YylexMatch(T_VARIABLE))
            if (curSymbolP != labelPtr)
                SyntaxError(ERR20); /* MISMATCHED IDENTIFIER at end OF BLOCK */
        ExpectSemiColon();
    }
}


static void State15()
{
    word stateWord;
    
    PopStateWord(&stateWord);
    WrByte(L_JMP);
    WrWord(stateWord);
    PushStateWord(stateWord);
    PushStateByte(14);
}


static void State16()
{   /* process if (condition) */
    WrLabelDefs();
    WrByte(L_IF);
    ParseExpresion(T_THEN);
    if (YylexNotMatch(T_THEN)) {
        TokenErrorAt(ERR28);    /* MISSING 'then' */
        WrByte(L_JMPFALSE);
        WrWord(0);
    } else {
        GenLocalLabel();
        PushStateWord(localLabelCnt);
        WrByte(L_JMPFALSE);
        WrWord(localLabelCnt);
        PushStateByte(17);
        PushStateByte(11);
        PushStateByte(10);
    }
}


static void State17()
{   /* process optional else */
    word stateWord;
    byte tmp;
    
    PopStateWord(&stateWord);   /* labelref for if (condition false */
    Yylex();
    tmp = lineInfoToWrite;          /* supress line info for labeldefs etc */
    lineInfoToWrite = false;
    if (tokenType == T_ELSE) {
        GenLocalLabel();
        WrByte(L_JMP);
        WrWord(localLabelCnt);  /* jump round else labelref */
        PushStateWord(localLabelCnt);   /* save labelref for end of else statement */
        PushStateByte(18);
        PushStateByte(11);
        PushStateByte(10);
    } else
        SetYyAgain();

    WrByte(L_LOCALLABEL);   /* emit label for if (condition false */
    WrWord(stateWord);
    lineInfoToWrite = tmp;
}



static void State18()
{       /* end of else */
    word stateWord;
    byte tmp;

    tmp = lineInfoToWrite;              /* supress line info for labeldefs */
    
    lineInfoToWrite = false;
    PopStateWord(&stateWord);       /* labelref for end of else */
    WrByte(L_LOCALLABEL);   /* emit label */
    WrWord(stateWord);
    lineInfoToWrite = tmp;
}

void Sub_6F00()
{
    stateIdx = 0;
    endSeen = false;
    PushStateByte(0);
    while (stateIdx != 0) {
        curState = stateStack[stateIdx];
        stateIdx = stateIdx - 1;
        switch (curState) {
            case 0: State0(); break;
            case 1: State1(); break;
            case 2: State2(); break;
            case 3: State3(); break;
            case 4: State4(); break;
            case 5: State5(); break;
            case 6: State6(); break;
            case 7: State7(); break;
            case 8: State8(); break;
            case 9: State9(); break;
            case 10: State10(); break;
            case 11: State11(); break;
            case 12: State12(); break;
            case 13: State13(); break;
            case 14: State14(); break;
            case 15: State15(); break;
            case 16: State16(); break;
            case 17: State17(); break;
            case 18: State18(); break;
            case 19: State19(); break;
            case 20: State20(); break;
            case 21: State21(); break;
        }
    }
}
