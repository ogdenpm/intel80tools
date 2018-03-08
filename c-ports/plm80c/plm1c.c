#include "plm.h"

word rValue;
bool use8bit;

// <restricted primary>
static void RestrictedPrimary()
{
    if (MatchTx1Item(L_NUMBER))
        rValue = tx1Item.dataw[0];
    else {
        if (MatchTx1Item(L_IDENTIFIER))
            ChkIdentifier();
        WrTx2ExtError(151); /* INVALID OPERAND IN RESTRICTED Expression */
        rValue = 0;
    }
    use8bit = use8bit && (High(rValue) == 0);   // if > 255 then switch off any 8 bit mode
}

// <restricted secondary>
static void RestrictedSecondary()
{
    if (MatchTx1Item(L_MINUSSIGN)) {
        RestrictedPrimary();
        if (use8bit) 
            rValue = - Low(rValue);
        else
            rValue = - rValue;
    } else
        RestrictedPrimary();
}

//  <restricted sum>
static void RestrictedSum()
{
    word p;

    RestrictedSecondary();  // get number
    p = rValue;
    while (1) {
        if (MatchTx1Item(L_PLUSSIGN)) {     // + number
            RestrictedSecondary();
            if (use8bit)
                p = (p + rValue) & 0xff;
            else 
                p += rValue;
        } else if (MatchTx1Item(L_MINUSSIGN)) { // - number
            RestrictedSecondary();
            if (use8bit)
                p = (p - rValue) & 0xff;
            else
                p -= rValue;
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
    RestrictedSum();
    ExpectRParen(150);  /* MISSING ') ' AT END OF RESTRICTED SUBSCRIPT */
}

static void GetRestrictedVar()
{
    if (NotMatchTx1Item(L_IDENTIFIER)) {
        WrTx2Error(ERR147);    /* MISSING IDENTIFIER FOLLOWING DOT OPERATOR */
        RecoverRPOrEndExpr();
        return;
    }
    ChkIdentifier();
    var.infoOffset = curInfoP;   // record the var info
    if (GetType() == BUILTIN_T) {   // can't take the address of a builtin!!
        WrTx2Error(ERR123);    /* INVALID DOT OPERAND, BUILT-IN PROCEDURE ILLEGAL */
        RecoverRPOrEndExpr();
        return;
    }
    if (MatchTx1Item(L_LPAREN)) {   // is indexed
        GetRestrictedArrayIndex();
        var.arrayIndex = rValue;
    }
    if (MatchTx1Item(L_PERIOD))     // a struct ref
    {
        curInfoP = var.infoOffset;  // recover the base  info
        if (GetType() != STRUCT_T) {    // not on a stuct
            WrTx2ExtError(ERR148); /* INVALID QUALIFICATION IN RESTRICTED REFERENCE */
            RecoverRPOrEndExpr();
            return;
        }
        if (NotMatchTx1Item(L_IDENTIFIER)) {       // no identifier after the .
            WrTx2Error(ERR147);    /* MISSING IDENTIFIER FOLLOWING DOT OPERATOR */
            var.infoOffset = var.infoOffset - botInfo;  // normalise to offset
            return;
        }
        ChkStructureMember();
        var.infoOffset = curInfoP;
        if (MatchTx1Item(L_LPAREN)) {   // its a member array element reference
            GetRestrictedArrayIndex();
            var.nestedArrayIndex = rValue;  // value of member array index 
        }
    }
    var.infoOffset = var.infoOffset - botInfo;  // normalise to offset
}

void RestrictedExpression()
{
    var.infoOffset = var.arrayIndex = var.nestedArrayIndex = var.val = 0;
    if (MatchTx1Item(L_PERIOD)) {       // <restricted reference> := .<identifier> [<restricted subscript>] [.<identifier> [<restricted subscript>]]
        GetRestrictedVar();
        if (MatchTx1Item(L_PLUSSIGN))   // [<restricted adding operator>]
            ;
        else if (MatchTx1Item(L_MINUSSIGN)) // let RestrictedSum handle -
            SetRegetTx1Item();
        else
            return;                     // no so all done
        use8bit = false;                // operations will be 16 bit
        RestrictedSum();                // get sum
        var.val = rValue;
    }
    else if (MatchTx1Item(L_RPAREN))    // <restricted reference> := <restricted sum>
        SetRegetTx1Item();
    else {
        use8bit = true;                 // assume 8 bit
        RestrictedSum();                // get sum
        var.val = rValue;
    }
}

word InitialValueList(offset_t infoOffset)
{
    word p;

    p = 0;
    WrAtFileByte(ATI_DHDR);
    WrAtFileWord(infoOffset - botInfo);     // normalise
    WrAtFileWord(curStmtNum);
    while (1) {
        if (MatchTx1Item(L_STRING)) {       // string initialisation
            WrAtFileByte(ATI_STRING);
            WrAtFileWord(tx1Item.dataw[0]); // write length
            WrAtFile((pointer)&tx1Item.dataw[1], tx1Item.dataw[0]); // and the string
            curInfoP = infoOffset;
            if (GetType() == ADDRESS_T)     // count items written (scaled by 2 if address)
                p += (tx1Item.dataw[0] + 1) / 2;
            else 
                p += tx1Item.dataw[0];
        } else {                            
            RestrictedExpression();         // get the restricted Expression
            WrAtFileByte(ATI_DATA);
            WrAtFile((pointer)&var, sizeof(var));   // and write the info
            p++;
        }
        if (NotMatchTx1Item(L_COMMA))       // keep going if comma
            break;
        else if (MatchTx1Item(L_RPAREN)) {  // ) with no preceeding data
            WrTx2Error(151);    /* INVALID OPERAND IN RESTRICTED Expression */
            SetRegetTx1Item();
            break;
        }
    }
    WrAtFileByte(ATI_END);
    ExpectRParen(152);  /* MISSING ') ' AFTER CONSTANT LIST */
    curInfoP = infoOffset;;
    return p;                               // items read i.e. dimension
}

void ResetStacks()
{
    parseSP = exSP = operatorSP = stSP = 0;
}

void PushParseWord(word arg1w)
{
    if (parseSP == 99)
        FatalError(ERR119);    /* LIMIT EXCEEDED: PROGRAM TOO COMPLEX */
    parseStack[++parseSP] = arg1w;
}


void PopParseStack()
{
    if (parseSP == 0)
        FatalError(159);    /* COMPILER ERROR: PARSE STACK UNDERFLOW */
    parseSP = parseSP - 1;
}


void PushParseByte(byte arg1b)
{
    PushParseWord(arg1b);
}

static void ExprPush3(byte arg1b, byte arg2b, word arg3w)
{
    if (exSP == EXPRSTACKSIZE - 1)
        FatalError(121);    /* LIMIT EXCEEDED: Expression TOO COMPLEX */
    ex1Stack[++exSP] = arg1b;
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
        FatalError(161);    /* COMPILER ERROR: ILLEGAL OPERAND STACK EXCHANGE */
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

void ExprPush2(byte icode, word val)
{
    ExprPush3(icode, 0, val);
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
        FatalError(120);    /* LIMIT EXCEEDED: Expression TOO COMPLEX */
    operatorStack[operatorSP = operatorSP + 1] = arg1b;
}


void PopOperatorStack()
{
    if (operatorSP == 0)
        FatalError(162);    /* COMPILER Error: OPERATOR STACK UNDERFLOW */
    operatorSP = operatorSP - 1;
}


void ExprMakeNode(byte icode, byte argCnt)
{
    word w;
    byte i, j;

    w = stSP + 1;               // note base of where args will be on stmt stack

    if (exSP < argCnt)
        FatalError(ERR163);    /* COMPILER ERROR: GENERATION FAILURE  */
    j = exSP - argCnt + 1;
    i = argCnt;
    while (i != 0) {            // push the arg information to the stmt statck
        StmtPush3(ex1Stack[j], ex2Stack[j], ex3Stack[j]);
        i = i - 1;
        j = j + 1;
    }

    exSP -= argCnt;             // remove from expr stack
    if (argCnt == 0)            // no args so reset base
        w = 0;
    ExprPush3(icode, argCnt, w);    // replace with new expr item
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


static void PushIdentifier()
{
    word p;

    if (TestInfoFlag(F_MEMBER)) {           // if member create I_MEMBER node
        p = curInfoP;
        curInfoP = GetParentOffset();
        ExprPush2(I_IDENTIFIER, curInfoP);  // struct info
        ExprPush2(I_IDENTIFIER, p);         // member info
        ExprMakeNode(I_MEMBER, 2);          // new node
    }
    else
        ExprPush2(I_IDENTIFIER, curInfoP);  // simple identifier node
}

void FixupBased(offset_t arg1w)
{
    curInfoP = arg1w;                       // var info
    if (TestInfoFlag(F_BASED)) {            // is it based?
        curInfoP = GetBaseOffset();         // get the info of the base
        PushIdentifier();                   // save it
        curInfoP = arg1w;
        SwapOperandStack();                 // swap var and base on stack
        ExprMakeNode(I_BASED, 2);           // create a based node
    }
}

void Sub_4D2C()
{
    PushIdentifier();
    FixupBased(curInfoP);
}


void ChkTypedProcedure()
{
    if (GetDataType() == 0)
        WrTx2ExtError(131); /* ILLEGAL REFERENCE to UNTYPED PROCEDURE */
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
        WrTx2ExtError(153); /* INVALID NUMBER OF ARGUMENTS IN CALL, TOO MANY */
        k = j - i;
        while (k != 0) {
            ExprPop();
            k = k - 1;
        }
    } else {
        WrTx2ExtError(154); /* INVALID NUMBER OF ARGUMENTS IN CALL, TOO FEW */
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
        WrTx2ExtError(ERR124); /* MISSING ARGUMENTS FOR BUILT-IN PROCEDURE */
        ExprPush2(I_NUMBER, 0);
    } else {
        if (NotMatchTx1Item(L_IDENTIFIER)) {
            WrTx2Error(ERR125);    /* ILLEGAL ARGUMENT FOR BUILT-IN PROCEDURE */
            ExprPush2(I_NUMBER, 0);
        } else {
            ChkIdentifier();
            if (MatchTx1Item(L_LPAREN)) {
                if (TestInfoFlag(F_ARRAY)) {
                    ResyncRParen();
                    if (MatchTx1Item(L_RPAREN))
                    {
                        if (arg1b == I_LENGTH || arg1b == I_LAST)
                        {
                            WrTx2ExtError(ERR125); /*  ILLEGAL ARGUMENT FOR BUILT-IN PROCEDURE */
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
                    ChkStructureMember();
            }
            if (MatchTx1Item(L_LPAREN)) {
                if (TestInfoFlag(F_ARRAY)) {
                    ResyncRParen();
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
        ExpectRParen(126);  /* MISSING ') ' AFTER BUILT-IN PROCEDURE ARGUMENT LIST */
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
            ex1Stack[exSP] = st1Stack[q = p - 1];  /* pull up Expression */
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
        ExpectRParen(108);  /* MISSING ') ' AFTER Input/OUTPUT PORT NUMBER */
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

void ConstantList()
{
    offset_t p;
    word q;

    p = curSymbolP;                 // save current symbol
    curSymbolP = 0;                 // no symbol
    CreateInfo(256, BYTE_T);        // create an info block to hold the list
    SetInfoFlag(F_DATA);            // mark as data
    curSymbolP = p;                 // restore symbol
    ExprPush2(I_IDENTIFIER, curInfoP);  // push the identifier
    SetInfoFlag(F_ARRAY);               // set as a byte array with * dim
    SetInfoFlag(F_STARDIM);
    q = InitialValueList(curInfoP); // get the list
    SetDimension(q);                // and record number of bytes
}
