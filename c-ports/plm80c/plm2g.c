#include "plm.h"

static byte curParamCnt;
static byte bC2D1;
static byte bC2D2, bC2D3;
static word wC2D4;


void FindParamInfo(byte arg1b)
{
	curInfoP = blkCurInfo[blkSP];
	while (arg1b != 0) {
		AdvNxtInfo();
		arg1b = arg1b - 1;
	}
}



void Sub_9514()
{
	if (GetType() == ADDRESS_T) {
		wC1DC[0] = bC2D1;
		EncodeFragData(CF_MOVMRPR);
		pc = pc + 3;
	}
	else {
		wC1DC[0] = bC2D1;
		EncodeFragData(CF_MOVMLR);
		pc = pc + 1;
	}
	if (bC2D2 != curParamCnt) {
		EncodeFragData(CF_DCXH);
		pc = pc + 1;
	}
}

void Sub_9560()
{
	byte i;

	if ((i = curParamCnt) == 1)
		bC2D1 = 1;
	else
		bC2D1 = 2;
	for (bC2D2 = 1; bC2D2 <= curParamCnt; bC2D2++) {
		FindParamInfo(i);
		if (bC2D2 == 2)
			bC2D1 = 1;
		else if (bC2D2 == 3) {
			wC1DC[0] = 2;	/*  pop d */
			wC1DC[1] = 8;
			EncodeFragData(CF_POP);
			wC1DC[0] = 1;	/*  pop b */
			wC1DC[1] = 8;
			EncodeFragData(CF_POP);
			pc = pc + 2;
		}
		else if (bC2D2 > 3) {
			wC1DC[0] = 1;	/*  pop b */
			wC1DC[1] = 8;
			EncodeFragData(CF_POP);
			pc = pc + 1;
		}
		Sub_9514();
		i = i - 1;
	}
	if (curParamCnt > 2) {
		wC1DC[0] = 2;	/*  push d */
		wC1DC[1] = 8;
		EncodeFragData(CF_PUSH);
		pc = pc + 1;
	}
}

void Sub_9624(word arg1w)
{
	wC1DC[0] = 9;
	wC1DC[1] = arg1w;
	EncodeFragData(CF_6);
	pc = pc + 4;
}


void Sub_9646(word arg1w)
{
	if (Shr(arg1w, 1) + (arg1w & 1) <= 5) {
		if (arg1w) {
			EncodeFragData(CF_DCXSP);
			pc = pc + 1;
		}
		while (arg1w > 1) {
			wC1DC[0] = 3;	/*  push h */
			wC1DC[1] = 8;
			EncodeFragData(CF_PUSH);
			pc = pc + 1;
			arg1w = arg1w - 2;
		}
	}
	else {
		Sub_9624(-arg1w);
		EncodeFragData(CF_SPHL);
		pc = pc + 1;
	}
}


void Inxh()
{
	wC1DC[0] = 3;
	EncodeFragData(CF_INX);
	pc = pc + 1;
}


void OpB(byte arg1b)
{
	wC1DC[0] = 1;
	EncodeFragData(arg1b);
	pc = pc + 1;
}

void OpD(byte arg1b)
{
	wC1DC[0] = 2;
	EncodeFragData(arg1b);
	pc = pc + 1;
}


void Sub_9706()
{
	Inxh();
	if (GetType() == ADDRESS_T) {
		OpB(CF_MOVLRM);
		if (bC2D3 == 1)
			OpD(CF_MOVMLR);

		Inxh();
		OpB(CF_MOVHRM);
	}
	else {
		OpB(CF_MOVHRM);
		if (bC2D3 == 1)
			OpD(CF_MOVMLR);
		Inxh();
	}
	if (bC2D3 == 1)
		OpD(CF_MOVMHR);
}



void MovDem()
{
	OpD(CF_MOVRPM);
	pc = pc + 2;
}


void Sub_975F()
{
	wC1DC[0] = bC2D1;
	wC1DC[1] = 8;
	EncodeFragData(CF_PUSH);
	pc = pc + 1;
	if (GetType() == BYTE_T)
	{
		EncodeFragData(CF_INXSP);
		pc = pc + 1;
	}
}


void Sub_978E()
{
	if ((bC2D3 = curParamCnt) > 2)
		Sub_9624(wC2D4);
	if (curParamCnt == 1)
		bC2D1 = 1;
	else
		bC2D1 = 2;
	for (bC2D2 = 1; bC2D2 <= curParamCnt; bC2D2++) {
		FindParamInfo(bC2D3);
		if (bC2D2 > 3)
			Sub_9706();
		else if (bC2D2 == 3) {
			MovDem();
			Sub_9706();
		}
		else if (GetType() == BYTE_T) {
			wC1DC[0] = bC2D1;
			EncodeFragData(CF_MOVHRLR);
			pc = pc + 1;
		}
		Sub_975F();
		bC2D1 = 1;
		bC2D3 = bC2D3 - 1;
	}
}


void Sub_981C()
{
	byte i, j;
	curParamCnt = GetParamCnt();
	if (TestInfoFlag(F_INTERRUPT)) {
		for (j = 0; j <= 3; j++) {
			wC1DC[0] = 3 - j;
			wC1DC[1] = 8;	/*  push h, push d, push b, push psw */
			EncodeFragData(CF_PUSH);
			pc = pc + 1;
		}
	}
	if (TestInfoFlag(F_REENTRANT)) {
		wC1C7 = GetParentVal(); /* or Size() */;
		if (curParamCnt > 0) {
			FindParamInfo(curParamCnt);
			wC2D4 = wC1C7 - GetLinkVal() - 1;
			if (GetType() == ADDRESS_T)
				wC2D4 = wC2D4 - 1;
			Sub_9646(wC2D4);
			Sub_978E();
		}
		else
			Sub_9646(wC1C7);

		if (curParamCnt > 2)
			wC1C7 = wC1C7 + Shl(curParamCnt - 2, 1);

		wC1C5 = 0;
	}
	else {
		if (curParamCnt > 0) {
			FindParamInfo(curParamCnt);	/*  locate info for first param */
			if (GetType() == ADDRESS_T)
				i = 1;
			else
				i = 0;
			wC1DC[0] = 3;
			wC1DC[1] = 0xb;
			wC1DC[2] = i;
			wC1DC[3] = curInfoP - botInfo;	/*  info for first param */
			EncodeFragData(CF_7);
			Sub_9560();
			pc = pc + 3;
		}
		wC1C7 = 0;
		if (curParamCnt > 2)
			wC1C5 = Shl(curParamCnt - 2, 1);
		else
			wC1C5 = 0;
	}
}

void Sub_994D()
{
	byte i, j;

	if (curOp == T2_LABELDEF) {
		boC1CC = 0;
		curInfoP = tx2op1[tx2qp] + botInfo;
		SetLinkVal(pc);
	}
	else if (curOp == T2_LOCALLABEL) {
		boC1CC = 0;
		WordP(localLabelsP)[tx2op1[tx2qp]] = pc;
		ByteP(w381E)[tx2op1[tx2qp]] = curExtProcId;
	}
	else if (curOp == T2_CASELABEL) {
		WordP(localLabelsP)[tx2op1[tx2qp]] = pc;
		ByteP(w381E)[tx2op1[tx2qp]] = curExtProcId;
		if ((w3822 - botMem) / 2 >= wC1CF) {
			WordP(botMem)[wC1CF] = tx2op1[tx2qp];
			wC1CF = wC1CF + 1;
		}
		else {
			EmitTopItem();
			Tx2SyntaxError(ERR202);	/*  LIMIT EXCEEDED: NUMBER OF */
						/*  ACTIVE CASES */
		}
	}
	else if (curOp == T2_JMP || curOp == T2_JNC || curOp == T2_JNZ || curOp == T2_GO_TO) {
		i = tx2opc[tx2qp - 1];
		if (i == T2_RETURN || i == T2_RETURNBYTE || i == T2_RETURNWORD || i == T2_GO_TO)
			return;
		Sub_5795(0);
	}
	else if (curOp == T2_INPUT || (T2_SIGN <= curOp && curOp <= T2_CARRY)) {
		bC0B7[0] = 0;
		bC0B7[1] = 0;
		bC0B5[0] = 8;
		bC0B5[1] = 8;
		Sub_597E();
		Sub_5D6B(0);
		bC045[0] = 0;
		bC04E[0] = tx2qp;
		boC057[0] = 0;
		bC0A8[0] = 0;
		tx2Aux1b[tx2qp] = 0;
		tx2Aux2b[tx2qp] = 9;
	}
	else if (curOp == T2_STMTCNT) {
		j = tx2qp + 1;

		while (tx2opc[j] != T2_STMTCNT && tx2opc[j] != T2_EOF && j < 0xFF) {
			if ((b5124[tx2opc[j]] & 0x20) == 0 || tx2opc[j] == T2_MODULE)
				goto L9B8D;
			j = j + 1;
		}
		curOp = CF_134;
		tx2opc[tx2qp] = CF_134;
	}
L9B8D:
	EmitTopItem();
	pc = pc + (b43F8[curOp] & 0x1f);
}
