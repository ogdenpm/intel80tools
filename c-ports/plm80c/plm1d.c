#include "plm.h"

void ExprParse0()
{
	PushOperator(0);
	if (MatchTx2Item(L_VARIABLE)) {
		PushParseByte(1);
		Sub_45E0();
		PushParseByte(11);
	} else {
		PushParseByte(3);
		PushParseByte(4);
	}
}

void ExprParse1()
{
	if (MatchTx2Item(L_COLONEQUALS)) {
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
	if (MatchTx2Item(L_MINUSSIGN))
		PushOperator(I_UNARYMINUS);
	else if (MatchTx2Item(L_NOT))
		PushOperator(I_NOT);
	PushParseByte(5);
}

void ExprParse5()
{
	word p;
    pointer q;

	if (MatchTx2Item(L_VARIABLE)) {
		Sub_45E0();
		if (GetType() == BUILTIN_T && GetBuiltinId() == 9) {	 /* 9 -> OUTPUT */
			WrTx2ExtError(ERR130);	/* ILLEGAL REFERENCE to OUTPUT FUNCTION */
			if (MatchTx2Item(L_LPAREN))
				ResyncRparen();
			ExprPush2(I_NUMBER, 0);
		} else
			PushParseByte(11);
	}
	else if (MatchTx2Item(L_NUMBER))
		ExprPush2(I_NUMBER, tx1Item.dataw[0]);
	else if (MatchTx2Item(L_STRING)) {
		if (tx1Item.dataw[0] == 1)
			p = Low(tx1Item.dataw[1]);
		else if (tx1Item.dataw[0] == 2) {
			q = (pointer)&p;
			q[0] = High(tx1Item.dataw[1]);
			q[1] = Low(tx1Item.dataw[1]);
		} else {
			WrTx2ExtError(ERR100);	/* INVALID STRING CONSTANT IN EXPRESSION  */
			p = 0;
		}
		ExprPush2(I_NUMBER, p);
	}
	else if (MatchTx2Item(L_LPAREN)) {
		PushParseByte(6);
		PushParseByte(0);
	}
	else if (MatchTx2Item(L_PERIOD)) {
		if (MatchTx2Item(L_VARIABLE)) {
			PushParseByte(7);
			PushParseByte(8);
		} else if (MatchTx2Item(L_LPAREN)) {
			Sub_521B();
			PushParseByte(7);
		} else {
			WrTx2ExtError(ERR101);	/* INVALID ITEM FOLLOWS DOT OPERATOR */
			PushParseByte(5);
		}
	} else {
		WrTx2ExtError(ERR102);	/* MISSING PRIMARY OPERAND */
		ExprPush2(I_NUMBER, 0);
	}
}


void ExprParse6()
{
	ExpectRparen(ERR103);	/* MISSING ') ' at end OF SUBEXPRESSION */
}

void ExprParse7()
{
	ExprMakeNode(I_ADDRESSOF, 1);
}


void ExprParse8()
{
	Sub_45E0();
	if (GetType() == BUILTIN_T) {
		PopParseStack();
		WrTx2ExtError(ERR123);	/* INVALID DOT OPERAND, BUILT-IN procedure ILLEGAL */
		if (MatchTx2Item(L_LPAREN))
			ResyncRparen();
		ExprPush2(I_NUMBER, 0);
	} else {
		ExprPush2(I_VARIABLE, curInfoP);
		if (GetType() == PROC_T) {
			if (MatchTx2Item(L_LPAREN)) {
				WrTx2ExtError(ERR104);	/* ILLEGAL procedure INVOCATION WITH DOT OPERATOR */
				ResyncRparen();
			}
		} else {
			if (GetType() == LABEL_T)
				WrTx2ExtError(ERR158);	/* INVALID DOT OPERAND, label ILLEGAL */
			PushParseByte(9);
		}
	}
}

void ExprParse9()
{
	PushParseWord(curInfoP);
	PushParseByte(0xa);
	if (MatchTx2Item(L_LPAREN)) {
		if (!TestInfoFlag(F_ARRAY))
			WrTx2ExtError(ERR127);	/* INVALID SUBSCRIPT ON NON-ARRAY */
		PushParseByte(0x13);
	}
}

void ExprParse10()
{
	offset_t p;
	p = parseStack[parseSP];
	PopParseStack();
	if (MatchTx2Item(L_PERIOD)) {
		curInfoP = p;
		if (GetType() != STRUCT_T)
			WrTx2ExtError(ERR110);	/* INVALID LEFT OPERAND OF QUALIFICATION, not A structure */
		else if (NotMatchTx2Item(L_VARIABLE))
			WrTx2ExtError(ERR111);	/* INVALID RIGHT OPERAND OF QUALIFICATION, not IDENTIFIER */
		else {
			Sub_4631();
			ExprPush2(I_VARIABLE, curInfoP);
			PushParseWord(p);
			PushParseByte(14);
			if (MatchTx2Item(L_LPAREN))
				PushParseByte(19);
		}
	} else
		Sub_4CFD(p);
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
			Sub_4D38();
			if (MatchTx2Item(L_LPAREN)) {
				PushParseByte(15);
				PushParseByte(0);
			}
		}
	} else if (GetType() == PROC_T) {
		Sub_50D5();
		ExprPush2(I_VARIABLE, curInfoP);
		Sub_4D38();
		PushParseWord(GetParamCnt());
		PushOperator(0);
		PushParseByte(16);
		if (MatchTx2Item(L_LPAREN)) {
			PushParseByte(15);
			PushParseByte(0);
		}
	} else
		PushParseByte(12);
}

void ExprParse12()
{
	ExprPush2(I_VARIABLE, curInfoP);
	PushParseWord(curInfoP);
	if (GetType() == LABEL_T)
		WrTx2ExtError(ERR132);	/* ILLEGAL USE OF label */
	PushParseByte(13);
	if (TestInfoFlag(F_ARRAY)) {
		if (MatchTx2Item(L_LPAREN))
			PushParseByte(19);
		else
			WrTx2ExtError(ERR133);	/* ILLEGAL REFERENCE to UNSUBSCRIPTED ARRAY */
	} else if (MatchTx2Item(L_LPAREN)) {
		WrTx2ExtError(ERR127);	 /* INVALID SUBSCRIPT ON NON-ARRAY */
		PushParseByte(19);
	}
}

void ExprParse13()
{
	word p;

	p = curInfoP = parseStack[parseSP];
	PopParseStack();
	if (MatchTx2Item(L_PERIOD)) {
		if (GetType() != STRUCT_T)
			WrTx2ExtError(ERR110);	/* INVALID LEFT OPERAND OF QUALIFICATION, not A structure */
		else if (NotMatchTx2Item(L_VARIABLE))
			WrTx2ExtError(ERR111);	/* INVALID RIGHT OPERAND OF QUALIFICATION, not IDENTIFIER */
		else {
			Sub_4631();
			ExprPush2(I_VARIABLE, curInfoP);
			PushParseWord(p);
			PushParseByte(14);
			if (TestInfoFlag(F_ARRAY)) {
				if (MatchTx2Item(L_LPAREN))
					PushParseByte(19);
				else
					WrTx2ExtError(ERR134);	/* ILLEGAL REFERENCE to UNSUBSCRIPTED MEMBER ARRAY */
			} else if (MatchTx2Item(L_LPAREN))
				WrTx2ExtError(ERR127);	/* INVALID SUBSCRIPT ON NON-ARRAY */
		}
	} else {
		if (GetType() == STRUCT_T)
			WrTx2ExtError(ERR135);	/* ILLEGAL REFERENCE to AN UNQUALIFIED structure */
		Sub_4CFD(p);
	}
}

void ExprParse14()
{
	offset_t p;
	p = parseStack[parseSP];
	PopParseStack();
	ExprMakeNode(I_MEMBER, 2);
	Sub_4CFD(p);
}


void ExprParse15()
{
	operatorStack[operatorSP] = operatorStack[operatorSP] + 1;
	if (MatchTx2Item(L_COMMA)) {
		PushParseByte(0xf);
		PushParseByte(0);
	} else
		ExpectRparen(ERR113);	/* MISSING ') ' at end OF ARGUMENT LIST */
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


void ExprParse19()
{
	PushParseByte(20);
	PushParseByte(0);
}


void ExprParse20()
{
	if (MatchTx2Item(L_COMMA)) {
		WrTx2ExtError(ERR114);	/* INVALID SUBSCRIPT, MULTIPLE SUBSCRIPTS ILLEGAL */
		Sub_4599();
	}
	ExpectRparen(ERR115);	/* MISSING ') ' at end OF SUBSCRIPT */
	MkIndexNode();
}

void ExprParse()
{
	word p;
	while (parseSP != 0) {
		p = parseStack[parseSP];
		parseSP = parseSP - 1;
		switch (p) {
		case 0: ExprParse0(); break;
		case 1: ExprParse1(); break;
		case 2: ExprParse2(); break;
		case 3: ExprParse3(); break;
		case 4: ExprParse4(); break;
		case 5: ExprParse5(); break;
		case 6: ExprParse6(); break;
		case 7: ExprParse7(); break;
		case 8: ExprParse8(); break;
		case 9: ExprParse9(); break;
		case 10: ExprParse10(); break;
		case 11: ExprParse11(); break;
		case 12: ExprParse12(); break;
		case 13: ExprParse13(); break;
		case 14: ExprParse14(); break;
		case 15: ExprParse15(); break;
		case 16: ExprParse16(); break;
		case 17: ExprParse17(); break;
		case 18: ExprParse18(); break;
		case 19: ExprParse19(); break;
		case 20: ExprParse20(); break;
		}
	}
}
