#include "plm.h"

word rValue;
bool use8bit;

static void SimpleUExpr()
{
    if (MatchTx1Item(L_NUMBER))
        rValue = tx1Item.dataw[0];
    else {
        if (MatchTx1Item(L_IDENTIFIER))
            Sub_45E0();
        WrTx2ExtError(151); /* INVALID OPERAND IN RESTRICTED EXPRESSION */
        rValue = 0;
    }
    use8bit = use8bit && (High(rValue) == 0);
}

static void SimpleNExpr()
{
    if (MatchTx1Item(L_MINUSSIGN)) {
        SimpleUExpr();
        if (use8bit) 
            rValue = - Low(rValue);
        else
            rValue = - rValue;
    } else
        SimpleUExpr();
}

static void EvalSimpleExpr()
{
    word p;

    SimpleNExpr();
    p = rValue;
    while (1) {
        if (MatchTx1Item(L_PLUSSIGN)) {
            SimpleNExpr();
            if (use8bit)
                p = (p = p + rValue) & 0xff;
            else 
                p =  p + rValue;
        } else if (MatchTx1Item(L_MINUSSIGN)) {
            SimpleNExpr();
            if (use8bit)
                p = (p = p - rValue) & 0xff;
            else
                p =  p - rValue;
        } else
            break;
    }
    rValue = p;
}


static void GetRestrictedArrayIndex()
{
    use8bit = true;
    if (!(TestInfoFlag(F_ARRAY)))
        WrTx2ExtError(149); /* INVALID SUBSCRIPTING IN RESTRICTED REFERENCE */
    EvalSimpleExpr();
    ExpectRparen(150);  /* MISSING ') ' at end OF RESTRICTED SUBSCRIPT */
}

static void GetRestrictedVar()
{
    if (NotMatchTx1Item(L_IDENTIFIER)) {
        WrTx2Error(ERR147);    /* MISSING IDENTIFIER FOLLOWING DOT OPERATOR */
        RecoverRPOrEndExpr();
        return;
    }
    Sub_45E0();
    var.infoOffset = curInfoP;
    if (GetType() == BUILTIN_T) {
        WrTx2Error(ERR123);    /* INVALID DOT OPERAND, BUILT-IN PROCEDURE ILLEGAL */
        RecoverRPOrEndExpr();
        return;
    }
    if (MatchTx1Item(L_LPAREN)) {
        GetRestrictedArrayIndex();
        var.arrayIndex = rValue;
    }
    if (MatchTx1Item(L_PERIOD))
    {
        curInfoP = var.infoOffset;
        if (GetType() != STRUCT_T) {
            WrTx2ExtError(ERR148); /* INVALID QUALIFICATION IN RESTRICTED REFERENCE */
            RecoverRPOrEndExpr();
            return;
        }
        if (NotMatchTx1Item(L_IDENTIFIER)) {
            WrTx2Error(ERR147);    /* MISSING IDENTIFIER FOLLOWING DOT OPERATOR */
            var.infoOffset = var.infoOffset - botInfo;
            return;
        }
        Sub_4631();
        var.infoOffset = curInfoP;
        if (MatchTx1Item(L_LPAREN)) {
            GetRestrictedArrayIndex();
            var.nestedArrayIndex = rValue;
        }
    }
    var.infoOffset = var.infoOffset - botInfo;
}

void GetRestrictedExpr()
{
    var.infoOffset = var.arrayIndex = var.nestedArrayIndex = var.val = 0;
    if (MatchTx1Item(L_PERIOD)) {       // <restricted reference>
        GetRestrictedVar();
        if (MatchTx1Item(L_PLUSSIGN))
            ;
        else if (MatchTx1Item(L_MINUSSIGN))
            SetRegetTx1Item();
        else
            return;
        use8bit = false;
        EvalSimpleExpr();
        var.val = rValue;
    }
    else if (MatchTx1Item(L_RPAREN))
        SetRegetTx1Item();
    else {
        use8bit = true;
        EvalSimpleExpr();
        var.val = rValue;
    }
}

word ParseDataItems(offset_t arg1w)
{
    word p;

    p = 0;
    WrAtFileByte(ATI_DHDR);
    WrAtFileWord(arg1w - botInfo);
    WrAtFileWord(curStmtNum);
    while (1) {
        if (MatchTx1Item(L_STRING)) {
            WrAtFileByte(ATI_STRING);
            WrAtFileWord(tx1Item.dataw[0]);
            WrAtFile((pointer)&tx1Item.dataw[1], tx1Item.dataw[0]);
            curInfoP = arg1w;
            if (GetType() == ADDRESS_T) 
                p = p + (tx1Item.dataw[0] + 1) / 2;
            else 
                p = p + tx1Item.dataw[0];
        } else {
            GetRestrictedExpr();
            WrAtFileByte(ATI_DATA);
            WrAtFile((pointer)&var, 8);
            p = p + 1;
        }
        if (NotMatchTx1Item(L_COMMA))
            break;
        else if (MatchTx1Item(L_RPAREN)) {
            WrTx2Error(151);    /* INVALID OPERAND IN RESTRICTED EXPRESSION */
            SetRegetTx1Item();
            break;
        }
    }
    WrAtFileByte(ATI_END);
    ExpectRparen(152);  /* MISSING ') ' AFTER CONSTANT LIST */
    curInfoP = arg1w;;
    return p;
}

void ResetStacks()
{
    parseSP = exSP = operatorSP = stSP = 0;
}

void PushParseWord(word arg1w)
{
    if (parseSP == 0x63)
        FatalError(119);    /* LIMIT EXCEEDED: PROGRAM TOO COMPLEX */
    parseStack[parseSP = parseSP + 1] = arg1w;
}


void PopParseStack()
{
    if (parseSP == 0)
        FatalError(159);    /* COMPILER Error: PARSE STACK UNDERFLOW */
    parseSP = parseSP - 1;
}


void PushParseByte(byte arg1b)
{
    PushParseWord(arg1b);
}

static void ExprPush3(byte arg1b, byte arg2b, word arg3w)
{
    if (exSP == 0x63)
        FatalError(121);    /* LIMIT EXCEEDED: EXPRESSION TOO COMPLEX */
    ex1Stack[exSP = exSP + 1] = arg1b;
    ex2Stack[exSP] = arg2b;
    ex3Stack[exSP] = arg3w;
}

void ExprPop()
{
    if (exSP == 0)
        FatalError(160);    /* COMPILER Error: OPERAND STACK UNDERFLOW */
    exSP = exSP - 1;
}


static void SwapOperandStack()
{
    word i;
    byte op1, op2;
    word op3;

    if (exSP < 2)
        FatalError(161);    /* COMPILER Error: ILLEGAL OPERAND STACK EXCHANGE */
    i = exSP - 1;
    op1 = ex1Stack[exSP];
    op2 = ex2Stack[exSP];
    op3 = ex3Stack[exSP];
    ex1Stack[exSP] = ex1Stack[i];
    ex2Stack[exSP] = ex2Stack[i];
    ex3Stack[exSP] = ex3Stack[i];
    ex1Stack[i] = op1;
    ex2Stack[i] = op2;
    ex3Stack[i] = op3;
}

void ExprPush2(byte arg1b, word arg2w)
{
    ExprPush3(arg1b, 0, arg2w);
}

static void StmtPush3(byte arg1b, byte arg2b, word arg3w)
{
    if (stSP == 299)
        FatalError(122);    /* LIMIT EXCEEDED: PROGRAM TOO COMPLEX   */
    st1Stack[stSP = stSP + 1] = arg1b;
    st2Stack[stSP] = arg2b;
    st3Stack[stSP] = arg3w;
}

void MoveExpr2Stmt()
{
    StmtPush3(ex1Stack[exSP], ex2Stack[exSP], ex3Stack[exSP]);
    ExprPop();
}

void PushOperator(byte arg1b)
{
    if (operatorSP == 0x31)
        FatalError(120);    /* LIMIT EXCEEDED: EXPRESSION TOO COMPLEX */
    operatorStack[operatorSP = operatorSP + 1] = arg1b;
}


void PopOperatorStack()
{
    if (operatorSP == 0)
        FatalError(162);    /* COMPILER Error: OPERATOR STACK UNDERFLOW */
    operatorSP = operatorSP - 1;
}


void ExprMakeNode(byte arg1b, byte arg2b)
{
    word w;
    byte i, j;

    w = stSP + 1;

    if (exSP < arg2b)
        FatalError(163);    /* COMPILER Error: GENERATION FAILURE  */
    j = exSP - arg2b + 1;
    i = arg2b;
    while (i != 0) {
        StmtPush3(ex1Stack[j], ex2Stack[j], ex3Stack[j]);
        i = i - 1;
        j = j + 1;
    }

    exSP = exSP - arg2b;
    if (arg2b == 0)
        w = 0;
    ExprPush3(arg1b, arg2b, w);
}

void AcceptOpAndArgs()
{
    byte i;

    i = (byte)operatorStack[operatorSP];
    if (i == I_NOT || i == I_UNARYMINUS)
        ExprMakeNode(i, 1);
    else
        ExprMakeNode(i, 2);
    PopOperatorStack();
}


static void Sub_4CC2()
{
    word p;

    if (TestInfoFlag(F_MEMBER)) {
        p = curInfoP;
        curInfoP = GetParentOffset();
        ExprPush2(I_IDENTIFIER, curInfoP);
        ExprPush2(I_IDENTIFIER, p);
        ExprMakeNode(I_MEMBER, 2);
    }
    else
        ExprPush2(I_IDENTIFIER, curInfoP);
}

void Sub_4CFD(offset_t arg1w)
{
    curInfoP = arg1w;
    if (TestInfoFlag(F_BASED)) {
        curInfoP = GetBaseOffset();
        Sub_4CC2();
        curInfoP = arg1w;
        SwapOperandStack();
        ExprMakeNode(I_BASED, 2);
    }
}

void Sub_4D2C()
{
    Sub_4CC2();
    Sub_4CFD(curInfoP);
}


void Sub_4D38()
{
    if (GetDataType() == 0)
        WrTx2ExtError(131); /* ILLEGAL REFERENCE to UNTYPED procedure */
}


byte GetCallArgCnt()
{
    byte i, j, k;
    i = (byte)parseStack[parseSP];
    j = (byte)operatorStack[operatorSP];
    PopParseStack();
    PopOperatorStack();
    if (i == j)
        return i;
    if (i < j) {
        WrTx2ExtError(153); /* INVALID NUMBER OF ARGUMENTS IN call, TOO MANY */
        k = j - i;
        while (k != 0) {
            ExprPop();
            k = k - 1;
        }
    } else {
        WrTx2ExtError(154); /* INVALID NUMBER OF ARGUMENTS IN call, TOO FEW */
        k = i - j;
        while (k != 0) {
            ExprPush2(I_NUMBER, 0);
            k = k - 1;
        }
    }
    return i;
}

void Sub_4DCF(byte arg1b)
{
    if (NotMatchTx1Item(L_LPAREN)) {
        WrTx2ExtError(124); /* MISSING ARGUMENTS FOR BUILT-IN procedure */
        ExprPush2(I_NUMBER, 0);
    } else {
        if (NotMatchTx1Item(L_IDENTIFIER)) {
            WrTx2Error(125);    /* ILLEGAL ARGUMENT FOR BUILT-IN procedure */
            ExprPush2(I_NUMBER, 0);
        } else {
            Sub_45E0();
            if (MatchTx1Item(L_LPAREN)) {
                if (TestInfoFlag(F_ARRAY)) {
                    ResyncRparen();
                    if (MatchTx1Item(L_RPAREN))
                    {
                        if (arg1b == I_LENGTH || arg1b == I_LAST)
                        {
                            WrTx2ExtError(125); /*  ILLEGAL ARGUMENT FOR BUILT-IN procedure */
                            ExprPush2(I_NUMBER, 0);
                        }
                        else 
                            ExprPush2(arg1b, curInfoP);
                        return;
                    }
                } else 
                    WrTx2ExtError(127); /* INVALID SUBSCRIPT ON NON-ARRAY */
            } 
            if (MatchTx1Item(L_PERIOD)) {
                if (GetType() != STRUCT_T)
                    WrTx2ExtError(110); /* INVALID LEFT OPERAND OF QUALIFICATION, not A structure */
                else if (NotMatchTx1Item(L_IDENTIFIER))
                    WrTx2ExtError(111); /* INVALID RIGHT OPERAND OF QUALIFICATION, not IDENTIFIER */
                else
                    Sub_4631();
            }
            if (MatchTx1Item(L_LPAREN)) {
                if (TestInfoFlag(F_ARRAY)) {
                    ResyncRparen();
                    if (arg1b == I_LENGTH || arg1b == I_LAST)
                    {
                        WrTx2ExtError(125); /* ILLEGAL ARGUMENT FOR BUILT-IN procedure */
                        ExprPush2(I_NUMBER, 0);
                    }
                    else 
                        ExprPush2(arg1b, curInfoP);
                } else {
                    WrTx2ExtError(127); /* INVALID SUBSCRIPT ON NON-ARRAY */
                    ExprPush2(I_NUMBER, 0);
                }
            } else if (arg1b == I_LENGTH || arg1b == I_LAST) {
                if (TestInfoFlag(F_ARRAY)) 
                    ExprPush2(arg1b, curInfoP);
                else {
                    WrTx2ExtError(157);
                        /* INVALID ARGUMENT, ARRAY REQUIRED FOR length() or last() */
                    ExprPush2(I_NUMBER, 0);
                }
            } else {
                ExprPush2(arg1b, curInfoP);
                ExprPush2(I_LENGTH, curInfoP);
                ExprMakeNode(I_STAR, 2);
            }
        }
        ExpectRparen(126);  /* MISSING ') ' AFTER BUILT-IN procedure ARGUMENT LIST */
    }
}


void MkIndexNode()
{
    word p, q, r;
    byte i;

    curInfoP = ex3Stack[exSP-1];    /* get var */
    if (ex1Stack[exSP] == I_PLUSSIGN) {   /* see if (index is of form expr + ?? */
        p = ex3Stack[exSP] + 1;
        if (st1Stack[p] == I_NUMBER) {   /* expr + number */
            ex1Stack[exSP] = st1Stack[q = p - 1];  /* pull up expression */
            ex2Stack[exSP] = st2Stack[q];
            ex3Stack[exSP] = st3Stack[q];
            ExprPush2(I_NUMBER, st3Stack[p]);   /* and get the number as an offset */
        } else 
            ExprPush2(I_NUMBER, 0);     /* no simple 0 offset */
    } else
        ExprPush2(I_NUMBER, 0);         /* 0 offset */

    if (GetType() == ADDRESS_T)     /* simple word array */
        i = I_WORDINDEX;
    else {
        i = I_BYTEINDEX;
        if (GetType() == STRUCT_T) {   /* scale structure index */
            r = exSP - 1;                   /* the index expr (ex offset) */
            ExprPush3(ex1Stack[r], ex2Stack[r], ex3Stack[r]);   /* calc dimension */
            ExprPush2(I_SIZE, curInfoP);
            ExprMakeNode(I_STAR, 2);
            ex1Stack[r] = ex1Stack[exSP];   /* replace index expr */
            ex2Stack[r] = ex2Stack[exSP];
            ex3Stack[r] = ex3Stack[exSP];
            ExprPop();                      /* waste intermediate */
        }
    }
    ExprMakeNode(i, 3);
}


void ParsePortNum(byte arg1b)
{
    word p;

    p = 0;
    if (MatchTx1Item(L_LPAREN)) {
        if (MatchTx1Item(L_NUMBER)) {
            if (tx1Item.dataw[0] <= 255)
                p = tx1Item.dataw[0];
            else
                WrTx2ExtError(106); /* INVALID Input/OUTPUT PORT NUMBER */
        } else
            WrTx2ExtError(107);/* ILLEGAL Input/OUTPUT PORT NUMBER, not NUMERIC CONSTANT */
        ExpectRparen(108);  /* MISSING ') ' AFTER Input/OUTPUT PORT NUMBER */
    } else
        WrTx2ExtError(109); /* MISSING Input/OUTPUT PORT NUMBER */

    ExprPush2(arg1b, p);
}


void Sub_50D5()
{
    word p;
    byte i;

    p = curInfoP;
    if ((curInfoP = curProcInfoP) == 0)
        i = false;
    else
        i = TestInfoFlag(F_REENTRANT);
    curInfoP = p;
    if (TestInfoFlag(F_REENTRANT) && i)
        return;
    if (! TestInfoFlag(F_DECLARED)) 
        WrTx2ExtError(169);     /* ILLEGAL FORWARD call */
    else if (! TestInfoFlag(F_DEFINED))
        WrTx2ExtError(170);     /* ILLEGAL RECURSIVE call */
}


byte Sub_512E(word arg1w)
{
    byte c;

    if ((c = ex1Stack[arg1w]) == I_OUTPUT || c == I_STACKPTR || c == I_BASED)
        return false;
    if (c == I_IDENTIFIER) 
        curInfoP = ex3Stack[arg1w];
    else if (c == I_BYTEINDEX || c == I_WORDINDEX)
        curInfoP = st3Stack[ex3Stack[arg1w]];
    else if (c == I_MEMBER) {
        if (st1Stack[ex3Stack[arg1w]] == I_IDENTIFIER)
            curInfoP = st3Stack[ex3Stack[arg1w]];
        else
            curInfoP = st3Stack[st3Stack[ex3Stack[arg1w]]];
    } else
        return true;
    if (TestInfoFlag(F_DATA))
        WrTx2ExtError(173);
    /*  INVALID LEFT SIDE OF ASSIGNMENT: VARIABLE DECLARED WITH data ATTRIBUTE */
    return false;
}

void Sub_521B()
{
    offset_t p;
    word q;

    p = curSymbolP;
    curSymbolP = 0;
    CreateInfo(256, BYTE_T);
    SetInfoFlag(F_DATA);
    curSymbolP = p;
    ExprPush2(I_IDENTIFIER, curInfoP);
    SetInfoFlag(F_ARRAY);
    SetInfoFlag(F_STARDIM);
    q = ParseDataItems(curInfoP);
    SetDimension(q);
}
