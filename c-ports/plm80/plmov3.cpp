// $Id: plmov3.cpp,v 1.1 2003/10/04 21:08:48 Mark Ogden Exp $
#include <stdio.h>
#include "plm.hpp"
#include "common.hpp"
#include "trace.hpp"


byte b3EBF = 0xA4;
byte b3EC0 = 0x9C;

#pragma pack(push, 1)

typedef struct {
		byte type;
		word len;
		byte data[300];
} rec300;

typedef struct {
		byte type;
		word len;
		byte data[1021];	// possibly 1020
} rec1020;

struct {
		byte type;
		word len;
		byte data[51];
} rec2 = {2};

struct {
		byte type;
		word len;
		byte seg;
		word addr;
		byte data[300];
} rec6 = {6};

typedef struct {
		byte type;
		word len;
		byte data[150];
} rec150;

#pragma pack(pop)

static byte tx1Buf[1280];
byte nmsBuf[1280];
static byte atBuf[1280];
byte objBuf[1280];
word w7197;
byte b7199;
rec150 rec22 = {0x22,0 , {3}};	// might be rec151
rec150 rec24_1 = {0x24, 0, {1, 3}};	// might be rec151
rec150 rec24_2 = {0x24, 0, {2, 3}};	// might be rec151
rec150 rec24_4 = {0x24, 0, {4, 3}};	// might be rec151
rec150 rec20 = {0x20,0 , {3}};
static atFData_t atFData;
byte ba8016[255];
word w8115;
word w8117;
word w8119;
word w811B;
byte b811D;

byte b4789[] = {
	2, 2, 3, 4, 3, 4, 2, 2, 3, 4, 2, 3, 2, 3, 3, 3,
	3, 2, 2, 3, 4, 2, 3, 2, 3, 2, 2, 2, 2, 3, 2, 2,
	2, 3, 2, 3, 2, 2, 3, 2, 2, 1, 2, 2, 3, 4
};

byte b47B7[] = {
	   0,    2,    4,    7,  0xB,  0xE, 0x12, 0x14,
	0x16, 0x19, 0x1D, 0x1F, 0x22, 0x24, 0x27, 0x2A,
	0x2D, 0x30, 0x32, 0x34, 0x37, 0x3B, 0x3D, 0x40,
	0x42, 0x45, 0x47, 0x49, 0x4B, 0x4D, 0x50, 0x52,
	0x54, 0x56, 0x59, 0x5B, 0x5E, 0x60, 0x62, 0x65,
	0x67, 0x69, 0x6A, 0x6C, 0x6E, 0x71, 0x24, 0x24,
	0x24, 0x24, 0x13, 0x13, 0x18, 0x18, 0x18, 0x18,
	0x16, 0x2C, 0x15, 0x1F, 0x1F, 0x20, 0x20, 0x19,
	0x19, 0x19, 0x19,    8,    8,    9,    9,    6,
	   7, 0x25, 0x25, 0x25, 0x25, 0x25,  0xA,  0xA,
	 0xB,  0xB, 0x14, 0x14, 0x14, 0x14, 0x14, 0x39,
	0x1A, 0x1A, 0x1A, 0x1A
};

byte b4813[] = {
	3, 7, 3,    7,   2, 3,   8, 1, 3,   1, 8, 2, 3,   8,    1,   3,
	1, 8, 3,    7,   3, 7,   2, 3, 8,   1, 3, 1, 8,   2, 0x1D,   3,
	1, 7, 2, 0x12,   2, 1, 0xA, 2, 1,   8, 2, 1, 8,   2,    1,   7,
	3, 7, 3,    7,   2, 3,   8, 1, 3,   1, 8, 1, 6,   3,    1, 0xB,
	1, 6, 3,    1, 0xB, 1,   6, 1, 6,   3, 7, 3, 7,   2,    3,   8,
	3, 8, 3,    9,   1, 6,   3, 1, 6,   1, 7, 3, 1, 0xC,    3,   7,
	3, 7, 2,    3,   8, 3,   8, 3, 9, 0xC, 3, 7, 3,   7,    2,   3,
	8, 1, 3,    1,   8
};


void recAddWord(rec *arg1w, byte arg2b, word arg3w);
void recAddByte(rec *arg1w, byte arg2b, byte arg3b);
void writeRec(rec *arg1w, byte arg2b);
void advNextInfo();
void sub_4DA8();
void sub_4D85();
void sub_4D13();
void sub_4CF9();
void sub_4CAC();
void sub_4C7A();
void sub_4BF4();
void sub_4B6C();
void sub_4A31();
void sub_49F9();
void writeError(word arg1w, word arg2w, word arg3w);
word sub_4984();
word sub_4938();
void sub_4908(rec *arg1w, word arg2w, byte arg3b);
void AddRecPStr(rec *recp, byte off, byte len, byte *str);
void sub_4889();
void sub_4746();
void sub_46B7();
void sub_436C();
void sub_426E();
void sub_4201();
void sub_4105();
void sub_40B6();
void sub_402F();
void sub_3FE2();
void sub_3F3C();


int overlay3()
{
	sub_3F3C();
	if (IXREF || b7199)
		sub_3FE2();
	sub_402F();
	if (IXREF || b7199) {
		sub_40B6();
		sub_4105();
		initDataSize = w7197;
		sub_4201();
	}
	if (OBJECT) {
		sub_426E();
		sub_436C();
		sub_46B7();
	}
	sub_49F9();
	sub_4746();
	if (b7199)
		return 4;
	closeFile(&tx1File);
	deleteFile(&tx1File);
	if (IXREF)
		return 5;
	printExitMsg();
	return -1;
}


void sub_3F3C()
{
	b7199 = OBJECT | PRINT;
	if (OBJECTSet) {
		deleteFile(&objFile);
		OBJECTSet = 0;
	}
	if (!lstFileOpen && PRINTSet) {
		deleteFile(&lstFile);
		PRINTSet = 0;
	}
	closeFile(&tx2File);
	deleteFile(&tx2File);
//	assignFileBuffer(&tx1File, tx1Buf, 1280, 2);
//	if (IXREF || b7199)
//		assignFileBuffer(&nmsFile, nmsBuf, 1280, 1);
//	assignFileBuffer(&atFile, atBuf, 1280, 1);
	if (OBJECT) {
		openFile(&objFile, 3);
//		assignFileBuffer(&objFile, objBuf, 1280, 2);
		SeekEnd(&objFile);
	}
	w7197 = initDataSize;
}




void sub_3FE2()
{
	word p;
	for (p = 1; p <= procCnt; p++) {
		curInfo_p = off2Info(procInfo[p]);
		if (! testInfoFlag(F_EXTERNAL)) {
			setAddr(w7197);
			w7197 += getDimension();
		}
	}
}



void sub_402F()
{
	word p, q;
	curInfo_p = off2Info(2);
	while (curInfo_p != 0) {
		if (getInfoType() == LABEL_T) {
			if (! testInfoFlag(F_LABEL)) 
				writeError(172, info2Off(curInfo_p), 0);
			else if (! testInfoFlag(F_EXTERNAL)) {
				p = getAddr();
				q = curInfo_p;
				curInfo_p = off2Info(procInfo[getInfoScope() >> 8]);
				p += getAddr();
				curInfo_p = q;
				setAddr(p);
			}
		}
		advNextInfo();
	}
}




void sub_40B6()
{
	word p;

	for (p = 1; p <= localLabelCnt; p++) {
		curInfo_p = off2Info(procInfo[((byte *)word_381E)[p]]);
		((word *)localLabels_p)[p] += getAddr();
	}
}




void sub_4105()
{
	byte i, j, k, m;
	word p;

	if (!byte_3C3B)
		return;

	ifwrite(&tx1File, &b3EBF, 1);
	ifwrite(&tx1File, &procInfo[1], 2);
	curInfo_p = off2Info(procInfo[1]);
	p = w7197 - getAddr();
	ifwrite(&tx1File, &p, 2);
	for (i = 0; i <= 0x2d; i++) {
		j = (k = b47B7[i]) + b4789[i];
		m = 0;
		while (j > k) { // loc_41F9
			if (m) {
				((word *)helpers_p)[k] = w7197;
				w7197 += b4813[k];
			} else if (((word *)helpers_p)[k] != 0) {
				m = 0xFF;
				((word *)helpers_p)[k] = w7197;
				w7197 += b4813[k];
			}
			k++;
		}
	}
}



void sub_4201()
{
	byte i;

	curSymbol_p = (topSymbol = localLabels_p - 3) - 1;
	ifread(&nmsFile, &i, 1);
	while (i != 0) {
		curSymbol_p -= i + 1;
		curSymbol_p->name[0] = i;
		ifread(&nmsFile, &curSymbol_p->name[1], i);
		ifread(&nmsFile, &i, 1);
	}
	botSymbol = curSymbol_p + 4;
	botMem = (word)botSymbol;
}


void sub_426E()
{

	curInfo_p = off2Info(procInfo[1]);
	if ((curSymbol_p = getInfoSymbol()) == 0)
		recAddByte((rec *)&rec2, 0, 0);
	else 
		AddRecPStr((rec *)&rec2, 0, curSymbol_p->name[0], &curSymbol_p->name[1]);
	recAddByte((rec *)&rec2, 0, 1);
	recAddByte((rec *)&rec2, 0, (version[1] << 4) | (version[3] & 0xf));
	recAddByte((rec *)&rec2, 0, 1);
	recAddWord((rec *)&rec2, 0, initDataSize);
	recAddByte((rec *)&rec2, 0, 3);
	recAddByte((rec *)&rec2, 0, 2);
	recAddWord((rec *)&rec2, 0, uninitDataSize);
	recAddByte((rec *)&rec2, 0, 3);
	recAddByte((rec *)&rec2, 0, 3);
	recAddWord((rec *)&rec2, 0, sub_4938());
	recAddByte((rec *)&rec2, 0, 3);
	recAddByte((rec *)&rec2, 0, 4);
	recAddWord((rec *)&rec2, 0, 0);
	recAddByte((rec *)&rec2, 0, 3);
	writeRec((rec *)&rec2, 0);
}


rec300 rec18 = { 0x18};
rec300 rec16_1 = { 0x16};
rec300 rec16_2 = { 0x16, 0, {1}};
rec300 rec16_3 = { 0x16, 0, {2}};
rec300 rec16_4 = { 0x16, 0, {4}};
rec1020 rec12 = { 0x12, 0, {1}};

void sub_436C()
{
	rec *p;
	word  q;
	byte i, j;
	word r, s;
	byte k, m;
	byte t[6];



	s = 0;
	j = 0;
	curInfo_p = off2Info(2);
	while (curInfo_p != 0) {
		curSymbol_p = getInfoSymbol();
		if (getInfoType() >= LABEL_T && getInfoType() <= PROC_T && curSymbol_p != 0) { // 45E6
			if (testInfoFlag(F_EXTERNAL) && !testInfoFlag(F_AT)) { // 4418
				if (rec18.len + curSymbol_p->name[0] + 2 >= 299)
					writeRec((rec *)&rec18.type, 0);
				++s;
				AddRecPStr((rec *)&rec18.type, 0, curSymbol_p->name[0], &curSymbol_p->name[1]);
				recAddByte((rec *)&rec18.type, 0, 0);
			} else if (!(testInfoFlag(F_AUTOMATIC) || testInfoFlag(F_BASED) || testInfoFlag(F_MEMBER))) {
				if (testInfoFlag(F_DATA) || getInfoType() == LABEL_T || getInfoType() == PROC_T) {
					p = (rec *)&rec16_2;
					q = rec16_2.len;
					i = 1;
				} else if (testInfoFlag(F_MEMORY)) { // loc_4485
					p = (rec *)&rec16_4;
					q = rec16_4.len;
					i = 4;
				} else if (testInfoFlag(F_ABSOLUTE)) { // loc_44A2
					p = (rec *)&rec16_1;
					q = rec16_1.len;
					i = 0;
				} else {
					p = (rec *)&rec16_3;
					q = rec16_3.len;
					i = 2;
				}

				if (testInfoFlag(F_PUBLIC)) { // loc_4514
					if (q + curSymbol_p->name[0] + 4 >= 299)
						writeRec(p, 1);
					recAddWord(p, 1, getAddr());
					AddRecPStr(p, 1, curSymbol_p->name[0], &curSymbol_p->name[1]);
					recAddByte(p, 1, 0);
				}
				if (DEBUG) { // loc_45E6
					if (curInfo_p == off2Info(procInfo[1]))
						j = 0xFF;
					else if (! testInfoFlag(F_PARAMETER))
						j = testInfoFlag(F_EXTERNAL);
					else {
						r = curInfo_p;
						curInfo_p = off2Info(procInfo[getInfoScope()>>8]);
						j = testInfoFlag(F_EXTERNAL);
						curInfo_p = r;
					}
					if (!j) { // loc_45E6
						if (rec12.data[0] != i || curSymbol_p->name[0] + rec12.len + 4 >= 1019)
							writeRec((rec *)&rec12, 1);
						rec12.data[0] = i;
						recAddWord((rec *)&rec12, 1, getAddr());
						AddRecPStr((rec *)&rec12, 1, curSymbol_p->name[0], &curSymbol_p->name[1]);
						recAddByte((rec *)&rec12, 1, 0);
					}
				}
			}
		}
		advNextInfo();
	} // of while

	t[0] = '@';
	t[1] = 'P';
	if (!byte_3C3B) {
		for (k = 0; k <= 0x74; k++) {
			if (((word *)helpers_p)[k] != 0) {
				((word *)helpers_p)[k] = s;
				++s;
				if (rec18.len + 8 >= 299)
					writeRec((rec *)&rec18, 0);
				m = num2Asc(k, 0xfc, 10, (char *)&t[2]);
				AddRecPStr((rec *)&rec18, 0, 6, t);
				recAddByte((rec *)&rec18, 0, 0);
			}
		}	
	}
	writeRec((rec *)&rec18, 0);
	writeRec((rec *)&rec16_1, 1);
	writeRec((rec *)&rec16_2, 1);
	writeRec((rec *)&rec16_3, 1);
	writeRec((rec *)&rec16_4, 1);
	writeRec((rec *)&rec12, 1);
}



void sub_46B7()
{
	word p;

	if (intVecNum == 0)
			return;
	for (p = 1; p <= procCnt; p++) {
		curInfo_p = off2Info(procInfo[p]);
			if (testInfoFlag(F_INTERRUPT)) { 
				rec6.addr = intVecLoc + intVecNum * getIntrNo();
				recAddByte((rec *)&rec6, 3, 0xC3);
				recAddWord((rec *)&rec6, 3, getAddr());
				writeRec((rec *)&rec6, 3);
				recAddWord((rec *)&rec24_1, 2, rec6.addr + 1);
				writeRec((rec *)&rec24_1, 2);
		}
	}
}



void sub_4746()
{
	if (IXREF || b7199) {
		ifwrite(&tx1File, &b3EC0, 1);
//		flushFile(&tx1File);
		rewindFile(&tx1File);
		closeFile(&nmsFile);
		deleteFile(&nmsFile);
//		flushFile(&objFile);
	}
	closeFile(&atFile);
	deleteFile(&atFile);
}


byte b4888 = 0xA3;



void sub_4889()
{
	writeRec((rec *)&rec6, 3);
	writeRec((rec *)&rec22, 1);
	writeRec((rec *)&rec24_1, 2);
	writeRec((rec *)&rec24_2, 2);
	writeRec((rec *)&rec24_4, 2);
	writeRec((rec *)&rec20, 1);
}



void AddRecPStr(rec *recp, byte off, byte len, byte *str)
{
	byte i;
	recAddByte(recp, off, len);
	for (i = 0; len != i; i++) {
		recAddByte(recp, off, str[i]);
	}
}




void sub_4908(rec *arg1w, word arg2w, byte arg3b)
{
	if (arg1w->len + arg3b >= arg2w) {
		sub_4889();
		rec6.addr = w7197;
	}
}




word sub_4938()
{
	word p, q;
	q = 0;
	for (p = 1; p <= procCnt; p++) {
		curInfo_p = off2Info(procInfo[p]);
		if (q < getBasedOffset())
			q = getBasedOffset();
	}
	return q;
}




word sub_4984()
{
	switch (getInfoType()) {
	case BYTE_T:	return 1;
	case ADDRESS_T:	return 2;
	case STRUCT_T: return getOwningStructure();
	}
	return 0;
}


void writeError(word arg1w, word arg2w, word arg3w)
{
	word tmp[3];
	tmp[0] = arg1w;
	tmp[1] = arg2w;
	tmp[2] = arg3w;

	if (b7199) { // loc_49F1
		ifwrite(&tx1File, &b4888, 1);
		ifwrite(&tx1File, tmp, 6);
	} else
		++programErrCnt;
}



void sub_49F9()
{
	while (1) {
		ifread(&atFile, &atFData.type, 1);
		if (atFData.type == AT_AHDR)
			ifread(&atFile, &atFData.info_p, 12);
		else if (atFData.type == AT_DHDR)
			sub_4A31();
		else
			return;
	}
}
	

void sub_4A31()
{
word w811E;
	ifread(&atFile, &atFData.info_p, 4);
	curInfo_p = off2Info(w811E = atFData.info_p);
	w8117 = w8115 = w811B = w8119 = 0;
	if (testInfoFlag(F_DATA)) 
		rec6.seg = 1;
	else if (testInfoFlag(F_MEMORY))
		rec6.seg = 4;
	else if (testInfoFlag(F_ABSOLUTE))
		rec6.seg = 0;
	else
		rec6.seg = 2;

	rec6.addr = getAddr();
	w7197 = rec6.addr;
	if (off2Info(0) == curInfo_p)
		b811D = 0;
	else if (testInfoFlag(F_EXTERNAL)) {
		writeError(217, w811E, atFData.stmtNum);
		b811D = 0;
	} else {
		sub_4B6C();
		b811D = 0xff;
	}

	sub_4BF4();
	if (atFData.type == AT_END) {
		sub_4889();
		return;
	}

	while (1) {
		sub_4C7A();
		sub_4BF4();
		if (atFData.type == AT_END) { 
			sub_4889();
			return;
		} else if (b811D) {
			if (w811B > 1) 
				--w811B;
			else {
				advNextInfo();
				while (curInfo_p != 0) {
					if (getInfoType() >= BYTE_T && getInfoType() <= STRUCT_T)
						break;
					advNextInfo();
				}
				sub_4B6C();
				if (!b811D) 
					writeError(0xd1, w811E, atFData.stmtNum);
			}
		}
	}
}



void sub_4B6C()
{
	if (curInfo_p == 0 || !testInfoFlag(F_MEMBER)) {
		if (w8119 > 1) {
			--w8119;
			curInfo_p = atFData.info_p;
		} else if (curInfo_p == 0) {
			b811D = 0;
			return;
		} else {
			if (! testInfoFlag(F_PACKED))
				b811D = 0;
			if (getInfoType () == STRUCT_T) {
				if (testInfoFlag(F_ARRAY))
					w8119 = getDimension();
				advNextInfo();
				atFData.info_p = curInfo_p;
			}
		}
	}
	if (testInfoFlag(F_ARRAY))
		w811B = getDimension();
}



void sub_4BF4()
{
	if (!b811D || w8117 >= w8115) {
		ifread(&atFile, &atFData.type, 1);
		switch (atFData.type) {
		case AT_2: ifread(&atFile, &atFData.val, 2);
			break;
		case AT_STRING:
			ifread(&atFile, &w8115, 2);
			ifread(&atFile, ba8016, w8115);
			w8117 = 0;
			break;
		case AT_DATA:
			ifread(&atFile, &atFData.varInfoOffset, 8);
			break;
		case AT_END: break;
		}
	}
}



void sub_4C7A()
{
	if (b811D) {
		switch (atFData.type) {
		case AT_2: sub_4D85(); break;
		case AT_STRING: sub_4D13(); break;
		case AT_DATA: sub_4DA8(); break;
		}
	}
}



void sub_4CAC()
{
	if (getInfoType() == BYTE_T) {
		sub_4908((rec*)&rec6, 0x12c, 1);
		recAddByte((rec*)&rec6, 3, (byte)atFData.val);
		++w7197;
	} else {
		sub_4908((rec*)&rec6, 0x12C, 2);
		recAddWord((rec*)&rec6, 3, atFData.val);
		w7197 += 2;
	}
}

void sub_4CF9()
{
	writeError(210, info2Off(curInfo_p), atFData.stmtNum);
	sub_4CAC();
}



void sub_4D13()
{
	byte *w8120;
	if (getInfoType() == BYTE_T) {
		atFData.val = ba8016[w8117];
		++w8117;
	} else {
			w8120 = (byte *)&atFData.val;
			w8120[1] = ba8016[w8117];
			++w8117;
			if (w8117 < w8115) {
				w8120[0] = ba8016[w8117];
				++w8117;
			} else {
					w8120[0] = w8120[1];
					w8120[1] = 0;
			}
	}
	sub_4CAC();
}



void sub_4D85()
{
	if (atFData.val > 255 && getInfoType() == BYTE_T)
		sub_4CF9();
	else
		sub_4CAC();
}



void sub_4DA8()
{
	byte i, j;
	word p;
	rec *q;

	if (atFData.varInfoOffset == 0)
		sub_4D85();
	else if (getInfoType() == BYTE_T)
		sub_4CF9();
	else {
		p = curInfo_p;
		curInfo_p = off2Info(atFData.varInfoOffset);
		if (testInfoFlag(F_MEMBER)) { // loc_4E01
			atFData.val += getAddr() + sub_4984() * atFData.varNestedArrayIndex;
			curInfo_p = getOwningStructure();
		}

		atFData.val += getAddr() + sub_4984() * atFData.varArrayIndex;
		if (testInfoFlag(F_EXTERNAL)) { // loc_4E5F
			i = getExternId();
			curInfo_p = p;
			sub_4908((rec *)&rec20, 0x95, 4);
			sub_4CAC();
			recAddWord((rec *)&rec20, 1, i);
			recAddWord((rec *)&rec20, 1, w7197 - 2);
		} else if (testInfoFlag(F_ABSOLUTE)) { // loc_4E74
			curInfo_p = p;
			sub_4CAC();
		} else {
			if (getInfoType() == PROC_T || getInfoType() == LABEL_T || testInfoFlag(F_DATA)) {
				q = (rec *)&rec24_1;
				i = 1;
			} else if (testInfoFlag(F_MEMORY)) {
				q = (rec *)&rec24_4;
				i = 4;
			} else {
				q = (rec *)&rec24_2;
				i = 2;
			}

			if (rec6.seg == i) {
				q = (rec *)&rec22;
				j = 1;
			} else
				j = 2;

			curInfo_p = p;
			sub_4908(q, 0x95, 2);
			sub_4CAC();
			recAddWord(q, j, w7197 - 2);
		}
	}
}


void writeRec(rec *recp, byte adjust)
{
	byte *buf;
	byte crc;
	word p;
	word len;

	buf = (byte *)recp;
	if (recp->len > 0 && OBJECT) {
			recp->len += adjust + 1;
			len = recp->len + 2;
			crc = 0;
			for (p = 0; p < len; p++)
				crc -= buf[p];
			buf[len] = crc;		// put in checksum
			ifwrite(&objFile, buf, len + 1);
	}
	recp->len = 0;
}



void recAddByte(rec *recp, byte off, byte val)
{
	recp->data[recp->len++ + off] = val;
}



void recAddWord(rec *recp, byte off, word val)
{
	recAddByte(recp, off, (byte)val);
	recAddByte(recp, off, (byte)(val >> 8));
}




