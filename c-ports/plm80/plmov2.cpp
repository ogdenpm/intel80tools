// $Id: plmov2.cpp,v 1.1 2003/10/04 21:08:48 Mark Ogden Exp $
#include <stdio.h>
#include "plm.hpp"
#include "common.hpp"
#include "trace.hpp"



void sub_3F27();
void sub_3F7D();
void wrFragData();
void putTx1Byte(byte arg1b);
void putTx1Word(word arg1w);
void encodeFragData(byte arg1b);
void sub_5410();
void sub_545D();
void emitTopItem();
void tx2SyntaxError(byte arg1b);
byte sub_5679(byte arg1b);
void sub_56A0(byte arg1b, byte arg2b);
byte sub_5748(byte arg1b);
word sub_575E(word arg1w);
void sub_5795(word arg1w);
byte enterBlk();
byte exitBlk();
void sub_58F5(byte arg1b);
void sub_597E();
void sub_5B96(byte arg1b, byte arg2b);
void sub_5C1D(byte arg1b);
void sub_5C97(byte arg1b);
void sub_5D27(byte arg1b);
void sub_5D6B(byte arg1bx);
void sub_5E16();
void sub_5E66(byte arg1b);
void sub_5EE8();
void sub_5F4B(word arg1w, word arg2w, byte arg3b, byte arg4b);
void sub_5FBF(byte arg1b, word *arg2w, word *arg3w);
void sub_611A();
void sub_61A9(byte arg1b);
void sub_61E0(byte arg1b);
void sub_636A(byte arg1b);
void sub_63AC(byte arg1b);
void sub_6416(byte arg1b);
void getTx2Item();
void sub_652B();
void fillTx2Q();
void sub_67A9();
void skipBB(byte arg1b, byte arg2b);
void sub_689E();
void sub_68E8();
void sub_69EB(byte arg1b, word *arg2w);
void sub_6AA4();
void sub_6BD6();
void sub_6C54(byte arg1b);
void sub_6C73(byte arg1b);
void sub_6D52();
void sub_6EAB(word *arg1w);
void sub_6EE1(byte arg1b);
void sub_6F20(byte arg1b);
byte sub_6FE2()	;
void sub_7018(byte arg1b);
void sub_7055();
byte bsub_70BC(byte arg1b);
void sub_7111();
void sub_717B();
byte bsub_7254(byte arg1b, word arg2w);
word sub_72F2();
void sub_73C5();
void sub_7550();
byte bsub_765B(byte arg1b, byte arg2b, byte arg3b);
byte bsub_76E2(byte arg1b);
void sub_7754();
byte bsub_7801();
void sub_7925();
void sub_7A85();
void sub_7D32();
void sub_7D54();
void sub_7D7E(byte arg1b);
void sub_7DA9();
void sub_7F19();
byte bsub_7FD9(byte arg1b);
void sub_7FFC();
void sub_8086();
void sub_8148(byte arg1b, byte arg2b);
void sub_8207();
void sub_841E();
void sub_84ED();
byte sub_8683(byte arg1b);
void sub_8698(byte arg1b, byte arg2b);
void sub_87CB();
byte bsub_8861();
void sub_88C1();
void sub_894A();
void sub_89D1();
void sub_8A9C();
void sub_8CF5();
void sub_8DCD();
byte sub_8E7E(byte arg1b);
void sub_8ECD(byte arg1b, byte arg2b);
void sub_8F16();
void sub_8F35();
void sub_90EB();
void sub_940D();
void sub_9457();
void findParamInfo(byte arg1b);
void sub_9514();
void sub_9560();
void sub_9624(word arg1w);
void sub_9646(word arg1w);
void inxh();
void opB(byte arg1b);
void opD(byte arg1b);
void sub_9706();
void movDEM();
void sub_975F();
void sub_978E();
void sub_981C();
void sub_994D();
void sub_9BB0();
byte bsub_9C33();
void sub_9D06();
void sub_9DD7();
void sub_9EAA(byte arg1b, byte arg2b);
void sub_9EF8();
void sub_9F14();
void sub_9F2F();
void sub_9F9F();
void sub_A072(byte arg1b);
void sub_A0C4();
void sub_A10A();
void sub_A153();
void sub_A266();
void advNextInfo();



static byte tx2Buf[512];
static byte tx1Buf[512];
word blkCurInfo[20];
word wB488[20];
word wB4B0[20];
word wB4D8[20];
byte extProcId[20];
byte procChainNext[20];
word wB528[10];
word wB53C[10];
byte tx2opc[255] = { T2_SEMICOLON, T2_LOCALLABEL, T2_SEMICOLON, T2_SEMICOLON };
byte tx2Aux1b[255] = { 0xc, 9 };
byte tx2Aux2b[255];
word tx2op1[255];
word tx2op2[255];
word tx2op3[255];
word tx2Auxw[255] = { 0, 1};
byte bC045[9];
byte bC04E[9];
byte boC057[9];
byte boC060[9];
byte boC069[9];
byte boC072[9];
byte boC07B[9];
word wC084[9];
word wC096[9];
byte bC0A8[9];
byte bC0B1;
byte bC0B2;
byte bC0B3[2];
byte bC0B5[2];
byte bC0B7[2];

byte bC0B9[2];
byte bC0BB[2];
byte bC0BD[2];
byte bC0BF[2];
byte bC0C1[2];
byte bC0C3[125];
byte bC140[126];
byte tx2qp;
byte bC1BF = 4;
byte tx2qEnd = 4;
word pc;
word wC1C3;
word wC1C5;
word wC1C7;
byte blkSP;
byte blkOverCnt;
byte procCallDepth;
bool boC1CC;
bool boC1CD;
byte eofSeen;
word wC1CF;
byte curOp;
byte bC1D2;
byte padC1D3;
byte curExtProcId = 1;
byte procChainId;
word wC1D6;
bool boC1D8;
byte bC1D9;
byte cfrag1;
byte bC1DB;
word wC1DC[5];
byte bC1E6;
byte buf_C1E7[34];
const byte bC209[] = {4, 5, 3, 2, 0, 1};
bool boC20F;

byte MEMORY;

//	.db 2, 2, 3, 4,	3, 4, 2, 2, 3, 4, 2, 3,	2, 3, 3, 3, 3
//	.db 2, 2, 3, 4,	2, 3, 2, 3, 2, 2, 2, 2,	3, 2, 2, 2, 3
//	.db 2, 3, 2, 2,	3, 2, 2, 1, 2, 2, 3, 4


const byte b3FCD[] = {
	   0,    2,    4,    7, 0x0B, 0x0E, 0x12, 0x14,
	0x16, 0x19, 0x1D, 0x1F, 0x22, 0x24, 0x27, 0x2A,
	0x2D, 0x30, 0x32, 0x34, 0x37, 0x3B, 0x3D, 0x40,
	0x42, 0x45, 0x47, 0x49, 0x4B, 0x4D, 0x50, 0x52,
	0x54, 0x56, 0x59, 0x5B, 0x5E, 0x60, 0x62, 0x65,
	0x67, 0x69, 0x6A, 0x6C, 0x6E, 0x71, 0x24, 0x24,
	0x24, 0x24, 0x13, 0x13, 0x18, 0x18, 0x18, 0x18,
	0x16, 0x2C, 0x15, 0x1F, 0x1F, 0x20, 0x20, 0x19,
	0x19, 0x19, 0x19,    8,    8,    9,    9,    6,
	   7, 0x25, 0x25, 0x25, 0x25, 0x25, 0x0A, 0x0A,
	0x0B, 0x0B, 0x14, 0x14, 0x14, 0x14, 0x14, 0x39,
	0x1A, 0x1A, 0x1A, 0x1A
};

const byte bt4029[] = {	// indexed by T2 item
	    0,    0,    0,    0, 0x26, 0x30, 0x30, 0x26,
	 0x30, 0x20, 0x30, 0x12, 0x12, 0x12,    0, 0x10,
	 0x10, 0x10, 0x10, 0x10, 0x10, 0x60,    0, 0x26,
	 0x20, 0x20,    0,    0,    0,    0,    0,    0,
	 0x10, 0x80, 0x80, 0x80, 0x90, 0x90, 0x40, 0xA0,
	 0xA0, 0xA0, 0x80, 0xB0, 0x90, 0x80, 0xB0, 0x90,
	 0x80, 0xB0, 0x90, 0x80, 0xB0, 0x90, 0x80, 0xB0,
	 0x90, 0x20, 0x30, 0x30, 0x30, 0x10, 0x10, 0x70,
	 0x70, 0x30, 0x30, 0x30, 0x30, 0x80, 0x80, 0x80,
	 0x80, 0x80, 0x80, 0x80, 0x80,    0,    0,    0,
	    0,    0,    0,    0,    0,    0,    0,    0,
	 0x20, 0x20,    0,    0, 0x20,    0,    0, 0x2C,
	 0x40,    0, 0x10, 0x10, 0x60, 0x20,    0,    0,
	 0xA0, 0xA0, 0xA0, 0xA0, 0x32, 0x16, 0x10, 0x20,
	    0, 0x10, 0x10, 0x10, 0x10, 0x10, 0x60,    0,
	    0,    0,    0,    0,    0,    0,    0,    0,
	    0,    0,    0,    0,    0,    0,    0, 0x70,
	 0x60, 0x60, 0x70, 0x50, 0x70, 0x60, 0x60, 0xE0,
	    0,    0,    0,    0,    0,    0,    0,    0,
	    0,    0,    0,    0,    0,    0,    0,    0,
	    0,    0,    0,    0,    0,    0, 0x80, 0x80,
	 0x80, 0x80, 0x80, 0x80,    0,    0, 0x80, 0x80,
	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80
};

const byte fragFlg_Len[] = {	// indexed by T2 item
	    0, 0x20, 0x40, 0x60, 0x81, 0x81, 0x84, 0x83,
	 0x83,    1, 0x83, 0x82, 0x83, 0x81, 0x81, 0x82,
	 0x83, 0x83,    1, 0x83, 0x81, 0x82, 0x81, 0x81,
	 0x81, 0x81, 0x81, 0x81, 0x21, 0x22, 0x23, 0x24,
	 0x82, 0x81, 0x82, 0x83,    1,    2,    2, 0x21,
	 0x22, 0x23,    1,    2,    1,    2,    3,    2,
	    4,    5,    4,    4,    5,    4,    3,    4,
	    3, 0x21, 0x85, 0x83, 0x83, 0x84, 0x83,    3,
	 0x23, 0x88, 0x89, 0x8D, 0x91,    1,    2,    3,
	    4,    5,    4,    3,    2,    5,    4,    3,
	    2,    4,    5,    6,    6,    5,    4,    3,
	 0x62, 0x41,    1,    2,    2,    2,    1, 0x8A,
	 0x82, 0x83,    1,    1, 0x84, 0x61, 0x0C, 0x0E,
	 0x41, 0x42, 0x61, 0x62,    3,    2,    1, 0x62,
	 0x61,    0,    0,    0,    0,    0, 0x84,    0,
	    0,    0,    0,    0,    0,    0,    0,    0,
	    0,    0,    0,    0,    0,    0, 0x20, 0x20,
	 0x20, 0x20, 0x20, 0x22, 0x23, 0x23, 0x23, 0x23,
	    6,    6,    6,    1,    1,    1,    2, 0x20,
	 0x60,    3, 0x20, 0x40,    0,    0,    0,    0,
	    0,    0, 0x60, 0x60, 0x40,    0, 0x80, 0x84,
	    7,    7,    4,    5,    0,    0, 0x84, 0x84,
	 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	 0x84,    7,    7,    7,    7,    7,    7,    7,
	    7,    7,    7,    7,    7,    7,    7,    7,
	    7,    7,    7,    7,    7,    7,    7,    4,
	    4,    4,    4,    4,    4,    4,    4,    4,
	    4,    4,    5,    5,    5,    5,    5,    5,
	    5,    5,    5,    5,    5, 0x43, 0x43, 0x43,
	 0x63, 0x63, 0x63, 0x23, 0x23, 0x23, 0x23, 0x23,
	 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23,    3,
	    3, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23
};


const byte b4128[] = { 0, 0, 1, 1, 2, 2, 3, 3,
	     4, 4, 5, 5, 6, 6, 7, 7,
	     8, 9, 10 };



const byte b413B[] = {
	   0,    2,    4,    6,    8, 0x0A, 0x0C, 0x10,
	0x11, 0x0E, 0x12,    0,    2,    4,    6,    8,
	0x0A, 0x0C, 0x10, 0x11, 0x0E, 0x12,    0,    2,
	   4,    6,    8, 0x0A, 0x0C, 0x10, 0x11, 0x0E,
	0x12,    0,    2,    4,    6,    8, 0x0A, 0x0C,
	0x10, 0x11, 0x0E, 0x12,    0,    2,    4,    6,
	   8, 0x0A, 0x0C, 0x10, 0x11, 0x0E, 0x12, 0x0C,
	0x11, 0x12, 0x0C, 0x11, 0x12,    2,    6,    0,
	   4,    6,    8, 0x0A, 0x0C, 0x0E, 0x10, 0x11,
	0x12, 0x0D, 0x0F,    7,    9, 0x0B,    1,    5,
	   3
};


const byte b418C[] = {
	0x90, 0x91, 0x94, 0x95, 0x98, 0x99, 0x9A, 0x9C,
	0xA0, 0xA1, 0x9D, 0x6C, 0x6D, 0x70, 0x71, 0x74,
	0x75, 0x76, 0x78, 0x7C, 0x7D, 0x79,    0,    0,
       0,    0,    0,    0, 0x64, 0x65,    0,    0,
	   0,    0,    0,    0,    0,    0,    0, 0x68,
	0x69,    0,    0,    0,    0, 0x58,    0,    0,
	0x59, 0x5A, 0x54, 0x55,    0,    0,    0,    0,
	0x60,    0,    0, 0x61, 0x62, 0x5C, 0x5D,    0,
	   0,    0,    0, 0x84,    0,    0, 0x85, 0x86,
	0x80, 0x81,    0,    0,    0,    0, 0x8C,    0,
	   0, 0x8D, 0x8E, 0x88, 0x89,    0,    0,    0,
	   0,    0,    0,    0, 0x10, 0x11, 0x12, 0x14,
	0x15, 0x16, 0x17,    0,    1,    4,    5,    8,
	   9, 0x0A, 0x0C, 0x0D, 0x0E, 0x0F,    0, 0x30,
	   0, 0x31,    0,    0,    0,    0,    0,    0,
	   0,    0, 0x28,    0, 0x29,    0,    0,    0,
	   0,    0,    0,    0, 0x18, 0x19, 0x1C, 0x1D,
	0x20, 0x21, 0x22, 0x24, 0x25, 0x26, 0x27, 0x44,
	0x45, 0x48, 0x49, 0x4C, 0x4D, 0x4E, 0x50, 0x51,
	0x52, 0x53, 0xA8, 0xA9, 0xAC, 0xAD, 0xB0, 0xB1,
	0xB2, 0xB4, 0xB5, 0xB6, 0xB7, 0x34, 0x36, 0x35,
	0x38, 0x39, 0x3A,    0,    0,    0,    0,    0,
	0x3C, 0x3D, 0x3E, 0x40, 0x41, 0x42,    0,    0,
	   0,    0,    0,    0, 0x2C,    0,    0, 0x2D,
	0x2E,    0,    0,    0,    0,    0, 0xA4,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0
};

const byte b425D[] = {
	   8, 0x1C, 0x0C,    0, 0x0B, 0x11,    1, 0x0A,
	0x0F, 0x10, 0x0D,    9,    2,    3,    4,    5,
	   6,    7,    0, 0x1E, 0x12, 0x0E
};


const byte b4273[] = {
	   3,    3,    3,    3,    3,    3, 0x0C, 0x0D,
	0x0E, 0x0F, 0x10, 0x11,    0,    0,    0,    0,
	   0,    0,    0,    0, 0x12,    7,    4,    4,
	   2, 0x0A, 0x15,    0,    0,    0,    0,    8,
	   9,    0,    0,    0, 0x0B,    6, 0x12, 0x12,
	0x12, 0x12, 0x12, 0x12,    5,    1,    1,    1,
	0x13, 0x13, 0x13,    1,    1,    1, 0x13, 0x13,
	0x13, 0x14,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    1,    1, 0x0C, 0x0C, 0x0C,
	0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D,
	0x0D, 0x0D, 0x0D, 0x0D, 0x0D,    0, 0x0E, 0x0E,
	0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F,    0, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0E, 0x0E, 0x0E, 0x0E,    0,
	0x10, 0x10, 0x10, 0x10, 0x10,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   1,    1,    0,    0,    0,    0
};

const byte b42F9[] = {	// indexed by T2 item
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0, 0x10,    0,    0,    0,    0,    0,    0,
	   0,    0, 0x10,    0, 0x20,    0,    0, 0x13,
	   0,    0,    0,    0,    0,    0,    0,    0,
	0x50,    0,    0,    0, 0x10, 0x10, 0x20, 0x10,
	0x10, 0x10,    0, 0x50, 0x50,    0, 0x50, 0x50,
	   0, 0x50, 0x50,    0, 0x50, 0x50,    0, 0x50,
	0x50, 0x40,    0, 0x10, 0x10, 0x20, 0x20, 0x60,
	0x60, 0x60, 0x60, 0x60, 0x60,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	0x40, 0x40,    0,    0, 0x10,    0,    0, 0x21,
	0x10,    0, 0x10, 0x10, 0x10, 0x40,    0,    0,
	0x10, 0x10, 0x10, 0x10, 0x12,    0,    0, 0x40,
	   0,    0,    0,    0,    0,    0, 0x10,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0
};


byte bt44F7[] = {
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x50, 0x5C,
	0x5D, 0x5E,    5, 0x50, 0x5C, 0x5D, 0xED, 0xCD,
	0xDE, 0xDC, 0xDE, 0xDC, 0xDE, 0xDC, 0x85, 0x85,
    0x85,    5, 0x15, 0x25, 0x35, 0xC5, 0xD5, 0xE5,
	   5, 0x15, 0x25, 0x35, 0xC5, 0xD5, 0xE5, 0xC5,
	0xD5, 0xE5, 0xFE, 0x85, 0x40, 0xB0, 0xB1, 0xB2,
	0xBC, 0xBD, 0x80, 0x81, 0x82, 0x8C, 0x8D, 0x5E,
	0x15, 0x25, 0x35, 0xC5, 0xD5, 0xE5, 0xC5, 0xD5,
	0xE5,    5,    5, 0xE5,    5, 0xE5,    5, 0xE5,
	   5, 0xE5,    5,    5,    5,    5,    5,    5,
	   5,    5,    5,    5,    5,    5,    5,    5,
	0xB1, 0xE1, 0xD1, 0xB1, 0xE1, 0xD1, 0x81,    1,
	0x81,    1, 0x81,    1, 0xB1, 0xE1, 0xD1,    5,
	   5, 0xC5, 0xD5, 0xE5, 0x95, 0xB5, 0xA5,    5,
	0xC5, 0xD5, 0xE5, 0x95, 0xA5, 0xB5, 0xC5, 0xD5,
	0xE5, 0x95, 0xA5, 0xB5,    5, 0x15, 0x25, 0x35,
	   5, 0x15, 0x25, 0x35, 0xC5, 0xD5, 0xE5, 0x65,
    0x75, 0x85, 0x95, 0xA5, 0xB5, 0xC5, 0xD5, 0xE5,
	0x65, 0x75, 0x85, 0x95, 0xA5, 0xB5, 0xBC, 0xBD,
	0x9E, 0xAE, 0x8C, 0x8D, 0x6E, 0x7E, 0x6A, 0x7B,
	0x8A, 0x86, 0x87, 0x68, 0x78, 0x89, 0x8A, 0x6B,
	0x7B, 0xE6, 0xE7, 0xC8, 0xD8, 0xE9, 0xEA, 0xCB,
	0xDB, 0xA6, 0xB7, 0xA8, 0xA9, 0xBA, 0xAB, 0xC5,
	0xD5, 0xE5, 0xC5, 0xD5, 0xE5, 0xC5, 0xD5, 0xE5,
	0xC5, 0xD5, 0xE5, 0xC5, 0xD5, 0xE5, 0xC5, 0xD5,
	0xE5,    5, 0xC5, 0xD5, 0xE5, 0xE5, 0x55, 0xED,
	0xCE, 0xDE, 0xEC, 0xA0, 0xB0, 0xBD, 0xAE, 0x0A,
	0x0B, 0xDB, 0xEA, 0x80, 0x50, 0x10, 0x20, 0x30,
	0x9A, 0xBA, 0xCA,    8,    5,    1,    2,    3,
	0xA9, 0xAB, 0xAC, 0xA0, 0xB0, 0xBD, 0xAE, 0x0A,
	0x0B, 0xDB, 0xEA, 0x80, 0x50, 0x10, 0x20, 0x30,
	0x9A, 0xBA, 0xCA,    8,    5,    1,    2,    3,
	0xA9, 0xAB, 0xAC, 0xE0, 0xED, 0x0D, 0xCD, 0x0E,
	0xDE, 0xD0, 0xDC, 0x0E, 0xDE, 0xD0, 0xDC,    8,
	   5,    1,    2,    3, 0xA9, 0xAB, 0xAC, 0x0B,
	0xDB, 0xA0, 0xAE, 0xA9, 0xAB, 0xAC, 0x0B, 0xDB,
	0xA0, 0xAE, 0x0E, 0xDE, 0xD0, 0xDC,    8,    5,
	   1,    2,    3,    8,    5,    1,    2,    3,
	0xA9, 0xAB, 0xAC, 0x0B, 0xDB, 0xA0, 0xAE, 0x0E,
	0xDE, 0xD0, 0xDC, 0x9A, 0xBA, 0xCA, 0xB0, 0xBD,
	0x0A, 0xEA, 0xE0, 0xED, 0x0D, 0xCD, 0x80, 0x50,
	0x10, 0x20, 0x30, 0x80, 0x50, 0x10, 0x20, 0x30,
	0x9A, 0xBA, 0xCA, 0xB0, 0xBD, 0x0A, 0xEA, 0xE0,
	0xED, 0x0D, 0xCD,    8,    5,    1,    2,    3,
	0xA9, 0xAB, 0xAC, 0x0B, 0xDB, 0xA0, 0xAE, 0x0E,
	0xDE, 0xD0, 0xDC, 0x80, 0x50, 0x10, 0x20, 0x30,
	0x9A, 0xBA, 0xCA, 0xB0, 0xBD, 0x0A, 0xEA, 0xE0,
	0xED, 0x0D, 0xCD,    8,    5,    1,    2,    3,
	0xA9, 0xAB, 0xAC, 0x0B, 0xDB, 0xA0, 0xAE, 0x0E,
	0xDE, 0xD0, 0xDC, 0x80, 0x50, 0x10, 0x20, 0x30,
	0x9A, 0xBA, 0xCA, 0xB0, 0xBD, 0x0A, 0xEA, 0xE0,
	0xED, 0x0D, 0xCD,    8,    5,    1,    2,    3,
	0xA9, 0xAB, 0xAC, 0x0B, 0xDB, 0xA0, 0xAE, 0x0E,
	0xDE, 0xD0, 0xDC, 0x80, 0x50, 0x10, 0x20, 0x30,
	0x9A, 0xBA, 0xCA, 0xB0, 0xBD, 0x0A, 0xEA, 0xE0,
	0xED, 0x0D, 0xCD,    8,    5,    1,    2,    3,
	0xA9, 0xAB, 0xAC, 0x0B, 0xDB, 0xA0, 0xAE, 0x0E,
	0xDE, 0xD0, 0xDC, 0x80, 0x50, 0x10, 0x20, 0x30,
	0x9A, 0xBA, 0xCA, 0xB0, 0xBD, 0x0A, 0xEA, 0xE0,
	0xED, 0x0D, 0xCD, 0xCD, 0x35, 0xE5, 0x15, 0x25,
	0x35, 0xC5, 0xD5, 0xE5, 0xC5, 0xD5, 0xE5, 0xE5,
	0x65, 0x75, 0x85, 0x95, 0xA5, 0xB5, 0x65, 0x75,
	0x85, 0x65, 0x75, 0x85
};

const word wt48DF[] = {
	     0,      1, 0x1000, 0x1000, 0x2000, 0x2000, 0x2000, 0x4000,
	0x4000, 0x5000, 0x6008, 0x6000, 0x5080, 0x8040, 0xD088, 0x3010,
	0x8008, 0x9008, 0x8000, 0x8001, 0x9000, 0xA008, 0xD088, 0xC000,
	0xF000, 0x5080, 0xD088, 0x3010, 0x9008, 0x9000, 0xA000, 0xB008,
	0xA008, 0xD088, 0xD000, 0xE008, 0xE000, 0xE000, 0xF000, 0xF000,
	0xB000, 0xB000, 0xB000, 0xE000, 0xE000, 0xE000, 0xC000
};



const byte b499B[] = {
	0x10, 0x10, 0x20, 0x20, 0x10, 0x10,    2,    2,
	   5,    5,    5,    5,    1,    3,    1,    6,
	   3,    3,    3, 0x1C, 0x10,    4,    2,    2,
	0x20, 0x20, 0x20, 0x0E,    7,    4,    8,    7,
	   7,    6,    6,    6, 0x20, 0x10, 0x10, 0x10,
	0x20, 0x20, 0x10, 0x10,    3,    7,    7,    3,
	   7,    7,    3,    1,    1,    1,    1,    1,
	   1,    1,    6,    3,    1, 0x0E,    4,    2,
	   6, 0x0D,    1,    1,    1,    1,    1,    4,
	   1,    1,    3,    3,    3,    1,    1,    1,
	   1,    1,    1,    1,    1,    1,    1,    1,
	   1,    1,    1,    1,    1,    4,    1,    1,
	   1,    1,    1,    1,    1,    1,    4,    1,
	   1,    1,    1,    1,    1,    1,    1,    1,
	   2,    2,    2,    2,    1,    1,    1,    1,
	   1,    1,    1,    1,    1,    1,    1,    3,
	   3,    3, 0x1C,    2,    1,    1
};



const byte bt4C15[] = {
	0x21, 0x42, 0x42, 0x43, 0x41, 0x24, 0x34, 0x43,
	0x42, 0x42, 0x42, 0x43, 0x45, 0x45, 0x52, 0x53,
	0x50, 0x50,    0,    0, 0x60, 0x10, 0x70, 0x70
};
const byte bt4C2D[] = {
	0x79,    1, 0x61, 0x61, 0x61, 0x61, 0x89, 0x88,
	0x89,    1, 0x89, 0x89, 0x49, 0x89, 0x89, 0x89,
	0x69, 0x89, 0x8F, 0x6F, 0x80, 0x60, 0x80, 0x80
};
const byte bt4C45[] = {
	0x63,    0, 0x37, 0x46,    4,    5,    6, 0x3B,
	0x3C,    3, 0x3A,    3,    4,    4,    6, 0x3A,
	0x3B, 0x3B,    5,    6,    3,    4,    5,    4,
	   5,    6,    5,    5,    7,    7,    8,    8,
	   9,    9, 0x0B, 0x0B, 0x0C, 0x3B, 0x3C, 0x3C,
	0x3C, 0x3D, 0x3E, 0x3E, 0x3F, 0x3F, 0x40, 0x40,
	   3,    2,    2, 0x39,    1,    2,    4,    4,
	0x38, 0x39,    1, 0x38,    3,    4,    4,    5,
	   7,    8, 0x3B, 0x3C,    3,    3, 0x3A,    1,
	0x38,    3,    5,    6,    7,    9, 0x0A, 0x3C,
	0x3D, 0x3E,    2, 0x39,    1,    2,    3,    4,
	0x38, 0x39,    2,    2,    4,    4,    5, 0x39,
	0x39,    1,    2,    4,    2,    4,    5,    6,
	   8,    9, 0x39, 0x3B, 0x3C, 0x3D, 0x38
};

const byte bt4D23[][16] = {
 {    1, 0x47, 0x47, 0x47, 0, 0,    0,    0,
		 0,    0,    0,    0, 0x49, 0x49, 0x49, 0 },
 { 0x47,    1, 0x47, 0x47, 0, 0,    0,    0,
		 0,    0,    0,    0, 0x31, 0x49, 0x49, 0 },
 { 0x47, 0x47,    1, 0x61, 0, 0,    0,    0,
		 0,    0,    0,    0, 0x49, 0x31, 0x63, 0 },
 { 0x47, 0x47, 0x61,    1, 0, 0,    0,    0,
		 0,    0,    0,    0, 0x49, 0x63, 0x31, 0 },
 {    9, 0x0D, 0x0C, 0x0B, 1, 0, 0x30, 0x30,
		 0x30,    0,    0,    0, 0x0E, 0x13, 0x12, 0 },
 { 0x32, 0x32, 0x32, 0x32, 0, 1,    0,    0,
		 0,    0,    0,    0, 0x30, 0x30, 0x30, 0 },
 { 0x34, 0x35, 0x35, 0x35, 0, 0,    1, 0x52,
		 0x52,    0,    0,    0, 0x36, 0x37, 0x37, 0 },
 { 0x34, 0x64, 0x64, 0x64, 0, 0, 0x52,    1,
		 0x61,    0,    0,    0, 0x65, 0x65, 0x65, 0 },
 { 0x3A, 0x3A, 0x3A, 0x3A, 0, 0, 0x52, 0x61,
		 1,    0,    0,    0, 0x3C, 0x3C, 0x3C, 0 },
 { 0x38, 0x39, 0x39, 0x39, 0, 0,    2, 0x53,
		 0x53,    1, 0x52, 0x52, 0x4A, 0x4A, 0x4B, 0 },
 { 0x38, 0x6A, 0x6A, 0x6A, 0, 0, 0x53,    2,
		 0x6E, 0x52,    1, 0x61, 0x65, 0x65, 0x66, 0 },
 { 0x3B, 0x3B, 0x3B, 0x3B, 0, 0, 0x53, 0x6E,
		 2, 0x52, 0x61,    1, 0x45, 0x45, 0x3E, 0 },
 { 0x48,    2, 0x48, 0x48, 0, 0,    0,    0,
		 0,    0,    0,    0,    1, 0x52, 0x52, 0 },
 { 0x48, 0x48,    2, 0x6E, 0, 0,    0,    0,
		 0,    0,    0,    0, 0x52,    1, 0x61, 0 },
 { 0x48, 0x48, 0x6E,    2, 0, 0,    0,    0,
		 0,    0,    0,    0, 0x52, 0x61,    1, 0 },
 { 0x0A, 0x11, 0x10, 0x0F, 0, 0,    0,    0,
		 0, 0x30, 0x30, 0x30, 0x16, 0x15, 0x14, 1 },
 { 0x33, 0x33, 0x33, 0x33, 0, 0,    0,    0,
		 0,    0,    0,    0, 0x30, 0x30, 0x30, 0 },
 {    0,    0,    0,    0, 0, 0, 0x30, 0x30,
		 0x30, 0x30, 0x30, 0x30,    0,    0,    0, 0 },
 { 0x4B, 0x4C, 0x4C, 0x4C, 0, 0, 0x4A, 0x4A,
		 0x4B,    0,    0,    0, 0x4D, 0x4D, 0x4D, 0 },
 { 0x66, 0x67, 0x67, 0x67, 0, 0, 0x65, 0x65,
		 0x66,    0,    0,    0, 0x68, 0x68, 0x68, 0 },
 { 0x3D, 0x3F, 0x3F, 0x3F, 0, 0, 0x44, 0x44,
		 0x3D,    0,    0,    0, 0x40, 0x40, 0x40, 0 },
 { 0x50, 0x51, 0x51, 0x51, 0, 0, 0x4F, 0x4F,
		 0x50, 0x4A, 0x4A, 0x4B, 0x4D, 0x4D, 0x4E, 0 },
 { 0x6C, 0x6D, 0x6D, 0x6D, 0, 0, 0x6B, 0x6B,
		 0x6C, 0x65, 0x65, 0x66, 0x68, 0x68, 0x69, 0 },
 { 0x42, 0x43, 0x43, 0x43, 0, 0, 0x46, 0x46,
		 0x42, 0x44, 0x44, 0x3D, 0x40, 0x40, 0x41, 0 },
 { 0x47,    1, 0x47, 0x47, 0, 0,    0,    0,
		 0,    0,    0,    0,    1, 0x52, 0x52, 0 },
 { 0x47, 0x47,    1, 0x61, 0, 0,    0,    0,
		 0,    0,    0,    0, 0x52,    1, 0x61, 0 },
 { 0x47, 0x47, 0x61,    1, 0, 0,    0,    0,
		 0,    0,    0,    0, 0x52, 0x61,    1, 0 },
 {    8,    8,    8,    7, 0, 3,    0,    0,
		 0,    0,    0,    0,    6,    5,    4, 0 },
 {    0,    0,    0,    0, 0, 0,    0,    0,
		 0,    0,    0,    0,    0,    0,    0, 0 },
 {    0,    0,    0,    0, 0, 0, 0x19, 0x18,
		 0x17, 0x19, 0x18, 0x17,    0,    0,    0, 0 },
 { 0x2D, 0x2F, 0x2E, 0x2E, 0, 0, 0x2B, 0x2A,
		 0x2C, 0x1D, 0x1C, 0x1E, 0x23, 0x22, 0x24, 0 },
 { 0x59, 0x60, 0x5F, 0x5F, 0, 0, 0x58, 0x58,
		 0x58, 0x54, 0x54, 0x54, 0x5D, 0x5C, 0x5E, 0 },
 { 0x28, 0x28, 0x27, 0x27, 0, 0, 0x29, 0x26,
		 0x25, 0x19, 0x18, 0x17, 0x1D, 0x1C, 0x1E, 0 },
 { 0x59, 0x58, 0x58, 0x58, 0, 0,    0,    0,
		 0,    0,    0,    0, 0x54, 0x54, 0x54, 0 },
 { 0x55, 0x54, 0x54, 0x54, 0, 0,    0,    0,
		 0,    0,    0,    0, 0x54, 0x54, 0x54, 0 },
 { 0x1F, 0x21, 0x20, 0x20, 0, 0, 0x1D, 0x1C,
		 0x1E,    0,    0,    0, 0x23, 0x22, 0x22, 0 },
 { 0x55, 0x5B, 0x5A, 0x5A, 0, 0, 0x54, 0x54,
		 0x54,    0,    0,    0, 0x5D, 0x5C, 0x5C, 0 },
 { 0x1B, 0x1B, 0x1A, 0x1A, 0, 0, 0x19, 0x18,
		 0x17,    0,    0,    0, 0x1D, 0x1C, 0x1C, 0 },
 { 0x55, 0x54, 0x54, 0x54, 0, 0,    0,    0,
		 0,    0,    0,    0, 0x56, 0x56, 0x56, 0 },
 { 0x54, 0x55, 0x55, 0x55, 0, 0,    0,    0,
		 0,    0,    0,    0, 0x57, 0x57, 0x57, 0 }
};

const byte bt5048[] = {
	   0, 0x2A,    0, 0x2A,    0, 0x32,    0, 0x32,
	   0, 0x2A, 0x2A,    0, 0x2A,    0, 0x5A,    2,
	0x0A,    0,    2,    0,    9,    9, 0x11, 0x11,
	0x19, 0x71, 0x59, 0x69, 0x51, 0x61,    1,    1,
	   1,    0,    1,    1,    0, 0x0A, 0x0A, 0x12,
	0x12, 0x1A, 0x72, 0x5A, 0x6A, 0x52, 0x62,    2,
	   2,    2,    0,    0, 0x11, 0x21, 0x29, 0x39,
	0x41,    9,    9,    0,    9,    9,    0,    8,
	   0, 0x59, 0x59, 0x71, 0x71,    0,    0,    0,
	   0,    1,    1,    1, 0x5B, 0x73,    0,    1,
	   1,    1, 0x59, 0x71,    0,    2, 0x0A, 0x12,
	   2, 0x0A, 0x12,    0,    1,    0,    1,    0,
	   1,    0,    1,    1,    0
};

const byte b50AD[] = {
	0xFF, 0x22, 0xFF, 0x22, 0xFF, 0x22, 0xFF, 0x22,
	0xFF, 0x22, 0x62, 0xFF, 0x22, 0xFF,    4,    4,
	   4, 0xFF, 0x50, 0xFF, 0x64, 0x85, 0x64, 0x85,
	0x64, 0x64, 0xA5, 0x64, 0xA5, 0x64, 0x64,    5,
	   4, 0xFF, 0x64,    5, 0xFF, 0x64, 0x85, 0x64,
	0x85, 0x64, 0x64, 0xA5, 0x64, 0xA5, 0x64, 0x64,
	   5,    4, 0xFF,    4,    4,    4,    4,    4,
	   4, 0x64,    4, 0xFF, 0x64,    4, 0xFF,    4,
	0xFF, 0x25, 0x24, 0x64,    4,    4, 0xFF,    4,
	0xFF, 0x64,    5,    4, 0x24,    4, 0xFF, 0x64,
	   5,    4, 0x25, 0x64, 0xFF, 0x14, 0x14, 0x14,
	   4,    4,    4, 0xFF,    4, 0xFF,    4, 0xFF,
	   4, 0xFF, 0x14,    4, 0xFF
};

const byte b5112[] = {
	0x5A, 0x61, 0x69, 0x92, 0x92, 0x92, 0x91, 0x91,
	0x91, 0x5D, 0xD3, 0xD3, 0xDC, 0x9B, 0x9B, 0x98,
	0x98, 0x1A
};

const byte b5124[] = {
// indexed by T2 item
// low 2 bits give T2 size type
	   2,    2,    2,    2,    2,    2,    2,    2,
	   2,    2,    2,    2, 0x0E,    1,    1,    1,
	   1,    1,    1,    2,    2,    2,    2,    2,
	   2,    2,    2,    2,    3,    3,    2,    1,
	   1,    1,    1,    1,    2,    2,    2,    2,
	   2,    2,    2,    2,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    1,    1,    1,    1,    2,    2,    6,
	   1,    0,    0,    0,    0,    3,    0,    9,
	   9,    8,    1,    1,    1,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    1,
	   1,    1,    2,    9, 0x0B, 0x0B, 0x81, 0x99,
	0x91, 0x91, 0x91, 0x81, 0x89, 0x89, 0x89, 0x8A,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x90, 0xA1,
	0xA3, 0xA0, 0xA1, 0xA2, 0xB0, 0xA0, 0xA0, 0xA0,
	0xA0, 0xA0, 0xA3, 0xA3,    0,    0,    0,    0,
	   0,    0,    0,    0, 0x41, 0x41, 0x41, 0x40,
	   0,    0,    0,    0,    0, 0xC0, 0xC0, 0xC0,
	0xC0, 0xC0, 0xD8, 0x41, 0x41, 0x41, 0xC0
};
const byte b52B5[] = {
	   0,    1,    2,    3,    4,    8,    1,    2,
	   3,    1,    2,    3,    1,    2,    3,    4,
	   8,    4,    1,    2,    3,    1,    2,    3,
	   1,    2,    3,    8,    8, 0x0A, 0x0A,    9,
	0x0A,    9,    9, 0x0A,    9, 0x0A,    9,    9
};




const byte b5221[] = {
	   0, 0x4D,    0, 0x55,    0, 0x5D,    0, 0x66,
	   0, 0x6F, 0x6F,    0, 0x77,    0, 0xB6, 0x8D,
	0xB6,    0, 0x41,    0, 0x2D, 0x2D, 0x2E, 0x2E,
	0x2F, 0x30, 0x30, 0x31, 0x31, 0x32, 0x41, 0x41,
	0x12,    0, 0x1F, 0x1F,    0, 0x30, 0x30, 0x31,
	0x31, 0x32, 0x2D, 0x2D, 0x2E, 0x2E, 0x2F, 0x41,
	0x41, 0x41,    0, 0xAD, 0x42, 0x43, 0x44, 0x2C,
	0x46, 0x41, 0x12,    0, 0x41, 0x12,    0, 0xAD,
	   0, 0x41, 0x12, 0x41, 0x12, 0xAD,    0, 0xAD,
	   0, 0x41, 0x41, 0x12, 0xAD, 0xAD,    0, 0x41,
	0x41, 0x12, 0x20, 0x20,    0, 0x4A, 0x4B, 0x4C,
	0x7F, 0x80, 0x81,    0, 0x0D,    0, 0x0E,    0,
	0x0F,    0, 0x10, 0x11,    0
};

const byte b52DD[][11] = {
  { 0,    1,    2,    3,    4,    0,    0,    0,    5, 0x26, 0x25 },
  { 0, 0x0C, 0x0D, 0x0E, 0x0F,    0,    0,    0, 0x10, 0x21, 0x20 },
  { 0,    6,    7,    8,    0,    0,    0,    0,    0, 0x24, 0x23 },
  { 0,    9, 0x0A, 0x0B,    0,    0,    0,    0,    0, 0x1F, 0x1E },
  { 0, 0x12, 0x13, 0x14,    0,    0,    0,    0,    0,    0,    0 },
  { 0, 0x15, 0x16, 0x17,    0,    0,    0,    0,    0,    0,    0 },
  { 0, 0x18, 0x19, 0x1A,    0,    0,    0,    0,    0, 0x22,    0 },
  { 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
  { 0,    0,    0,    0, 0x11,    0,    0,    0,    0,    0, 0x1D },
  { 0,    0,    0,    0,    0,    0,    0,    0, 0x1C,    0,    0 },
  { 0,    0,    0,    0,    0,    0,    0,    0, 0x1B,    0,    0 },
  { 0,    0,    0,    0,    0,    0,    0,    0,    0, 0x27,    0 }
};





/*const*/ word wAF54[] = {
	0x11B, 0x14B, 0x12B, 0x12B, 0x11B, 0x14B,  0x60,  0x60,
	 0x62,	0x62,  0x5B,  0x62,	  0xB, 0x1E4, 0x1E7, 0x1E8,
	0x1EE, 0x1F1,  0x8D,  0xCF,	0x10B,   0xE,  0x12,  0x14,
	 0xEB,	0xEB,  0xEB,  0xA9,	 0x9A,  0x96,  0xA1,  0x69,
	 0x68,	0x70,  0x76,  0x90,	 0xEB, 0x10B, 0x15B, 0x18B,
	0x1BB, 0x19B, 0x17B, 0x16B,	 0x58,  0x19,  0x20,  0x27,
	 0x19,	0x20,  0x27,  0x16,	 0x17,  0x18,  0x16,  0x17,
	 0x18,	 0xA,  0x38,  0x3E,	 0x67,  0x2A,     6,   0xC,
	0x1DE,	0x80,  0x43,  0x45,	 0x47, 0x1DB,  0x49,  0xC9,
	 0xCD,	0xCE,  0xB7,  0xBA,	 0xBD,  0x41,  0x42,  0x44,
	 0x46,	0x48,  0x4A,  0x4B,	 0x4C,  0x41,  0x42,  0x44,
	 0x46,	0x48,  0x4A,  0x4B,	 0x4C,  0x7C,  0x42,  0x44,
	 0x46,	0x48,  0x48,  0x46,	 0x44,  0x42,  0x7C,  0x42,
	 0x44,	0x46,  0x48,  0x48,	 0x46,  0x44,  0x42,  0x4F,
	 0x42,  0x44,  0x46,  0x48,	 0x4A,  0x4D,  0x4E,  0x57,
	 0x50,  0x51,  0x52,  0x53,	 0x54,  0x55,  0x56,  0xC0,
	 0xC3,  0xC6,  0xCF, 0x1DC,     0,     0
};

const byte b46EB[] = {
	0x18, 0x26, 0x18, 0x18, 0x18, 0x18,    0, 0x12,
	0x12, 0x12, 0x2E, 0x27, 0x27, 0x27, 0x2D, 0x2A,
	0x2D, 0x2A, 0x2B, 0x28, 0x2C, 0x29,    0,    0,
	   0, 0x12,    7,    5,    2,    7,    5,    2,
	0x12,    7,    5,    2,    7,    5,    2,    7,
	   5,    2,    0,    0,    0,    1,    1,    1,
	   1,    1,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0, 0x12, 0x12,    2, 0x12,    2, 0x12,    2,
	0x12,    2, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
	0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
	0x0A,    9, 0x0B, 0x0A,    9, 0x0B, 0x17, 0x17,
	0x17, 0x17, 0x17, 0x17, 0x23, 0x22, 0x24, 0x12,
	0x12, 0x1E, 0x1E, 0x1D, 0x1F, 0x20, 0x1C, 0x12,
	0x12, 0x12, 0x12, 0x12, 0x12,    2, 0x12, 0x12,
	0x12, 0x0D, 0x10, 0x13, 0x12,    7,    5,    2,
	0x12,    7,    5,    2,    7,    5,    2,    7,
	   5,    2,    7,    5,    2,    7,    5,    2,
	   7,    5,    2,    7,    5,    2,    2,    2,
	   2,    2,    2,    2,    2,    2, 0x1C, 0x20,
	0x1C,    2,    2,    2,    2,    2,    2,    2,
	   2,    2,    2,    2,    2,    2,    2,    2,
	   2, 0x1C, 0x20, 0x1C, 0x1C, 0x20, 0x1C,    7,
	   5,    2,    7,    5,    2,    7,    5,    2,
	   7,    5,    2,    7,    5,    2,    7,    5,
	   2,    0,    0,    0,    0,    0,    0,    2,
	   2,    2,    2, 0x1C, 0x20, 0x20, 0x1C, 0x1C,
	0x20, 0x20, 0x1C, 0x12, 0x12, 0x12, 0x12, 0x12,
	0x1A, 0x21, 0x1C, 0x12, 0x12, 0x12, 0x12, 0x12,
	0x1A, 0x21, 0x1C, 0x1C, 0x20, 0x20, 0x1C, 0x1C,
	0x20, 0x20, 0x1C, 0x12, 0x12, 0x12, 0x12, 0x12,
	0x1A, 0x21, 0x1C, 0x12, 0x12, 0x12, 0x12, 0x12,
	0x1A, 0x21, 0x1C, 0x1B, 0x1D, 0x19, 0x1D, 0x1B,
	0x1D, 0x19, 0x1D, 0x1B, 0x1D, 0x19, 0x1D, 0x12,
	0x12, 0x12, 0x12, 0x12, 0x1A, 0x21, 0x1C, 0x20,
	0x20, 0x1C, 0x1C, 0x1A, 0x21, 0x1C, 0x20, 0x20,
	0x1C, 0x1C, 0x1B, 0x1D, 0x19, 0x1D,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	0x1A, 0x21, 0x1C, 0x20, 0x20, 0x1C, 0x1C, 0x1B,
	0x1D, 0x19, 0x1D, 0x1A, 0x21, 0x1C, 0x20, 0x20,
	0x1C, 0x1C, 0x1B, 0x1D, 0x19, 0x1D,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	0x1A, 0x21, 0x1C, 0x20, 0x20, 0x1C, 0x1C, 0x1B,
	0x1D, 0x19, 0x1D, 0x12, 0x12, 0x12, 0x12, 0x12,
	0x0E, 0x16, 0x11, 0x15, 0x15, 0x11, 0x11, 0x0F,
	0x14, 0x0C, 0x14, 0x12, 0x12, 0x12, 0x12, 0x12,
	0x0E, 0x16, 0x11, 0x15, 0x15, 0x11, 0x11, 0x0F,
	0x14, 0x0C, 0x14, 0x12, 0x12, 0x12, 0x12, 0x12,
	0x0E, 0x16, 0x11, 0x15, 0x15, 0x11, 0x11, 0x0F,
	0x14, 0x0C, 0x14, 0x12, 0x12, 0x12, 0x12, 0x12,
	0x0E, 0x16, 0x11, 0x15, 0x15, 0x11, 0x11, 0x0F,
	0x14, 0x0C, 0x14, 0x12, 0x12, 0x12, 0x12, 0x12,
	0x0E, 0x16, 0x11, 0x15, 0x15, 0x11, 0x11, 0x0F,
	0x14, 0x0C, 0x14, 0x12, 0x12, 0x12, 0x12, 0x12,
	0x0E, 0x16, 0x11, 0x15, 0x15, 0x11, 0x11, 0x0F,
	0x14, 0x0C, 0x14, 0x12, 0x12, 0x12, 0x12, 0x12,
	0x0E, 0x16, 0x11, 0x15, 0x15, 0x11, 0x11, 0x0F,
	0x14, 0x0C, 0x14, 0x12, 0x12, 0x12, 0x12, 0x12,
	0x0E, 0x16, 0x11, 0x15, 0x15, 0x11, 0x11, 0x0F,
	0x14, 0x0C, 0x14, 0x25, 0x26, 0x26,    0,    0,
	   0,    0,    0,    0,    7,    5,    2,    2,
	   7,    5,    2,    7,    5,    2,    7,    5,
	   2,    7,    5,    2
};


const byte b4A21[] = {
	0x3F, 0x40, 0x44, 0x43, 0x41, 0x42, 0x60, 0x6C,	// 0
	0x6C, 0x6C, 0x61, 0x64, 0x5F, 0x5F, 0xEB, 0xEF,
	0xEB, 0xEF, 0xEB, 0xEF, 0xEB, 0xEF, 0x21, 0x22,	// 10
	0x23, 0x24, 0x24, 0x24, 0x24, 0x27, 0x27, 0x27,
	0x25, 0x25, 0x25, 0x25, 0x28, 0x28, 0x28, 0x29,	// 20
	0x29, 0x29, 0x3B, 0x26, 0x3C, 0x3D, 0x3D, 0x3D,
	0x3E, 0x3E, 0x14, 0x14, 0x14, 0x14, 0x14, 0x16,	// 30
	0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17,
	0x17, 0x4C, 0x45, 0x1C, 0x46, 0x1D, 0x47, 0x1E,	// 40
	0x48, 0x1F, 0x49, 0x4A, 0x4B, 0x4D, 0x4E, 0x4F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,	// 50
	0xFE, 0xFB, 0xFA, 0xFE, 0xFB, 0xFA, 0xF7, 0xF8,
	0xF7, 0xF8, 0xF7, 0xF8, 0xFE, 0xFB, 0xFA, 0x5E,	// 60
	0x5A, 0xF9, 0xFA, 0xFB, 0xFC, 0xFE, 0xFD, 0x5B,
	0x62, 0x62, 0x62,    9,    9, 0x12, 0x63, 0x63,	// 70
	0x63, 0x5C, 0x5C, 0x5D,    0,    0,    0,    0,
	   0,    0,    0,    0,    1,    1,    1,    2,	// 80
	   2,    2,    3,    3,    3,    1,    1,    1,
	   1,    1,    1,    1,    1,    1, 0x58, 0x58,	// 90
	0x6F, 0x6F, 0x59, 0x59, 0x59, 0x59, 0xE5, 0xE6,
	0xE7, 0x59, 0x59, 0x59, 0x59, 0x65, 0x65, 0x65,	// A0
	0x65, 0x59, 0x59, 0x59, 0x59, 0x65, 0x65, 0x65,
	0x65, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA,    2,	// B0
	   2,    2, 0x68, 0x68, 0x68, 0x69, 0x69, 0x69,
	   3,    3,    3, 0x6A, 0x6A, 0x6A, 0x6B, 0x6B,	// C0
	0x6B, 0x1B, 0x20, 0x20, 0x20, 0x1B, 0x1B, 0x39,
	0x39, 0x39, 0x39, 0xF3, 0xF4, 0xF5, 0xF6, 0xF3,	// D0
	0xF4, 0xF5, 0xF6, 0x2A, 0x2B, 0x2C, 0x2C, 0x2C,
	0xF0, 0xF1, 0xF2, 0x2A, 0x2B, 0x2C, 0x2C, 0x2C,	// E0
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF3,
	0xF4, 0xF5, 0xF6, 0x2A, 0x2B, 0x2C, 0x2C, 0x2C,	// F0
	0xF0, 0xF1, 0xF2, 0x2A, 0x2B, 0x2C, 0x2C, 0x2C,
	0xF0, 0xF1, 0xF2, 0xED, 0xEB, 0xEE, 0xEF, 0xED,	// 100
	0xEB, 0xEE, 0xEF, 0xED, 0xEB, 0xEE, 0xEF, 0x2A,
	0x2B, 0x2C, 0x2C, 0x2C, 0xF0, 0xF1, 0xF2, 0xF4,	// 110
	0xF5, 0xF3, 0xF6, 0xF0, 0xF1, 0xF2, 0xF4, 0xF5,
	0xF3, 0xF6, 0xED, 0xEB, 0xEE, 0xEF, 0x2A, 0x2B,	// 120
	0x2C, 0x2C, 0x2C, 0x2A, 0x2B, 0x2C, 0x2C, 0x2C,
	0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xAE,	// 130
	0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6,
	0xB7, 0xB8, 0xAE, 0xAF, 0xB0, 0xB1, 0x2A, 0x2B,	// 140
	0x2C, 0x2C, 0x2C, 0x2A, 0x2B, 0x2C, 0x2C, 0x2C,
	0xF0, 0xF1, 0xF2, 0xF4, 0xF5, 0xF3, 0xF6, 0xED,	// 150
	0xEB, 0xEE, 0xEF, 0x2D, 0x2E, 0x2F, 0x2F, 0x2F,
	0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xCF,	// 160
	0xD0, 0xD1, 0xD2, 0x2D, 0x2E, 0x2F, 0x2F, 0x2F,
	0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xCF,	// 170
	0xD0, 0xD1, 0xD2, 0x36, 0x37, 0x38, 0x38, 0x38,
	0xDE, 0xDF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xDA,	// 180
	0xDB, 0xDC, 0xDD, 0x36, 0x37, 0x38, 0x38, 0x38,
	0xDE, 0xDF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xDA,	// 190
	0xDB, 0xDC, 0xDD, 0x30, 0x31, 0x32, 0x32, 0x32,
	0xBD, 0xBE, 0xBF, 0xC0, 0xC1, 0xC2, 0xC3, 0xB9,	// 1A0
	0xBA, 0xBB, 0xBC, 0x30, 0x31, 0x32, 0x32, 0x32,
	0xBD, 0xBE, 0xBF, 0xC0, 0xC1, 0xC2, 0xC3, 0xB9,	// 1B0
	0xBA, 0xBB, 0xBC, 0x33, 0x34, 0x35, 0x35, 0x35,
	0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xC4, // 1C0
	0xC5, 0xC6, 0xC7, 0x33, 0x34, 0x35, 0x35, 0x35,
	0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xC4,	// 1D0
	0xC5, 0xC6, 0xC7, 0x66,    0,    1, 0x17, 0x17,
	0x17, 0x17, 0x17, 0x17,    2,    2,    2, 0x70,	// 1E0
	   2,    2,    2,    3,    3,    3,    2,    2,
	   2,    3,    3,    3				// 1F0
};



const byte b4CB4[] = {
	0x63,    0,    1, 0x32,    1,    2,    2,    1,
	   2,    1,    1,    1, 0x0B, 0x0D, 0x0D,    1,
	0x0B, 0x0D,    1, 0x0B,    1, 0x0B, 0x0D,    1,
	0x0B, 0x0D, 0x11, 0x13, 0x11, 0x13, 0x11, 0x1B,
	0x19, 0x1B, 0x19, 0x1B, 0x19,    1, 0x0B, 0x11,
	0x13, 0x0D, 0x11, 0x13, 0x11, 0x1B, 0x19, 0x1B,
	   1,    2,    1,    1,    1,    2,    1,    2,
	   1,    2, 0x2F, 0x2F, 0x2F, 0x2D, 0x2F, 0x2D,
	0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2F, 0x2D,    3,
	   3,    3, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
	0x29, 0x29,    3,    3,    1,    1,    1,    1,
	   1,    1, 0x1D, 0x1F, 0x1D, 0x1F, 0x1D, 0x1D,
	0x1F,    3,    3,    3, 0x2B, 0x2B, 0x2B, 0x2B,
	0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B,    3
};

const byte b4FA3[] = {
	0x17, 0x16, 0x16, 0x16,    0,    0,    0,    0,
	   0,    1,    1,    2,    2,    2,    2,    2,
	   2,    2,    3,    3,    4,    4,    4,    5,
	   5,    5,    5,    5,    5,    5,    5,    5,
	   5,    5,    5,    5,    5,    5,    5,    5,
	   5,    5,    5,    5,    5,    5,    5,    5,
	   6,    7,    8,    8,    9,    9,    9,    9,
	   9,    9, 0x0A, 0x0A, 0x0B, 0x0C, 0x0C, 0x0C,
	0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0E,
	0x0E, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x11, 0x11, 0x12, 0x12, 0x12, 0x12,
	0x12, 0x12, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13,
	0x13, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14,
	0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14
};


const word w493D[] = {
	0x0123, 0x0123, 0x0124, 0x0126, 0x0134, 0x0143, 0x0163, 0x0423,
	0x0623, 0x0624, 0x0634, 0x0634, 0x4026, 0x4123, 0x4126, 0x4106,
	0x4123, 0x4126, 0x4123, 0x4123, 0x4126, 0x4136, 0x4326, 0x4623,
	0x4666, 0x6024, 0x6124, 0x6104, 0x6124, 0x6124, 0x6134, 0x6114,
	0x6134, 0x6324, 0x6624, 0x6634, 0x6634, 0x6663, 0x6664, 0x6666,
	0x5146, 0x5164, 0x5154, 0x5346, 0x5364, 0x5354, 0x5623
};



inline int fragLen(int n) { return fragFlg_Len[n] & 0x1f; }


int overlay2() {
	try {
		sub_3F27();
		for (;;) {
			fillTx2Q();
			sub_67A9();
			if (tx2opc[4] == T2_EOF)
				break;
			sub_689E();

			sub_6BD6();
			sub_A153();
		}
	}
	catch (...) {
		printf("throw caught\n");
	}
	sub_3F7D();
	return 3;
}

void sub_3F27() {
	botMem = 0xC4BD; // .MEMORY;
	if ((word)word_3822 < botMem)
		fatal("COMPILER ERROR: INSUFFICIENT MEMORY FOR CODE GENERATION", 0x37);
//	assignFileBuffer(&tx1File, tx1Buf, 512, 2);
//	assignFileBuffer(&tx2File, tx2Buf, 512, 1);
	FILL(10, &wC1DC[0], 0);
	blkCurInfo[0] = off2Info(procInfo[1]);
	programErrCnt = 0;
}


void sub_3F7D() {
	curInfo_p = off2Info(procInfo[1]);
	setDimension(pc);
	setBasedOffset(wC1C5);
//	flushFile(&tx1File);
}


void wrFragData() {
	if (!(OBJECT | PRINT)) {
		if (buf_C1E7[0] == 0x9A || buf_C1E7[0] == 0x9B || buf_C1E7[0] == 0xA3)
			programErrCnt++;
		return;
	}
	ifwrite(&tx1File, buf_C1E7, bC1E6);
}


void putTx1Byte(byte arg1b) {
	buf_C1E7[bC1E6++] = arg1b;
}


void putTx1Word(word arg1w) {
	putTx1Byte((byte)arg1w);
	putTx1Byte((byte)(arg1w >> 8));
}

byte arg1b_53D6;

void encodeFragData(byte arg1b) {
	arg1b_53D6 = arg1b;
	bC1E6 = 0;
	putTx1Byte(arg1b_53D6);
	if (bt4029[arg1b_53D6] & 0x80)
		putTx1Byte(curOp);
	sub_5410();
	FILL(10, &wC1DC[0], 0);
	wrFragData();
}


byte bC214;
byte bC215;

void sub_5410() {
	bC214 = 0;
	bC215 = bC1E6;
	if (((bt4029[arg1b_53D6] >> 4) & 7) != 0) {
		sub_545D();
		if (wC1DC[bC214] != 0 || wC1DC[0] <= 12)
			sub_545D();
	}
}

void sub_545D() {
	byte j;
	j = (byte)wC1DC[bC214];
	if (wC1DC[0] <= 12) {
		if (bC215 == bC1E6)
			putTx1Byte(j);
		else					// ROL(buf_C1E7[bC215], 4)
			buf_C1E7[bC215]  = (buf_C1E7[bC215] << 4) | (buf_C1E7[bC215] >> 4) | j;
	}
	bC214++;
	if (7 < j) {
		if (j == 8 || j == 0xd || j == 0xa)
			putTx1Byte((byte)wC1DC[bC214]);
		else
			putTx1Word(wC1DC[bC214]);

		bC214++;
		if (j >= 0xa && j < 0xc) {
			putTx1Word(wC1DC[bC214]);
			bC214++;
		} else if (j == 0xc) {
			putTx1Word(wC1DC[bC214]);
			putTx1Word(wC1DC[bC214+1]);
			bC214 += 2;
		}
	}
}

void emitTopItem()
{
	bC1E6 = 0;
	if (! PRINT)
		if (tx2opc[tx2qp] == T2_LINEINFO || tx2opc[tx2qp] == T2_INCLUDE)
			return;
	putTx1Byte(tx2opc[tx2qp]);
	if (bt4029[tx2opc[tx2qp]] & 0x80) {
		putTx1Byte((byte)tx2op2[tx2qp]);
		putTx1Word(tx2op1[tx2qp]);
	} else 
		switch(b5124[tx2opc[tx2qp]] & 3) {
	case 0: break;
	case 1: putTx1Word(tx2op1[tx2qp]);
		break;
	case 2: putTx1Word(tx2op1[tx2qp]);
		putTx1Word(tx2op2[tx2qp]);
		break;
	case 3: putTx1Word(tx2op1[tx2qp]);
		putTx1Word(tx2op2[tx2qp]);
		putTx1Word(tx2op3[tx2qp]);
	}
	wrFragData();
}

void tx2SyntaxError(byte arg1b) {
	tx2opc[tx2qp] = T2_SYNTAXERROR;
	tx2op1[tx2qp] = arg1b;
}

byte sub_5679(byte arg1b) {
	if (arg1b == 0)
		return bt44F7[wC1D6] >> 4;
	return bt44F7[wC1D6] & 0xf;
}

void sub_56A0(byte arg1b, byte arg2b)
{
	tx2opc[arg2b] = tx2opc[arg1b];
	tx2Aux1b[arg2b] = tx2Aux1b[arg1b];
	tx2Aux2b[arg2b] = tx2Aux2b[arg1b];
	tx2op1[arg2b] = tx2op1[arg1b];
	tx2op2[arg2b] = tx2op2[arg1b];
	tx2op3[arg2b] = tx2op3[arg1b];
	tx2Auxw[arg2b] = tx2Auxw[arg1b];
}


byte sub_5748(byte arg1b) { 
	if (arg1b == 8)
		return 2;
	return arg1b + 2;
}

word sub_575E(word arg1w)
{
	byte i;

	curInfo_p = arg1w;
	i = getInfoType();
	if (i == BYTE_T)
		return 1;
	if (i == ADDRESS_T)
		return 2;
	if (i == STRUCT_T)
		return getOwningStructure();
	return curInfo_p + 1;	// I suspect its realy an error
}

void sub_5795(word arg1w)
{
	word p, q;
	p = wC1C3 * 2 + arg1w;
	q = (p >> 1) + (p & 1) + 2;
	if (curOp == T2_RETURNWORD)
		q -= 2;
	if (7 < q) {
		if (bC0B5[0] == 3)
			encodeFragData(CF_XCHG);
		wC1DC[0] = 9;
		wC1DC[1] = p;
		encodeFragData(CF_6);
		encodeFragData(CF_SPHL);
		if (bC0B5[0] == 3) {
			encodeFragData(CF_XCHG);
			pc += 7;
		} else 
			pc += 5;
	} else {
		if (p & 1) {
			encodeFragData(CF_INXSP);
			pc++;
		}
		while (1 < p) {
			if (bC0B5[0] == 3)
				wC1DC[0] = 2;	// pop d
			else
				wC1DC[0] = 3;	// pop h
			wC1DC[1] = 8;
			encodeFragData(CF_POP);
			pc++;
			p -= 2;
		}
	}
	if (0xff00 < arg1w)
		wC1C3 = ((-arg1w) & 0xffff) >> 1;
	else
		wC1C3 = 0;
}

byte enterBlk()
{
	if (blkSP < 0x14) {
		blkSP++;
		return 0xff;
	}
	if (blkOverCnt == 0) {
		tx2SyntaxError(ERR204);	// LIMIT EXCEEDED: NUMBER OF ACTIVE
					// PROCEDURES AND DO CASE GROUPS
		emitTopItem();
	}
	blkOverCnt++;
	return 0;
}



byte exitBlk()
{
	if (0 < blkOverCnt) { 
		blkOverCnt--;
		return 0;
	}
	if (0 < blkSP) { 
		blkSP--;
		return 0xff;
	}
	tx2SyntaxError(ERR205);	// ILLEGAL NESTING OF BLOCKS, ENDS NOT BALANCED
	emitTopItem();
	return 0;
}


void sub_58F5(byte arg1b)
{
	fatalErrorCode = arg1b;
	buf_C1E7[0] = 0x9a;
	buf_C1E7[1] = fatalErrorCode;
	buf_C1E7[2] = 0;
	bC1E6 = 3;
	wrFragData();

	while (0 < blkSP) {
		if (exitBlk()) {
			if (blkSP < procChainId) {
				curInfo_p = blkCurInfo[procChainId];
				setDimension(pc);
				setBasedOffset(wC1C5);
				procChainId = procChainNext[procChainId];
				pc = wB488[procChainId];
				wC1C5 = wB4D8[procChainId];
			}
		}
	}
	throw "error";
}

void sub_597E()
{
	byte i, j, k, m, n;
	bC0B2 = 0;
	bC0B1 = 0;
	for (i = 0; i <= 3; i++) {
		boC069[i] = 0;
		boC072[i] = 0;
		boC07B[i] = 0;
		k = bC04E[i];
		n = b5124[tx2opc[k]] & 0xc0;
		if (k != 0) {
			boC060[i] = 0xff;
			if (bC0B5[0] == i || bC0B5[1] == i) {
				boC069[i] = 0xff;
				if (0 < i)
					bC0B2++;
			}
			if (n == 0 || n == 0x80) {
				if (tx2op3[k] == 0) {
					if (tx2Aux1b[k] == bC045[i]
					  || (tx2Aux1b[k] == 0 && bC045[i] == 6)) {
						if (1 < tx2Auxw[k] || boC069[i]
						  || (k != bC0B7[0] && k != bC0B7[1]))
							boC072[i] = 0xFF;
					}
				}
			}
		} else 
			boC060[i] = boC057[i];
	}

	for (i = 0; i <= 3; i++) {
		if (boC072[i]) {
			boC07B[i] = 0xff;
			m = boC069[i];
			for (j = 3; j > i; j--) {
				if (boC072[j]) {
					if (bC04E[j] == bC04E[i]) {
						boC072[j] = 0;
						if (i != 0) {
							boC072[i] = 0;
							m |= boC069[j];
						}
					}
				}
			}
			if (i != 0 && !m)
				bC0B1++;
		}
	}
}


void sub_5B96(byte arg1b, byte arg2b) 
{
	bC04E[arg2b] = bC04E[arg1b];
	bC045[arg2b] = bC045[arg1b];
	wC084[arg2b] = wC084[arg1b];
	bC0A8[arg2b] = bC0A8[arg1b];
	wC096[arg2b] = wC096[arg1b];
	boC057[arg2b] = boC057[arg1b];
}


void sub_5C1D(byte arg1b)
{
	byte i;
	sub_597E();
	bC140[wC1C3] = i = bC04E[arg1b];
	if (boC072[arg1b])
		tx2op3[i] = wC1C3;

	if (arg1b != 0)
		bC0C3[wC1C3] = (bC045[arg1b] << 4) | (bC0A8[arg1b] & 0xf);
	else
		bC0C3[wC1C3] = 0xB0;
}

void sub_5C97(byte arg1b)
{
	byte i;
	bC04E[arg1b] = i = bC140[wC1C3];
	if (tx2op3[i] == wC1C3)
		tx2op3[i] = 0;

	boC057[arg1b] = 0;
	bC045[arg1b] = bC0C3[wC1C3] >> 4;
	bC0A8[arg1b] = bC0C3[wC1C3] & 0xf;
	if (bC0A8[arg1b] > 7) 
		bC0A8[arg1b] |= 0xf0;
}


void sub_5D27(byte arg1b) 
{
	if (wC1C5 < ++wC1C3 * 2)
		wC1C5 = wC1C3 * 2;
	sub_5C1D(arg1b);
	wC1DC[0] = arg1b;
	wC1DC[1] = 0xA;
	wC1DC[2]= wC1C3;
	encodeFragData(CF_PUSH);
	pc++;
}

byte arg1b_5D6B;	// nested
byte bC233;

void sub_5D6B(byte arg1b) 
{
	arg1b_5D6B = arg1b;
	if (boC072[arg1b_5D6B]) {
		if (boC069[arg1b_5D6B])
			sub_5E16();
		sub_5D27(arg1b_5D6B);
	} else if (boC069[arg1b_5D6B]) {
		for (bC233 = 0; bC233 <= 3; bC233++) {
			if (bC04E[arg1b_5D6B] == bC04E[bC233]) {
				if (arg1b_5D6B != bC233) {
					if (bC045[bC233] == bC045[arg1b_5D6B]) {
						if (arg1b_5D6B == bC0B5[0])
							bC0B5[0] = bC233;
						else
							bC0B5[1] = bC233;
						return;
					}
				}
			}
		}
		sub_5E16();
		sub_5D27(arg1b_5D6B);
	}
}

void sub_5E16()	// nested
{
	for (bC233 = 0; bC233 <= 3; bC233++) {
		if (boC072[bC233])
			if (!boC069[bC233])
				sub_5D27(bC233);
	}
	if (arg1b_5D6B == bC0B5[0])
		bC0B5[0] = 9;
	else
		bC0B5[1] = 9;
}


void sub_5E66(byte arg1b)
{
	byte i, j, k;
	j = bC0B7[0];
	k = bC0B7[1];
	bC0B7[0] = 0;
	bC0B7[1] = 0;
	arg1b = (arg1b >> 3) | (arg1b << 5); // ROR(arg1b, 3);
	sub_597E();
	for (i = 0; i <= 3; i++) {
		if (arg1b & 1) {
			if (boC072[i])
				sub_5D27(i);
			boC057[i] = 0;
			bC04E[i] = 0;
			sub_597E();
		}
		arg1b = ((arg1b << 1) | (arg1b >> 7));	// ROL(arg1b, 1)
	}
	bC0B7[0] = j;
	bC0B7[1] = k;
}


void sub_5EE8()
{
	byte i;
	sub_5795(wC1C7);
	if ((curInfo_p = blkCurInfo[procChainId]) != 0) {
		if (testInfoFlag(F_INTERRUPT)) {
			for (i = 0; i <= 3; i++) {
				wC1DC[0] = i;	// pop psw, pop b, pop d, pop h
				wC1DC[1] = 8;
				encodeFragData(CF_POP);
			}
			encodeFragData(CF_EI);
			pc += 5;
		}
	}
}


void sub_5F4B(word arg1w, word arg2w, byte arg3b, byte arg4b) 
{
	tx2op2[tx2qp] = arg1w;
	tx2op1[tx2qp] = arg2w;
	tx2Aux1b[tx2qp] = arg3b;
	tx2Aux2b[tx2qp] = arg4b;
	if (arg4b == 8)
		tx2opc[tx2qp] = T2_NUMBER;
	else
		tx2opc[tx2qp] = T2_VARIABLE;
}

void sub_5FBF(byte arg1b, word *arg2w, word *arg3w)
{
	if (tx2opc[arg1b] == T2_VARIABLE || tx2opc[arg1b] == T2_NUMBER) {
		*arg2w = tx2op2[arg1b];
		if ((curInfo_p = tx2op1[arg1b]) == 0) 
			*arg3w = 0;
		else if (testInfoFlag(F_MEMBER)) 
			*arg3w = 0;
		else if (testInfoFlag(F_BASED))
			*arg3w = 0;
		else if (testInfoFlag(F_ABSOLUTE))
			*arg3w = 0;
		else if (testInfoFlag(F_AUTOMATIC))
			*arg3w = 0x100;
		else if (testInfoFlag(F_EXTERNAL))
			*arg3w = getExternId() | 0x400;
		else if (testInfoFlag(F_MEMORY))
			*arg3w = 0x800;
		else if (getInfoType() == PROC_T)
			*arg3w = getProcId() | 0x1000;
		else if (testInfoFlag(F_DATA))
			*arg3w = 0x200;
		else
			*arg3w = 0x2000;
	} else if (arg1b == 0) { 
		*arg2w = 0;
		*arg3w = 0;
	} else {
		*arg2w = 0;
		*arg3w = 0x4000;
	}
}


void sub_611A()
{
	byte i, j, k;
	for (i = 0; i <= 1; i++) {
		if ((j = bC0B7[i]) != 0) {
			if (--tx2Auxw[j] == 0) {
				for (k = 0; k <= 3; k++) {
					if (bC04E[k] == j)
						bC04E[k] = 0;
				}
				bC140[tx2op3[j]] = 0;
			}
		}
	}
}



void sub_61A9(byte arg1b)
{
	bC0C1[arg1b] = b52DD[bC0B3[arg1b]][bC0B5[arg1b]];
}

void sub_61E0(byte arg1b)
{
	if ((b5124[tx2opc[arg1b]] & 0xc0) == 0) {
		wC1DC[bC1DB] = 0xa;
		wC1DC[bC1DB + 1] = tx2op3[arg1b];
		wC1DC[bC1DB + 2] = (wC1C3 - tx2op3[arg1b]) * 2;
		bC1DB += 3;
	} else if (tx2op1[arg1b] != 0) {
		wC1DC[bC1DB + 2] = info2Off((curInfo_p = tx2op1[arg1b]));
		wC1DC[bC1DB + 1] = tx2op2[arg1b] - getAddr();
		if (testInfoFlag(F_AUTOMATIC)) {
			wC1DC[bC1DB] = 0xc;
			wC1DC[bC1DB + 3] = tx2op2[arg1b] + wC1C3 * 2;
			bC1DB += 4;
		} else {
			wC1DC[bC1DB] = 0xb;
			bC1DB += 3;
		}
	} else {
		if (tx2op2[arg1b] < 0x100)
			wC1DC[bC1DB] = 8;
		else
			wC1DC[bC1DB] = 9;
		wC1DC[bC1DB + 1] = tx2op2[arg1b];
		bC1DB += 2;
	}
}


void sub_636A(byte arg1b)
{
	if (bC0B5[arg1b] <= 3) {
		wC1DC[bC1DB] = bC0B5[arg1b];
		bC1DB++;
	} else
		sub_61E0(bC0B7[arg1b]);
}


void sub_63AC(byte arg1b)
{
	if (3 < arg1b)
		return;
	if (6 >= bC045[arg1b]) {
		while (bC0A8[arg1b] != 0) {
			if (bC0A8[wC1DC[0] = arg1b] > 0x7f) {
				bC0A8[arg1b]++;
				encodeFragData(CF_INX);
			} else {
				bC0A8[arg1b]--;
				encodeFragData(CF_DCX);
			}
			pc++;
		}
	}
}


void sub_6416(byte arg1b) 
{
	wC1DC[0] = arg1b;
	wC1DC[1] = 0xa;
	wC1DC[2] = wC1C3;
	encodeFragData(CF_POP);
	pc++;
	sub_5C97(arg1b);
	wC1C3--;
}

void getTx2Item()
{
	ifread(&tx2File, &tx2opc[tx2qp], 1);
	switch (b5124[tx2opc[tx2qp]] & 3) {
	case 0:
		if (tx2opc[tx2qp] == T2_EOF)
			eofSeen = 0xff;
		break;
	case 1:
		ifread(&tx2File, &tx2op1[tx2qp], 2);
		break;
	case 2:
		ifread(&tx2File, &tx2op1[tx2qp], 2);
		ifread(&tx2File, &tx2op2[tx2qp], 2);
		break;
	case 3:
		ifread(&tx2File, &tx2op1[tx2qp], 2);
		ifread(&tx2File, &tx2op2[tx2qp], 2);
		ifread(&tx2File, &tx2op3[tx2qp], 2);
	}
}


void sub_652B() 
{
	if (curOp == T2_MODULE) {
		if (tx2opc[tx2qp - 1] == T2_LABELDEF) {
			curInfo_p = off2Info(tx2op1[tx2qp - 1]);
			if (! (testInfoFlag(F_MODGOTO) || testInfoFlag(F_PUBLIC)))
				tx2qp--;
		}
	} else if (curOp == T2_LINEINFO) {
		if (tx2op2[tx2qp] == 0) {
			if (tx2opc[tx2qp - 1] == T2_LINEINFO) {
				if (tx2op2[tx2qp - 1] == 0) {
					tx2op3[tx2qp - 1] = tx2op1[tx2qp];
					tx2qp--;
				}
			}
		}
	}
}


void fillTx2Q()
{
	byte i, j, k;
	tx2qp = 4;
	if (bC1BF < tx2qEnd) {
		k = tx2qEnd - bC1BF;
		movemem(k, &tx2opc[bC1BF], &tx2opc[tx2qp]);
		movemem(k, &tx2Aux1b[bC1BF], &tx2Aux1b[tx2qp]);
		movemem(k, &tx2Aux2b[bC1BF], &tx2Aux2b[tx2qp]);
		movemem(k * 2, &tx2op1[bC1BF], &tx2op1[tx2qp]);
		movemem(k * 2, &tx2op2[bC1BF], &tx2op2[tx2qp]);
		movemem(k * 2, &tx2op3[bC1BF], &tx2op3[tx2qp]);
		movemem(k * 2, &tx2Auxw[bC1BF], &tx2Auxw[tx2qp]);
		tx2qp = k + 4;
		bC1BF += k;
	}
	i = 0xff;
	j = 0;
	while (tx2qp < 255 && !eofSeen) {
		getTx2Item();
		curOp = tx2opc[tx2qp];
		sub_652B();
		if (tx2qp == 4) {
			if (curOp == T2_STMTCNT || curOp == T2_LOCALLABEL
			  || curOp == T2_EOF)
				tx2qp++;
			else if (b5124[curOp] & 0x20)
				emitTopItem();
			else if (i) {
				i = 0;
				tx2SyntaxError(ERR200);	// LIMIT EXCEEDED
							// STATEMENT SIZE
				emitTopItem();
			}
		} else
			tx2qp++;

		if (b5124[curOp] & 8)
			j = 0xff;
		if (j)
			if (curOp  == T2_STMTCNT || curOp == T2_EOF
			  || curOp == T2_LOCALLABEL)
				break;
	}
	tx2qEnd = tx2qp;
}



void sub_67A9()
{
	byte i;
	skipBB(tx2qEnd, 0xff);
	i = 0;
	if (bC1BF == 4 && ! eofSeen)
		bC1BF = tx2qEnd;
	else
		for (tx2qp = 4; tx2qp <= bC1BF - 1; tx2qp++) {
			bC1D2 = b5124[tx2opc[tx2qp]];
			if (bC1D2 & 0x10) {
				if (i + 4 < tx2qp) {
					if (tx2opc[tx2qp] == T2_LOCALLABEL)
						bC1BF = tx2qp;
					else
						skipBB(tx2qp, 0xff);
					return;
				}
				i++;
			}
			if (bC1D2 & 8) {
				skipBB(tx2qp, 1);
				return;
			}
			if (bC1D2 & 0x20)
				i++;
		}
}


void skipBB(byte arg1b, byte arg2b)
{
	bC1BF = arg2b + arg1b;
	while (tx2opc[bC1BF] != T2_STMTCNT && tx2opc[bC1BF] != T2_EOF
	  && tx2opc[bC1BF] != T2_LOCALLABEL)
		bC1BF += arg2b;
}

void sub_689E()
{
	for (tx2qp = 4; tx2qp <= bC1BF - 1; tx2qp++) {
		curOp = tx2opc[tx2qp];
		bC1D2 = b5124[curOp];
		if ((bC1D2 & 0xc0) == 0)
			sub_68E8();
		else if ((bC1D2 & 0xc0) == 0x40)
			sub_6AA4();
	}
}


byte bC252;	// nested
void sub_68E8()
{
	byte i;
	bC252 = bC1D2 & 3;
	if (bC1D2 & 4) {
		tx2op2[1] = tx2op1[tx2qp];
		tx2op1[tx2qp] = 1;
	} else
		sub_69EB(1, &tx2op1[tx2qp]);

	sub_69EB(2, &tx2op2[tx2qp]);
	if (bC252 == 3) {
		if (curOp == T2_CALL)
			tx2op3[tx2qp] = off2Info(tx2op3[tx2qp]);
		else if ( curOp == T2_BYTEINDEX || curOp == T2_WORDINDEX) {
			i = (byte)tx2op1[tx2qp];
			tx2op2[i] += sub_575E(tx2op1[i]) * tx2op3[tx2qp];
		} else
			sub_69EB(3, &tx2op3[tx2qp]);
	}
	tx2Aux1b[tx2qp] = 0xc;
	tx2Aux2b[tx2qp] = 9;
}

void sub_69EB(byte arg1b, word *arg2w)
{
	word p;
	if (*arg2w != 0) {
		if (bC252 < arg1b)
			*arg2w = 0;
		else {
			p = *arg2w;
			*arg2w = tx2qp;

			while (p != 0) {
				p--;
				--*arg2w;
				if (tx2opc[*arg2w] == T2_LINEINFO) {
					if (tx2op2[*arg2w] == 0) {
						if (tx2op3[*arg2w] != 0) 
							p -= tx2op3[*arg2w] - tx2op1[*arg2w];
					}
				}
			}
		}
	}
}


byte b5286[] = {12, 9, 0, 1, 8, 0, 0};

void sub_6AA4()
{
	if (curOp == T2_VARIABLE) {
		tx2op1[tx2qp] = curInfo_p = off2Info(tx2op1[tx2qp]);
		if (testInfoFlag(F_MEMBER)) {
			curInfo_p = getOwningStructure();
			tx2Aux2b[tx2qp] = 4;
		} else if (testInfoFlag(F_AUTOMATIC))
			tx2Aux2b[tx2qp] = 0xa;
		else
			tx2Aux2b[tx2qp] = 4;
		curInfo_p = tx2op1[tx2qp];
		tx2op2[tx2qp] = getAddr();
		tx2Aux1b[tx2qp] = b5286[getInfoType()];
	} else if (T2_BIGNUMBER >= curOp) {
		tx2op2[tx2qp] = tx2op1[tx2qp];
		tx2Aux2b[tx2qp] = 8;
		tx2op1[tx2qp] = 0;
		if (curOp == T2_BIGNUMBER) {
			tx2Aux1b[tx2qp] = 1;
			tx2opc[tx2qp] = T2_NUMBER;
		} else
			tx2Aux1b[tx2qp] = 0;
	} else {
		tx2Aux1b[tx2qp] = 0;
		tx2op2[tx2qp] = 0;
	}
}

byte bC259;
byte bC25A;

void sub_6BD6() 
{
	bC259 = 4;
	bC25A = 4;
	for (tx2qp = 4; tx2qp <= bC1BF - 1; tx2qp++) {
		tx2Auxw[tx2qp] = 0;
		curOp = tx2opc[tx2qp];
		bC1D2 = b5124[curOp];
		if ((bC1D2 & 0xc0) == 0)
			sub_6D52();
		else if ((bC1D2 & 0xc0) == 0x40)
			sub_7111();
		if (curOp == T2_BEGCALL || curOp == T2_BEGMOVE)
			procCallDepth++;

	}
}


void sub_6C54(byte arg1b)
{
	if (arg1b != 0)
		tx2Auxw[arg1b]++;
}



void sub_6C73(byte arg1b)
{
	if (OPTIMIZE) 
		while (tx2qp - 1 > arg1b) {
			if (tx2opc[++arg1b] == curOp) {
				if (tx2op1[arg1b] == tx2op1[tx2qp]) {
					if (tx2op2[arg1b] == tx2op2[tx2qp]) {
						if (tx2Aux1b[arg1b] == tx2Aux1b[tx2qp]) {
							if (tx2op3[arg1b] != 0xff00) {
								tx2opc[tx2qp] = T2_OPTBACKREF;
								tx2op1[tx2qp] = arg1b;
								return;
							}
						}
					}
				}
			}
		} 
	if ((bC1D2 & 0xc0) == 0) {
		sub_6C54((byte)tx2op1[tx2qp]);
		sub_6C54((byte)tx2op2[tx2qp]);
	}
	tx2op3[tx2qp] = tx2qp;
}


void sub_6D52() 
{
	sub_6EAB(&tx2op1[tx2qp]);
	sub_6EAB(&tx2op2[tx2qp]);
	if (curOp == T2_STORE)
		sub_7055();
	else if (0 < procCallDepth) {
		sub_6C54((byte)tx2op1[tx2qp]);
		sub_6C54((byte)tx2op2[tx2qp]);
		if (curOp == T2_CALL)
			procCallDepth--;
		else if (curOp == T2_MOVE || curOp == T2_CALLVAR) {
			procCallDepth--;
			sub_6EAB(&tx2op3[tx2qp]);
			sub_6C54((byte)tx2op3[tx2qp]);
		} else
			tx2op3[tx2qp] = 0xff00;
	} else if (curOp == T2_OUTPUT || curOp == T2_TIME) {
		sub_6C54((byte)tx2op1[tx2qp]);
		sub_6C54((byte)tx2op2[tx2qp]);
	} else {
		tx2op3[tx2qp] = 0;
		sub_6EE1(tx2qp);
		sub_6C73(sub_6FE2());
		if (curOp == T2_JMPFALSE) {
			if (tx2opc[tx2qp - 1] == T2_NOT) {
				boC20F = true;
				tx2op2[tx2qp] = tx2op1[tx2qp - 1];
				sub_56A0(tx2qp, tx2qp - 1);
				tx2opc[tx2qp] = T2_SEMICOLON;
			}
		}
	}
}


void sub_6EAB(word *arg1w)
{
	if (*arg1w != 0) {
		if (tx2opc[*arg1w] == T2_OPTBACKREF) 
			*arg1w = tx2op1[*arg1w];
	}
}

byte boC25D;


void sub_6EE1(byte arg1b)
{
	boC25D = 0;
	if (tx2opc[arg1b] == T2_MEMBER) {
		sub_6F20((byte)tx2op1[arg1b]);
		sub_6F20((byte)tx2op2[arg1b]);
	} else
		sub_6F20(arg1b);
}


void sub_6F20(byte arg1b)
{
	byte i;
	if ((i = tx2opc[arg1b]) == T2_BASED)
		boC25D = 0xff;
	else if (i == T2_BYTEINDEX || i == T2_WORDINDEX) {
		if (tx2opc[tx2op2[arg1b]] != T2_NUMBER)
			boC25D = 0xff;
		else {
			curInfo_p = tx2op1[tx2op1[arg1b]];
			if (tx2op2[tx2op2[arg1b]] >= getDimension() || testInfoFlag(F_AT))
				boC25D = 0xff;
		}
	} else if (i == T2_VARIABLE) {
		curInfo_p = tx2op1[arg1b];
		if (testInfoFlag(F_AT))
			boC25D = 0xff;
	}
}

byte bC263;

byte sub_6FE2()	
{
	if (boC25D)
		bC263 = bC259;
	else
		bC263 = bC25A;

	sub_7018((byte)tx2op1[tx2qp]);
	sub_7018((byte)tx2op2[tx2qp]);
	return bC263;
}


void sub_7018(byte arg1b) 
{
	if (tx2opc[arg1b] != T2_CALL)
		arg1b = (byte)tx2op3[arg1b];
	if (arg1b != 0)
		if (bC263 < arg1b)
			bC263 = arg1b;
}


void sub_7055()
{
	if (bsub_70BC(tx2qp))
		bC25A = tx2qp;
	if (tx2op3[tx2op1[bC259 = tx2qp]] != 0xff00)
		tx2op3[tx2op1[tx2qp]] = tx2qp;
	sub_6C54((byte)tx2op1[tx2qp]);
	sub_6C54((byte)tx2op2[tx2qp]);
}

byte bsub_70BC(byte arg1b)
{
	sub_6EE1((byte)tx2op1[arg1b]);
	if (boC25D)
		return 0xFF;
	if (tx2op1[arg1b] == 0xac) {
		curInfo_p = tx2op1[tx2op1[arg1b]];
		if (testInfoFlag(F_AUTOMATIC))
			return 0xFF;
	}
	return 0;
}


void sub_7111()
{
	if (0 < procCallDepth)
		tx2op3[tx2qp] = 0xff00;
	else {
		tx2op3[tx2qp] = 0;
		if (curOp == T2_VARIABLE) {
			curInfo_p = tx2op1[tx2qp];
			if (testInfoFlag(F_AT))
				sub_6C73(bC259);
			else
				sub_6C73(bC25A);
		} else
			sub_6C73(bC25A);
	}
}

byte bC266;
word wC267;
word wC269;
word wC26B;
word wC26D;
byte bC26F;

byte bC272;
byte bC273;

void sub_717B()
{
	word p;

	bC266 = curOp - 0x12;
	bC26F = (byte)tx2op1[tx2qp];
	sub_5FBF(bC26F, &wC267, &wC269);
	if (bsub_7254(b5112[bC266] >> 6, wC269)) {
		bC26F = (byte)tx2op2[tx2qp];
		sub_5FBF(bC26F, &wC26B, &wC26D);
		if (bsub_7254((b5112[bC266] >> 3) & 0x7, wC26D)) {
			sub_611A();
			p = sub_72F2();
			sub_73C5();
			if (bC272 == 0 && bC273 == 8)
				p &= 0xFF;
			sub_5F4B(p, curInfo_p, bC272, bC273);
			bC1D2 = b5124[curOp = tx2opc[tx2qp]];
		}
	}
}

byte bsub_7254(byte arg1b, word arg2w)
{
	if (arg1b == 0) {
		if (tx2Aux2b[bC26F] != 4)
			return 0;
	} else if (arg1b != 3) {
		if (tx2Aux2b[bC26F] != 8)
			return 0;
		if (arg2w != 0) {
			if (arg1b == 2)
				return 0;
			if (arg1b == 4) {
				if (wC269 != 0)
					return 0;
			} else if (arg1b == 5) {
				if (wC269 != wC26D)
					return 0;
			}
		}
	} else if (0x4000 == arg2w)
			return 0;
	return 0xff;
}


word sub_72F2()
{
	switch (bC266) {
	case 0: return wC267;			// T2_DOUBLE
	case 1: return wC26B + wC267;	// T2_ADDB
	case 2: return wC267 - wC26B;	// T2_MINUSSIGN
	case 3: return wC26B * wC267;	// T2_STAR
	case 4: return wC267 / wC26B;	// T2_SLASH
	case 5: return wC267 % wC26B;	// T2_MOD
	case 6: return wC26B & wC267;	// T2_AND
	case 7: return wC26B | wC267;	// T2_OR
	case 8: return wC26B ^ wC267;	// T2_XOR
	case 9: return wC26B + wC267;	// T2_BASED
	case 10: return wC26B + wC267;	// T2_BYTEINDEX
	case 11: return (wC26B * 2) + wC267;	// T2_WORDINDEX
	case 12: return wC26B + wC267;	// T2_MEMBER
	case 13: return -wC267;			// T2_UNARYMINUS
	case 14: return ~wC267;			// T2_NOT
	case 15: return wC267 & 0xff;	// T2_LOW
	case 16: return (wC267 >> 8) & 0xff;	// T2_HIGH
	case 17: return wC267;			// T2_ADDRESSOF
	default: return 0;		// shouldn't happen
	}
}

void sub_73C5()
{
	if (curOp == T2_MEMBER && tx2Aux2b[tx2op1[tx2qp]] == 0xa) {
		bC272 = tx2Aux1b[tx2op2[tx2qp]];
		bC273 = 0x0A;
	} else 
		switch(b5112[bC266] & 7) {
		case 0:
			bC272 = 0;
			bC273 = 8;
			break;
		case 1:
			if (tx2Aux1b[tx2op1[tx2qp]] == 0 && tx2Aux1b[tx2op2[tx2qp]] == 0)
				bC272 = 0;
			else
				bC272 = 1;
			bC273 = 8;
			break;
		case 2:
			bC272 = 1;
			bC273 = 8;
			break;
		case 3:
			bC272 = tx2Aux1b[tx2op1[tx2qp]];
			bC273 = tx2Aux2b[tx2op1[tx2qp]];
			break;
		case 4:
			bC272 = tx2Aux1b[tx2op2[tx2qp]];
			bC273 = tx2Aux2b[tx2op2[tx2qp]];
			break;
		case 5:
			bC272 = sub_5748(tx2Aux1b[tx2op2[tx2qp]]);
			bC273 = 8;
		}
	if (wC269 == wC26D)
		curInfo_p = 0;
	else if (wC26D == 0) 
		curInfo_p = tx2op1[tx2op1[tx2qp]];
	else
		curInfo_p = tx2op1[tx2op2[tx2qp]];
}


byte b51E3[] = {
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0, 0x12, 0x14, 0x22, 0x33, 0x47, 0x47,
	0x41, 0x49, 0x4F,    0, 0x5C, 0x5E, 0x60
};
byte b5202[] = {
	   0,    0,    0,    0,    0,    0,    1,    3,
	   5,    7,    9, 0x0C, 0x0E,    0,    0,    0,
	   0,    0,    0, 0x14, 0x25, 0x33, 0x3C, 0x3F,
	0x41, 0x49, 0x4F, 0x55,    0,    0, 0x62
};

void sub_7550()
{
	if (T2_GT >= curOp) {
		if (tx2opc[tx2qp+1] == T2_JMPFALSE && tx2Auxw[tx2qp] == 1) {
			tx2opc[tx2qp + 1] = T2_JNZ;
			tx2op1[tx2qp + 1] = tx2op2[1];
			if (boC20F) {
				tx2op2[tx2qp + 1] = bC209[curOp];
				boC20F = false;
			} else
				tx2op2[tx2qp + 1] = curOp;
			tx2Auxw[tx2qp] = 0;
		} else
			curOp = tx2opc[tx2qp] = curOp + 0x26;
	} else if (!bsub_765B(b5202[curOp], (byte)tx2op2[tx2qp], (byte)tx2op1[tx2qp]))
		bsub_765B(b51E3[curOp], (byte)tx2op1[tx2qp], (byte)tx2op2[tx2qp]);
}



byte arg1b_765B;
byte arg2b_765B;
byte arg3b_765B;

byte bC27A;
word wC27B;
word wC27D;
byte bC27F;


byte bsub_765B(byte arg1b, byte arg2b, byte arg3b)
{
	byte i;
	arg1b_765B = arg1b;
	arg2b_765B = arg2b;
	arg3b_765B = arg3b;
	sub_5FBF(arg2b_765B, &wC27B, &wC27D);
	bC27A = 0;
	i = 0xff;

	while (i) {
		if ((bC27F = b50AD[arg1b_765B]) == 0xff)
			return 0;
		if (bC27F & 0x10) {
			i = ~bsub_76E2(arg2b_765B);
			if ((bC27F & 4) && !i)
				i = ~bsub_7801();
		} else 
			i = ~bsub_7801();
		arg1b_765B++;
	}
	sub_7925();
	return 0xff;
}

byte bsub_76E2(byte arg1b)
{
	if (bC27F & 0x40) {
		if (tx2Aux1b[arg1b] == 1)
			return 0xFF;
		if (tx2Aux1b[arg1b] == 3)
			return 0xFF;
		return 0;
	} else if (tx2Aux1b[arg1b] == 0)
		return 0xFF;
	else if (tx2Aux1b[arg1b] == 2)
		return 0xFF;
	else if (tx2Aux1b[arg1b] == 8)
		return 0xFF;
	return 0;
}

void sub_7754()
{
	byte i, j;
	i = tx2qp + 1;
	while (tx2opc[i] == T2_OPTBACKREF && i < bC1BF)
		i++;
	j = tx2qp;
	if (tx2opc[i] == T2_STORE) {
		if (arg3b_765B == tx2op1[i]) {
			if (tx2op2[i] == j) {
				if (tx2Auxw[j] == 1) {
					bC27A = 6;
					tx2opc[i] = T2_SEMICOLON;
					tx2Auxw[j] = 0;
					tx2Auxw[arg3b_765B]--;
				}
			}
		}
	}
}

word w502A[] = { 0, 1, 2, 3, 4, 8, 9, 0x0A,
	 0x10, 0xFD, 0xFE, 0xFF, 0xFFFD, 0xFFFE, 0xFFFF };
// look at mod model below
byte bsub_7801() 
{
	word p;
	if (wC27D != 0 || (curOp < T2_BASED && tx2Aux2b[arg2b_765B] != 8))
		return 0;
	if (bC27F & 0x20)
		if (!bsub_76E2(arg3b_765B))
			return 0;
	if (bC27F & 1)
		if (bsub_76E2(arg2b_765B) != bsub_76E2(arg3b_765B))
			return 0;
	if (bC27F & 4) {
		if (bC27F & 8)
			if (wC27B != 0)
				return 0xFF;
			else
				return 0;
		if ((p = w502A[bt5048[arg1b_765B] >> 3]) != wC27B)
			return 0;
		if (bC27F & 0x80)
			sub_7754();
		return 0xFF;
	} else if ((bC27F & 0x40) == 0) {
		p = w502A[bt5048[arg1b_765B] >> 3];
		bC27A = wC27B % p;
		return 0xFF;
	} else if (wC27B <= 4) {
		bC27A = (byte)wC27B;
		return 0xff;
	}
	return 0;
}

void sub_7925()
{
	byte i, j, k;
	i = b5221[--arg1b_765B] + bC27A;
	j = bt5048[arg1b_765B] & 3;
	if (i == 0x0AD) {
		if (j == 0)
			wC27B = 0;
		sub_611A();
		bC27F = 0;
		k = 1;
		if (curOp != T2_STAR)
		    if (curOp != T2_SLASH)
			if (curOp != T2_MOD)
			    if (wC27B < 0x100)
				if (bsub_76E2(arg2b_765B))
				    if (arg3b_765B == 0)
					k = 0;
				    else if (bsub_76E2(arg2b_765B))
					k = 0;
		sub_5F4B(wC27B, 0, k, 8);
	} else {
		if (j == 1) {
			tx2op1[tx2qp] = arg3b_765B;
			tx2Auxw[arg2b_765B]--;
		} else {
			if (tx2op2[tx2qp] != 0)
				tx2Auxw[tx2op2[tx2qp]]--;
			if (bC1D2 & 4) {
				tx2op1[tx2qp] = tx2op2[1];
				if (boC20F) {
					i = 0x43 - i;
					boC20F = false;
				}
			}
		}
		tx2opc[tx2qp] = i;
		tx2op2[tx2qp] = 0;
	}
	bC1D2 = b5124[curOp = i];
}

byte b7A81[] = {0x3C, 0x46, 0x50, 0x5A};

byte bC28B[4];
byte bC28F;

void sub_7A85()
{
	byte i, j, k, m, n;
	sub_597E();
	for (i = 0; i <= 3; i++) {
		if (boC072[i] || boC069[i])
			bC28B[i] = 0xc8;
		else if (boC060[i])
			bC28B[i] = b7A81[i] + 0xf;
		else
			bC28B[i] = b7A81[i];
	}

	if ((bC0C3[wC1C3] >> 4) != 0xb && bC140[wC1C3] != 0)
		bC28B[0] = 0x0C8;
	for (m = 0; m <= 1; m++) {
		if (bC0B7[m] != 0) {
			if (bC0B5[m] == 9 && bC140[wC1C3] == bC0B7[m]) {
				k = m;
				bC28F = 0xce;
				boC1D8 = false;
			} else
				bC28F = 0x32;
			sub_7D7E(b52B5[bC0BF[m]]);
			sub_7D7E(bt4C2D[bC0BD[m]] >> 5);
		}
	}

	n = 0xc8;
	for (i = 0; i <= 3; i++) {
		if (bC28B[i] <= n)
			n = bC28B[j = i];
	}

	if (n == 0x0C8) {
		if (boC069[3]) {
			sub_7D32();
			if (bC0B5[0] == 3) {
				bC0B5[0] = 2;
				sub_61A9(0);
			} else {
				bC0B5[1] = 2;
				sub_61A9(1);
			}
		}
		sub_7D54();
		j = 3;
	} else
		sub_6416(j);

	if (bC045[j] == 0xb) {
		bC045[j] = 0;
		if (j != 0 && bC04E[j] != 0) {
			wC1DC[0] = j;
			encodeFragData(CF_MOVLRHR);
			pc++;
		}
	}
	if (!boC1D8) {
		if (bC0B5[1 - k] == 9) {
			if (bC0B7[1 - k] == bC0B7[k]) {
				bC0B5[1 - k] = j;
				bC0B3[1 - k] = bC045[j];
				sub_61A9(1 - k);
			} else
				boC1D8 = true;
		}
		bC0B5[k] = j;
		bC0B3[k] = bC045[j];
		sub_61A9(k);
	}
}

void sub_7D32()
{
	encodeFragData(CF_XCHG);
	pc++;
	sub_5B96(3, 4);
	sub_5B96(2, 3);
	sub_5B96(4, 2);
}

void sub_7D54()
{

	wC1DC[0] = 0xa;
	wC1DC[1] = wC1C3;
	encodeFragData(CF_XTHL);
	pc++;
	sub_5C97(4);
	sub_5C1D(3);
	sub_5B96(4, 3);
}


void sub_7D7E(byte arg1b)
{
	if (3 >= arg1b) 
		if (bC28B[arg1b] < 0xc8)
			bC28B[arg1b] += bC28F;
}


const byte b5012[] = {
	   6,    8, 0x0A, 0x3A, 0x0A,    6,    7, 0x0F,
	0x6D,    9, 0x12, 0x11, 0x10, 0x10, 0x0D, 0x0C,
	0x0B, 0x0B,    4,    4, 0x0E,    5,    0,    0
};

const byte b528D[] = {
	0, 0, 0, 0, 0, 0, 2, 2, 2, 3, 3, 3, 1, 1, 1, 1,
	1, 8, 4, 4, 4, 5, 5, 5, 6, 6, 6,10, 9, 8, 3, 3,
	1, 1, 6, 2, 2, 0, 0,11
};

byte bC294;
byte bC295;
byte bC296;
byte bC297;
byte bC298;


void sub_7DA9() 
{
	byte i;
	sub_7F19();
	if (bC298 == 0x17)
		sub_58F5(ERR214);
	else if (bC298 == 0x16) {
		bC0C1[bC295] = bC0BF[bC295];
		bC0B3[bC295] = b528D[bC0C1[bC295]];
		bC0B5[bC295] = b52B5[bC0C1[bC295]];
	} else if (bC298 == 0x12)
		boC1D8 = true;
	else {
		if ((bC294 = bt4C2D[bC298] >> 5) > 3)
			bC294 = b52B5[bC0BF[bC295]];
		bC297 = bC0B5[bC295];
		sub_597E();
		sub_7FFC();
		i = b5012[bC298];
		sub_8086(); 
		bC1DB = 0;
		sub_8148((bt4C2D[bC298] >> 3) & 3, (bt4029[i] >> 4) & 7);
		sub_8148((bt4C2D[bC298] >> 1) & 3, (bt4029[i] >> 1) & 7);
		sub_8207();
		sub_841E();
		bC0B3[bC295] = bC045[bC294];
		bC0B5[bC295] = bC294;
		sub_61A9(0);
		sub_61A9(1);
		encodeFragData(i);
		pc += fragLen(i);


	}
}

void sub_7F19()
{
	for (bC295 = 0; bC295 <= 1; bC295++) {
		if (bC0B7[bC295] != 0 && bC0BB[bC295] != 0) {
			if (bC0BB[bC296 = 1 - bC295] != 0) {
				if (bsub_7FD9(bt4C2D[bC0BD[bC295]] >> 5)
				  || bsub_7FD9(b52B5[bC0BF[bC295]]))
					bC0BB[bC296] += 0xa;
			}
		}
	}

	if (bC0BB[1] < bC0BB[0])
		bC295 = 0;
	else
		bC295 = 1;

	bC296 = 1 - bC295;
	bC298 = bC0BD[bC295];
}


byte bsub_7FD9(byte arg1b)
{
	if (3 >= arg1b)
		if (bC0B5[bC296] == arg1b) 
			return 0xFF;
	return 0;
}


void sub_7FFC()
{
	if (bC298 == 0x13) {
		if (boC069[3] || boC072[3])
			bC298 = 0x15;
	} else if (bC298 == 0x14) {
		bC294 = 5 - bC297;
		if (bC0B5[1] == bC0B5[0])
			if (bC0B3[bC295] == 0)
				bC298 = 0x0E;
			else
				bC298 = 0x11;
	} else if (bC298 == 8) {
		if (tx2op1[bC0B7[bC295]] != 0) {
			bC298 = 6;
			if (bC294 == 0)
				bC294 = 1;
		}
	}
}


void sub_8086()
{
	if (bC298 >= 9 && 0xD >= bC298)
		sub_63AC(bC297);

	if (bt4C2D[bC298] & 1) {
		if (bC297 != bC294) 
			sub_5D6B(bC294);
		else if (bC298 >= 9 && 0xD >= bC298) {
			bC0B5[bC295] = 9;
			sub_597E();
			sub_5D6B(bC294);
			bC0B5[bC295] = bC297;
			sub_597E();
		}
	} else if (bC298 == 0x15) {
		if (boC069[3])
			bC0B5[bC296] = 9;
	} else if (bC298 == 0x14) { 
		if (bC0B5[bC296] == bC294)
			bC0B5[bC296] = bC297;
	}
}

void sub_8148(byte arg1b, byte arg2b)
{
	byte i;
	if (arg2b == 0)
		return;
	if (arg2b == 1 || arg2b == 2) {
		if (arg1b == 0)
			wC1DC[bC1DB] = bC297;
		else
			wC1DC[bC1DB] = bC294;
		bC1DB++;
	} else if (arg1b == 2) {
		wC1DC[bC1DB] = 8;
		bC1DB +=2;
	} else if (arg1b == 3) {
		wC1DC[bC1DB] = 0xA;
		wC1DC[bC1DB+1] = wC1C3;
		bC1DB += 3;
	} else {
		i = bC1DB;
		sub_636A(bC295);
	}

}

void sub_8207()
{
	switch (bt4C15[bC298] >> 4) {
	case 0:
		sub_5C97(bC294);
		wC1C3--;
		break;
	case 1:
		sub_5C97(4);
		sub_5C1D(3);
		sub_5B96(4, 3);
		break;
	case 2:
		boC057[bC294] = 0xFF;
		bC0A8[bC294] = 0;
		bC04E[bC294] = bC0B7[bC295];
		wC096[bC294] = 0x100;
		if (wC1DC[0] == 0xA) {
			wC084[bC294] = - (wC1DC[1]*2);
			if (bC0C3[tx2op3[bC0B7[bC295]]] == 0xb0)
				if (bC298 == 5) {
					wC084[bC294]--;
					wC1DC[2]++;
				}
		} else
			wC084[bC294] = wC1DC[3] - wC1C3 * 2;
		break;
	case 3:
		boC057[bC294] = 0xFF;
		bC0A8[bC294] = 0;
		bC04E[bC294] = bC0B7[bC295];
		sub_5FBF(bC04E[bC294], &wC084[bC294], &wC096[bC294]);
		break;
	case 4:
		boC057[bC294] = 0;
		bC04E[bC294] = bC0B7[bC295];
		if (bC0B3[bC295] == 4 || bC0B3[bC295] == 5) {
			bC0A8[bC294] = bC0C3[tx2op3[bC0B7[bC295]]] & 0xf;
			if (bC0A8[bC294] > 7)
				bC0A8[bC294] |= 0xf0;
		} else
			bC0A8[bC294] = 0;
		break;
	case 5:
		sub_5B96(bC297, bC294);
		break;
	case 6:
		sub_5B96(3, 4);
		sub_5B96(2, 3);
		sub_5B96(4, 2);
		break;
	case 7:
		break;
	}
}

void sub_841E()
{
	switch (bt4C15[bC298] & 0xf) {
	case 0: break;
	case 1: bC045[bC294] = 1;
		break;
	case 2:	bC045[bC294] = 0;
		break;
	case 3: bC045[bC294] = 6;
		break;
	case 4:	if (bC0B5[bC295] != 8)
			bC045[bC294] = sub_5748(bC0B3[bC295]);
		else if (bC0B3[bC295] == 0)
			bC045[bC294] = 6;
		else
			bC045[bC294] = bC0B3[bC295];
		break;
	case 5:
		bC045[bC294] = bC0B3[bC295] - 2;
		bC0A8[3]++;
		break;
	}
}




void sub_84ED()
{
	byte i;
	if (cfrag1 > CF_3) {
		bC1DB = 0;
		sub_8698((b42F9[cfrag1] >> 4), (bt4029[cfrag1] >> 4) & 7);
		if (cfrag1 == CF_67 || cfrag1 == CF_68)
			wC1DC[bC1DB - 1] += 2;
		sub_8698(b42F9[cfrag1] & 0xf, (bt4029[cfrag1] >> 1) & 7);
		encodeFragData(cfrag1);
		pc += fragLen(cfrag1);
		if (cfrag1 == CF_DELAY) { 
			((word *)helpers_p)[105] = 1;
			if (wC1C5 < (wC1C3 + 1) * 2)
				wC1C5 = (wC1C3 + 1) * 2;
		} else if (cfrag1 > CF_171) {
			i = b413B[cfrag1 - CF_174]; // ???
			i = b4128[i] + b425D[b4273[curOp]] * 11;
			i = (b418C[i] & 3) + b3FCD[b418C[i] >> 2];
			((word *)helpers_p)[i] = 1;
			if (curOp == T2_SLASH || curOp == T2_MOD
			 || curOp == T2_44) {
				if (wC1C5 < (wC1C3 + 2) * 2)
					wC1C5 = (wC1C3 + 2) * 2;
			} else if (wC1C5 < (wC1C3 + 1) * 2)
					wC1C5 = (wC1C3 + 1) * 2;
		}
	}
}

byte sub_8683(byte arg1b) 
{
	if (arg1b == bC0B5[0])
		return 1;
	return 0;
}


void sub_8698(byte arg1b, byte arg2b)
{
	byte i;
	word p;
	switch (arg1b) {
	case 0:	return;
	case 1: i = 0;
		break;
	case 2: i = 1;
		break;
	case 3: wC1DC[bC1DB] = 0xA;
		wC1DC[bC1DB + 1] = wC1C3;
		bC1DB += 3;
		return;
	case 4:
		i = sub_8683(3);
		break;
	case 5: i = sub_8683(0);
		break;
	case 6: if (arg2b == 7) {
			wC1DC[bC1DB] = 0x10;
			wC1DC[bC1DB + 1] = info2Off(tx2op3[tx2qp]);
			bC1DB += 2;
		} else
			sub_61E0((byte)tx2op3[tx2qp]);
		return;
	}
	if (3 >= arg2b)
		sub_636A(i);
	else {
		wC1DC[bC1DB] = arg2b + 9;
		if (arg2b == 6)
			wC1DC[bC1DB + 1] = tx2op2[1];
		else
			sub_5FBF(bC0B7[i], &wC1DC[bC1DB+1], &p);
		bC1DB += 2;
	}
}

byte bC2A8;
word wC2A9;
word wC2AB;
word wC2AD;


void sub_87CB() 
{
	bC0B7[0] = (byte)tx2op1[tx2qp];
	bC0B7[1] = (byte)tx2op2[tx2qp];
	wC2AB = wAF54[curOp];
	wC2AD = b499B[curOp] + wC2AB - 1;
	sub_8CF5();

	while (1) {
		sub_8DCD();	// OK
		if (bC0B9[0] == 0)
			if (bC0B9[1] == 0)
				break;
		if (boC1D8)
			sub_7A85();
		else
			sub_7DA9();
	}
	sub_8F16();
	sub_611A();
	sub_5E66(wt48DF[bC1D9] >> 0xc);
	sub_8F35();
	sub_84ED();
	sub_90EB();
}

byte bC2A5;
byte bC2A6;

byte bsub_8861()
{
	byte i;
	for (wC1D6 = wC2AB; wC1D6 <= wC2AD; wC1D6++) {
		i = sub_5679(bC2A5);
		if (i >= 0 && 3 >= i)
			return 0xFF;
		if (i >= 0xC && 0xE >= i)
			return 0xFF;
	}
	return 0;
}


void sub_88C1()
{
	if (bsub_8861()) {
		for (bC2A8 = 0; bC2A8 <= 3; bC2A8++) {
			if (bC04E[bC2A8] == bC2A6) {
				if (bC045[bC2A8] == 0 || bC045[bC2A8] == 1
				   || bC045[bC2A8] == 6) {
					bC0B3[bC2A5] = bC045[bC2A8];
					bC0B5[bC2A5] = bC2A8;
					if (bC0B5[1 - bC2A5] != bC2A8)
						return;
				}
			}
		}
	}
}

void sub_894A()
{
	if (bC0B5[bC2A5] > 3) {
		for (bC2A8 = 1; bC2A8 <= 3; bC2A8++) {
			if (bC04E[bC2A8] == bC2A6) {
				if (bC045[bC2A8] == 2 || bC045[bC2A8] == 3) {
					bC0B3[bC2A5] = bC045[bC2A8];
					bC0B5[bC2A5] = bC2A8;
					if (bC0B5[1 - bC2A5] != bC2A8)
						return;
				}
			}
		}
	}
}


void sub_89D1() 
{
	byte i;
	word p;
	if (bC0B5[bC2A5] == 0xA) 
		wC2A9 = tx2op2[bC2A6];
	else if (bC0B5[bC2A5] == 9) {
		wC2A9 = tx2op3[bC2A6];
		if ((!boC069[0] && boC072[0]) || bC0B1 > 0 || wC1C3 != wC2A9) {
			i = bC0B2 + bC0B1;
			for (p = wC2A9; p <= wC1C3; p++) {
				if (bC140[p] != 0)
					i++;
			}
			if (i < 4)
				boC1D8 = true;
			else
				bC0B5[bC2A5] = 0xA;
		}
		wC2A9 = - (wC2A9 * 2);
	}
}

void sub_8A9C()
{
	word p, q, r;
	byte i, j;
	if (bC0B5[bC2A5] == 0xA) {
		p = wC2A9;
		q = 0x100;
		i = 4;
		j = sub_5748(bC0B3[bC2A5]);
	} else if (bC0B5[bC2A5] == 8 && bC0B3[bC2A5] == 1) {
		sub_5FBF(bC0B7[bC2A5], &p, &q);
		i = 2;
		j = 1;
	} else if ((bC0B5[bC2A5] == 4) &
	   ((bC0B3[bC2A5] == 0) | (bC0B3[bC2A5] == 8) | (~bsub_8861()))) {
		sub_5FBF(bC0B7[bC2A5], &p, &q);
		i = 2;
		j = sub_5748(bC0B3[bC2A5]);
	} else
		return;

	for (bC2A8 = 1; bC2A8 <= 3; bC2A8++) {
		if (boC069[bC2A8]) {
			if (bC0B7[1] == bC0B7[0] && curOp != T2_STORE)
				if (bC0B5[bC2A5] > 3)
					bC0B5[bC2A5] = bC2A8;
		} else if (!boC072[bC2A8] && wC096[bC2A8] == q
			    && boC057[bC2A8] && bC045[bC2A8] >= 1
			    && bC045[bC2A8] <= 6) {
			r = wC084[bC2A8] + bC0A8[bC2A8] - p;
			if (r > 0xff)
				r = -r;
			if (i > r) {
				bC0B5[bC2A5] = bC2A8;
				i = (byte)r;
			}
		}
	}
	if (bC0B5[bC2A5] <= 3) {
		bC045[bC2A8 = bC0B5[bC2A5]] = j;
		bC0B3[bC2A5] = j;
		bC04E[bC2A8] = bC0B7[bC2A5];
		bC0A8[bC2A8] += wC084[bC2A8] - p;
		wC084[bC2A8] = p;
	}
}



void sub_8CF5()
{
	byte i;
	bC0B5[0] = 8;
	bC0B5[1] = 8;
	for (bC2A5 = 0; bC2A5 <= 1; bC2A5++) {
		if ((bC2A6 = bC0B7[bC2A5]) == 0) {
			bC0B3[bC2A5] = 0xC;
		} else if ((i = tx2opc[bC2A6]) == T2_STACKPTR) {
			bC0B3[bC2A5] = 0xA;
		} else if (i == T2_LOCALLABEL) { 
			bC0B3[bC2A5] = 9;
		} else {
			bC0B3[bC2A5] = tx2Aux1b[bC2A6];
			bC0B5[bC2A5] = tx2Aux2b[bC2A6];
			sub_88C1(); 
			sub_894A();// checked
		}
	}
	for (bC2A5 = 0; bC2A5 <= 1; bC2A5++) {
		bC2A6 = bC0B7[bC2A5];
		sub_597E();
		sub_89D1();
		sub_8A9C();
		sub_61A9(bC2A5);
	}
}



void sub_8DCD()
{
	byte h, i, j, k, m, n;
	j = 198;
	for (wC1D6 = wC2AB; wC1D6 <= wC2AD; wC1D6++) {
		k = sub_8E7E(0);
		m = sub_8E7E(1);
		n = bt4C45[k] + bt4C45[m] + fragLen(b4A21[wC1D6]);
		if (n < j) {
			j = n;
			h = k;
			i = m;
			cfrag1 = b4A21[wC1D6];
			bC1D9 = b46EB[wC1D6];
			bC0BF[0] = sub_5679(0);
			bC0BF[1] = sub_5679(1);
		}
	}
	sub_8ECD(0, h);
	sub_8ECD(1, i);
}




byte sub_8E7E(byte arg1b) 
{
	byte i;
	if (bC0B7[arg1b] == 0 || bC0B7[arg1b] == 1)
		return 1;
	i = sub_5679(arg1b);
	return bt4D23[bC0C1[arg1b]][i];
}




void sub_8ECD(byte arg1b, byte arg2b) 
{
	bC0B9[arg1b] = bt4C45[arg2b];
	bC0BB[arg1b] = b4CB4[arg2b];
	bC0BD[arg1b] = b4FA3[arg2b];
}



void sub_8F16()
{
	if (bC0B7[0] != 0)
		sub_63AC(bC0B5[0]);

	if (bC0B7[1] != 0) 
		sub_63AC(bC0B5[1]);

}


void sub_8F35()
{
	word p;
	if (curOp == T2_STKARG || curOp == T2_STKBARG || curOp == T2_STKWARG) {
		sub_5795(-(wB53C[procCallDepth]*2));
		wB53C[procCallDepth]++;
		wC1C3++;
	} else if (curOp == T2_CALL) {
		sub_5795(-(wB53C[procCallDepth]*2));
		curInfo_p = tx2op3[tx2qp];
		if (testInfoFlag(F_EXTERNAL))
			p = (wB53C[procCallDepth] + 1) * 2;
		else
			p = (wB528[procCallDepth] + 1) * 2 + getBasedOffset();
		if (wC1C5 < p)
			wC1C5 = p;
	} else if (curOp == T2_CALLVAR) {
		sub_5795(-(wB53C[procCallDepth]*2));
		if (wC1C5 < wC1C3 * 2)
			wC1C5 = wC1C3 * 2;
	} else if (curOp == T2_RETURN || curOp == T2_RETURNBYTE
	    || curOp == T2_RETURNWORD) {
		boC1CD = 0xff;
		sub_5EE8();
	} else if (curOp == T2_JMPFALSE) {
		sub_5795(0);
		if (boC20F) {
			cfrag1 = CF_JMPTRUE;
			boC20F = false;
		}
	} else if (curOp == T2_63) 
		sub_5795(0);
	else if (curOp == T2_MOVE) {
		if (wB53C[procCallDepth] != wC1C3) {
			sub_5795(-((wB53C[procCallDepth] + 1) * 2));
			sub_6416(3);
		}
		if (bC045[3] == 1)
			cfrag1 = CF_MOVE_HL;
	}
}


void sub_90EB()
{
	word p, q;
	byte i, j, k;
	p = wt48DF[bC1D9] * 16;
	q = w493D[bC1D9];
	k = 0;
	if (curOp == T2_STORE) {
		sub_940D();
		if (tx2Auxw[bC0B7[1]] == 0  && tx2Auxw[bC0B7[0]] > 0) {
			if (cfrag1 == CF_MOVMLR || cfrag1 == CF_STA) {
				bC045[bC0B5[1]] = 0;
				bC04E[bC0B5[1]] = bC0B7[0];
			} else if (cfrag1 == CF_SHLD || cfrag1 == CF_MOVMRP) {
				bC045[bC0B5[1]] = 1;
				bC04E[bC0B5[1]] = bC0B7[0];
			}
		}
	} else if (curOp >= T2_51 && T2_56 >= curOp)
		sub_940D();
	for (bC2A8 = 5; bC2A8 <= 8; bC2A8++) {
		i = p >> 0xd;
		j = q >> 0xc;
		p <<= 3;
		q <<= 4;
		if (3 >= j) {
			sub_5B96(j, bC2A8);
			if (i == 1)
				bC0A8[bC2A8]++;
			else if (i == 2) {
				if (bC045[bC2A8] == 0) {
					bC045[bC2A8] = 6;
				} else {
					bC045[bC2A8] = 1;
					boC057[bC2A8] = 0;
				}
			}
		} else if (j == 4) {
			boC057[k = bC2A8] = 0;
			if (0 < tx2Auxw[tx2qp]) {
				bC04E[bC2A8] = tx2qp;
				tx2Aux1b[tx2qp] = bC045[bC2A8] = fragFlg_Len[cfrag1] >> 5;
				bC0A8[bC2A8] = 0;
			} else
				bC04E[bC2A8] = 0;
		} else if (j == 5) {
			bC04E[bC2A8] = 0;
			wC096[bC2A8] = 0;
			bC0A8[bC2A8] = 0;
			boC057[bC2A8] = 0xFF;
			bC045[bC2A8] = 0;
			wC084[bC2A8] = i;
		} else {
			bC04E[bC2A8] = 0;
			boC057[bC2A8] = 0;
		}
	}
	if (k == 0 && tx2Auxw[tx2qp] > 0) {
		for (bC2A8 = 5; bC2A8 <= 8; bC2A8++) {
			if (bC04E[bC2A8] == 0) 
				if (!boC057[k = bC2A8])
					break;
		}
		if (k != 0) {
			bC04E[k] = tx2qp;
			boC057[k] = 0;
			bC045[k] = 0;
			tx2Aux1b[tx2qp] = 0;
			bC0A8[k] = 0;
		}
	}
	for (bC2A8 = 0; bC2A8 <= 3; bC2A8++)
		sub_5B96(bC2A8 + 5, bC2A8);
}


void sub_940D()
{
	for (bC2A8 = 0; bC2A8 <= 3; bC2A8++) {
		if (bC04E[bC2A8] == bC0B7[0]) 
			if (bC045[bC2A8] < 2 || 5 < bC045[bC2A8])
				bC04E[bC2A8] = 0;
	}
}



void sub_9457()
{
	if (enterBlk()) {
		wB488[procChainId] = pc;
		wB4B0[procChainId] = wC1C3;
		wB4D8[procChainId] = wC1C5;
		extProcId[procChainId] = curExtProcId;
		procChainNext[blkSP] = procChainId;
		procChainId = blkSP;
		blkCurInfo[blkSP] = curInfo_p = off2Info(tx2op1[tx2qp]);
		curExtProcId = getProcId();
		pc = 0;
		emitTopItem();
		sub_981C();
	}
}

void findParamInfo(byte arg1b)
{
	curInfo_p = blkCurInfo[blkSP];
	while (arg1b != 0) {
		advNextInfo();
		arg1b--;
	}
}

byte curParamCnt;
byte bC2D1;
byte bC2D2;


void sub_9514()
{
	if (getInfoType() == ADDRESS_T) {
		wC1DC[0] = bC2D1;
		encodeFragData(CF_MOVMRPR);
		pc += 3;
	} else {
		wC1DC[0] = bC2D1;
		encodeFragData(CF_MOVMLR);
		pc++;
	}
	if (curParamCnt != bC2D2) {
		encodeFragData(CF_DCXH);
		pc++;
	}
}

void sub_9560() 
{
	byte i;
	if ((i = curParamCnt) == 1)
		bC2D1 = 1;
	else
		bC2D1 = 2;
	for (bC2D2 = 1; bC2D2 <= curParamCnt; bC2D2++) {
		findParamInfo(i);
		if (bC2D2 == 2) 
			bC2D1 = 1;
		else if (bC2D2 == 3) { 
			wC1DC[0] = 2;	// pop d
			wC1DC[1] = 8;
			encodeFragData(CF_POP);
			wC1DC[0] = 1;	// pop b
			wC1DC[1] = 8;
			encodeFragData(CF_POP);
			pc += 2;
		} else if (3 < bC2D2) {
			wC1DC[0] = 1;	// pop b
			wC1DC[1] = 8;
			encodeFragData(CF_POP);
			pc++;
		}
		sub_9514();
		i--;
	}
	if (2 < curParamCnt) { 
		wC1DC[0] = 2;	// push d
		wC1DC[1] = 8;
		encodeFragData(CF_PUSH);
		pc++;
	}
}

void sub_9624(word arg1w)
{
	wC1DC[0] = 9;
	wC1DC[1] = arg1w;
	encodeFragData(CF_6);
	pc += 4;
}


void sub_9646(word arg1w)
{
	if (5 >= ((arg1w >> 1) + (arg1w & 1))) { 
		if (arg1w & 1) {
			encodeFragData(CF_DCXSP);
			pc++;
		}
		while (1 < arg1w) {
			wC1DC[0] = 3;	// push h
			wC1DC[1] = 8;
			encodeFragData(CF_PUSH);
			pc++;
			arg1w -= 2;
		} 
	} else {
			sub_9624(-arg1w);
			encodeFragData(CF_SPHL);
			pc++;
	}
}


void inxh()
{
	wC1DC[0] = 3;
	encodeFragData(CF_INX);
	pc++;
}


void opB(byte arg1b) 
{
	wC1DC[0] = 1;
	encodeFragData(arg1b);
	pc++;
}

void opD(byte arg1b) 
{
	wC1DC[0] = 2;
	encodeFragData(arg1b);
	pc++;
}


byte bC2D3;
void sub_9706()
{
	inxh();
	if (getInfoType() == ADDRESS_T) {
		opB(CF_MOVLRM);
		if (bC2D3 == 1)
			opD(CF_MOVMLR);

		inxh();
		opB(CF_MOVHRM);
	} else {
		opB(CF_MOVHRM);
		if (bC2D3 == 1)
			opD(CF_MOVMLR);
		inxh();
	}
	if (bC2D3 == 1)
		opD(CF_MOVMHR);
}



void movDEM()
{
	opD(CF_MOVRPM);
	pc += 2;
}


void sub_975F() 
{
	wC1DC[0] = bC2D1;
	wC1DC[1] = 8;
	encodeFragData(CF_PUSH);
	pc++;
	if (getInfoType() == BYTE_T) {
		encodeFragData(CF_INXSP);
		pc++;
	}
}


word wC2D4; // nested var

void sub_978E()
{
	if ((bC2D3 = curParamCnt) > 2)
		sub_9624(wC2D4);
	bC2D1 = curParamCnt == 1 ? 1 : 2;
	for (bC2D2 = 1; bC2D2 <= curParamCnt; bC2D2++) {
		findParamInfo(bC2D3);
		if (3 < bC2D2)
			sub_9706();
		else if (bC2D2 == 3) {
			movDEM();
			sub_9706();
		} else if (getInfoType() == BYTE_T) {
			wC1DC[0] = bC2D1;
			encodeFragData(CF_MOVHRLR);
			pc++;
		}
		sub_975F();
		bC2D1 = 1;
		bC2D3--;
	}
}

void sub_981C()	
{
	byte i, j;
	curParamCnt = getParamCnt();
	if (testInfoFlag(F_INTERRUPT)) {
		for (j = 0; j <= 3; j++) {
			wC1DC[0] = (3 - j);
			wC1DC[1] = 8;	// push h, push d, push b, push psw
			encodeFragData(CF_PUSH);
			pc++;
		}
	}
	if (testInfoFlag(F_REENTRANT)) {
		wC1C7 = getParentOffsetOrSize();
		if (0 < curParamCnt) {
			findParamInfo(curParamCnt);
			wC2D4 = wC1C7 - getAddr() - 1;
			if (getInfoType() == ADDRESS_T)
				wC2D4--;
			sub_9646(wC2D4);
			sub_978E();
		} else
			sub_9646(wC1C7);

		if (curParamCnt > 2)
			wC1C7 += (curParamCnt - 2) * 2;

		wC1C5 = 0;
	} else {
		if (curParamCnt > 0) {
			findParamInfo(curParamCnt);	// locate info for first param
			i = getInfoType() == ADDRESS_T ? 1 : 0;
			wC1DC[0] = 3;
			wC1DC[1] = 0xb;
			wC1DC[2] = i;
			wC1DC[3] = info2Off(curInfo_p);	// info for first param
			encodeFragData(CF_7);
			sub_9560();
			pc += 3;
		}
		wC1C7 = 0;
		if (2 < curParamCnt)
			wC1C5 = (curParamCnt - 2) * 2;
		else
			wC1C5 = 0;
	}
}



void sub_994D()	
{
	byte i, j;

	if (curOp == T2_LABELDEF) {
		boC1CC = false;
		curInfo_p = off2Info(tx2op1[tx2qp]);
		setAddr(pc);
	} else if (curOp == T2_LOCALLABEL) {
		boC1CC = false;
		((word *)localLabels_p)[tx2op1[tx2qp]] = pc;
		((byte *)word_381E)[tx2op1[tx2qp]] = curExtProcId;
	} else if (curOp == T2_CASELABEL) {
		((word *)localLabels_p)[tx2op1[tx2qp]] = pc;
		((byte *)word_381E)[tx2op1[tx2qp]] = curExtProcId;
		if ((word_3822 - botMem)/2 >= wC1CF) {
			((word *)botMem)[wC1CF] = tx2op1[tx2qp];
			wC1CF++;
		} else {
			emitTopItem();
			tx2SyntaxError(ERR202);	// LIMIT EXCEEDED: NUMBER OF
						// ACTIVE CASES
		}
	} else if (curOp == T2_JMP || curOp == T2_JNC
	   || curOp == T2_JNZ || curOp == T2_GO_TO) {
		i = tx2opc[tx2qp - 1];
		if (i == T2_RETURN || i == T2_RETURNBYTE
		  || i == T2_RETURNWORD || i == T2_GO_TO)
			return;
		sub_5795(0);
	} else if (curOp == T2_INPUT || (curOp >= T2_SIGN && T2_CARRY >= curOp)) {
		bC0B7[0] = 0;
		bC0B7[1] = 0;
		bC0B5[0] = 8;
		bC0B5[1] = 8;
		sub_597E();
		sub_5D6B(0);
		bC045[0] = 0;
		bC04E[0] = tx2qp;
		boC057[0] = 0;
		bC0A8[0] = 0;
		tx2Aux1b[tx2qp] = 0;
		tx2Aux2b[tx2qp] = 9;
	} else if (curOp == T2_STMTCNT) {
			j = tx2qp + 1;

			while (tx2opc[j] != T2_STMTCNT && tx2opc[j] != T2_EOF
			    && j < 0xFF) {
				if ((b5124[tx2opc[j]] & 0x20) == 0 || tx2opc[j] == T2_MODULE)
					goto L9B8D;
				j++;
			}
			curOp = CF_134;
			tx2opc[tx2qp] = CF_134;
	}
L9B8D:
	emitTopItem();
	pc += fragLen(curOp);
}

byte b9BA8[2] = {0xc, 0xd};
byte b9BAA[2] = {1, 2};
byte b9BAC[2] = {0xc, 0xd};
byte b9BAE[2] = {1, 2};

void sub_9BB0()	
{
	bC0B7[0] = (byte)tx2op1[tx2qp];
	bC0B7[1] = (byte)tx2op2[tx2qp];
	if (curOp >= T2_DOUBLE && T2_ADDRESSOF >= curOp)
		sub_717B();
	if (T2_MEMBER >= curOp) {
		sub_7550();
		if (curOp == T2_65)
			if (bsub_9C33())
				return;
	}
	if ((bC1D2 & 0xc0) == 0) {
		sub_87CB();
		if (curOp == T2_MOVE)
			procCallDepth = 0;
	} else if ((bC1D2 & 0xc0) == 0x80)
		sub_994D();
}

byte bsub_9C33() 
{
	byte i, j, k;

	i = (byte)tx2op1[tx2qp];
	if ((b5124[tx2opc[i]] & 0xc0) == 0) {
		if (tx2Auxw[i] > 1)
			return 0;
		if (tx2op3[i] != 0)
			bC140[tx2op3[i]] = tx2qp;
	}
	k = (byte)tx2Auxw[tx2qp];
	sub_56A0(i, tx2qp);
	tx2Auxw[tx2qp] = k;
	tx2Auxw[i]--;
	for (j = 0; j <= 3; j++) {
		if (bC04E[j] == i)
			bC04E[j] = tx2qp;
	}
	return 0xff;
}

void sub_9D06()
{
	byte i, j, k, m;
	byte *p;

	if (procCallDepth <= 10) {
		curInfo_p = tx2op3[tx2qp];
		i = getDataType();
		wAF54[132] = (i == 3) ? 1 : 0;
		m = j = getParamCnt();
		p = &bt44F7[wAF54[132]];
		k = 0;

		while (j > 0) {
			advNextInfo();
			if (--j < 2) {
				*p <<= 4;
				if (getInfoType() == ADDRESS_T)
					*p |= b9BA8[k];
				else
					*p |= b9BAA[k];
				k = 1;
			}
		}

		if (m == 1)
			*p <<= 4;
		sub_9BB0();
		wC1C3 = wB528[procCallDepth];
	}
	procCallDepth--;
}


byte *pb_C2EB;	

void sub_9DD7() 
{
	byte i;
	if (procCallDepth <= 10) {
		i = (byte)tx2op3[tx2qp];
		if (tx2opc[i] == T2_VARIABLE) {
			curInfo_p = tx2op1[i];
			wAF54[133] = testInfoFlag(F_AUTOMATIC) ? 3 : 4;
		} else if (tx2op3[i] == wB53C[procCallDepth]) {
			wAF54[133] = 5;
			wB528[procCallDepth]--;
		} else
			wAF54[133] = 2;

		pb_C2EB = &bt44F7[wAF54[133]];
		sub_9EAA((byte)tx2op1[tx2qp], 0);
		sub_9EAA((byte)tx2op2[tx2qp], 1);
		sub_9BB0();
		wC1C3 = wB528[procCallDepth];
	}
	procCallDepth--;
}


void sub_9EAA(byte arg1b, byte arg2b)
{
	*pb_C2EB <<= 4;
	if (arg1b != 0)
		if (tx2Aux1b[arg1b] == 0)
			*pb_C2EB |= b9BAE[arg2b];
		else
			*pb_C2EB |= b9BAC[arg2b];
}

void sub_9EF8()
{
	procCallDepth = 1;
	sub_9BB0();
	wB53C[procCallDepth] = wC1C3;
}



void sub_9F14() {
	if (enterBlk())
		blkCurInfo[blkSP] = wC1CF;
}


void sub_9F2F()
{
	word p, q;

	q = p = blkCurInfo[blkSP];
	if (exitBlk()) {
		while (p < wC1CF) {
			wC1DC[0] = 14;
			wC1DC[1] = ((word *)botMem)[p];
			encodeFragData(CF_DW);
			pc += 2;
			p++;
		}
		if (q == wC1CF) {
			tx2SyntaxError(ERR201);	// Invalid DO CASE block,
						// at least on case required
			emitTopItem();
		}
		wC1CF = q;
	}
}



void sub_9F9F()
{
	if (exitBlk()) {
		curInfo_p = blkCurInfo[procChainId];
		if (! boC1CC) {
			sub_5EE8();
			encodeFragData(CF_RET);
			pc++;
		}
		if (testInfoFlag(F_INTERRUPT))
			wC1C5 += 8;

		setDimension(pc);
		setBasedOffset(wC1C7 + wC1C5);
		pc = wB488[procChainId = procChainNext[procChainId]];
		bC1E6 = 0;
		putTx1Byte(0xa4);
		putTx1Word(info2Off(blkCurInfo[procChainId]));
		putTx1Word(pc);
		wrFragData();
		wC1C3 = wB4B0[procChainId];
		wC1C5 = wB4D8[procChainId];
		wC1C7 = 0;
		curExtProcId = extProcId[procChainId];
	}
}

void sub_A072(byte arg1b)
{
	word p;
	curInfo_p = off2Info(tx2op1[tx2qp]);
	p = getDimension() - arg1b;
	if (p < 0x100)
		sub_5F4B(p, 0, 0, 8);
	else
		sub_5F4B(p, 0, 1, 8);
}


void sub_A0C4()	
{
	word p;
	p = sub_575E(off2Info(tx2op1[tx2qp]));
	if (p < 0x100)
		sub_5F4B(p, 0, 0, 8);
	else
		sub_5F4B(p, 0, 1, 8);
}

void sub_A10A()
{
	if (++procCallDepth <= 10) {
		sub_5E66(0xf);
		wB528[procCallDepth] = wC1C3;
		wB53C[procCallDepth] = wC1C3;
	} else if (procCallDepth == 11) {
		tx2SyntaxError(ERR203);	// LIMIT EXCEEDED: NESTING OF TYPED
					// PROCEDURE CALLS
		emitTopItem();
	}
}




void sub_A153()
{
	sub_A266();
	for (tx2qp = 4; tx2qp <= bC1BF - 1; tx2qp++) {
		curOp = tx2opc[tx2qp];
		switch ((bC1D2 = b5124[curOp]) >> 6) {
		case 0:
			if (curOp == T2_CALL)
				sub_9D06();
			else if (curOp == T2_CALLVAR)
				sub_9DD7();
			else if (curOp == T2_BEGMOVE)
				sub_9EF8();
			else
				sub_9BB0();
			break;
		case 1:
			if (curOp == T2_LENGTH)
				sub_A072(0);
			else if (curOp == T2_LAST)
				sub_A072(1);
			else if (curOp == T2_SIZE)
				sub_A0C4();
			break;
		case 2:
			if (curOp == T2_PROCEDURE)
				sub_9457();
			else
				sub_994D();
			break;
		case 3:
			if (curOp == T2_CASE)
				sub_9F14();
			else if (curOp == T2_ENDCASE)
				sub_9F2F();
			else if (curOp == T2_ENDPROC)
				sub_9F9F();
			else if (curOp == T2_BEGCALL)
				sub_A10A();
			break;
		}

		tx2op3[tx2qp] = 0;
	}
	sub_5795(0);
	boC1CC = boC1CD;
}



void sub_A266()
{
	byte i;

	boC1CD = 0;
	for (i = 0; i <= 3; i++) {
		bC045[i] = 0xc;
		bC04E[i] = 0;
		boC057[i] = 0;
	}
}



