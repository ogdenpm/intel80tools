#include "plm.h"

// <expression>
void ExprParse0()
{
    PushOperator(0);                // put end marker
    if (MatchTx1Item(L_IDENTIFIER)) {   // startes with identifier
        PushParseByte(1);
        ChkIdentifier();
        PushParseByte(11);
    } else {                        // should NOT or '-'
        PushParseByte(3);
        PushParseByte(4);
    }
}

void ExprParse1()
{
    if (MatchTx1Item(L_COLONEQUALS)) {
        if (Sub_512E(exSP)) {
            WrTx2ExtError(ERR128);	/* INVALID LEFT-HAND OPERAND OF ASSIGNMENT */
            ExprPop();
        } else
            PushParseByte(2);
        PushParseByte(3);
        PushParseByte(4);
    } else {
        if (ex1Stack[exSP] == I_OUTPUT) {
            WrTx2Error(ERR130);	/*  ILLEGAL REFERENCE to OUTPUT FUNCTION */
            ExprPop();
            ExprPush2(I_NUMBER, 0);
        }
        PushParseByte(3);
    }
}

void ExprParse2()
{
    ExprMakeNode(I_COLONEQUALS, 2);
}


void ExprParse3()
{
    if (MatchTx2AuxFlag(64)) {
        while (b4172[tx1Aux1] <= b4172[operatorStack[operatorSP]]) {
            if (b4172[tx1Aux1] == 50 && b4172[operatorStack[operatorSP]] == 50)
                WrTx2ExtError(ERR218);	/* ILLEGAL SUCCESSIVE USES OF RELATIONAL OPERATORS */
            AcceptOpAndArgs();
        }
        PushOperator(tx1Aux1);
        PushParseByte(3);
        PushParseByte(4);
    } else {
        while (operatorStack[operatorSP] != 0)
            AcceptOpAndArgs();
        PopOperatorStack();
    }
}


void ExprParse4()
{
    if (MatchTx1Item(L_MINUSSIGN))      // starts with - so convert to unary minus
        PushOperator(I_UNARYMINUS);
    else if (MatchTx1Item(L_NOT))       // starts with NOT
        PushOperator(I_NOT);
    PushParseByte(5);                   // <primary>
}

// <primary>
void ExprParse5()
{
    word p;
    pointer q;

    if (MatchTx1Item(L_IDENTIFIER)) {
        ChkIdentifier();
        if (GetType() == BUILTIN_T && GetBuiltinId() == 9) {	 /* 9 -> OUTPUT */
            WrTx2ExtError(ERR130);	/* ILLEGAL REFERENCE TO OUTPUT FUNCTION */
            if (MatchTx1Item(L_LPAREN))
                ResyncRParen();
            ExprPush2(I_NUMBER, 0);                 // error so assume 0
        } else
            PushParseByte(11);
    }
    else if (MatchTx1Item(L_NUMBER))
        ExprPush2(I_NUMBER, tx1Item.dataw[0]);      // push I_NUMBER & value
    else if (MatchTx1Item(L_STRING)) {
        if (tx1Item.dataw[0] == 1)
            p = Low(tx1Item.dataw[1]);              // single char
        else if (tx1Item.dataw[0] == 2) {           // double char
            q = (pointer)&p;
            q[0] = High(tx1Item.dataw[1]);          // swap bytes
            q[1] = Low(tx1Item.dataw[1]);
        } else {
            WrTx2ExtError(ERR100);	/* INVALID STRING CONSTANT IN EXPRESSION  */
            p = 0;                                  // error so use 0
        }
        ExprPush2(I_NUMBER, p);                     // push I_NUMBER & string constant value
    }
    else if (MatchTx1Item(L_LPAREN)) {
        PushParseByte(6);           // closing ')' of subexpression
        PushParseByte(0);           // <expression>
    }
    else if (MatchTx1Item(L_PERIOD)) {      // addressof
        if (MatchTx1Item(L_IDENTIFIER)) {   // <location reference> := .<variable reference>
            PushParseByte(7);       // <location reference> make addressof node
            PushParseByte(8);       // <variable reference>
        } else if (MatchTx1Item(L_LPAREN)) { // <location reference> := .<constant list>
            ConstantList();         // collect the constant list
            PushParseByte(7);       // <location reference> make addressof node
        } else {
            WrTx2ExtError(ERR101);	/* INVALID ITEM FOLLOWS DOT OPERATOR */
            PushParseByte(5);       // <primary>
        }
    } else {
        WrTx2ExtError(ERR102);	/* MISSING PRIMARY OPERAND */
        ExprPush2(I_NUMBER, 0);     // assume a 0
    }
}



// closing ')' of <subexpression>
void ExprParse6()
{
    ExpectRParen(ERR103);	/* MISSING ') ' AT END OF SUBEXPRESSION */
}

// <location reference> make address of node
void ExprParse7()
{
    ExprMakeNode(I_ADDRESSOF, 1);
}

// <variable reference>
void ExprParse8()
{
    ChkIdentifier();
    if (GetType() == BUILTIN_T) {   // can't be a bulitin
        PopParseStack();
        WrTx2ExtError(ERR123);	/* INVALID DOT OPERAND, BUILT-IN PROCEDURE ILLEGAL */
        if (MatchTx1Item(L_LPAREN))
            ResyncRParen();
        ExprPush2(I_NUMBER, 0);     // assume a 0
    } else {
        ExprPush2(I_IDENTIFIER, curInfoP);  // record the variable
        if (GetType() == PROC_T) {      // if proc then can't have (...) invocation
            if (MatchTx1Item(L_LPAREN)) {
                WrTx2ExtError(ERR104);	/* ILLEGAL PROCEDURE INVOCATION WITH DOT OPERATOR */
                ResyncRParen();         // skip invocation
            }
        } else {                        // labels are also bad
            if (GetType() == LABEL_T)
                WrTx2ExtError(ERR158);	/* INVALID DOT OPERAND, LABEL ILLEGAL */
            PushParseByte(9);           // <data reference>
        }
    }
}

// <data reference>
void ExprParse9()
{
    PushParseWord(curInfoP);        // save name info
    PushParseByte(10);              // <member specifier>
    if (MatchTx1Item(L_LPAREN)) {   // start of <subscript>
        if (!TestInfoFlag(F_ARRAY))
            WrTx2ExtError(ERR127);	/* INVALID SUBSCRIPT ON NON-ARRAY */
        PushParseByte(19);          // <subscript>
    }
}

// <member specifier>
void ExprParse10()
{
    offset_t p;
    p = parseStack[parseSP];        // name info
    PopParseStack();
    if (MatchTx1Item(L_PERIOD)) {   // start of <member specifier>
        curInfoP = p;
        if (GetType() != STRUCT_T)
            WrTx2ExtError(ERR110);	/* INVALID LEFT OPERAND OF QUALIFICATION, NOT A STRUCTURE */
        else if (NotMatchTx1Item(L_IDENTIFIER))
            WrTx2ExtError(ERR111);	/* INVALID RIGHT OPERAND OF QUALIFICATION, NOT IDENTIFIER */
        else {
            ChkStructureMember();
            ExprPush2(I_IDENTIFIER, curInfoP);
            PushParseWord(p);         // member info
            PushParseByte(14);        // make member node 
            if (MatchTx1Item(L_LPAREN))
                PushParseByte(19);    // <subscript>
        }
    } else
        FixupBased(p);
}

void ExprParse11()
{
    word p;
    if (GetType() == BUILTIN_T) {
        p = builtinsMap[GetBuiltinId()];
        if (p == I_INPUT || p == I_OUTPUT)
            ParsePortNum((byte)p);
        else if (p == I_LENGTH || p == I_LAST || p == I_SIZE)
            Sub_4DCF((byte)p);
        else {
            PushParseWord(p);
            PushParseWord(GetParamCnt());
            PushOperator(0);
            PushParseByte(17);
            ChkTypedProcedure();
            if (MatchTx1Item(L_LPAREN)) {
                PushParseByte(15);
                PushParseByte(0);           // <expression>
            }
        }
    } else if (GetType() == PROC_T) {
        Sub_50D5();
        ExprPush2(I_IDENTIFIER, curInfoP);
        ChkTypedProcedure();
        PushParseWord(GetParamCnt());
        PushOperator(0);
        PushParseByte(16);
        if (MatchTx1Item(L_LPAREN)) {
            PushParseByte(15);
            PushParseByte(0);               // <expression>
        }
    } else
        PushParseByte(12);
}

void ExprParse12()
{
    ExprPush2(I_IDENTIFIER, curInfoP);
    PushParseWord(curInfoP);
    if (GetType() == LABEL_T)
        WrTx2ExtError(ERR132);	/* ILLEGAL USE OF label */
    PushParseByte(13);
    if (TestInfoFlag(F_ARRAY)) {
        if (MatchTx1Item(L_LPAREN))
            PushParseByte(19);
        else
            WrTx2ExtError(ERR133);	/* ILLEGAL REFERENCE to UNSUBSCRIPTED ARRAY */
    } else if (MatchTx1Item(L_LPAREN)) {
        WrTx2ExtError(ERR127);	 /* INVALID SUBSCRIPT ON NON-ARRAY */
        PushParseByte(19);
    }
}

void ExprParse13()
{
    word p;

    p = curInfoP = parseStack[parseSP];
    PopParseStack();
    if (MatchTx1Item(L_PERIOD)) {
        if (GetType() != STRUCT_T)
            WrTx2ExtError(ERR110);	/* INVALID LEFT OPERAND OF QUALIFICATION, not A structure */
        else if (NotMatchTx1Item(L_IDENTIFIER))
            WrTx2ExtError(ERR111);	/* INVALID RIGHT OPERAND OF QUALIFICATION, not IDENTIFIER */
        else {
            ChkStructureMember();
            ExprPush2(I_IDENTIFIER, curInfoP);
            PushParseWord(p);
            PushParseByte(14);
            if (TestInfoFlag(F_ARRAY)) {
                if (MatchTx1Item(L_LPAREN))
                    PushParseByte(19);
                else
                    WrTx2ExtError(ERR134);	/* ILLEGAL REFERENCE to UNSUBSCRIPTED MEMBER ARRAY */
            } else if (MatchTx1Item(L_LPAREN))
                WrTx2ExtError(ERR127);	/* INVALID SUBSCRIPT ON NON-ARRAY */
        }
    } else {
        if (GetType() == STRUCT_T)
            WrTx2ExtError(ERR135);	/* ILLEGAL REFERENCE to AN UNQUALIFIED structure */
        FixupBased(p);
    }
}

// make member node
void ExprParse14()
{
    offset_t p;
    p = parseStack[parseSP];        // get member info
    PopParseStack();
    ExprMakeNode(I_MEMBER, 2);      // make member node
    FixupBased(p);
}


void ExprParse15()
{
    operatorStack[operatorSP] = operatorStack[operatorSP] + 1;
    if (MatchTx1Item(L_COMMA)) {
        PushParseByte(15);
        PushParseByte(0);       // <expression>
    } else
        ExpectRParen(ERR113);	/* MISSING ') ' at end OF ARGUMENT LIST */
}


void ExprParse16()
{
    ExprMakeNode(I_CALL, GetCallArgCnt() + 1);
}


void ExprParse17()
{
    byte i, j;
    j = GetCallArgCnt();
    i = (byte)parseStack[parseSP];
    PopParseStack();
    ExprMakeNode(i, j);
}


void ExprParse18()
{
    ExprMakeNode(I_CALLVAR, operatorStack[operatorSP] + 1);
    PopOperatorStack();
}

// <subscript>
void ExprParse19()
{
    PushParseByte(20);      // end of <subscript>
    PushParseByte(0);       // <expression>
}


// end of <subscript>
void ExprParse20()
{
    if (MatchTx1Item(L_COMMA)) {
        WrTx2ExtError(ERR114);	/* INVALID SUBSCRIPT, MULTIPLE SUBSCRIPTS ILLEGAL */
        RecoverRPOrEndExpr();
    }
    ExpectRParen(ERR115);	/* MISSING ') ' at end OF SUBSCRIPT */
    MkIndexNode();
}

void ExpressionStateMachine()
{
    while (parseSP != 0) {
        switch (parseStack[parseSP--]) {
        case 0: ExprParse0(); break;    // <expression>
        case 1: ExprParse1(); break;
        case 2: ExprParse2(); break;
        case 3: ExprParse3(); break;
        case 4: ExprParse4(); break;
        case 5: ExprParse5(); break;    // <primary>
        case 6: ExprParse6(); break;    // closing ')' of <subexpression>
        case 7: ExprParse7(); break;    // <location reference> make addressof node
        case 8: ExprParse8(); break;    // <variable reference>
        case 9: ExprParse9(); break;    // <data reference>
        case 10: ExprParse10(); break;  // <member specifier>
        case 11: ExprParse11(); break;
        case 12: ExprParse12(); break;
        case 13: ExprParse13(); break;
        case 14: ExprParse14(); break;  // make member node
        case 15: ExprParse15(); break;
        case 16: ExprParse16(); break;
        case 17: ExprParse17(); break;
        case 18: ExprParse18(); break;
        case 19: ExprParse19(); break;  // <subscript>
        case 20: ExprParse20(); break;  // end of <subscript>
        }
    }
}
