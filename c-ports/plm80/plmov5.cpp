// $Id: plmov5.cpp,v 1.1 2003/10/04 21:08:48 Mark Ogden Exp $
#include "plm.hpp"
#include "common.hpp"

byte sub_561E(byte arg1b);
void sub_557A();
void advNextInfo();
void sub_4EC5();
void ixiWrite(byte *arg1w, word arg2w);
void sub_4D03();
void sub_4C84();
void sub_4B4A(const char *arg1w);
void sub_4A92();
void sub_4A78(const char *arg1w);
void sub_4A42();
void sub_49BB();
void sub_499C();
void sub_4921();
void sub_48E2(word arg1w, word arg2w);
void sub_48A7();
void sub_480A();
void sub_46C4();
void sub_4602();
void sub_4541();
void sub_451A();
void sub_4287();
byte cmpSym(word arg1w, word arg2w);
void sub_4140();
void sub_4121(const char *arg1w);


void sub_77BF(byte arg1b, byte arg2b, byte arg3b);




char lstBuf[1280];
byte b66CF;
word w66D0;
word w66D4;
address w66D6;
address pw66D9;
address w66DB;
byte b6783;
byte b678E;
byte b678F;
byte b6790;
byte b6791;
byte b6792;
byte b6793;

byte b3F0B = 0xff;
byte b40DE = 0;
byte b40DF = 1;

int overlay5()
{
	botMem = 0x6A36;	// MEMORY
	topSymbol += 4;
	if (PRINT) {
		lstBufPtr = lstBuf;
		lstBufSize = 1279;
	}
	sub_4EC5();
	if (PRINT)
		lstModuleInfo();
	deleteFile(&xrfFile);
	printExitMsg();
	return -1;
}

void sub_4121(const char *arg1w)
{
	newLineLst();
	xputstr2cLst("*** WARNING -- ", 0);
	xputstr2cLst(arg1w, 0);
	newLineLst();
}

void sub_4140()
{
	w66DB = pw66D9 = botMem;
	w66D0 = 0;
	b66CF = 0;
	curInfo_p = off2Info(2);

	while (1) {
		if (getInfoType() < MACRO_T && getInfoSymbol() != 0) {
			if ((w66DB = w66DB + 2) >= (word)botSymbol) {
				sub_4121("INSUFFICIENT MEMORY FOR FULL DICTIONARY LISTING");
				return;
			}
			++w66D0;
			*(word *)w66DB = curInfo_p;
			setInfoScope(0);
			curSymbol_p = getInfoSymbol();
			if (*(byte *)curSymbol_p > b66CF)
				b66CF = *(byte *)curSymbol_p;
		}
		advNextInfo();
		if (curInfo_p == 0)
			return;
	}
}

byte cmpSym(word arg1w, word arg2w)
{
	byte i, j ;
	byte *p, *q, *r, *s;

	curInfo_p = arg1w;
	p = (byte *)getInfoSymbol();
	r = p + 1;
	curInfo_p = arg2w;
	q = (byte *)getInfoSymbol();
	s = q + 1;
	if (*p < *q) {
		i = *p;
		j = 0;
	} else if (*p > *q) {
		i = *q;
		j = 2;
	} else {
		i = *p;
		j = 1;
	}

	while (i != 0) {
		if (*r < *s)
			return 0;
		if (*r > *s)
			return 2;
		++r; ++s;
		i--;
	}
	return j;
}


void sub_4287()
{
	word p, q, r, s, t, u, v, w, x, y, z;
	if (w66D0 == 1)
		return;
	s = (r = w66D0 / 2) + 2;
	t = 1;
	p = 2;
	q = r;
	y = w66D0;
	u = q;
	w = ((word *)pw66D9)[u];

	while (1) {
		v = u * 2;
		if (y < v) {
			((word *)pw66D9)[u] = w;
			if (t == 2) {
				x = ((word *)pw66D9)[1];
				((word *)pw66D9)[1] = ((word *)pw66D9)[q];
				((word *)pw66D9)[q] = x;
				if (p >= w66D0)
					break;
				else {
					y = q = z - ++p;
					u = 1;
				}
			} else if (p >= r) { //	loc_4391
				t = 2;
				z = w66D0 + 2;
				p = 2;
				y = q = w66D0;
				u = 1;
			} else {
				q = s - ++p;
				y = w66D0;
				u = q;
			}
			w = ((word *)pw66D9)[u];
		} else {
			if (y != v && cmpSym(((word *)pw66D9)[v + 1], ((word *)pw66D9)[v]) > 1)
				++v;
			if (cmpSym(((word *)pw66D9)[v], w) <= 1) { // 44F5
				((word *)pw66D9)[u] = w;
				if (t == 2) {	// 4494
					x = ((word *)pw66D9)[1];
					((word *)pw66D9)[1] = ((word *)pw66D9)[q];
					((word *)pw66D9)[q] = x;
					if (p >= w66D0) 
						break;
					y = q = z - ++p;
					u = 1;
				} else if (p >= r) { //	loc_44C6
					t = 2;
					z = w66D0 + 2;
					p = 2;
					y = q = w66D0;
					u = 1;
				} else {
					q = s - ++p;
					y = w66D0;
					u = q;
				}
				w = ((word *)pw66D9)[u];
			} else {
				((word *)pw66D9)[u] = ((word *)pw66D9)[v];
				u = v;
			}
		}
	}
}


void sub_451A()
{
	word p;
	if ((p = w66DB + 2) >= botSymbol) {
		sub_4121("INSUFFICIENT MEMORY FOR ANY XREF PROCESSING");
		return;
	}
	w66D4 = p - 1;
	sub_4541();
	sub_4602();
}

void sub_4541()
{
	word pa[2];

	//byte xrfbuf[128];
	openFile(&xrfFile, 1);
	//assignFileBuffer(&xrfFile, xrfbuf, 128, 1);
	while (1) {
		ifread(&xrfFile, &b6783, 1);
		if (b6783 == 0)
			break;
		ifread(&xrfFile, pa, 4);
		if (b6783 == 0x42 || XREF) {
			curInfo_p = off2Info(pa[0]);
			w66D6 = w66D4 + 1;
			if ((w66D4 += 4) > botSymbol) {
				sub_4121("INSUFFICIENT MEMORY FOR FULL XREF PROCESSING");
				break;
			}
			*(word *)w66D6 = getInfoScope();
			setInfoScope(w66D6);
			if (b6783 == 0x42) 
				((word *)w66D6)[1] = -pa[1];
			else
				((word *)w66D6)[1] = pa[1];
		}
	}
	closeFile(&xrfFile);
}


void sub_4602()
{
	word p, q, r;

	for (p = 1; p <= w66D0; p++) {
		curInfo_p = ((word *)pw66D9)[p];
		w66D6 = getInfoScope();
		if ((word)w66D6 != 0) { //	loc_46B6
			q = 0;
			setInfoScope(0);
			while ((word)w66D6 != 0) { //	loc_4691
				r = *(word *)w66D6;
				if (((word *)w66D6)[1] & 0x8000)
					q = w66D6;
				else {
					*(word *)w66D6 = getInfoScope();
					setInfoScope(w66D6);
				}
				w66D6 = r;
			}

			if (q != 0) { //	loc_46B6
				w66D6 = q;
				*(word *)w66D6 = getInfoScope();
				setInfoScope(w66D6);
			}
		}
	}
}

void sub_46C4()
{
	byte i;
	word p;

	b678E = 3;
	b678F = b678E + 6;
	b6790 = b678F + 6;
	b6791 = b6790 + 7;
	b6792 = b6791 + b66CF + 2;
	i = b6792 + 1;
	sub_77BF(b6792, 0x2d, 3);
	newPageNextChLst();
	if (XREF) {
		xputstr2cLst("CROSS-REFERENCE LISTING", 0);
		newLineLst();
		xputnstrLst("------------------------------------", 23);
		newLineLst();
	} else {
		xputstr2cLst("SYMBOL LISTING", 0);
		newLineLst();
		xputnstrLst("------------------------------------", 14);
		newLineLst();
	}
	setSkipLst(2);
	tabLst(-b678E);
	xputstr2cLst(" DEFN", 0);
	tabLst(-b678F);
	xputstr2cLst(" ADDR", 0);
	tabLst(-b6790);
	xputstr2cLst(" SIZE", 0);
	tabLst(-b6791);
	xputstr2cLst("NAME, ATTRIBUTES, AND REFERENCES", 0);
	newLineLst();
	tabLst(-b678E);
	xputnstrLst("------------------------------------", 5);
	tabLst(-b678F);
	xputnstrLst("------------------------------------", 5);
	tabLst(-b6790);
	xputnstrLst("------------------------------------", 5);
	tabLst(-b6791);
	xputnstrLst("------------------------------------", 32);
	newLineLst();
	newLineLst();

	for (p = 1; p <= w66D0; p++) {
		curInfo_p = ((word *)pw66D9)[p];
		if (getInfoType() == BUILTIN_T) {
			if (getInfoScope() != 0)
				sub_4C84();
		} else
			sub_4C84();
	}
}



void sub_480A()
{
	word p;

	if (!XREF) { //	loc_4815
		newLineLst();
		return;
	}

	w66D6 = getInfoScope();
	if ((word)w66D6 == 0) { //	loc_482B
		newLineLst();
		return;
	}
	xputnstrLst(": ", 2);
	p = 0;

	while ((word)w66D6 != 0) { //	loc_489B
		if (p != ((word *)w66D6)[1]) {
			if (PAGEWIDTH < col + 5) {
				newLineLst();
				tabLst(-b6793);
			}
			tabLst(1);
			xnumLst(((word *)w66D6)[1], 0, 10);
			p = ((word *)w66D6)[1];
		}
		w66D6 = ((word *)w66D6)[0];
	}
	if (col != 0)
		newLineLst();
}



void sub_48A7()
{
	static char dots[] = ". . . . . . . . . . . . . . . . . . . . ";

	curSymbol_p = getInfoSymbol();
	tabLst(-b6791);
	xputnstrLst(curSymbol_p + 1, *(byte *)curSymbol_p);
	xputnstrLst(&dots[*(byte *)curSymbol_p], b6792 - col - 2);
	tabLst(1);
}

void sub_48E2(word arg1w, word arg2w)
{
	tabLst(-b678F);
	xnumLst(arg1w, 0x0FC, 0x0F0);
	if (arg2w != 0) { //	locret_4920
		tabLst(-b6790);
		xnumLst(arg2w, 5, 0x0A);
	}
}


void sub_4921()
{
	w66D6 = getInfoScope();
	if (getInfoType() == BUILTIN_T)
		return;
	if ((word)w66D6 != 0 && (((word *)w66D6)[1] & 0x8000) != 0) {
		tabLst(-b678E);
		xnumLst(-((word *)w66D6)[1], 5, 10);
		setInfoScope(((word *)w66D6)[0]);
	} else if (!testInfoFlag(F_LABEL)) { //	locret_499B
		tabLst(-b678E);
		xputnstrLst("------------------------------------", 5);
	}
}

void sub_499C()
{
	xputstr2cLst(" EXTERNAL(", 0);
	xnumLst(getExternId(), 0, 10);
	xputnstrLst(")", 1);
}


void sub_49BB()
{
	word p, q, r;

	p = curInfo_p;
	xputstr2cLst(" BASED(", 0);
	curInfo_p = getBase();
	if (testInfoFlag(F_MEMBER)) { //	loc_49ED
		r = getInfoSymbol();
		curInfo_p = getOwningStructure();
		q = getInfoSymbol();
	} else {
		q = getInfoSymbol();
		r = 0;
	}

	curSymbol_p = q;
	xputnstrLst(curSymbol_p + 1, *(byte *)curSymbol_p);
	if (r != 0) { //	loc_4A33
		xputnstrLst(".", 1);
		curSymbol_p = r;
		xputnstrLst(curSymbol_p + 1, *(byte *)curSymbol_p);
	}
	xputnstrLst(")", 1);
	curInfo_p = p;
}


void sub_4A42()
{
	word p;

	p = curInfo_p;
	curInfo_p = getOwningStructure();
	xputstr2cLst(" MEMBER(", 0);
	curSymbol_p = getInfoSymbol();
	xputnstrLst(curSymbol_p + 1, *(byte *)curSymbol_p);
	xputnstrLst(")", 1);
	curInfo_p = p;
}

void sub_4A78(const char *arg1w)
{
	sub_4921();
	sub_48A7();
	xputstr2cLst(arg1w, 0);
	sub_480A();
}


void sub_4A92()
{
	const char *p;

	sub_4921();
	sub_48E2(getAddr(), getDimension());
	sub_48A7();
	xputstr2cLst("PROCEDURE", 0);
	if (getDataType() != 0) {
		if (getDataType() == 2)
			p = " BYTE";
		else
			p = " ADDRESS";
		xputstr2cLst(p, 0);
	}
	if (testInfoFlag(F_PUBLIC))
		xputstr2cLst(" PUBLIC", 0);

	if (testInfoFlag(F_EXTERNAL))
		sub_499C();

	if (testInfoFlag(F_REENTRANT))
		xputstr2cLst(" REENTRANT",0);

	if (testInfoFlag(F_INTERRUPT)) { //	loc_4B2A
		xputstr2cLst(" INTERRUPT(", 0);
		xnumLst(getIntrNo(), 0, 10);
		xputnstrLst(")", 1);
	}
	if (!testInfoFlag(F_EXTERNAL)) { //	loc_4B46
		xputstr2cLst(" STACK=", 0);
		xnumLst(getBasedOffset(), 0xfc, 0xf0);
	}
	sub_480A();
}

void sub_4B4A(const char *arg1w)
{
	word p;
	byte i;

	sub_4921();
	i = getInfoType();
	if (i == BYTE_T)
		p = 1;
	else if (i == ADDRESS_T) 
		p = 2;
	else if (i == LABEL_T)
		p = 0;
	else
		p = getOwningStructure();

	if (testInfoFlag(F_ARRAY))
		p *= getDimension();
	sub_48E2(getAddr(), p);
	sub_48A7();
	xputstr2cLst(arg1w, 0);
	if (testInfoFlag(2))
		sub_49BB();
	if (testInfoFlag(F_ARRAY)) {
		xputstr2cLst(" ARRAY(", 0);
		xnumLst(getDimension(), 0, 10);
		xputnstrLst(")", 1);
	}
	if (testInfoFlag(F_PUBLIC))
		xputstr2cLst(" PUBLIC",0);

	if (testInfoFlag(F_EXTERNAL)) 
		sub_499C();

	if (testInfoFlag(F_AT))
		xputstr2cLst(" AT", 0);
	if (testInfoFlag(F_DATA))
		xputstr2cLst(" DATA", 0);
	if (testInfoFlag(F_INITIAL))
		xputstr2cLst(" INITIAL", 0);

	if (testInfoFlag(F_MEMBER)) 
		sub_4A42();

	if (testInfoFlag(F_PARAMETER))
		xputstr2cLst(" PARAMETER", 0);

	if (testInfoFlag(F_AUTOMATIC))
		xputstr2cLst(" AUTOMATIC", 0);

	if (testInfoFlag(F_ABSOLUTE))
		xputstr2cLst(" ABSOLUTE",0);

	sub_480A();
}

static byte i;
void sub_4C84()
{
	
	curSymbol_p = getInfoSymbol();
	if (i != ((byte *)curSymbol_p)[1]) {
		newLineLst();
		i = ((byte *)curSymbol_p)[1];
	}
	if (getInfoType() < MACRO_T)
		switch(getInfoType()) {
		case LIT_T: sub_4A78("LITERALLY"); break;
		case LABEL_T: sub_4B4A("LABEL"); break;
		case BYTE_T: sub_4B4A("BYTE"); break;
		case ADDRESS_T: sub_4B4A("ADDRESS"); break;
		case STRUCT_T: sub_4B4A("STRUCTURE"); break;
		case PROC_T: sub_4A92(); break;
		case BUILTIN_T: sub_4A78("BUILTIN"); break;
	}
}



void sub_4D03()
{
	word p, q, r;
	byte i, j, /*ixibuf[128], */ k;

	openFile(&ixiFile, 2);
	//assignFileBuffer(&ixiFile, ixibuf, 128, 2);
	curInfo_p = off2Info(procInfo[1]);
	curSymbol_p = getInfoSymbol();
	if (curSymbol_p != 0) { //	loc_4D6D
		ixiWrite(&b3F0B, 1);
		k = *(byte *)curSymbol_p + 22;
		ixiWrite(&k, 1);
		ixiWrite(curSymbol_p, 1 + *(byte *)curSymbol_p);
	}
	if (srcFileTable[0].filename[0] == ':')
		j = 2;
	else
		j = 0;
	ixiWrite((byte *)&srcFileTable[0].filename[j*2], 10);
	ixiWrite((byte *)"---------", 9);

	for (p = 1; p <= w66D0; p++) {
		curInfo_p = ((word *)pw66D9)[p];
		i = getInfoType();
		if (i >= LIT_T && i <= PROC_T &&
		   (testInfoFlag(F_PUBLIC) || (testInfoFlag(F_EXTERNAL) && !testInfoFlag(F_AT)))) {
			if (testInfoFlag(F_PUBLIC))
				ixiWrite(&b40DE, 1);
			else
				ixiWrite(&b40DF, 1);

			curSymbol_p = getInfoSymbol();
			k = *(byte *)curSymbol_p + 6;
			ixiWrite(&k, 1);
			ixiWrite(curSymbol_p, 1);
			ixiWrite(curSymbol_p + 1, *(byte *)curSymbol_p);
			ixiWrite(&i, 1);
			if (getInfoType() == PROC_T) {
				r = getDataType();
				ixiWrite((byte *)&r, 2);	// not portable
			} else {
				if (testInfoFlag(F_ARRAY))
					q = getDimension();
				else
					q = 0;
				ixiWrite((byte *)&q, 2);	// not portable
			}
		}
	}
//	flushFile(&ixiFile);
	closeFile(&ixiFile);
}

void ixiWrite(byte *arg1w, word arg2w)
{
	ifwrite(&ixiFile, arg1w, arg2w);
}


void sub_4EC5()
{
	sub_4140();
	sub_4287();
	sub_451A();
	if ((SYMBOLS || XREF) && PRINT)
		sub_46C4();
	if (IXREF)
		sub_4D03();
}

void sub_557A()
{
	byte i, line[30], j;
	word actual;

	for (i = 0; i <= 25; i++)
		debugSwitches[i] = 0;
	if (!OBJECTSet)
		return;

	printf("ENTER DEBUG SWITCHES\r\n*");
	readFile(&conFile, line, 30, &actual);
	if (actual != 0) { //	loc_5615
		--actual;
		for (i = 0; i <= actual; i++) {
			if ((j = line[i]) >= 'A' && j <= 'Z')
				debugSwitches[j - 'A'] = 0xff;	
		}
	}
	printf("BEGINNING PHASE EXECUTION\r\n");
}


byte sub_561E(byte arg1b)
{
	if (debugSwitches[arg1b - 'A'])
		return 0xff;
	return 0;
}


