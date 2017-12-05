// vim:ts=4:expandtab:shiftwidth=4:
#include "asm80.h"

static byte copyright[] = "(C) 1976,1977,1979,1980 INTEL CORP";
keyword_t extKeywords[151] = {
    /*
        4 byte packed keyword
        byte opcode base
        byte offset to next entry or 0 if end
        byte type
        byte flags
    */
 /* [     token     ] opBase offset type        flags */
    { 0xA528, 0,      0,      0,      K_NE,      0},      /* NE */
    { 0x63B0, 0,      0xCC,   0,      K_IMM16,   0},      /* CZ */
    { 0x6E60, 0x9C40, 0,      0,      K_ENDM,    0},      /* DoEndm */
    { 0xBFB6, 0,      0xC0,   0,      K_SINGLE,  0},      /* RNZ */
    { 0xB354, 0,      0xC1,   0,      K_REG16,   0},      /* POP */
    { 0x6EE9, 0,      0,      0,      K_EQU,     0},      /* EQU */
    { 0xC6CD, 0,      0x32,   0x80,   K_IMM16,   0},      /* STA */
    { 0xA1A5, 0,      6,      0x80,   K_MVI,     0},      /* MVI */
    { 0xC6CF, 0,      0x37,   0,      K_SINGLE,  0},      /* STC */
    { 0xC6CD, 0x6158, 0,      0,      K_SPECIAL, 0x1B},   /* STACK */
    { 0x6615, 0,      0x27,   0,      K_SINGLE,  0},      /* DAA */
    { 0x988D, 0xE100, 0xA,    0,      K_LDSTAX,  0},      /* LDAX */
    { 0x61F6, 0,      0xC4,   0,      K_IMM16,   0},      /* CNZ */
    { 0x53CF, 0,      0x88,   0x88,   K_ARITH,   0},      /* ADC */
    { 0x53D0, 0,      0x80,   0,      K_ARITH,   0},      /* ADD */
    { 0x8D9F, 0,      0xD2,   0xA0,   K_IMM16,   0},      /* JNC */
	{ 0,      0,      0,      0,      0,         0},
	{ 0,      0,      0,      0,      0,         0},
	{ 0,      0,      0,      0,      0,         0},
    { 0x6E60, 0,      0,      0x88,   K_END,     0},      /* END */
    { 0x6A40, 0,      3,      0xA0,   K_REGNAME, 0},      /* E */
	{ 0,      0,      0,      0,      0,         0},
    { 0xAD70, 0,      0,      0,      K_OR,      0},      /* OR */
    { 0xAE08, 0,      0xD3,   0,      K_IMM8,    0},      /* OUT */
    { 0xA088, 0,      0,      0,      K_MOD,     0},      /* MOD */
    { 0xAD83, 0,      0,      0,      K_ORG,     0},      /* ORG */
	{ 0,      0,      0,      0,      0,         0},
    { 0x8F70, 0,      0xCA,   0x70,   K_IMM16,   0},      /* JZ */
    { 0x68E9, 0x76C0, 0,      0,      K_DSEG,    0},      /* DSEG */
    { 0xC488, 0,      0,      0,      K_SET,     0},      /* SET */
    { 0x8BD8, 0,      0xDA,   0x10,   K_IMM16,   0},      /* JC */
    { 0xC521, 0,      0x30,   0,      K_SINGLE,  2},      /* SIM */
    { 0x6618, 0,      9,      8,      K_REG16,   0},      /* DAD */
    { 0xB16C, 0x9600, 0xE9,   0,      K_SINGLE,  0},      /* PCHL */
    { 0xA4A1, 0x6A40, 0,      0,      K_NAME,    0},      /* NAME */
    { 0x6748, 0,      0xF3,   0,      K_SINGLE,  0},      /* DI */
    { 0x53D5, 0,      0xC6,   0x18,   K_IMM8,    0},      /* ADI */
    { 0x6630, 0,      0,      0,      K_DB,      0},      /* DB */
    { 0x8DB6, 0,      0xC2,   0,      K_IMM16,   0},      /* JNZ */
    { 0xAD7D, 0,      0xB0,   0,      K_ARITH,   0},      /* ORA */
    { 0xB3FB, 0,      6,      0,      K_REGNAME, 0},      /* PSW */
    { 0xAD85, 0,      0xF6,   0,      K_IMM8,    0},      /* ORI */
    { 0xA09A, 0,      0x40,   0xB8,   K_MOV,     0},      /* MOV */
    { 0x8610, 0,      0,      0xB8,   K_IF,      0},      /* IF */
	{ 0,      0,      0,      0,      0,         0},
    { 0x9938, 0x6400, 0x2A,   0xB0,   K_IMM16,   0},      /* LHLD */
    { 0x9600, 0,      5,      0xB0,   K_REGNAME, 0},      /* L */
    { 0xC6F6, 0,      0x90,   0,      K_ARITH,   0},      /* SUB */
    { 0xC634, 0x9600, 0xF9,   0,      K_SINGLE,  0},      /* SPHL */
    { 0xC405, 0,      0xDE,   0,      K_IMM8,    0},      /* SBI */
	{ 0,      0,      0,      0,      0,         0},
    { 0xBF4F, 0,      7,      0,      K_SINGLE,  0},      /* RLC */
    { 0x5780, 0,      0,      0,      K_REGNAME, 0},      /* B */
	{ 0,      0,      0,      0,      0,         0},
    { 0xE614, 0x9600, 0xE3,   0xE8,   K_SINGLE,  0},      /* XTHL */
	{ 0,      0,      0,      0,      0,         0},
	{ 0,      0,      0,      0,      0,         0},
    { 0xB11B, 0x6A40, 2,      0,      K_SPECIAL, 0},      /* PAGE */
	{ 0,      0,      0,      0,      0,         0},
	{ 0,      0,      0,      0,      0,         0},
    { 0xA7C0, 0,      0,      0xD8,   K_NUL,     0},      /* NUL */
    { 0x8750, 0,      0xDB,   0,      K_IMM8,    0},      /* IN */
    { 0x6E60, 0x8610, 0,      0xD0,   K_ENDIF,   0},      /* ENDIF */
	{ 0,      0,      0,      0,      0,         0},
	{ 0,      0,      0,      0,      0,         0},
    { 0xC3FE, 0,      0x98,   0,      K_ARITH,   0},      /* SBB */
    { 0xC6D7, 0x9A10, 0,      0,      K_STKLN,   0},      /* STKLN */
    { 0xBFE0, 0,      0xF0,   0,      K_SINGLE,  0},      /* RP */
    { 0xC078, 0,      0xC7,   0,      K_RST,     0},      /* RST */
    { 0xC4F8, 0x6400, 0x22,   0,      K_IMM16,   0},      /* SHLD */
    { 0x9E57, 0xBFB8, 0,      0,      K_MACRO,   0},      /* MACRO */
    { 0x6231, 0,      0xEC,   0,      K_IMM16,   0},      /* CPE */
    { 0xB436, 0x9957, 0,      0xA8,   K_PUBLIC,  0},      /* PUBLIC */
    { 0xBDA0, 0,      0x17,   0xA8,   K_SINGLE,  0},      /* RAL */
    { 0x6FF5, 0xCBE8, 0,      0xD8,   K_EXITM,   0},      /* DoExitm */
    { 0x6235, 0,      0xFE,   0,      K_IMM8,    0},      /* CPI */
    { 0xBF68, 0,      0xF8,   0,      K_SINGLE,  0},      /* RM */
	{ 0,      0,      0,      0,      0,         0},
	{ 0,      0,      0,      0,      0,         0},
    { 0xBDA6, 0,      0x1F,   0,      K_SINGLE,  0},      /* RAR */
    { 0x805B, 0x7D00, 0,      0xB8,   K_HIGH,    0},      /* HIGH */
    { 0x6400, 0,      2,      0xC8,   K_REGNAME, 0},      /* D */
    { 0xB447, 0x7D00, 0xC5,   0xD0,   K_REG16,   0},      /* PUSH */
    { 0xC6FD, 0,      0xD6,   8,      K_IMM8,    0},      /* SUI */
    { 0x6220, 0,      0xF4,   0,      K_IMM16,   0},      /* CP */
    { 0x61A8, 0,      0xFC,   0,      K_IMM16,   0},      /* CM */
	{ 0,      0,      0,      0,      0,         0},
    { 0x5629, 0x76C0, 0,      0,      K_ASEG,    0},      /* ASEG */
    { 0xBFF1, 0,      0xE8,   0,      K_SINGLE,  0},      /* RPE */
    { 0x880C, 0x5DC0, 0,      0xA8,   K_IRPC,    0},      /* IRPC */
    { 0xBE48, 0,      0xC9,   0,      K_SINGLE,  0},      /* RET */
    { 0x876E, 0,      4,      0,      K_INRDCR,  0},      /* INR */
    { 0xBEE1, 0,      0x20,   0,      K_SINGLE,  2},      /* RIM */
    { 0xBFFB, 0,      0xE0,   0,      K_SINGLE,  0},      /* RPO */
    { 0xC6CD, 0xE100, 2,      0x28,   K_LDSTAX,  0},      /* STAX */
    { 0x988D, 0,      0x3A,   0x98,   K_IMM16,   0},      /* LDA */
    { 0x7000, 0xBF90, 0,      0,      K_EXTRN,   0},      /* EXTRN */
    { 0x8774, 0,      3,      0x90,   K_REG16,   0},      /* INX */
    { 0x61B5, 0,      0x2F,   0x90,   K_SINGLE,  0},      /* CMA */
    { 0x876C, 0x5449, 1,      0,      K_SPECIAL, 0},      /* INPAGE */
    { 0x9A47, 0x5500, 0,      0x98,   K_LOCAL,   0},      /* DoLocal */
    { 0xE5BD, 0,      0xA8,   8,      K_ARITH,   0},      /* XRA */
    { 0x80E0, 0,      0x76,   0,      K_SINGLE,  0},      /* HLT */
    { 0x8DE0, 0,      0xF2,   0,      K_IMM16,   0},      /* JP */
    { 0x9A5B, 0,      0,      0,      K_LOW,     0},      /* LOW */
    { 0x6D88, 0,      0xFB,   0,      K_SINGLE,  0},      /* EI */
    { 0x623B, 0,      0xE4,   0,      K_IMM16,   0},      /* CPO */
    { 0x6676, 0,      5,      0x68,   K_INRDCR,  0},      /* DCR */
    { 0xE5C5, 0,      0xEE,   0,      K_IMM8,    0},      /* XRI */
    { 0x555D, 0,      0xA0,   0,      K_ARITH,   0},      /* ANA */
    { 0x62A9, 0x76C0, 0,      0,      K_CSEG,    0},      /* CSEG */
    { 0x8D68, 0,      0xFA,   0,      K_IMM16,   0},      /* JM */
    { 0x5560, 0,      0,      0,      K_AND,     0},      /* AND */
    { 0x5140, 0,      7,      0x58,   K_REGNAME, 0},      /* A */
    { 0xBE44, 0xC800, 0,      0,      K_REPT,    0},      /* DoRept */
    { 0x8DF1, 0,      0xEA,   0,      K_IMM16,   0},      /* JPE */
    { 0x880C, 0,      0,      0,      K_IRP,     0},      /* IRP */
    { 0x5565, 0,      0xE6,   0,      K_IMM8,    0},      /* ANI */
    { 0x9B00, 0,      0,      0,      K_LT,      0},      /* LT */
    { 0x61B7, 0,      0x3F,   0,      K_SINGLE,  0},      /* CMC */
    { 0x8DFB, 0,      0xE2,   0,      K_IMM16,   0},      /* JPO */
    { 0x7BC0, 0,      0,      0,      K_GT,      0},      /* GT */
    { 0x6978, 0,      0,      0x78,   K_DW,      0},      /* DW */
    { 0x6EC8, 0,      0,      0,      K_EQ,      0},      /* EQ */
    { 0x68D8, 0,      0,      0x48,   K_DS,      0},      /* DS */
    { 0x7968, 0,      0,      0xA0,   K_GE,      0},      /* GE */
    { 0xA6D4, 0,      0,      0,      K_SINGLE,  0},      /* NOP */
    { 0x9F01, 0xAD95, 0,      0,      K_SPECIAL, 0x1C},   /* MEMORY */
    { 0x5FE0, 0x9600, 0xCD,   0,      K_IMM16,   0},      /* CALL */
    { 0xBDD8, 0,      0xD8,   0,      K_SINGLE,  0},      /* RC */
    { 0x53AD, 0,      0xCE,   0x80,   K_IMM8,    0},      /* ACI */
    { 0xBF9F, 0,      0xD0,   0,      K_SINGLE,  0},      /* RNC */
    { 0xE36C, 0x76C0, 0xEB,   0,      K_SINGLE,  0},      /* XCHG */
    { 0x61C4, 0,      0xB8,   8,      K_ARITH,   0},      /* CMP */
    { 0x667C, 0,      0xB,    0,      K_REG16,   0},      /* DCX */
    { 0xC620, 0,      6,      0,      K_SP,      0},      /* SP */
    { 0x9C40, 0,      6,      0,      K_REGNAME, 0},      /* M */
    { 0x98A8, 0,      0,      0x38,   K_LE,      0},      /* LE */
    { 0x6018, 0,      0xDC,   0,      K_IMM16,   0},      /* CC */
    { 0x7D00, 0,      4,      0x50,   K_REGNAME, 0},      /* H */
    { 0xC03F, 0,      0xF,    0x50,   K_SINGLE,  0},      /* RRC */
    { 0xC4F8, 0,      0,      0,      K_SHL,     0},      /* SHL */
    { 0x5DC0, 0,      1,      0,      K_REGNAME, 0},      /* C */
    { 0xC170, 0,      0xC8,   0,      K_SINGLE,  0},      /* RZ */
    { 0xE556, 0,      0,      0,      K_XOR,     0},      /* XOR */
    { 0x6E1F, 0x6A40, 0,      0,      K_ELSE,    0},      /* ELSE */
    { 0xA6D8, 0,      0,      0,      K_NOT,     0},      /* NOT */
    { 0xC4FE, 0,      0,      0,      K_SHR,     0},      /* SHR */
    { 0x9BB5, 0,      1,      0,      K_LXI,     0},      /* LXI */
    { 0x8D84, 0,      0xC3,   0,      K_IMM16,   0},      /* JMP */
	{ 0x61DF, 0,      0xD4,   0,      K_IMM16,   0}       /* CNC */
};

