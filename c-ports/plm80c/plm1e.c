#include "plm.h"

byte Sub_5945()
{
    byte i;

    ResetStacks();
    i = 0;
    while (1) {
        if (NotMatchTx1Item(L_IDENTIFIER)) {
            WrTx2ExtError(ERR128);  /* INVALID LEFT-HAND OPERAND OF ASSIGNMENT */
            return 0;
        }
        Sub_45E0();
        PushParseByte(11);
        ExprParse();
        if (Sub_512E(exSP)) {
            WrTx2ExtError(ERR128);  /* INVALID LEFT-HAND OPERAND OF ASSIGNMENT */
            ExprPop();
        } else
            i = i + 1;
        if (NotMatchTx1Item(L_COMMA))
            break;
    }

    if (MatchTx1Item(L_EQ)) {
        PushParseByte(0);
        ExprParse();
        if (i != 0) {
            ExprMakeNode(I_COLONEQUALS, i + 1);
            MoveExpr2Stmt();
            markedStSP = stSP;
            return true;
        } else
            return false;
    } else {
        WrTx2ExtError(ERR116);  /* MISSING '=' IN ASSIGNMENT STATEMENT */
        return false;
    }
}

byte Sub_59D4()
{
    byte i;
    ResetStacks();
    if (NotMatchTx1Item(L_IDENTIFIER)) {
        WrTx2ExtError(ERR117);  /* MISSING procedure NAME IN call STATEMENT */
        return false;
    }
    Sub_467D();
    i = GetType();
    if (i == PROC_T) {
        Sub_50D5();
        if (GetDataType() != 0)
            WrTx2ExtError(ERR129);  /* ILLEGAL 'call' WITH TYPED procedure */
        ExprPush2(I_IDENTIFIER, curInfoP);
        PushParseWord(GetParamCnt());
        PushOperator(0);
        PushParseByte(16);
        if (MatchTx1Item(L_LPAREN)) {
            PushParseByte(15);
            PushParseByte(0);
        }
    } else if (i == BUILTIN_T) {
        if (GetDataType() != 0) {
            WrTx2ExtError(ERR129);  /* ILLEGAL 'call' WITH TYPED procedure */
            return false;
        }
        PushParseWord(builtinsMap[GetBuiltinId()]);
        PushParseWord(GetParamCnt());
        PushOperator(0);
        PushParseByte(17);
        if (MatchTx1Item(L_LPAREN)) {
            PushParseByte(15);
            PushParseByte(0);
        }
    } else {
        if (i != ADDRESS_T || TestInfoFlag(F_ARRAY)) {
            WrTx2ExtError(ERR118);  /* INVALID INDIRECT call, IDENTIFIER not AN address SCALAR */
            return false;
        }
        Sub_4D2C();
        PushOperator(0);
        PushParseByte(18);
        if (MatchTx1Item(L_LPAREN)) {
            PushParseByte(15);
            PushParseByte(0);
        }
    }
    ExprParse();
    MoveExpr2Stmt();
    markedStSP = stSP;
    return true;
}

void Sub_5AD8()
{
    ResetStacks();
    PushParseByte(0);
    ExprParse();
    MoveExpr2Stmt();
    markedStSP = stSP;
}

// lifted to file scope
static word w99BF;
static word w99C1;

static void FlgVisited(word arg1w, word arg2w)
{
    st1Stack[arg1w] = 255;
    st3Stack[arg1w] = arg2w;
}

static void Sub_5BF5(word arg1w)
{
    byte i, j;
    offset_t p;

    p = st3Stack[arg1w];
    if ((i = st1Stack[arg1w]) == I_IDENTIFIER)
        w99C1 = WrTx2Item1Arg(T2_IDENTIFIER, p - botInfo);
    else if (i == I_NUMBER) {
        if (High(p) == 0)
            j = T2_NUMBER;
        else
            j = T2_BIGNUMBER;
        w99C1 = WrTx2Item1Arg(j, p);
    } else if (i == I_INPUT)
        w99C1 = WrTx2Item1Arg(T2_INPUT, p);
    else if (i == I_LENGTH || i == I_LAST || i == I_SIZE)
        w99C1 = WrTx2Item1Arg(icodeToTx2Map[i], p - botInfo);
    else
        w99C1 = WrTx2Item(icodeToTx2Map[i]);
    FlgVisited(arg1w, w99C1);
}

static void StmtParse0()
{
    byte i;

    w99BF = parseStack[parseSP];
    PopParseStack();
    if ((i = st1Stack[w99BF]) == I_OUTPUT)
        return;
    if (st2Stack[w99BF] == 0)
        Sub_5BF5(w99BF);
    else {
        PushParseWord(w99BF);
        if (i == I_CALL)
            PushParseByte(3);
        else if (i == I_CALLVAR) {
            PushParseByte(6);
            PushParseWord(st3Stack[w99BF]);
            PushParseByte(0);
        } else if (i == I_COLONEQUALS)
            PushParseByte(9);
        else if (i == I_MOVE) {
            PushParseByte(14);
            PushParseWord(st3Stack[w99BF]);
            PushParseByte(0);
        } else if (i == I_BYTEINDEX || i == I_WORDINDEX) {
            PushParseByte(8);
            PushParseWord(2);       /* serialise 2 leaves */
            PushParseWord(st3Stack[w99BF]);
            PushParseByte(1);
        } else {
            PushParseByte(13);                  /* post serialisation */
            PushParseWord(st2Stack[w99BF]); /* num args */
            PushParseWord(st3Stack[w99BF]); /* loc of args */
            PushParseByte(1);
        }
    }
}


static void StmtParse1()
{  /* serialise 1 leaf) check */
    w99BF = parseStack[parseSP];
    PushParseByte(2);   /* flag to check for more leaves */
    PushParseWord(w99BF);   /* serialise this leaf */
    PushParseByte(0);
}

static void StmtParse2()
{  /* check for any more leaves */
    word p;

    p = parseSP - 1;
    parseStack[p] = parseStack[p] - 1;
    if (parseStack[p] == 0) {
        PopParseStack();
        PopParseStack();
    } else {
        parseStack[parseSP] = parseStack[parseSP] + 1;
        PushParseByte(1);
    }
}

static void StmtParse3()
{  /* parse args */
    w99BF = parseStack[parseSP];
    PushParseByte(5);   /* final call wrap up */
    if (st2Stack[w99BF] > 1) {   /* any args */
        PushParseWord(st2Stack[w99BF] - 1); /* num args  */
        PushParseWord(st3Stack[w99BF] + 1); /* index of arg info */
        curInfoP = st3Stack[st3Stack[w99BF]];   /* info of procedure */
        AdvNxtInfo();                           /* adv to arginfo */
        PushParseWord(curInfoP);
        PushParseByte(4);
        PushParseWord(st3Stack[w99BF] + 1); /* index of arg info */
        PushParseByte(0);                       /* serialise the arg */
    }
    w99C1 = WrTx2Item(T2_BEGCALL);
}

static void StmtParse4()
{
    word p, q;
    byte i, j;

    q = (p = parseSP - 1) - 1;
    curInfoP = parseStack[parseSP];
    w99BF = parseStack[p];
    i = (byte)parseStack[q];
    if (i > 2) {   /* all bar first 2 args to stack */
        if (curInfoP == 0)
            w99C1 = WrTx2Item1Arg(T2_STKARG, Sub_42EF(st3Stack[w99BF]));
        else {
            if (GetType() == BYTE_T)
                j = T2_STKBARG;
            else
                j = T2_STKWARG;
            w99C1 = WrTx2Item1Arg(j, Sub_42EF(st3Stack[w99BF]));
            AdvNxtInfo();
            parseStack[parseSP] = curInfoP;
        }
        FlgVisited(w99BF, w99C1);
    }
    i = i - 1;
    if (i == 0) {           /* all done, clear working data */
        PopParseStack();
        PopParseStack();
        PopParseStack();
    } else {
        parseStack[q] = i;
        w99BF = w99BF + 1;
        parseStack[p] = w99BF;
        PushParseByte(4);
        PushParseWord(w99BF);   /* serialise the arg */
        PushParseByte(0);
    }
}


static void StmtParse5()
{
    word p, q, r;
    byte i;

    w99BF = parseStack[parseSP];
    PopParseStack();
    r = st3Stack[w99BF];
    i = st2Stack[w99BF] - 1;
    p = q = 0;
    if (i > 1) {
        p = Sub_42EF(st3Stack[r + i - 1]);
        q = Sub_42EF(st3Stack[r + i]);
    } else if (i > 0)
        p = Sub_42EF(st3Stack[r + i]);
    w99C1 = WrTx2Item3Arg(T2_CALL, p, q, st3Stack[r] - botInfo);
    FlgVisited(w99BF, w99C1);
}

static void StmtParse6()
{
    byte i;
    word p;
    w99C1 = WrTx2Item(T2_BEGCALL);
    w99BF = parseStack[parseSP];
    PushParseByte(7);
    if ((i = st2Stack[w99BF] - 1) != 0) {
        PushParseWord(i);
        p = st3Stack[w99BF] + 1;
        PushParseWord(p);
        PushParseWord(0);   /* no arg info */
        PushParseByte(4);
        PushParseWord(p);
        PushParseByte(0);
    }
}

static void StmtParse7()
{
    byte i;
    word p, q, r;

    w99BF = parseStack[parseSP];
    PopParseStack();
    i = st2Stack[w99BF] - 1;
    p = st3Stack[w99BF];
    q = r = 0;
    if (i > 1) {
        q = Sub_42EF(st3Stack[p + i - 1]);
        r = Sub_42EF(st3Stack[p + i]);
    } else if (i > 0)
        q = Sub_42EF(st3Stack[p + i]);
    w99C1 = WrTx2Item3Arg(T2_CALLVAR, q, r, Sub_42EF(st3Stack[p]));
    FlgVisited(w99BF, w99C1);
}

static void StmtParse8()
{
    byte i;
    word p;

    w99BF = parseStack[parseSP];
    PopParseStack();
    i = st1Stack[w99BF];
    p = st3Stack[w99BF];
    w99C1 = WrTx2Item3Arg(icodeToTx2Map[i], Sub_42EF(st3Stack[p]), Sub_42EF(st3Stack[p + 1]), st3Stack[p + 2]);
    FlgVisited(w99BF, w99C1);
}


static void StmtParse9()
{
    w99BF = parseStack[parseSP];
    PushParseByte(10);  /* post serialise LHS */
    PushParseWord(st3Stack[w99BF] + st2Stack[w99BF] - 1);
    PushParseByte(0);
}


static void StmtParse10()
{
    byte i;
    word p;

    w99BF = parseStack[parseSP];
    PushParseByte(12);      /* mark LHS as used at end */
    i = st2Stack[w99BF] - 1;    /* num RHS */
    p = st3Stack[w99BF];        /* base RHS */
    PushParseWord(i);
    PushParseWord(p);
    PushParseWord(p + i);           /* LHS */
    PushParseByte(11);              /* after serialised leaf */
    PushParseWord(p);               /* do the leaf */
    PushParseByte(0);
}


static void StmtParse11()
{     /* do one RHS assignment */
    word p, q;
    byte i;
    word r, s;

    i = (byte)parseStack[q = (p = parseSP - 1) - 1];
    s = parseStack[parseSP];
    if (st1Stack[r = parseStack[p]] == I_OUTPUT) {
        w99C1 = WrTx2Item1Arg(T2_NUMBER, st3Stack[r]);
        w99C1 = WrTx2Item2Arg(T2_OUTPUT, Sub_42EF(w99C1), Sub_42EF(st3Stack[s]));
    } else
        w99C1 = WrTx2Item2Arg(T2_COLONEQUALS, Sub_42EF(st3Stack[r]), Sub_42EF(st3Stack[s]));
    i = i - 1;
    if (i == 0) {       /* all done */
        PopParseStack();
        PopParseStack();
        PopParseStack();
    } else {           /* no so do another */
        parseStack[q] = i;
        r = r + 1;
        parseStack[p] = r;
        PushParseByte(11);  /* state 11 after serialise */
        PushParseWord(r);   /* serialise leaf */
        PushParseByte(0);
    }
}

static void StmtParse12()
{     /* mark LHS as used */
    w99BF = parseStack[parseSP];
    PopParseStack();
    w99C1 = st3Stack[st3Stack[w99BF] + st2Stack[w99BF] - 1];
    FlgVisited(w99BF, w99C1);
}


static void StmtParse13()
{ /* binary or unary op */
    word p;
    byte i;

    w99BF = parseStack[parseSP];
    PopParseStack();
    p = st3Stack[w99BF];
    i = icodeToTx2Map[st1Stack[w99BF]];
    if (st2Stack[w99BF] == 1)
        w99C1 = WrTx2Item1Arg(i, Sub_42EF(st3Stack[p]));
    else
        w99C1 = WrTx2Item2Arg(i, Sub_42EF(st3Stack[p]), Sub_42EF(st3Stack[p + 1]));
    FlgVisited(w99BF, w99C1);
}


static void StmtParse14()
{
    word p;
    p = st3Stack[w99BF = parseStack[parseSP]];
    /* emit the count leaf */
    w99C1 = WrTx2Item1Arg(T2_BEGMOVE, Sub_42EF(st3Stack[p]));
    FlgVisited(p, w99C1);
    PushParseByte(15);      /* Move() post serialise */
    PushParseWord(2);       /* serialise the address leaves */
    PushParseWord(p + 1);
    PushParseByte(1);
}

static void StmtParse15()
{     /* rest of Move() */
    word p;

    w99BF = parseStack[parseSP];
    PopParseStack();
    p = st3Stack[w99BF];
    w99C1 = WrTx2Item3Arg(T2_MOVE, Sub_42EF(st3Stack[p + 1]), Sub_42EF(st3Stack[p + 2]), Sub_42EF(st3Stack[p]));
    FlgVisited(w99BF, w99C1);
}

word StmtParse(word arg1w)
{
    byte i;
    word p;

    p = curInfoP;
    parseSP = 0;
    PushParseWord(arg1w);
    PushParseByte(0);
    while (parseSP != 0) {
        i = (byte)parseStack[parseSP];
        parseSP = parseSP - 1;
        switch (i) {
        case 0: StmtParse0(); break;
        case 1: StmtParse1(); break;
        case 2: StmtParse2(); break;
        case 3: StmtParse3(); break;
        case 4: StmtParse4(); break;
        case 5: StmtParse5(); break;
        case 6: StmtParse6(); break;
        case 7: StmtParse7(); break;
        case 8: StmtParse8(); break;
        case 9: StmtParse9(); break;
        case 10: StmtParse10(); break;
        case 11: StmtParse11(); break;
        case 12: StmtParse12(); break;
        case 13: StmtParse13(); break;
        case 14: StmtParse14(); break;
        case 15: StmtParse15(); break;
        }
    }
    curInfoP = p;
    return st3Stack[arg1w];
}

static byte b99FF[20];
static bool b9A13[20];
static word procInfoStack[20];
static word hNodes[20];
static word eNodes[20];
static word w9A9F[20];
static word w9AC7[20];
static word controlSP;

static void PushScope(word arg1w)
{
    if (blockDepth == 0x22)
        FatalError(ERR164); /* COMPILER Error: SCOPE STACK OVERFLOW */
    else
        procChains[blockDepth = blockDepth + 1] = arg1w;
}

static void PopScope()
{
    if (blockDepth == 0)
        FatalError(ERR165); /* COMPILER Error: SCOPE STACK UNDERFLOW */
    else
        blockDepth = blockDepth - 1;
}

static void PushControl(byte arg1b)
{
    if (controlSP == 0x13)
        FatalError(ERR84);  /*  LIMIT EXCEEDED: BLOCK NESTING */
    else {
        b99FF[controlSP = controlSP + 1] = arg1b;
        b9A13[controlSP] = false;
        procInfoStack[controlSP] = 0;
        hNodes[controlSP] = 0;
        eNodes[controlSP] = 0;
        w9A9F[controlSP] = 0;
        w9A9F[controlSP] = 0;   /* duplicate !! */
    }
}


static void PopControl()
{
    if (controlSP == 0)
        FatalError(ERR167); /* COMPILER Error: CONTROL STACK UNDERFLOW */
    else {
        if (b99FF[controlSP] != 0)
            b9A13[controlSP - 1] = b9A13[controlSP] | b9A13[controlSP - 1];
        controlSP = controlSP - 1;
    }
}

static word NewLocalLabel()
{
    Alloc(3, 3);
    return (localLabelCnt = localLabelCnt + 1);
}


static word Sub_671D(offset_t arg1w)
{
    word p, q;

    curInfoP = arg1w;
    q = WrTx2Item1Arg(T2_IDENTIFIER, arg1w - botInfo);
    if (TestInfoFlag(F_MEMBER)) {
        curInfoP = GetParentOffset();
        p = WrTx2Item1Arg(T2_IDENTIFIER, curInfoP - botInfo);
        q = WrTx2Item2Arg(T2_MEMBER, Sub_42EF(p), Sub_42EF(q));
    }
    return q;
}

static word Sub_677C(offset_t arg1w)
{
    word p, q;
    q = Sub_671D(arg1w);
    if (TestInfoFlag(F_BASED)) {
        p = Sub_671D(GetBaseOffset());
        q = WrTx2Item2Arg(T2_BASED, Sub_42EF(p), Sub_42EF(q));
    }
    return q;
}

static void  ChkEndOfStmt()
{
    if (MatchTx2AuxFlag(128)) {
        WrTx2ExtError(ERR32);   /* INVALID SYNTAX, TEXT IGNORED UNTIL ';' */
        while ((tx1Aux2 & 0x80) != 0) {
            GetTx1Item();
        }
        SetRegetTx1Item();
    }
}


static void Sub_67E3()
{
    word p, q, r, s, t, u, v, w;

    p = procInfoStack[controlSP];
    u = hNodes[controlSP];
    v = eNodes[controlSP];
    if ((w = w9A9F[controlSP]) == 0) {
        curInfoP = p;
        if (GetType() == BYTE_T)
            t = WrTx2Item1Arg(T2_LOCALLABEL, NewLocalLabel());
        q = Sub_677C(p);
        r = Sub_677C(p);
        s = WrTx2Item1Arg(T2_NUMBER, 1);
        if (GetType() == BYTE_T) {
            r = WrTx2Item2Arg(T2_PLUSSIGN, Sub_42EF(r), Sub_42EF(s));
            q = WrTx2Item2Arg(T2_COLONEQUALS, Sub_42EF(q), Sub_42EF(r));
            q = WrTx2Item2Arg(T2_JNZ, u, 3);
        } else {
            r = WrTx2Item2Arg(T2_ADDW, Sub_42EF(r), Sub_42EF(s));
            q = WrTx2Item2Arg(T2_COLONEQUALS, Sub_42EF(q), Sub_42EF(r));
            q = WrTx2Item1Arg(T2_JNC, u);
        }
    } else
        q = WrTx2Item1Arg(T2_JMP, w);
    q = WrTx2Item1Arg(T2_LOCALLABEL, v);
}


static word Sub_6917()
{
    Sub_5AD8();
    return StmtParse(markedStSP);
}


static void Sub_6923()
{
    word p;
    p = Sub_6917();
    ChkEndOfStmt();
    if (MatchTx1Item(L_JMPFALSE))
        p = WrTx2Item2Arg(T2_JMPFALSE, tx1Item.dataw[0], Sub_42EF(p));
    else
        FatalError(ERR168);  /* COMPILER ERROR: BRANCH MISSING IN 'IF' STATEMENT */
}

static void ParseStmtcnt()
{
    t2CntForStmt = 0;
    MapLToT2();
    curStmtNum = tx1Item.dataw[0];
}


static void ParseIf()
{
    Sub_6923();
}

static void ParseProcedure()
{
    tx1Item.dataw[0] = tx1Item.dataw[0] - botInfo;
    MapLToT2();
    PushControl(0);
    procInfoStack[controlSP] = curProcInfoP;
    curProcInfoP = curInfoP = tx1Item.dataw[0] + botInfo;
    SetInfoFlag(F_DECLARED);
}

static void ParseWhile()
{
    word p, q, r;
    PushControl(2);
    p = NewLocalLabel();
    q = NewLocalLabel();
    hNodes[controlSP] = p;
    eNodes[controlSP] = q;
    r = WrTx2Item1Arg(T2_LOCALLABEL, p);
    r = Sub_6917();
    r = WrTx2Item2Arg(T2_JMPFALSE, q, Sub_42EF(r));
    ChkEndOfStmt();
}

static void ParseCASE()
{
    word p, q;
    PushControl(3);
    MapLToT2();
    q = Sub_6917();
    p = NewLocalLabel();
    q = WrTx2Item2Arg(T2_63, p, Sub_42EF(q));
    hNodes[controlSP] = p;
    ChkEndOfStmt();
}

static void ParseDOLOOP()
{
    word p, q, r, s, t, u, v, w;

    PushControl(1);
    if (NotMatchTx1Item(L_IDENTIFIER)) {
        WrTx2Error(ERR138); /* MISSING INDEX VARIABLE */
        return;
    }
    Sub_467D();
    w = curInfoP;
    procInfoStack[controlSP] = w;
    if ((GetType() != BYTE_T && GetType() != ADDRESS_T) || TestInfoFlag(F_ARRAY)) {
        WrTx2ExtError(ERR139);  /* INVALID INDEX VARIABLE TYPE, not byte or address */
        return;
    }
    if (NotMatchTx1Item(L_EQ)) {
        WrTx2ExtError(ERR140);  /* MISSING '=' FOLLOWING INDEX VARIABLE */
        return;
    }
    b99FF[controlSP] = 4;
    p = Sub_677C(w);
    q = Sub_6917();
    p = WrTx2Item2Arg(T2_COLONEQUALS, Sub_42EF(p), Sub_42EF(q));
    s = NewLocalLabel();
    hNodes[controlSP] = s;
    p = WrTx2Item1Arg(T2_LOCALLABEL, s);
    p = Sub_677C(w);

    if (MatchTx1Item(L_TO))
        q = Sub_6917();
    else {
        WrTx2ExtError(ERR141);  /* MISSING 'to' CLAUSE */
        q = WrTx2Item1Arg(T2_NUMBER, 0);
    }

    p = WrTx2Item2Arg(T2_LE, Sub_42EF(p), Sub_42EF(q));
    t = NewLocalLabel();
    eNodes[controlSP] = t;
    p = WrTx2Item2Arg(T2_JMPFALSE, t, Sub_42EF(p));

    if (NotMatchTx1Item(L_BY))
        return;
    v = NewLocalLabel();
    w9AC7[controlSP] = v;
    p = WrTx2Item1Arg(T2_JMP, v);
    u = NewLocalLabel();
    w9A9F[controlSP] = u;
    p = WrTx2Item1Arg(T2_LOCALLABEL, u);
    p = Sub_677C(w);
    q = Sub_677C(w);
    r = Sub_6917();
    q = WrTx2Item2Arg(T2_ADDW, Sub_42EF(q), Sub_42EF(r));
    p = WrTx2Item2Arg(T2_COLONEQUALS, Sub_42EF(p), Sub_42EF(q));
    p = WrTx2Item1Arg(T2_JNC, s);
    p = WrTx2Item1Arg(T2_JMP, t);
    p = WrTx2Item1Arg(T2_LOCALLABEL, v);
}

static void ParseEND()
{
    word p;
    switch (b99FF[controlSP]) {
    case 0:
        curInfoP = curProcInfoP;
        SetInfoFlag(F_DEFINED);
        if (GetDataType() != 0 && !b9A13[controlSP])
            WrTx2Error(ERR156); /* MISSING return STATEMENT IN TYPED procedure */
        p = WrTx2Item(T2_ENDPROC);
        curProcInfoP = procInfoStack[controlSP];
        break;
    case 1: break;
    case 2:
        p = WrTx2Item1Arg(T2_JMP, hNodes[controlSP]);
        p = WrTx2Item1Arg(T2_LOCALLABEL, eNodes[controlSP]);
        break;
    case 3:
        p = WrTx2Item1Arg(T2_LOCALLABEL, hNodes[controlSP]);
        p = WrTx2Item(T2_ENDCASE);
        break;
    case 4: Sub_67E3();
        break;
    }
    PopControl();
    PopScope();
}

static void ParseStatement()
{
    word p;
    if (Sub_5945())
        p = StmtParse(markedStSP);
    ChkEndOfStmt();
}


static void ParseCALL()
{
    word p;
    if (Sub_59D4())
        p = StmtParse(markedStSP);
    ChkEndOfStmt();
}

static void ParseReturn()
{
    word p;
    byte i;

    if (curProcInfoP == 0) {
        WrTx2Error(ERR155); /* INVALID return IN MAIN PROGRAM */
        return;
    }
    curInfoP = curProcInfoP;
    i = GetDataType();
    if (MatchTx2AuxFlag(128)) { /* there is an expression */
        SetRegetTx1Item();
        if (i == 0)
            WrTx2Error(ERR136); /* INVALID return FOR UNTYPED procedure, VALUE ILLEGAL */
        p = Sub_6917();
        if (i == 2)
            p = WrTx2Item1Arg(T2_RETURNBYTE, Sub_42EF(p));
        else
            p = WrTx2Item1Arg(T2_RETURNWORD, Sub_42EF(p));
    } else {
        if (i != 0)
            WrTx2Error(ERR137); /* MISSING VALUE IN return FOR TYPED procedure */
        p = WrTx2Item(T2_RETURN);
    }
    b9A13[controlSP] = true;
    ChkEndOfStmt();
}

static  void ParseEIDIHLT()
{
    MapLToT2();
    ChkEndOfStmt();
}

static void ParseGoto()
{
    word p;
    if (NotMatchTx1Item(L_IDENTIFIER))
        WrTx2Error(ERR142); /* MISSING IDENTIFIER FOLLOWING goto */
    else {
        Sub_45E0();
        if (GetType() != LABEL_T)
            WrTx2ExtError(ERR143);  /* INVALID REFERENCE FOLLOWING goto, not A label */
        else {
            if (High(GetScope()) == 1 && High(procChains[blockDepth]) != 1)
                SetInfoFlag(F_MODGOTO);
            if (High(GetScope()) == 1 || High(GetScope()) == High(procChains[blockDepth])) {
                p = WrTx2Item1Arg(T2_GO_TO, curInfoP - botInfo);
                ChkEndOfStmt();
            } else
                WrTx2ExtError(ERR144); /* INVALID goto label, not at LOCAL or MODULE LEVEL */
        }
    }
}


static void ParseGo()
{
    if (NotMatchTx1Item(L_TO))
        WrTx2Error(ERR145);     /* MISSING 'to' FOLLOWING 'go' */
    ParseGoto();
}

static void ParseAT()
{
    offset_t p;
    p = tx1Item.dataw[0];

    GetTx1Item();
    GetRestrictedExpr();
    WrAtFileByte(ATI_AHDR);
    WrAtFileWord(p - botInfo);
    WrAtFileWord(curStmtNum);
    WrAtFileWord(var.infoOffset);
    WrAtFileWord(var.arrayIndex);
    WrAtFileWord(var.nestedArrayIndex);
    WrAtFileWord(var.val);
    ExpectRparen(ERR146);   /* MISSING ') ' AFTER 'AT' RESTRICTED EXPRESSION */
}

void ParseDATA_INITIAL()
{
    word p, q;
    p = tx1Item.dataw[0];
    GetTx1Item();
    q = ParseDataItems(p);
    if ((curInfoP = p) != botInfo)
        if (TestInfoFlag(F_STARDIM))
            SetDimension(q);
}

void ParseLexItems()
{


    controlSP = 0;
    GetTx1Item();
    while (tx1Item.type != L_EOF) {
        curInfoP = 0;
        switch (lexHandlerIdxTable[tx1Item.type]) {
        case 0: ParseStmtcnt(); break;     /* L_STMTCNT */
        case 1: PushScope(tx1Item.dataw[0]); break;  /* L_SCOPE */
        case 2: ParseEND(); break; /* L_END */
        case 3: ParseIf(); break;  /* L_IF */
        case 4:
            ParseDOLOOP();      /* L_DOLOOP */
            ChkEndOfStmt();
            break;
        case 5: ParseWhile(); break;   /* L_WHILE */
        case 6: PushControl(1); break; /* L_DO */
        case 7: ParseProcedure(); break;   /* L_PROCEDURE */
        case 8: ParseCASE(); break;    /* L_CASE */
        case 9: ParseStatement(); break;   /* L_STATEMENT */
        case 10: ParseCALL(); break;    /* L_CALL */
        case 11: ParseReturn(); break;  /* L_RETURN */
        case 12: ParseGoto(); break;    /* L_GOTO */
        case 13: ParseGo(); break;  /* L_GO */
        case 14: break;           /* L_SEMICOLON */
        case 15: ParseEIDIHLT(); break; /* L_ENABLE, L_DISABLE, L_HALT */
        case 16: ParseAT(); break;  /* L_AT */
        case 17: ParseDATA_INITIAL(); break;    /* L_DATA, L_INITIAL */
        case 18: break;       /* simple items */
        case 19: MapLToT2(); break; /* L_LABELDEF, L_LOCALLABEL, L_JMP, L_JMPFALSE, L_CASELABEL */
        case 20:
            curInfoP = tx1Item.dataw[0];    /* L_EXTERNAL */
            SetInfoFlag(F_DECLARED);
            SetInfoFlag(F_DEFINED);
            break;
        }
        GetTx1Item();
    }
}
