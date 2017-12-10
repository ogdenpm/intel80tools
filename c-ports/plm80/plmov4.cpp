// $Id: plmov4.cpp,v 1.1 2003/10/04 21:08:48 Mark Ogden Exp $
#include <stdio.h>
#include "plm.hpp"
#include "common.hpp"
#include "trace.hpp"




byte bo8102;
byte bo8103;
byte printOn;
byte locLabStr[32];		// used to hold symbol name
word wa8125[3];
byte bo812B = 0xff;
word baseAddr;
byte b812E;
word w812F;
word lineNo;
word depth;
word stmtNo;
word w8137;
word w8139;
byte bo813B =  0xff;
byte bo813C = 0xff;
byte lstLine[131];

#pragma pack(push, 1)
struct {
	byte type;
	word len;
	byte seg;
	word addr;
	byte data[1017];
	byte crcspc;
} rec6_4 = { 6, 0, 1};
static struct {
	byte type;
	word len;
	byte fixtype;
	word data[508];
	byte crcspc;
} rec22 = { 0x22, 0, 3};
byte pad89BD;
static struct {
	byte type;
	word len;
	byte seg;
	byte fixtype;
	word data[508];
	byte crcspc;
} rec24_1 = { 0x24, 0, 2, 3};
static struct {
	byte type;
	word len;
	byte seg;
	byte fixtype;
	word data[49];
	byte crcspc;
} rec24_2 = { 0x24, 0, 3, 3};
struct {
	byte type;
	word len;
	byte seg;
	byte fixtype;
	word data[49];
	byte crc;
} rec24_3 = { 0x24, 0, 4, 3};
static struct {
	byte type;
	word len;
	byte fixtype;
	struct {
		word symId;
		word addr;
	} fixdata[254];
	byte crcspc;
} rec20 = { 0x20, 0, 3};
byte pad9289;
struct {
	byte type;
	word len;
	byte seg;
	struct {
		word addr;
		word line;
	} xref[254];
	byte crcspc;
} rec8 = { 8, 0, 1};
byte pad8687[3];
struct {
	byte type;
	word len;
	byte subtype;
	byte seg;
	word addr;
	byte crc;
} rec4 = { 4, 4, 0, 1};
#pragma pack(pop)

byte b9692;
byte helperId;
byte helperStr[] = {0, '@', 'P', ' ', ' ', ' ', ' ', ':'};
byte b969C;
byte b969D;
byte *w969E;
word wValAry[4];	// word array
byte *sValAry[4];	// matching string representation
byte b96B0[38];
byte b96D6;
word w96D7;
byte curExtId;
byte commentStr[41] = { 0, ';', ' '};
byte line[81];
byte opByteCnt;
byte opBytes[3];
byte dstRec;
byte srcbuf[640];
byte tx1buf[640];
byte objbuf[640];
char lstbuf[640];
word wA17D;
word wA17F;
byte bA187;
word wA18D;
byte bA18F;
byte bA190;
byte bA1AB;
word wA1BD;


byte b42A8[] = {
	2, 2, 3, 4, 3, 4, 2, 2, 
	3, 4, 2, 3, 2, 3, 3, 3, 
	3, 2, 2, 3, 4, 2, 3, 2, 
	3, 2, 2, 2, 2, 3, 2, 2, 
	2, 3, 2, 3, 2, 2, 3, 2, 
	2, 1, 2, 2, 3, 4};

byte b42D6[] = {
	   0,  2,    4,  7,  0xB, 0xE,0x12,0x14,
	0x16,0x19,0x1D,0x1F,0x22,0x24,0x27,0x2A,
	0x2D,0x30,0x32,0x34,0x37,0x3B,0x3D,0x40,
	0x42,0x45,0x47,0x49,0x4B,0x4D,0x50,0x52,
	0x54,0x56,0x59,0x5B,0x5E,0x60,0x62,0x65,
	0x67,0x69,0x6A,0x6C,0x6E,0x71};

byte b4304[] = {
	0x24,0x24,0x24,0x24,0x13,0x13,0x18,0x18,
	0x18,0x18,0x16,0x2C,0x15,0x1F,0x1F,0x20,
	0x20,0x19,0x19,0x19,0x19,   8,   8,   9,
	   9,   6,   7,0x25,0x25,0x25,0x25,0x25,
	 0xA, 0xA, 0xB, 0xB,0x14,0x14,0x14,0x14,
	0x14,0x39,0x1A,0x1A,0x1A,0x1A};

byte b4332[] = {
	   0,   0,   0,   0,0x26,0x30,0x30,0x26,
	0x30,0x20,0x30,0x12,0x12,0x12,   0,0x10,
	0x10,0x10,0x10,0x10,0x10,0x60,   0,0x26,
	0x20,0x20,   0,   0,   0,   0,   0,   0,
	0x10,0x80,0x80,0x80,0x90,0x90,0x40,0xA0,
	0xA0,0xA0,0x80,0xB0,0x90,0x80,0xB0,0x90,
	0x80,0xB0,0x90,0x80,0xB0,0x90,0x80,0xB0,
	0x90,0x20,0x30,0x30,0x30,0x10,0x10,0x70,
	0x70,0x30,0x30,0x30,0x30,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,   0,   0,   0,
	   0,   0,   0,   0,   0,   0,   0,   0,
	0x20,0x20,   0,   0,0x20,   0,   0,0x2C,
	0x40,   0,0x10,0x10,0x60,0x20,   0,   0,
	0xA0,0xA0,0xA0,0xA0,0x32,0x16,0x10,0x20,
	   0,0x10,0x10,0x10,0x10,0x10,0x60,   0,
	   0,   0,   0,   0,   0,   0,   0,   0,
	   0,   0,   0,   0,   0,   0,   0,0x70,
	0x60,0x60,0x70,0x50,0x70,0x60,0x60,0xE0,
	   0,   0,   0,   0,   0,   0,   0,   0,
	   0,   0,   0,   0,   0,   0,   0,   0,
	   0,   0,   0,   0,   0,   0,0x80,0x80,
	0x80,0x80,0x80,0x80,   0,   0,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80};

byte b4431[] = {
	0, 0, 1, 1, 2, 2, 3, 3,
	4, 4, 5, 5, 6, 6, 7, 7,
	8, 9, 0xA};
byte b4444[] = {
	   0,   2,   4,   6,   8, 0xA, 0xC,0x10,
	0x11, 0xE,0x12,   0,   2,   4,   6,   8,
	 0xA, 0xC,0x10,0x11, 0xE,0x12,   0,   2,
	   4,   6,   8, 0xA, 0xC,0x10,0x11, 0xE,
	0x12,   0,   2,   4,   6,   8, 0xA, 0xC,
	0x10,0x11, 0xE,0x12,   0,   2,   4,   6,
	   8, 0xA, 0xC,0x10,0x11, 0xE,0x12, 0xC,
	0x11,0x12, 0xC,0x11,0x12,   2,   6,   0,
	   4,   6,   8, 0xA, 0xC, 0xE,0x10,0x11,
	0x12, 0xD, 0xF,   7,   9, 0xB,   1,   5,
	   3};

byte b4495[][11] = {
	{0x90,0x91,0x94,0x95,0x98,0x99,0x9A,0x9C,0xA0,0xA1,0x9D},
	{0x6C,0x6D,0x70,0x71,0x74,0x75,0x76,0x78,0x7C,0x7D,0x79},
	{   0,   0,   0,   0,   0,   0,0x64,0x65,   0,   0,   0},
	{   0,   0,   0,   0,   0,   0,0x68,0x69,   0,   0,   0},
	{   0,0x58,   0,   0,0x59,0x5A,0x54,0x55,   0,   0,   0},
	{   0,0x60,   0,   0,0x61,0x62,0x5C,0x5D,   0,   0,   0},
	{   0,0x84,   0,   0,0x85,0x86,0x80,0x81,   0,   0,   0},
	{   0,0x8C,   0,   0,0x8D,0x8E,0x88,0x89,   0,   0,   0},
	{   0,   0,   0,   0,0x10,0x11,0x12,0x14,0x15,0x16,0x17},
	{   0,   1,   4,   5,   8,   9, 0xA, 0xC, 0xD, 0xE, 0xF},
	{   0,0x30,   0,0x31,   0,   0,   0,   0,   0,   0,   0},
	{   0,0x28,   0,0x29,   0,   0,   0,   0,   0,   0,   0},
	{0x18,0x19,0x1C,0x1D,0x20,0x21,0x22,0x24,0x25,0x26,0x27},
	{0x44,0x45,0x48,0x49,0x4C,0x4D,0x4E,0x50,0x51,0x52,0x53},
	{0xA8,0xA9,0xAC,0xAD,0xB0,0xB1,0xB2,0xB4,0xB5,0xB6,0xB7},
	{0x34,0x36,0x35,0x38,0x39,0x3A,   0,   0,   0,   0,   0},
	{0x3C,0x3D,0x3E,0x40,0x41,0x42,   0,   0,   0,   0,   0},
	{   0,0x2C,   0,   0,0x2D,0x2E,   0,   0,   0,   0,   0},
	{0xA4,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}};

byte b4566[] = {
	   8,0x1C, 0xC,   0, 0xB,0x11,   1, 0xA,
	 0xF,0x10, 0xD,   9,   2,   3,   4,   5,
	   6,   7,   0,0x1E,0x12,0xE};
byte b457C[] = {
	   3,   3,   3,   3,   3,   3, 0xC, 0xD,
	 0xE, 0xF,0x10,0x11,   0,   0,   0,   0,
	   0,   0,   0,   0,0x12,   7,   4,   4,
	   2, 0xA,0x15,   0,   0,   0,   0,   8,
	   9,   0,   0,   0, 0xB,   6,0x12,0x12,
	0x12,0x12,0x12,0x12,   5,   1,   1,   1,
	0x13,0x13,0x13,   1,   1,   1,0x13,0x13,
	0x13,0x14,   0,   0,   0,   0,   0,   0,
	   0,   0,   0,   0,   0,   0,   0,   0,
	   0,   0,   0,   1,   1, 0xC, 0xC, 0xC,
	 0xC, 0xC, 0xC, 0xC, 0xC, 0xD, 0xD, 0xD,
	 0xD, 0xD, 0xD, 0xD, 0xD,   0, 0xE, 0xE,
	 0xE, 0xE, 0xF, 0xF, 0xF, 0xF,   0, 0xF,
	 0xF, 0xF, 0xF, 0xE, 0xE, 0xE, 0xE,   0,
	0x10,0x10,0x10,0x10,0x10,   0,   0,   0,
	   0,   0,   0,   0,   0,   0,   0,   0,
	   1,   1,   0,   0,   0,0};
byte b4602[] = {
	0xA7,0xA7,0xA7,0xA7,0xA7,0xA7,0xA7,0xA7,
	0xA7,0xA7,0xA7,0xA8,0xA8,0xA8,0xA8,0xA8,
	0xA8,0xA8,0xA8,0xA8,0xA8,0xA8,0xA9,0xA9,
	0xA9,0xA9,0xA9,0xA9,0xA9,0xA9,0xA9,0xA9,
	0xA9,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,
	0xAA,0xAA,0xAA,0xAA,0xAB,0xAB,0xAB,0xAB,
	0xAB,0xAB,0xAB,0xAB,0xAB,0xAB,0xAB,0xA6,
	0xA6,0xA6,0xA6,0xA6,0xA6,0xA6,0xA6,0xA6,
	0xA6,0xA6,0xA6,0xA6,0xA6,0xA6,0xA6,0xA6,
	0xA6,0xA6,0xA6,0xA6,0xA6,0xA6,0xA6,0xA6,
	0xA6};
char *opcodes = 
	"\x3SUB" "\x3SBB" "\x3RLC" "\x3RRC"
	"\x3RAL" "\x3RAR" "\x3SHL" "\x3SHR"
	"\x3""ADD" "\x3""ADC" "\x3MUL" "\x3""DIV"
	"\x3""ANA" "\x3ORA" "\x3XRA" "\x3NEG"
	"\x3NOT" "\x3M10" "\x3""CPI" "\x3""CMP"
	"\x3SUI" "\x3SBI" "\x3""ADI" "\x3""ACI"
	"\x3""ANI" "\x3ORI" "\x3XRI" "\x3INR"
	"\x3INX" "\x3""DCR" "\x3""DCX" "\x5""ADD\tA"
	"\x3PSW" "\x1" "A" "\x1" "B" "\x1" "C"
	"\x1""D" "\x1""E" "\x1H" "\x1L"
	"\x1M" "\x2""AH" "\x2""DH" "\x2""DA"
	"\x2""DB" "\x3""DBP" "\x3""DHP" "\x3PDB"
	"\x3""APH" "\x3""DPH" "\x3PDA" "\x3PDH"
	"\x2PB" "\x2PD" "\x2PH" "\x3""BPH"
	"\x3""BBA";

byte regNo[] = { 7, 0, 2, 4, 7, 1, 3, 5, 6};
					// A B D H A C E L M
byte regIdx[] = { 0x86,0x88,0x8C,0x90,0x86,0x8A,0x8E,0x92,0x94};
byte stkRegNo[] = { 3, 0, 1, 2 };
byte stkRegIdx[] = { 0x82, 0x88, 0x8C, 0x90 };	// psw b d h
byte b473D[] = {
	0x90, 0x98, 7, 0xF, 0x17, 0x1F, 0, 0, 
	0x80, 0x88, 0, 0, 0xA0, 0xB0, 0xA8, 0, 
	0, 0, 0, 0xFE, 0xB8, 0xD6, 0xDE, 0xC6, 
	0xCE, 0xE6, 0xF6, 0xEE, 4, 3, 5, 0xB, 
	0x87};
byte b475E[] = {
	9, 0x1D, 0xC, 1, 0, 0, 1, 0, 
	0, 0, 0xD, 9, 2, 3, 4, 5, 
	0x20, 0, 1, 0x1F, 0, 0xE};
byte b4774[] = {
	0x17, 0, 0x19, 0x13, 0, 0, 0x16, 0, 0, 0, 0x1A, 0x18, 3, 
	2, 5, 4, 0, 0, 0x15, 0, 0, 0x1B};
byte b478A[] = {
	8, 0, 0xC, 0x14, 0, 0, 1, 0, 
	0, 0, 0xD, 9, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0xE};
byte b47A0[] = {
	0,4,8,0xC,0x10,0x14,0x18,0x1C,
	0x20,0x24,0x28,0x2C,0x30,0x34,0x38,0x3C,
	0x40,0x44,0xCF,0x48,0x4C,0x50,0x54,0x58,
	0x5C,0x60,0x64,0x68,0x6C,0x70,0x74,0x78,
	0x7C};
word w47C1[] = {
	0,0,0,0,0x1000,0x1001,0x2002,0x1004,
	0x1005,0x1006,0x1007,0x2008,0x200A,0x100C,0x100D,0x100E,
	0x300F,0x2012,0x1014,0x3015,0x1018,0x1019,0x101A,0x101B,
	0x101C,0x101D,0x101E,0x101F,0x1022,0x2022,0x3022,0x4022,
	0x2020,0x1026,0x2026,0x3026,0x1029,0x2029,0x120F,0x102C,
	0x202C,0x302C,0x102F,0x1030,0x1031,0x2032,0x2034,0x2036,
	0x3038,0x303B,0x303E,0x3041,0x3044,0x3047,0x304A,0x304D,
	0x3050,0x1053,0x2054,0x1056,0x1057,0x3058,0x305B,0x105E,
	0x105E,0x405F,0x7063,0x906A,0xD073,0x1083,0x2083,0x3083,
	0x4083,0x5083,0x4080,0x3081,0x2082,0x4088,0x308C,0x208F,
	0x2091,0x3093,0x4096,0x509A,0x509F,0x40A4,0x30A8,0x20AB,
	0x20AE,0x10AD,0x10B0,0x20B1,0x20B3,0x20B5,0x10B7,0x80B8,
	0x10C0,0x10FC,0x10C1,0x10C2,0x20C3,0x10C5,0x80C6,0xA0CE,
	0x102C,0x202C,0x102C,0x202C,0x220D,0x1210,0x1211,0x2212,
	0x1022,0x1214,0x1215,0x1216,0x1217,0x1218,0x2219,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0x10D8,
	0,0,0,0x10D9,0x10DA,0x10DA,0x10DB,0x10DC,
	0x30DD,0x30E0,0x30E3,0x10E6,0x10E7,0x10E8,0x20E9,0,
	0,0x10EB,0,0,0,0,0,0,
	0,0,0,0,0,0,0x10EC,0x20ED,
	0x40EF,0x40F3,0x20F7,0x30F9};
word w4919[] = {
	0x134,0x137,0x13F,0x142,0x14A,0x14D,0x151,0x15A,
	0x15C,0x15F,0x161,0x14A,0x14D,0x151,0x15A,0x15C,
	0x15F,0x161,0x134,0x137,0x13F,0x142,0x14A,0x14D,
	0x151,0x15A,0x15C,0x15F,0x161,0x11A,0x11D,0xFD,
	0x101,0x103,0x10B,0x10E,0x1F6,0x1F9,0x1FB,0x1E9,
	0x1EC,0x1EE,0x1DB,0x1DE,0x1E0,0x1CE,0x1D1,0x1D3,
	0x134,0x137,0x13F,0x142,0x14A,0x14D,0x151,0x15A,
	0x15C,0x15F,0x161,0x183,0x185,0x1AE,0x1B2,0x1B4,
	0x183,0x185,0x1BE,0x1C2,0x1C4,0x183,0x185,0x183,
	0x185,0x134,0x137,0x13F,0x142,0x14A,0x14D,0x151,
	0x177,0x17A,0x16A,0x16D,0x183,0x185,0x192,0x196,
	0x198,0x18A,0x18C,0x19D,0x1A1,0x1A3,0x134,0x137,
	0x13F,0x142,0x14A,0x14D,0x151,0x177,0x17A,0x16A,
	0x16D,0x205,0x134,0x137,0x13F,0x142,0x14A,0x14D,
	0x151,0x15A,0x15C,0x15F,0x161};
byte b4A03[] = {
	0xB, 8, 0xB, 8, 0x10, 0xD, 9, 0x10, 
	0xE, 0xB, 9, 0x10, 0xD, 9, 0x10, 0xE, 
	0xB, 9, 0xB, 8, 0xB, 8, 0x10, 0xD, 
	9, 0x10, 0xE, 0xB, 9, 0x1A, 0x17, 0xE, 
	0xA, 8, 0xF, 0xC, 0xF, 0xC, 0xA, 0xD, 
	0xA, 8, 0xE, 0xB, 9, 0xD, 0xA, 8, 
	0xB, 8, 0xB, 8, 0x10, 0xD, 9, 0x10, 
	0xE, 0xB, 9, 7, 5, 0x10, 0xC, 0xA, 
	7, 5, 0x10, 0xC, 0xA, 7, 5, 7, 
	5, 0xB, 8, 0xB, 8, 0x10, 0xD, 9, 
	0xC, 9, 0xD, 0xA, 7, 5, 0xB, 7, 
	5, 8, 6, 0x11, 0xD, 0xB, 0xB, 8, 
	0xB, 8, 0x10, 0xD, 9, 0xC, 9, 0xD, 
	0xA, 8, 0xB, 8, 0xB, 8, 0x10, 0xD, 
	9, 0x10, 0xE, 0xB, 9};

byte *b4A78 = (byte *)
/* 0000 */ "\x8D" "\tADC\tL\x80"
/* 0008 */ "\xC6" "\tADI\t\x84\xFF\x80"
/* 0011 */ "\xE6" "\tANI\t\x84\x01\x80"
/* 001A */ "\xE6" "\tANI\t\x84\x03\x80"
/* 0023 */ "\xE6" "\tANI\t\x84\x07\x80"
/* 002C */ "\xE6" "\tANI\t\x84\x80"
/* 0034 */ "\x80" "\xE6\tANI\t\x84\xC0\x80"
/* 003E */ "\xE6" "\tANI\t\x84\xE0\x80"
/* 0047 */ "\xE6" "\tANI\t\x84\xF8\x80"
/* 0050 */ "\xE6" "\tANI\t\x84\xFC\x80"
/* 0059 */ "\xE6" "\tANI\t\x84\xFE\x80"
/* 0062 */ "\xE6" "\tANI\t\x84\xF0\x80"
/* 006B */ "\xCD" "\tCALL\t\x82\x80"
/* 0074 */ "\xCD" "\tCALL\t\x83 \x80"
/* 007E */ "\xCD" "\tCALL\t\x93     \x80"
/* 008C */ "\x2F" "\tCMA\x80"
/* 0092 */ "\x27" "\tDAA\x80"
/* 0098 */ "\x09" "\tDAD\tB\x80"
/* 00A0 */ "\x29" "\tDAD\tH\x80"
/* 00A8 */ "\x09" "\tDAD\t\xC0\x80"
/* 00B0 */ "\x39" "\tDAD\tSP\x80"
/* 00B9 */ "\x3D" "\tDCR\tA\x80"
/* 00C1 */ "\x0D" "\tDCR\tC\x80"
/* 00C9 */ "\x1B" "\tDCX\tD\x80"
/* 00D1 */ "\x2B" "\tDCX\tH\x80"
/* 00D9 */ "\xF3" "\tDI\x80"
/* 00DE */ "\x00" "\tDW\t\x82\x80"
/* 00E5 */ "\xFB" "\tEI\x80"
/* 00EA */ "\x00" "\x89" "AH:\x80"
/* 00F0 */ "\x00" "\x89" "APH:\x80"
/* 00F7 */ "\x00" "\x89" "B:\x80"
/* 00FC */ "\x00" "\x89" "BBA:\x80"
/* 0103 */ "\x00" "\x89" "BPH:\x80"
/* 010A */ "\x00" "\x89" "D:\x80"
/* 010F */ "\x00" "\x89" "DA:\x80"
/* 0115 */ "\x00" "\x89" "DB:\x80"
/* 011B */ "\x00" "\x89" "DBP:\x80"
/* 0122 */ "\x00" "\x89" "DH:\x80"
/* 0128 */ "\x00" "\x89" "DHP:\x80"
/* 012F */ "\x00" "\x89" "DPH:\x80"
/* 0136 */ "\x00" "\x89" "H:\x80"
/* 013B */ "\x00" "\x89" "PB:\x80"
/* 0141 */ "\x00" "\x89" "PD:\x80"
/* 0147 */ "\x00" "\x89" "PDA:\x80"
/* 014E */ "\x00" "\x89" "PDB:\x80"
/* 0155 */ "\x00" "\x89" "PDH:\x80"
/* 015C */ "\x00" "\x89" "PH:\x80"
/* 0162 */ "\x76" "\tHLT\x80"
/* 0168 */ "\xDB" "\tIN\t\x86\x80"
/* 016F */ "\x3C" "\tINR\tA\x80"
/* 0177 */ "\x13" "\tINX\tD\x80"
/* 017F */ "\x23" "\tINX\tH\x80"
/* 0187 */ "\xC2" "\tJ\x88\t\x82\x80"
/* 018E */ "\xFA" "\tJM\t\x87\x04\x80"
/* 0196 */ "\xC3" "\tJMP\t\x82\x80"
/* 019E */ "\xD2" "\tJNC\t\x82\x80"
/* 01A6 */ "\xD2" "\tJNC\t\x87\x04\x80"
/* 01AF */ "\xD2" "\tJNC\t\x87\x05\x80"
/* 01B8 */ "\xC2" "\tJNZ\t\x87\xFE\x80"
/* 01C1 */ "\xC2" "\tJNZ\t\x87\xEC\x80"
/* 01CA */ "\xC2" "\tJNZ\t\x87\xFD\x80"
/* 01D3 */ "\xC2" "\tJNZ\t\x87\xF9\x80"
/* 01DC */ "\xC2" "\tJNZ\t\x87\xF8\x80"
/* 01E5 */ "\xC2" "\tJNZ\t\x87\xF7\x80"
/* 01EE */ "\xEA" "\tJPE\t\x87\x04\x80"
/* 01F7 */ "\xCA" "\tJZ\t\x87\x04\x80"
/* 01FF */ "\x3A" "\tLDA\t\x82\x80"
/* 0207 */ "\x0A" "\tLDAX\t\xC0\x80"
/* 0210 */ "\x2A" "\tLHLD\t\x82\x80"
/* 0219 */ "\x21" "\tLXI\tH,\x82\t\x8A\x80"
/* 0225 */ "\x21" "\tLXI\tH,\x87\x0D\x80"
/* 0230 */ "\x21" "\tLXI\tH,\x87\x11\x80"
/* 023B */ "\x21" "\tLXI\tH,\x87\x08\x80"
/* 0246 */ "\x21" "\tLXI\tH,\x87\t\x80"
/* 0251 */ "\x21" "\tLXI\tH,\x94\x00\x80"
/* 025C */ "\x01" "\tLXI\t\xC0,\x92\x80"
/* 0266 */ "\x31" "\tLXI\tSP,\x85\x80"
/* 0271 */ "\x7A" "\tMOV\tA,D\x80"
/* 027B */ "\x7B" "\tMOV\tA,E\x80"
/* 0285 */ "\x7C" "\tMOV\tA,H\x80"
/* 028F */ "\x7D" "\tMOV\tA,L\x80"
/* 0299 */ "\x78" "\tMOV\tA,\xE0\x80"
/* 02A3 */ "\x78" "\tMOV\tA,\xE2\x80"
/* 02AD */ "\x7E" "\tMOV\tA,M\x80"
/* 02B7 */ "\x44" "\tMOV\tB,H\x80"
/* 02C1 */ "\x4F" "\tMOV\tC,A\x80"
/* 02CB */ "\x4D" "\tMOV\tC,L\x80"
/* 02D5 */ "\x57" "\tMOV\tD,A\x80"
/* 02DF */ "\x50" "\tMOV\tD,B\x80"
/* 02E9 */ "\x56" "\tMOV\tD,M\x80"
/* 02F3 */ "\x5F" "\tMOV\tE,A\x80"
/* 02FD */ "\x59" "\tMOV\tE,C\x80"
/* 0307 */ "\x5E" "\tMOV\tE,M\x80"
/* 0311 */ "\x67" "\tMOV\tH,A\x80"
/* 031B */ "\x66" "\tMOV\tH,M\x80"
/* 0325 */ "\x6F" "\tMOV\tL,A\x80"
/* 032F */ "\x46" "\tMOV\t\xD0,M\x80"
/* 0339 */ "\x40" "\tMOV\t\xD2,\xE0\x80"
/* 0343 */ "\x46" "\tMOV\t\xD2,M\x80"
/* 034D */ "\x40" "\tMOV\t\xD0,\xE1\x80"
/* 0357 */ "\x40" "\tMOV\t\xD2,\xE3\x80"
/* 0361 */ "\x70" "\tMOV\tM,\xE0\x80"
/* 036B */ "\x70" "\tMOV\tM,\xE2\x80"
/* 0375 */ "\x3E" "\tMVI\tA,\x84\x10\x80"
/* 0380 */ "\x3E" "\tMVI\tA,\x84\xFF\x80"
/* 038B */ "\x3E" "\tMVI\tA,\x84\x00\x80"
/* 0396 */ "\x06" "\tMVI\tB,\x84\x00\x80"
/* 03A1 */ "\x16" "\tMVI\tD,\x84\x00\x80"
/* 03AC */ "\x26" "\tMVI\tH,\x84\x00\x80"
/* 03B7 */ "\x06" "\tMVI\t\xD2,\x96\x80"
/* 03C1 */ "\x06" "\tMVI\t\xD0,\x84\x00\x80"
/* 03CC */ "\x36" "\tMVI\tM,\x84\x00\x80"
/* 03D7 */ "\x00" "\t\x9B\x80"
/* 03DB */ "\x00" "\t\x9B\t\xE5\x80"
/* 03E1 */ "\x00" "\t\x9B\t\xE7\x80"
/* 03E7 */ "\x00" "\t\x9B\t\xC0\x80"
/* 03ED */ "\x00" "\t\x9B\t\xEC\x80"
/* 03F3 */ "\x00" "\t\xAB\x80"
/* 03F7 */ "\x00" "\t\xAB\t\x86\x80"
/* 03FD */ "\x00" "\t\xBB\t\xEC\x80"
/* 0403 */ "\x00" "\t\x8B\t\xE9\x80"
/* 0409 */ "\x00" "\t\x8B\t\xEB\x80"
/* 040F */ "\x00" "\t\x8B\t\xEC\x80"
/* 0415 */ "\x00" "\t\x8B\t\xDC\x80"
/* 041B */ "\xB7" "\tORA\tA\x80"
/* 0423 */ "\xB5" "\tORA\tL\x80"
/* 042B */ "\xD3" "\tOUT\t\x86\x80"
/* 0433 */ "\xE9" "\tPCHL\x80"
/* 043A */ "\x00" ";\x20PROC\t\x81\x80"
/* 0444 */ "\xC1" "\tPOP\tB\x80"
/* 044C */ "\xC1" "\tPOP\t\xC0\t\x8A\x80"
/* 0456 */ "\xF1" "\tPOP\tPSW\x80"
/* 0460 */ "\xE5" "\tPUSH\tH\x80"
/* 0469 */ "\xC5" "\tPUSH\t\xC0\t\x8A\x80"
/* 0474 */ "\xF5" "\tPUSH\tPSW\x80"
/* 047F */ "\x17" "\tRAL\x80"
/* 0485 */ "\x1F" "\tRAR\x80"
/* 048B */ "\xC9" "\tRET\x80"
/* 0491 */ "\x9F" "\tSBB\tA\x80"
/* 0499 */ "\x98" "\tSBB\tB\x80"
/* 04A1 */ "\x9C" "\tSBB\tH\x80"
/* 04A9 */ "\x9E" "\tSBB\tM\x80"
/* 04B1 */ "\x22" "\tSHLD\t\x82\x80"
/* 04BA */ "\xF9" "\tSPHL\x80"
/* 04C1 */ "\x32" "\tSTA\t\x82\x80"
/* 04C9 */ "\x97" "\tSUB\tA\x80"
/* 04D1 */ "\x91" "\tSUB\tC\x80"
/* 04D9 */ "\x95" "\tSUB\tL\x80"
/* 04E1 */ "\x96" "\tSUB\tM\x80"
/* 04E9 */ "\xD6" "\tSUI\t\x84\x01\x80"
/* 04F2 */ "\xEB" "\tXCHG\x80"
/* 04F9 */ "\xE3" "\tXTHL\t\t\x8A\x80"
/* 0503 */ "\x03" "\tINX\t\xC0\x80"
/* 050B */ "\x0B" "\tDCX\t\xC0\x80"
/* 0513 */ "\x69" "\tMOV\tL,C\x80"
/* 051D */ "\x60" "\tMOV\tH,B\x80"
/* 0527 */ "\x4E" "\tMOV\tC,M\x80"
/* 0531 */ "\x46" "\tMOV\tB,M\x80"
/* 053B */ "\x1A" "\tLDAX\tD\x80"
/* 0544 */ "\x0A" "\tLDAX\tB\x80"
/* 054D */ "\x12" "\tSTAX\tD\x80"
/* 0556 */ "\xC2" "\tJNZ\t\x87\xFB\x80"
/* 055F */ "\x06" "\tMVI\tB,\x84\x0C\x80"
/* 056A */ "\x48" "\tMOV\tC,B\x80"
/* 0574 */ "\xC2" "\tJNZ\t\x87\xFF\x80"
/* 057D */ "\xC2" "\tJNZ\t\x87\xFA\x80"
/* 0586 */ "\x21" "\tLXI\tH,\x92\x80"
/* 0590 */ "\x03" "\tINX\tB\x80"
/* 0598 */ "\x78" "\tMOV\tA,\xE3\x80"
/* 05A2 */ "\x36" "\tMVI\tM,\x86\x80"
/* 05AC */ "\x00" "\t\x8B\t\xD2\x80"
/* 05B2 */ "\x00" "\t\xBB\t\xE2\x80"
/* 05B8 */ "\x2D" "\tDCR\tL\x80"
/* 05C0 */ "\x00" "\x89\x80"
/* 05C3 */ "\x40" "\tMOV\t\xD0,\xE2\x80"
/* 05CD */ "\x33" "\tINX\tSP\x80"
/* 05D6 */ "\x3B" "\tDCX\tSP\x80"
/* 05DF */ "\xDA" "\tJC\t\x82\x80"
/* 05E6 */ "\x2C" "\tINR\tL\x80"
/* 05EE */ "\xC3" "\tJMP\t\x87\x07\x80";

word w506F[] = {
/*  A4-1 */ 0x44c,
/*  A5-1 */ 0x4f9,
/*  A6-2 */ 0x219, 0xb0,
/*  A7-1 */ 0x25c,
/*  A8-1 */ 0x1ff,
/*  A9-1 */ 0x207,
/*  A10-1 */ 0x210,
/*  A11-2 */ 0x34d, 0x357,
/*  A12-2 */ 0x357, 0x3c1,
/*  A13-1 */ 0x357,
/*  A14-1 */ 0x4f2,
/*  A15-1 */ 0x3c1,
/*  A16-3 */ 0x343, 0x17f, 0x32f,
/*  A17-2 */ 0x343, 0x3c1,
/*  A18-1 */ 0x343,
/*  A19-3 */ 0x361, 0xd1, 0x36b,
/*  A20-1 */ 0x36b,
/*  A21-1 */ 0xde,
/*  A22-1 */ 0x4ba,
/*  A23-1 */ 0x469,
/*  A24-1 */ 0x503,
/*  A25-1 */ 0x50b,
/*  A26-1 */ 0xd1,
/*  A27-1 */ 0x48b,
/*  A32-2 */ 0x2a3, 0x48b,
/*  A28-1 A29-2 A30-3 A31-4 A112-1 */ 0xa0, 0xa0, 0xa0, 0xa0,
/*  A33-1 A34-2 A35-3 */ 0x415, 0x415, 0x415,
/*  A36-1 A37-2 */ 0x5ac, 0x5ac, 0x5ac,
/*  A39-1 A40-2 A41-3 A104-1 A105-2 A106-1 A107-2 */ 0x3e7, 0x3e7, 0x3e7,
/*  A42-1 */ 0x3fd,
/*  A43-1 */ 0x3f7,
/*  A44-1 */ 0x5b2,
/*  A45-2 */ 0x3fd, 0x491,
/*  A46-2 */ 0x3f7, 0x491,
/*  A47-2 */ 0x5b2, 0x491,
/*  A48-3 */ 0x3fd, 0x4e9, 0x491,
/*  A49-3 */ 0x3f7, 0x4e9, 0x491,
/*  A50-3 */ 0x5b2, 0x4e9, 0x491,
/*  A51-3 */ 0x3fd, 0x8, 0x491,
/*  A52-3 */ 0x3f7, 0x8, 0x491,
/*  A53-3 */ 0x5b2, 0x8, 0x491,
/*  A54-3 */ 0x3fd, 0x491, 0x8c,
/*  A55-3 */ 0x3f7, 0x491, 0x8c,
/*  A56-3 */ 0x5b2, 0x491, 0x8c,
/*  A57-1 */ 0xa8,
/*  A58-2 */ 0x210, 0x3ac,
/*  A59-1 */ 0x4b1,
/*  A60-1 */ 0x4c1,
/*  A61-3 */ 0x36b, 0x17f, 0x3cc,
/*  A62-3 */ 0x36b, 0x17f, 0x361,
/*  A63-1 A64-1 */ 0x6b,
/*  A65-4 */ 0x23b, 0x460, 0x210, 0x433,
/*  A66-7 */ 0x246, 0x4f9, 0x2ad, 0x17f, 0x31b, 0x325, 0x433,
/*  A67-9 */ 0x225, 0x460, 0x219, 0xb0, 0x2ad, 0x17f, 0x31b, 0x325, 0x433,
/*  A68-13 */ 0x230, 0x460, 0x219, 0xb0, 0x2ad, 0x17f, 0x31b, 0x325, 0x2ad, 0x17f, 0x31b, 0x325, 0x433,
/*  A74-4 */ 0x3f3,
/*  A75-3 */ 0x3f3,
/*  A76-2 */ 0x3f3,
/*  A69-1 A70-2 A71-3 A72-4 A73-5 */ 0x3d7, 0x3d7, 0x3d7, 0x3d7, 0x3d7,
/*  A77-4 */ 0x23, 0x485, 0x485, 0x485,
/*  A78-3 */ 0x1a, 0x485, 0x485,
/*  A79-2 */ 0x11, 0x485,
/*  A80-2 */ 0x41b, 0x485,
/*  A81-3 */ 0x59, 0x485, 0x485,
/*  A82-4 */ 0x50, 0x485, 0x485, 0x485,
/*  A83-5 */ 0x47, 0x485, 0x485, 0x485, 0x485,
/*  A84-5 */ 0x62, 0x47f, 0x47f, 0x47f, 0x47f,
/*  A85-4 */ 0x3e, 0x47f, 0x47f, 0x47f,
/*  A86-3 */ 0x35, 0x47f, 0x47f,
/*  A87-2 */ 0x2c, 0x47f,
/*  A89-1 */ 0xa8,
/*  A88-2 */ 0xa8, 0xa8,
/*  A90-1 */ 0x8c,
/*  A91-2 */ 0x8c, 0x16f,
/*  A92-2 */ 0x503, 0x207,
/*  A93-2 */ 0x17f, 0x2ad,
/*  A94-1 */ 0x92,
/*  A95-8 */ 0x586, 0xa8, 0xa8, 0x307, 0x17f, 0x2e9, 0x4f2, 0x433,
/*  A96-1 */ 0x42b,
/*  A98-1 */ 0x2a3,
/*  A99-1 */ 0x299,
/*  A100-2 */ 0x485, 0x19e,
/*  A101-1 */ 0xa8,
/*  A102-8 */ 0x5e6, 0x5ee, 0x544, 0x54d, 0x590, 0x177, 0x5b8, 0x556,
/*  A103-10 */ 0x17f, 0x5ee, 0x544, 0x54d, 0x590, 0x177, 0xd1, 0x285, 0x423, 0x1d3,
/*  A135-1 */ 0x43a,
/*  A139-1 */ 0x168,
/*  A140-1 A141-1 */ 0x196,
/*  A142-1 */ 0x19e,
/*  A143-1 */ 0x187,
/*  A144-3 */ 0x380, 0x18e, 0x4c9,
/*  A145-3 */ 0x380, 0x1f7, 0x4c9,
/*  A146-3 */ 0x380, 0x1ee, 0x4c9,
/*  A147-1 */ 0x491,
/*  A148-1 */ 0xd9,
/*  A149-1 */ 0xe5,
/*  A150-2 */ 0xe5, 0x162,
/*  A153-1 */ 0x266,
/*  A166-1 */ 0x74,
/*  A167-2 */ 0x74, 0x423,
/*  A168-4 */ 0x74, 0x423, 0x4e9, 0x491,
/*  A169-4 */ 0x74, 0x423, 0x8, 0x491,
/*  A170-2 */ 0x74, 0x491,
/*  A171-3 */ 0x74, 0x491, 0x8c,
/*  A97-1 */ 0x7e,
/*  B31-14 */ 0x15c, 0x307, 0x17f, 0x2e9,
/*  B32-10 */ 0x10a, 0x4f2,
/*  B33-8 */ 0x136, 0xa0, 0x460, 0xa0, 0xa0, 0x444, 0x98, 0x48b,
/*  B34-15 */ 0x122, 0x2b7, 0x2cb,
/*  B35-12 */ 0x115, 0x251, 0x375, 0xa0, 0x4f2, 0xa0, 0x4f2, 0x1a6, 0x98, 0xb9, 0x1e5, 0x48b,
/*  B29-26 */ 0x122, 0x2b7, 0x2cb,
/*  B30-23 */ 0x115, 0x251, 0x375, 0x474, 0xa0, 0x4f2, 0x4c9, 0xa0, 0x4f2, 0x0, 0x4d1, 0x325,
	      0x285, 0x499, 0x311, 0x177, 0x1af, 0x98, 0xc9, 0x456, 0xb9, 0x1c1, 0x48b,
/*  B0 B18 B48 B73 B94 B106 -11 */ 0xea, 0x2f3, 0x3a1,
/*  B1 B19 B49 B74 B95 B107 -8 */ 0x122, 0x27b, 0x409, 0x325, 0x271, 0x3e1, 0x311, 0x48b,
/*  B2 B20 B50 B75 B96 B108 -11 */ 0x10f, 0x2c1, 0x396,
/*  B3 B21 B51 B76 B97 B109 -8 */ 0x115, 0x27b, 0x403, 0x325, 0x271, 0x3db, 0x311, 0x48b,
/*  B4 B11 B22 B52 B77 B98 B110 -16 */ 0x11b, 0x513, 0x51d,
/*  B5 B12 B23 B53 B78 B99 B111 -13 */ 0x128, 0x527, 0x17f, 0x531,
/*  B6 B13 B24 B54 B79 B100 B112 -9 */ 0x14e, 0x53b, 0x403, 0x325, 0x177, 0x53b, 0x3db, 0x311, 0x48b,
/*  B7 B14 B25 B55 B113 -16 */ 0x147, 0x4f2,
/*  B8 B15 B26 B56 B114 -14 */ 0xf0, 0x2f3, 0x3a1,
/*  B9 B16 B27 B57 B115 -11 */ 0x12f, 0x4f2,
/*  B10 B17 B28 B58 B116 -9 */ 0x155, 0x53b, 0x409, 0x325, 0x177, 0x53b, 0x3e1, 0x311, 0x48b,
/*  B82 B103 -13 */ 0xf0, 0x2f3, 0x3a1,
/*  B83 B104 -10 */ 0x12f, 0x27b, 0x40f, 0x2f3, 0x271, 0x17f, 0x3ed, 0x2d5, 0x4f2, 0x48b,
/*  B80 B101 -12 */ 0x147, 0x325, 0x3ac,
/*  B81 B102 -9*/ 0x155, 0x53b, 0x409, 0x325, 0x177, 0x53b, 0x3e1, 0x311, 0x48b,
/*  B59 B64 B69 B71 B84 -7 */ 0x103, 0x2ad,
/*  B60 B65 B70 B72 B85 -5 */ 0xfc, 0x3d7, 0xc1, 0x1b8, 0x48b,
/*  B89-8 */ 0x103, 0x2ad,
/*  B90-6 */ 0xfc, 0x41b, 0x485, 0xc1, 0x1ca, 0x48b,
/*  B86-11 */ 0x15c, 0x307, 0x17f, 0x2e9,
/*  B87-7 */ 0x10a, 0x4f2,
/*  B88-5 */ 0x136, 0xa0, 0xc1, 0x1b8, 0x48b,
/*  B91-17 */ 0x15c, 0x307, 0x17f, 0x2e9,
/*  B92-13 */ 0x10a, 0x4f2,
/*  B93-11 */ 0x136, 0x285, 0x41b, 0x485, 0x311, 0x28f, 0x485, 0x325, 0xc1, 0x1dc, 0x48b,
/*  B61-16 */ 0x15c, 0x307, 0x17f, 0x2e9,
/*  B62-12 */ 0x10a, 0x4f2,
/*  B63-10 */ 0x136, 0x28f, 0x47f, 0x325, 0x285, 0x47f, 0x311, 0xc1, 0x1d3, 0x48b,
/*  B66-16 */ 0x15c, 0x307, 0x17f, 0x2e9,
/*  B67-12 */ 0x10a, 0x4f2,
/*  B68-10 */ 0x136, 0x285, 0x485, 0x311, 0x28f, 0x485, 0x325, 0xc1, 0x1d3, 0x48b,
/*  B45-13 */ 0xf7, 0x2fd, 0x2df,
/*  B46-10 */ 0x10a, 0x4f2,
/*  B47-8 */ 0x136, 0x28f, 0x8c, 0x325, 0x285, 0x8c, 0x311, 0x48b,
/*  B42-14 */ 0x13b, 0x513, 0x51d,
/*  B43-11 */ 0x15c, 0x4f2,
/*  B44-9 */ 0x141, 0x53b, 0x8c, 0x325, 0x177, 0x53b, 0x8c, 0x311, 0x48b,
/*  B39-13 */ 0xf7, 0x2fd, 0x2df,
/*  B40-10 */ 0x10a, 0x4f2,
/*  B41-8 */ 0x136, 0x4c9, 0x4d9, 0x325, 0x38b, 0x4a1, 0x311, 0x48b,
/*  B36-15 */ 0x13b, 0x2fd, 0x2df,
/*  B37-12 */ 0x141, 0x4f2,
/*  B38-10 */ 0x15c, 0x4c9, 0x4e1, 0x2f3, 0x38b, 0x17f, 0x4a9, 0x2d5, 0x4f2, 0x48b,
/*  B105-8 */ 0x5c0, 0x55f, 0x56a, 0xc1, 0x574, 0xb9, 0x57d, 0x48b,
/*  A108-2 */ 0x598, 0x42b,
/*  A38-1 */ 0x5a2,
/*  A109-1 */ 0x3b7,
/*  A110-1 */ 0x339,
/*  A111-2 */ 0xa0, 0xa8,
/*  A113-1 */ 0x5c3,
/*  A114-1 */ 0x32f,
/*  A115-1 */ 0x361,
/*  A116-1 */ 0x5cd,
/*  A117-1 */ 0x5d6,
/*  A118-2 */ 0x485, 0x5df
};


void putnstrLst(byte *arg1w, word arg2w);
void newLineLeaderLst();
void newPageLst();
void put2cLst(word arg1w);
void putcLst(byte arg1b);
void lstLineNo();
void xnumLst(word arg1w, byte arg2b, byte arg3b);
void setSkipLst(byte arg1b);
void setStartAndTabW(byte arg1b, byte arg2b);
void sub_77BF(byte arg1b, byte arg2b, byte arg3b);
void newPageNextChLst();
void tabLst(byte arg1b);
void newLineLst();
void flushLstBuf();
void xputcLst(byte arg1b);
void lstModuleInfo();
void sub_6FEA(word *arg1w, word arg2w);
void sub_6E5C(byte *arg1w, byte arg2b);
void sub_6B9B();
void sub_6B0E();
void sub_69E1(word arg1w);
void sub_6982();
void rdLocLab();
void rdWVal();
void rdBVal();
void sub_685C(byte arg1b, byte arg2b, byte arg3b);
void sub_67AD(byte arg1b, byte arg2b);
void sub_6720();
void sub_66F1();
void sub_668B();
void sub_654F();
void sub_64CF();
void emitHelperLabel();
void addccCode();
void addPCRel();
void addByte();
void addStackOrigin();
void addSmallNum();
void addHelper();
void addWord();
void sub_6175();
void pstrcat2Line(byte *arg1w);
void sub_603C();
void sub_5FE7(word arg1w, byte arg2b);
byte getSourceCh();
void getSourceLine();
void sub_5E3E();
void sub_5E1B(byte arg1b);
void sub_5DB7();
void emitError();
void emitLabel();
void emitStatementNo();
void sub_5BD3();
void addWrdDisp(byte *arg1w, word arg2w);
void flushRecs();
void setNewAddr();
void sub_5746();
void emitFullError();
void emitNearError();
void emitSimpleError();
void emitSymLabel();
void emitLocalLabel();
void addrCheck(word arg1w);
void newStatementNo();
void emitSource();
void sub_54BA();
void sub_423C();
void sub_4208();
void sub_4162();
void sub_408B();
void sub_3FC8();
int overlay4();



int overlay4()
{
	try {
		sub_408B();

		while (bo812B)
			sub_54BA();
		sub_4162();
		flushRecs();
		sub_5BD3();
	} catch (...) {
	}

	sub_423C();
	if (IXREF)
		return 5;
	if (PRINT) {
		if (SYMBOLS || XREF) 
			return 5;
		else
			lstModuleInfo();
	}
	printExitMsg();
	return -1;
}


void sub_3FC8()
{
	if (PRINT) {
		newPageNextChLst();
 		xputstr2cLst("ISIS-II PL/M-80 ", 0);
		xputnstrLst(version, 4);
		xputstr2cLst(" COMPILATION OF MODULE ", 0);
		curInfo_p = off2Info(procInfo[1]);
		curSymbol_p = getInfoSymbol();
		if (curSymbol_p != 0) 
			xputnstrLst((char *)&curSymbol_p->name[1], curSymbol_p->name[0]);
		newLineLst();
		if (OBJECT) { 
			xputstr2cLst("OBJECT MODULE PLACED IN ", 0);
			xputstr2cLst(objFile.fullName, ' ');
		} else
			xputstr2cLst("NO OBJECT MODULE REQUESTED", 0);

		newLineLst();
		if (word_382A == 1) {
			xputstr2cLst("COMPILER INVOKED BY:  ", 0);
			cmdLine_p = startCmdLine_p;
			while (cmdLine_p != 0) {
				tabLst(0xE9);
				xputstr2cLst(cmdLine_p->text, '\r');
				cmdLine_p = cmdLine_p->link;
			}
			newLineLst();
		}
	}
}

void sub_408B()
{
	if (PRINT) { //	loc_409E
		lstBufPtr = lstbuf;
		lstBufSize = 639;
	}

	sub_3FC8();
	if (botMem < 0xA40A)
		sub_6E5C((byte *)"COMPILER ERROR: INSUFFICIENT MEMORY FOR FINAL ASSEMBLY", 54);
	stmtNo = 0;
	if (PRINT) { //	loc_40E3
		srcFileIdx = 0;
		initFile(&srcFile, "SOURCE", srcFileTable[srcFileIdx].filename);
		openFile(&srcFile, 1);
	}

	//assignFileBuffer(&tx1File, tx1buf, 640, 1);
	//assignFileBuffer(&objFile, objbuf, 640, 2);
	curInfo_p = off2Info(procInfo[1]);
	rec6_4.addr = baseAddr = getAddr();
	setSkipLst(3);
	sub_77BF(0xb, 0x2d, 0xf);
	if (fatalErrorCode > 0) {
		wa8125[1] = wa8125[2] = 0;
		wa8125[0] = fatalErrorCode;
		emitError();
		setSkipLst(2);
	}
	bo8102 = PRINT;
	bo8103 = 0;
	printOn = 0;
	linesRead = programErrCnt = 0;
}



void sub_4162()
{
	byte i, j;

	if (! byte_3C3B)
		return;
	for (i = 0; i <= 0x2d; i++) {
		helperId = b42D6[i];
		j = helperId + b42A8[i];
		while (j > helperId) {
			if (((word *)helpers_p)[helperId] != 0) { // 41F9
				baseAddr = ((word *)helpers_p)[helperId];
				b969D = b457C[b969C = b4304[i]];
				sub_5FE7(w4919[helperId], b4A03[helperId]);
				break;
			}
			helperId++;
		}
	}
}



void sub_4208()
{
	if (haveModule) { //	loc_4228
		rec4.subtype = 1;
		curInfo_p = off2Info(procInfo[1]);
		rec4.addr = getAddr();
	} else {
		rec4.subtype = 0;
		rec4.addr = 0;
	}
	writeRec((rec *)&rec4.type, 0);
}

static byte b3EBF[4] = { 0xE, 1, 0, 0xF1};

void sub_423C()
{
	linesRead = w812F;
	sub_4208();
	closeFile(&tx1File);
	if (OBJECT) { //	loc_426B
		ifwrite(&objFile, b3EBF, 4);
//		flushFile(&objFile);
		closeFile(&objFile);
	}

	if (PRINT) { //	loc_42A1
		tellFile(&srcFile, (loc_t *)&srcFileTable[srcFileIdx].block);
		sub_6FEA(&srcFileTable[srcFileIdx].block, w8137 - w8139);
		closeFile(&srcFile);
		flushLstBuf();
	}

	deleteFile(&tx1File);
}


void sub_54BA()
{
	ifread(&tx1File, &b812E, 1);
	if (b812E == 0x98) 
		emitSource();
	else if (b812E == 0x86)
		ifread(&tx1File, &stmtNo, 2);
	else if (b812E == 0x97)
		newStatementNo();
	else if (b812E == 0x88 || b812E == 0x89)
		emitLocalLabel();
	else if (b812E == 0x8A)
		emitSymLabel();
	else if (b812E == 0xA4)
		setNewAddr();
	else if (b812E == 0x9A)
		emitSimpleError();
	else if (b812E == 0x9B)
		emitNearError();
	else if (b812E >= 0x9D && b812E <= 0xA2)
		sub_5746();
	else if (b812E == 0x9C)
		bo812B = 0;
	else if (b812E == 0xA3)
		emitFullError();
	else
		sub_668B();
}

void emitSource()
{
	word p[3], s, t;
	ifread(&tx1File, &p, 6);
	if (p[1] > 0 || p[2] == 0)
		s = p[0];
	else {
		s = p[2];
		p[2] = 0;
	}
	for (t = p[0]; t <= s; t++) {
		sub_5BD3();
		w812F = t;
		lineNo = p[1];
		depth = p[2];
		getSourceLine();
	}
}



void newStatementNo()
{
	ifread(&tx1File, &stmtNo, 2);
	if (stmtNo == 0)
		return;
	if (DEBUG) { //	loc_565B
		if (rec8.len + 4 >= 1020)
			writeRec((rec *)&rec8.type, 1);
		recAddWord((rec *)&rec8.type, 1, baseAddr);
		recAddWord((rec *)&rec8.type, 1, stmtNo);
	}
	if (printOn)
		emitStatementNo();
}


void addrCheck(word arg1w)
{
	if (baseAddr != arg1w)
		sub_5E1B(0xD8);
}


void emitLocalLabel()
{
	ifread(&tx1File, &w96D7, 2);
	locLabStr[1] = '@';
	locLabStr[0] = num2Asc(w96D7, 0, 10, (char *)&locLabStr[2]) + 1;
	addrCheck(((word *)localLabels_p)[w96D7]);
	emitLabel();
}

void emitSymLabel()
{
	ifread(&tx1File, &curInfo_p, 2);
	curInfo_p = off2Info(curInfo_p);
	curSymbol_p = getInfoSymbol();
	locLabStr[0] = curSymbol_p->name[0];
	movemem(locLabStr[0], &curSymbol_p->name[1], &locLabStr[1]);
	addrCheck(getAddr());
	emitLabel();
}

void emitSimpleError()
{
	ifread(&tx1File, wa8125, 2);
	wa8125[1] = 0;
	wa8125[2] = stmtNo;
	emitError();
}


void emitNearError()
{
	ifread(&tx1File, wa8125, 4);
	wa8125[2] = stmtNo;
	emitError();
}

void emitFullError()
{
	ifread(&tx1File, wa8125, 6);
	emitError();
}

void sub_5746()
{
	byte fname[19];

	switch (b812E) {
	case 0x9d:	bo8103 = 0;
		break;
	case 0x9e:	bo8103 = 0xFF;
		break;
	case 0x9f:	printOn = PRINT;
		break;
	case 0xa0:	printOn = 0;
		break;
	case 0xa1:	if (bo8102)
			newPageNextChLst();
		break;
	case 0xa2: sub_5BD3();
		tellFile(&srcFile, (loc_t *)&srcFileTable[srcFileIdx].block);
		sub_6FEA(&srcFileTable[srcFileIdx].block, w8137 - w8139);
		srcFileIdx++;		// increments by 10 in PLM
		ifread(&tx1File, fname+13, 6);
		ifread(&tx1File, fname+6, 7);
		ifread(&tx1File, fname, 7);
		movemem(16, fname+1, &srcFileTable[srcFileIdx]);
		closeFile(&srcFile);
		initFile(&srcFile, "SOURCE", (char *)fname+1);
		openFile(&srcFile, 1);
		w8139 = w8137;
		break;
	}
}

void setNewAddr()
{
	ifread(&tx1File, &curInfo_p, 2);
	ifread(&tx1File, &baseAddr, 2);
	curInfo_p = off2Info(curInfo_p);
	baseAddr += getAddr();
	flushRecs();
}

unsigned char b5862[] =
	"\xAC" "INVALID LABEL: UNDEFINED\0"
	"\xC8" "LIMIT EXCEEDED: STATEMENT SIZE\0"
	"\xC9" "INVALID DO CASE BLOCK, AT LEAST ONE CASE REQUIRED\0"
	"\xCA" "LIMIT EXCEEDED: NUMBER OF ACTIVE CASES\0"
	"\xCB" "LIMIT EXCEEDED: NESTING OF TYPED PROCEDURE CALLS\0"
	"\xCC" "LIMIT EXCEEDED: NUMBER OF ACTIVE PROCEDURES AND DO CASE GROUPS\0"
	"\xCD" "ILLEGAL NESTING OF BLOCKS, ENDS NOT BALANCED\0"
	"\xCE" "LIMIT EXCEEDED: CODE SEGMENT SIZE\0"
	"\xD1" "ILLEGAL INITIALIZATION OF MORE SPACE THAN DECLARED\0"
	"\xD2" "ILLEGAL INITIALIZATION OF A BYTE TO A VALUE > 255\0"
	"\xD6" "COMPILER ERROR: OPERAND CANNOT BE TRANSFORMED\0"
	"\xD7" "COMPILER ERROR: EOF READ IN FINAL ASSEMBLY\0"
	"\xD8" "COMPILER ERROR: BAD LABEL ADDRESS\0"
	"\xD9" "ILLEGAL INITIALIZATION OF AN EXTERNAL VARIABLE\0"
	"\0";


void flushRecs()
{
	writeRec((rec *)&rec8.type, 1);
	writeRec((rec *)&rec6_4.type, 3);
	writeRec((rec *)&rec22.type, 1);
	writeRec((rec *)&rec24_1.type, 2);
	writeRec((rec *)&rec24_2.type, 2);
	writeRec((rec *)&rec24_3.type, 2);
	writeRec((rec *)&rec20.type, 1);
	rec6_4.addr = baseAddr;
}



void addWrdDisp(byte *arg1w, word arg2w)
{
	if (arg2w != 0) { //	locret_5BD2
		++*arg1w;
		if (arg2w > 0x8000) {
			arg1w[*arg1w] = '-';
			arg2w = -arg2w;
		} else
			arg1w[*arg1w] = '+';
		*arg1w += num2Asc(arg2w, 0, -16, (char *)(arg1w + *arg1w + 1));
	}
}


void sub_5BD3()
{
	if (!bo813B && bo8102) {
		setStartAndTabW(15, 4);
		if (lineNo != 0)
			lstLineNo();
		tabLst(0xF9);
		if (depth < 10) {
			xputcLst(' ');
			xputcLst(" 123456789"[depth]);
		} else
			xnumLst(depth, 2, 0xA);
		if (srcFileIdx != 0) { //	loc_5C5B
			tabLst(0xF5);
			xputnstrLst("=", 1);
			if (srcFileIdx != 1) 
				xnumLst(srcFileIdx - 1, 1, 10);
		}
		if (lstLine[0] > 0) {
			tabLst(0xF1);
			xputnstrLst((char *)&lstLine[1], lstLine[0]);
		}

		newLineLst();
		bo813C = 0xFF;
	}
	bo813B = 0xFF;
	bo8102 = !bo8103 && PRINT;
}



void emitStatementNo()
{
	sub_5BD3();
	tabLst(0xCE);
	xputnstrLst("; STATEMENT # ", 14);
	xnumLst(stmtNo, 0, 0xA);
	newLineLst();
}


void emitLabel()
{
	if (printOn) { //	locret_5CCF
		sub_5BD3();
		tabLst(0xE6);
		xputnstrLst((char *)&locLabStr[1], locLabStr[0]);
		xputnstrLst(":", 1);
		newLineLst();
	}
}


void emitError()
{
	++programErrCnt;
	if (PRINT) { //	locret_5DB6
		bo813B = bo813C;
		bo8102 = 0xFF;
		sub_5BD3();
		xputnstrLst("*** ERROR #", 11);
		xnumLst(wa8125[0], 0, 0xA);
		xputnstrLst(", ", 2);
		if (wa8125[2] != 0) { //	loc_5D2E
			xputnstrLst("STATEMENT #", 11);
			xnumLst(wa8125[2], 0, 0xA);
			xputnstrLst(", ", 2);
		}
		if (wa8125[1] != 0) { //	loc_5D84
			xputnstrLst("NEAR '", 6);
			curInfo_p = off2Info(wa8125[1]);
			curSymbol_p = getInfoSymbol();
			if (curSymbol_p != 0)
				xputnstrLst((char *)&curSymbol_p->name[1], curSymbol_p->name[0]);
			else
				xputstr2cLst("<LONG CONSTANT>", 0);
			xputnstrLst("', ", 3);
		}
		sub_5DB7();
		tabLst(2);
		if (wA17F == 0) 
			xputnstrLst("UNKNOWN ERROR", 13);
		else
			xputnstrLst((char *)&b5862[wA17D], (byte)wA17F);
		newLineLst();
	}
}


void sub_5DB7()
{
	word p, q;

	for (q = 0; b5862[q] != 0; q++) {
		p = q;
		while (b5862[q] != 0)
			q++;
		if (b5862[p] == wa8125[0]) {
			wA17D = p + 1;
			wA17F = q - wA17D;
			return;
		}
	}
	wA17F = 0;
}


void sub_5E1B(byte arg1b)
{
	wa8125[0] = fatalErrorCode = arg1b;
	wa8125[1] = 0;
	wa8125[2] = stmtNo;
	emitError();
	throw "Done";
}

void sub_5E3E()
{
	byte i;
	if (printOn) { //	locret_5EA6
		if (opByteCnt > 0) {
			tabLst(0xF4);
			xnumLst(baseAddr, -4, 0x10);
			i = 0;
			tabLst(-18);
			while (opByteCnt > i) { 
				xnumLst(opBytes[i], -2, 0x10);
				i++;
			}
		}
		tabLst(-26);
		setStartAndTabW(26, 8);
		xputnstrLst((char *)&line[1], line[0]);
		newLineLst();
	}
}




void getSourceLine()
{
	lstLine[0] = 0;
	bA187 = 0;

	while (1) {
		lstLine[lstLine[0] + 1] = getSourceCh();
		if (lstLine[lstLine[0] + 1] == '\n') {
			bo813B = 0;
			bo813C = 0;
			return;
		}
		if (lstLine[lstLine[0] + 1] == '\r')
			bA187++;
		else if (lstLine[lstLine[0] + 1] != '\r' && lstLine[0] < 0x80)
			lstLine[0]++;
	}
}

byte getSourceCh()
{
	if (w8137 == w8139) {
		while (1) {
			readFile(&srcFile, srcbuf, 640, &w8137);
			w8139 = 0;
			if (w8137 > 0)
				break;
			if (lstLine[0] != 0)
				return '\n';

			if (srcFileIdx == 0) {
				if (bA187 != 0)
					return '\n';
				else
					sub_5E1B(0xD7);
			}

			bA187 = 0;
			closeFile(&srcFile);
			srcFileIdx--;
			initFile(&srcFile, "SOURCE", srcFileTable[srcFileIdx].filename);
			openFile(&srcFile, 1);
			seekFile(&srcFile, (loc_t*)&srcFileTable[srcFileIdx].block);
		}
		--w8137;
	} else
		++w8139;
	return srcbuf[w8139] & 0x7f;
}

byte ccBits[] = { 0x10, 0x18, 8, 0, 0x18, 0x10};
byte ccCodes[][3] = {{2, 'N', 'C'}, {1, 'C', ' '}, {1, 'Z', ' '},
		   {2, 'N', 'Z'}, {1, 'C', ' '}, {2, 'N', 'C'}};

word arg1w_5FE7;

void sub_5FE7(word arg1w, byte arg2b)
{
	word p;
	arg1w_5FE7 = arg1w;
	while (arg2b > 0) {
		sub_603C();
		sub_654F();
		sub_5E3E();
		++arg1w_5FE7;
		--arg2b;
		p = baseAddr + opByteCnt;
		if (baseAddr > p) {
			wa8125[1] = wa8125[2] = 0;
			wa8125[0] = 0xCE;
			emitError();
		} 
		baseAddr = p;
	}
}



void sub_603C()
{
	if (b4A78[wA18D = w506F[arg1w_5FE7]] == 0)
		opByteCnt = 0;
	else {
		opBytes[0] = b4A78[wA18D];
		opByteCnt = 1;
	}

	dstRec = 0;
	line[0] = 0;

	while (1) {
		if ((bA18F = b4A78[++wA18D]) < 0x80) {
			line[line[0]+1] = bA18F;
			line[0]++;
		} else if (bA18F >= 0xc0) 
			sub_6175();
		else {
			bA190 = (bA18F >> 4) & 3;
			switch (bA18F & 0xf) {
			case 0: return;
			case 1: pstrcat2Line(sValAry[bA190]); break;
			case 2: addWord(); break;
			case 3: addHelper(); break;
			case 4: addSmallNum(); break;
			case 5: addStackOrigin(); break;
			case 6: addByte(); break;
			case 7: addPCRel(); break;
			case 8: addccCode(); break;
			case 9: emitHelperLabel(); break;
			case 10: pstrcat2Line(w969E); break;
			case 11: sub_64CF(); break;
			}
		}
	}
}

void pstrcat2Line(byte *arg1w)
{
	if (arg1w != 0) { //	locret_6174
		movemem(*arg1w, arg1w + 1, &line[line[0]+1]);
		line[0] += *arg1w;
	}
}


void sub_6175()
{
	byte i, j;
	byte *p;

	j= (bA18F >> 4) & 3;	 // ROR(bA18F, 4) AND 3
	if ((bA190 = bA18F & 0xf) < 4) {
		i= (byte)wValAry[bA190];
		p= sValAry[bA190];
	} else if (j== 0) {
		i= stkRegNo[bA190 - 4];
		p= (byte *)&opcodes[stkRegIdx[bA190 - 4]];
	} else {
		i= regNo[bA190 - 4];
		p= (byte *)&opcodes[regIdx[bA190 - 4]];
	}

	switch(j) {
	case 0:
		i = (i << 4) | (i >> 4);	// ROL(i, 4);
		break;
	case 1: i = (i << 3) | (i >> 5);	// ROL(i, 3);
		break;
	case 2: break;
	}
	opBytes[0] |= i;
	pstrcat2Line(p);
}



void addWord()
{
	word *p;

	dstRec = b96D6;
	p = (word *) &opBytes[opByteCnt];
	*p = wValAry[bA190];
	opByteCnt += 2;
	pstrcat2Line(sValAry[bA190]);
}



void addHelper()
{
	byte i, j;
	word *p, q;

	p = (word *)&opBytes[opByteCnt];
	if (bA190 == 1) 
		q = 0x69;
	else {
		j = b4495[i = b4566[b969D]][b9692];
		q = b42D6[(j >> 2)] + (j & 3);
	}
	helperStr[0] = num2Asc(q, -4, 10, (char *)(helperStr + 3)) + 2;
	pstrcat2Line(helperStr);
	if (byte_3C3B) {
		*p = ((word *)helpers_p)[q];
		dstRec = 1;
	} else {
		*p = 0;
		dstRec = 5;
		curExtId = (byte)((word *)helpers_p)[q];
	}
	opByteCnt += 2;
}


void addSmallNum()
{
	byte i;
	
	opBytes[opByteCnt++] = i = b4A78[++wA18D];
	// extend to word on opBytes if not 0x84
	if (bA190 != 0)
		opBytes[opByteCnt++] = 0;
	line[0] += num2Asc(i, 0, 10, (char *)&line[line[0]+1]);
}



void addStackOrigin()
{
	dstRec = 3;
	opBytes[opByteCnt] = 0;
	opBytes[opByteCnt + 1] = 0;
	opByteCnt += 2;
	pstrcat2Line((byte *)"\xE@STACK$ORIGIN ");
}



void addByte()
{
	byte *p;

	opBytes[opByteCnt] = (byte)wValAry[bA190];
	opByteCnt++;
	if (wValAry[bA190] > 255) {		// reformat number to byte size
		p = sValAry[bA190];
		*p = num2Asc(wValAry[bA190] & 0xff, 0, -16, (char *)(p + 1));
	}
	pstrcat2Line(sValAry[bA190]);
}

void addPCRel()
{
	word *p, q;

	dstRec = 1;
	p = (word *)&opBytes[opByteCnt];
	if ((q = b4A78[++wA18D]) > 127)	// sign extend
		q |= 0xff00;
	*p = baseAddr + q;
	opByteCnt += 2;
	line[line[0]+1] = '$';
	line[0]++;
	addWrdDisp(&line[0], q);
}




void addccCode()
{
	opBytes[0] |= ccBits[b969C];
	pstrcat2Line(ccCodes[b969C]);
}


void emitHelperLabel()
{
	helperStr[0] = num2Asc(helperId, -4, 10, (char *)(helperStr + 3)) + 3;
	pstrcat2Line(helperStr);
	helperId++;
}


void sub_64CF()
{
	byte i;
	switch(bA190) {
	case 0: i= b4566[b969D]; break;
	case 1: i= b475E[b969D]; break;
	case 2: i= b4774[b969D]; break;
	case 3: i= b478A[b969D]; break;
	}
	opBytes[0] = b473D[i];
	opByteCnt = 1;
	pstrcat2Line((byte *)&opcodes[b47A0[i]]);
}


void sub_654F()
{
	byte i;
	word p;
	if (opByteCnt == 0 || !OBJECT)
		return;
	if (rec6_4.len + opByteCnt >= 1018)
		flushRecs();
	p = baseAddr + opByteCnt - 2;	
	switch (dstRec) {
	case 0:	break;
	case 1: if (rec22.len + 2 >= 1018)
				flushRecs();
		recAddWord((rec *)&rec22.type, 1, p);
		break;
	case 2: if (rec24_1.len + 2 >= 1017)
			flushRecs();
		recAddWord((rec *)&rec24_1.type, 2, p);
		break;
	case 3:	if (rec24_2.len + 2 >= 99)
			flushRecs();
		recAddWord((rec *)&rec24_2.type, 2, p);
		break;
	case 4: if (rec24_3.len + 2 >= 99)
		flushRecs();
		recAddWord((rec *)&rec24_3.type, 2, p);
		break;
	case 5: if (rec20.len + 4 >= 1018)
			flushRecs();
		recAddWord((rec *)&rec20.type, 1, curExtId);
		recAddWord((rec *)&rec20.type, 1, p);
	}
	for (i = 0; i <= opByteCnt - 1; i++)
		recAddByte((rec *)&rec6_4.type, 3, opBytes[i]);
}


void sub_668B()
{
	sub_66F1();
	sub_6720();
	if (b812E == 0x87) { 
		baseAddr = getAddr();
		if (DEBUG) {
			rec8.len -= 4;
			recAddWord((rec *)&rec8.type, 1, baseAddr);
			rec8.len += 2;
		}
		flushRecs();
	}
	sub_5BD3();
	sub_5FE7(w47C1[b812E] & 0xfff, w47C1[b812E] >> 12);
}


void sub_66F1()
{
	byte i;

	if (b812E >= 0xAE) { //	locret_671F
		b812E = b4602[i = b812E - 0xAE];
		b9692 = b4431[i = b4444[i]];
	}
}


void sub_6720()
{
	static byte i;

	b96D6 = 0;
	if (b4332[b812E] & 0x80) {
		ifread(&tx1File, &b969C, 1);
		b969D = b457C[b969C];
	}
	w969E = 0;
	if ((bA1AB = (b4332[b812E] >> 4) & 7) != 0) {
		if (bA1AB <= 3)
			ifread(&tx1File, &i, 1);
		sub_67AD((i >> 4) & 0xf, 0);
		bA1AB = (b4332[b812E] >> 1) & 7;
		sub_67AD(i & 0xf, 1);
	}
}


byte arg1b_67AD;
byte arg2b_67AD;

void sub_67AD(byte arg1b, byte arg2b)
{
	arg1b_67AD = arg1b;
	arg2b_67AD = arg2b;

	switch(bA1AB) {
	case 0:	return;
	case 1: sub_685C(arg2b_67AD, regNo[arg1b_67AD], regIdx[arg1b_67AD]);
		sub_685C(arg2b_67AD + 2, regNo[arg1b_67AD + 4], regIdx[arg1b_67AD + 4]);
		break;
	case 2: sub_685C(arg2b_67AD, stkRegNo[arg1b_67AD], stkRegIdx[arg1b_67AD]);
		break;
	case 3:	sub_6B9B(); break;
	case 4: rdBVal(); break;
	case 5: rdWVal(); break;
	case 6: rdLocLab(); break;
	case 7: sub_69E1(0); break;
	}
}

void sub_685C(byte arg1b, byte arg2b, byte arg3b)
{
	wValAry[arg1b] = arg2b;
	sValAry[arg1b] = (byte *)&opcodes[arg3b];
}



void rdBVal()
{
	ifread(&tx1File, &wValAry[arg2b_67AD], 1);
	wValAry[arg2b_67AD] &= 0xff;
	b96B0[0] = num2Asc(wValAry[arg2b_67AD], 0, -16, (char *)&b96B0[1]);
	sValAry[arg2b_67AD] = b96B0;
}




void rdWVal()
{
	ifread(&tx1File, &wValAry[arg2b_67AD], 2);
	b96B0[0] = num2Asc(wValAry[arg2b_67AD], 0, -16, (char *)&b96B0[1]);
	sValAry[arg2b_67AD] = &b96B0[0];
}

void rdLocLab()
{
	ifread(&tx1File, &w96D7, 2);
	wValAry[arg2b_67AD] = ((word *)localLabels_p)[w96D7];
	locLabStr[1] = '@';
	locLabStr[0] = num2Asc(w96D7, 0, 10, (char *)&locLabStr[2]) + 1;
	sValAry[arg2b_67AD] = locLabStr;
	b96D6 = 1;
}



void sub_6982()
{
#pragma pack(push, 1)
	struct {
		byte i;
		word p;
	} s;
#pragma pack(pop)
	ifread(&tx1File, &s, 3);
	w969E = commentStr;
	commentStr[0] = num2Asc(s.i, 0, 10, (char *)&commentStr[3]) + 2;
	wValAry[arg2b_67AD] = s.p;
	b96B0[0] = num2Asc(s.p, 0, 10, (char *)&b96B0[1]);
	sValAry[arg2b_67AD] = &b96B0[0];
}


void sub_69E1(word arg1w)
{
	ifread(&tx1File, &curInfo_p, 2);
	curInfo_p = off2Info(curInfo_p);
	wValAry[arg2b_67AD] = getAddr() + arg1w;
	curSymbol_p = getInfoSymbol();
	if (curSymbol_p != 0) { //	loc_6A4C
		b96B0[0] = curSymbol_p->name[0];
		movemem(b96B0[0], &curSymbol_p->name[1], &b96B0[1]);
	} else {
		b96B0[0] = 1;
		b96B0[1] = '$';
		arg1w = wValAry[arg2b_67AD] - baseAddr;
	}
	sValAry[arg2b_67AD] =  b96B0;
	addWrdDisp(sValAry[arg2b_67AD], arg1w);
	if (testInfoFlag(F_EXTERNAL)) {
		b96D6 = 5;
		curExtId = getExternId();
	} else if (getInfoType() == PROC_T)
		b96D6 = 1;
	else if (getInfoType() == LABEL_T) 
		b96D6 = 1;
	else if (testInfoFlag(F_MEMBER))
		;
	else if (testInfoFlag(F_BASED))
		;
	else if (testInfoFlag(F_DATA))
		b96D6 = 1;
	else if (testInfoFlag(F_MEMORY))
		b96D6 = 4;
	else if (!testInfoFlag(F_ABSOLUTE))
		b96D6 = 2;
}




void sub_6B0E()
{
	word p[3];
	ifread(&tx1File, p, 6);
	curInfo_p = off2Info(p[1]);
	wValAry[arg2b_67AD] = p[2];
	b96B0[0] = num2Asc(p[2], 0, -16, (char *)&b96B0[1]);
	sValAry[0] = b96B0;
	w969E = commentStr;
	curSymbol_p = getInfoSymbol();
	commentStr[0] = curSymbol_p->name[0] + 2;
	movemem(curSymbol_p->name[0], &curSymbol_p->name[1], (char *)&commentStr[3]);
	addWrdDisp(w969E, p[0]);
}



void sub_6B9B()
{
	switch (arg1b_67AD - 8) {
	case 0: rdBVal(); break;
	case 1: rdWVal(); break;
	case 2:	sub_6982(); break;
	case 3: ifread(&tx1File, &wA1BD, 2);
		sub_69E1(wA1BD);
		break;
	case 4: sub_6B0E(); break;
	}
}

void sub_6E5C(byte *arg1w, byte arg2b)
{
	printf("\r\n\nPL/M-80 FATAL ERROR --\r\n\n%*s\r\n\nCOMPILATION TERMINATED\r\n\n", arg2b, arg1w);
	exit(1);
}


// subtract arg2w from block/byte pair -> arg1w
void sub_6FEA(word *arg1w, word arg2w)
{
	word p, q;

	p = arg2w / 128;
	q = arg2w % 128;
	arg1w[0] -= p;
	if (arg1w[1] < q) {
		arg1w[0]--;
		arg1w[1] += 128 - q;
	} else
		arg1w[1] -= q;
}









void sub_77BF(byte arg1b, byte arg2b, byte arg3b)
{
	byte_3CFB = arg1b;
	byte_3CFC = arg2b;
	byte_3CFD = arg3b;
}


void setStartAndTabW(byte arg1b, byte arg2b)
{
	listingMargin = arg1b - 1;
	tabWidth = arg2b;
}



void lstLineNo()
{
	char buf[7];
	sprintf(buf, "%6d", lineNo);
	xputnstrLst(&buf[2], 4);
}













