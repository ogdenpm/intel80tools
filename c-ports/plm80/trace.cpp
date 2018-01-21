// $Id: trace.cpp,v 1.1 2003/10/04 21:08:48 Mark Ogden Exp $
#include <stdio.h>
#include "plm.hpp"
#include "common.hpp"
#include "trace.hpp"


char *tx1Names[] = {
"L_LINEINFO", "L_SYNTAXERROR", "L_TOKENERROR", "L_LIST", "L_NOLIST", "L_CODE", "L_NOCODE", "L_EJECT",
"L_INCLUDE", "L_STMTCNT", "L_LABELDEF", "L_LOCALLABEL", "L_JMP", "L_JMPFALSE", "L_PROCEDURE", "L_SCOPE",
"L_END", "L_DO", "L_DOLOOP", "L_WHILE", "L_CASE", "L_CASELABEL", "L_IF", "L_STATEMENT",
"L_CALL", "L_RETURN", "L_GO", "L_GOTO", "L_ILLEGAL", "L_ENABLE", "L_DISABLE", "L_HALT",
"L_EOF", "L_AT", "L_INITIAL", "L_DATA", "L_VARIABLE", "L_NUMBER", "L_STRING", "L_PLUSSIGN",
"L_MINUSSIGN", "L_PLUS", "L_MINUS", "L_STAR", "L_SLASH", "L_MOD", "L_COLONEQUALS", "L_AND",
"L_OR", "L_XOR", "L_NOT", "L_LT", "L_LE", "L_EQ", "L_NE", "L_GE",
"L_GT", "L_COMMA", "L_LPAREN", "L_RPAREN", "L_PERIOD", "L_TO", "L_BY", "L_UNUSED",
"L_MODULE", "L_LABELREF", "L_XREF", "L_EXTERNAL", "L_ERROR"
};


char *tx2Names[] = {
    "LT", "LE", "NE", "EQ", "GE", "GT", "ROL", "ROR",
    "SCL", "SCR", "SHL", "SHR", "JMPFALSE", "T2_13", "T2_14", "T2_15",
    "T2_16", "T2_17", "DOUBLE", "PLUSSIGN", "MINUSSIGN", "STAR", "SLASH", "MOD",
    "AND", "OR", "XOR", "T2_27", "T2_28", "T2_29", "T2_30", "T2_31",
    "NOT", "LOW", "HIGH", "T2_35", "PLUS", "MINUS", "T2_38", "T2_39",
    "T2_40", "T2_41", "T2_42", "T2_43", "T2_44", "T2_45", "T2_46", "T2_47",
    "T2_48", "T2_49", "T2_50", "T2_51", "T2_52", "T2_53", "T2_54", "T2_55",
    "T2_56", "TIME", "T2_58", "T2_59", "DEC", "COLONEQUALS", "T2_62", "T2_63",
    "T2_64", "T2_65", "T2_66", "T2_67", "T2_68", "MOVE", "T2_70", "RETURNBYTE",
    "RETURNWORD", "RETURN", "T2_74", "T2_75", "T2_76", "T2_77", "T2_78", "T2_79",
    "T2_80", "T2_81", "T2_82", "T2_83", "T2_84", "T2_85", "T2_86", "T2_87",
    "T2_88", "T2_89", "T2_90", "T2_91", "T2_92", "T2_93", "T2_94", "T2_95",
    "T2_96", "T2_97", "T2_98", "T2_99", "T2_100", "T2_101", "T2_102", "T2_103",
    "T2_104", "T2_105", "T2_106", "T2_107", "T2_108", "T2_109", "T2_110", "T2_111",
    "T2_112", "T2_113", "T2_114", "T2_115", "T2_116", "T2_117", "T2_118", "T2_119",
    "T2_120", "T2_121", "T2_122", "T2_123", "T2_124", "T2_125", "T2_126", "T2_127",
    "T2_128", "T2_129", "T2_130", "T2_131", "T2_132", "T2_133", "T2_134", "PROCEDURE",
    "LOCALLABEL", "CASELABEL", "LABELDEF", "T2_139", "GO_TO", "JMP", "T2_142", "T2_143",
    "SIGN", "ZERO", "PARITY", "CARRY", "DISABLE", "ENABLE", "HALT", "STMTCNT",
    "LINEINFO", "MODULE", "SYNTAXERROR", "TOKENERROR", "EOF", "LIST", "NOLIST", "CODE",
    "NOCODE", "EJECT", "INCLUDE", "T2_163", "T2_164", "T2_165", "T2_166", "T2_167",
    "T2_168", "T2_169", "T2_170", "T2_171", "VARIABLE", "NUMBER", "BIGNUMBER", "T2_175",
    "T2_176", "T2_177", "T2_178", "T2_179", "T2_180", "T2_181", "ILLEGAL", "OPTBAKREF",
    "CASE", "ENDCASE", "ENDPROC", "LENGTH", "LAST", "SIZE", "T2_190", "T2_191",
    "T2_192", "T2_193", "T2_194", "T2_195", "T2_196", "T2_197", "T2_198", "T2_199",
    "T2_200", "T2_201", "T2_202", "T2_203", "T2_204", "T2_205", "T2_206", "T2_207",
    "T2_208", "T2_209", "T2_210", "T2_211", "T2_212", "T2_213", "T2_214", "T2_215",
    "T2_216", "T2_217", "T2_218", "T2_219", "T2_220", "T2_221", "T2_222", "T2_223",
    "T2_224", "T2_225", "T2_226", "T2_227", "T2_228", "T2_229", "T2_230", "T2_231",
    "T2_232", "T2_233", "T2_234", "T2_235", "T2_236", "T2_237", "T2_238", "T2_239",
    "T2_240", "T2_241", "T2_242", "T2_243", "T2_244", "T2_245", "T2_246", "T2_247",
    "T2_248", "T2_249", "T2_250", "T2_251", "T2_252", "T2_253", "T2_254"
};



void traceTx1(void *buf, word len)
{
    static int ccnt = 0;
    static int slen;
    byte *p = (byte *)buf;

    while (len-- > 0) {
        if (ccnt == 0) {
            ccnt = tx1ItemLengths[*p];
            if (ccnt == 255)
                ccnt = -2;
            printf("@%s", tx1Names[*p++]);
            if (ccnt == 0)
                putchar('\n');
        } else if (ccnt == -2) {
            slen = *p++;
            ccnt++;
        } else if (ccnt == -1) {
            slen += *p++ * 256;
            ccnt = slen;
            printf(" (%d):", ccnt);
        } else {
            printf(" %02x", *p++);
            if (--ccnt == 0)
                putchar('\n');
        }
    }
}

void traceRdTx1(byte tx1type, byte *buf, word len)
{
       printf("#%s", tx1Names[tx1type]);
       while (len-- > 0)
           printf(" %02x", *buf++);
       putchar('\n');
}


void traceTx2(word cnt, byte *buf, byte len)
{
    printf(">>%d:%d :", cnt, *buf++);
    while (--len > 0)
        printf(" %02X", *buf++);
    putchar('\n');
}

static char *typenames[] = {"Literal", "Label", "Byte", "Address", "Structure",
					   "Procedure", "BuiltIn", "MacroExpand", "type8", "temp"};

void dumpInfo(info_pt p)
{
	if (p == 0) {
		printf("Info undefined\n");
		return;
	}
	printf("%s(%d) len = %d\n", typenames[p->type], p->type, p->len);
	printf("symAddr = %x", (word)(topSymbol - p->symbolOffset));
	printf(", Scope = %x, nextOffset = %x\n", p->infoScope, p->nextInfoOffset);
	if (p->type == 9)
		printf("flag[0] = %x", p->flags[0]);
	else if (p->type == 0 || p->type == 7)
		printf("flag[0] = %x, flag[1] = %x, litaddr = %x\n", p->flags[0],
		p->flags[1], p->litaddr);
	else if (p->type != 8)
		printf("flag[0] = %x, flag[1] = %x, flag[2] = %x litaddr = %x\n",
		p->flags[0], p->flags[1], p->flags[2], p->litaddr);
	if (p->type >= 1 && p->type <= 5) {
		printf("externId = %x ", p->externId);
		if (p->type > 1)
			printf("dim = %x, basedOffset = %x, parentOffsetOrSize = %x\n",
			p->dimension, p->basedOffset, p->parentOffset);
		else
			putchar('\n');
		if (p->type == 5)
			printf("intrNo = %x, paramCnt = %x, procID = %x\n",
			p->intrNo, p->paramCnt, p->procID);
	}
	putchar('\n');
}

void dumpSym(symbol_pt p)
{
	if (p == 0)
		printf("Symbol undefined\n");
	else {
		printf("%04x - %.*s\n", (word)p, p->name[0], p->name + 1);
		dumpInfo(p->infoChain);
	}
}


void dumpTx2Q(int n)
{
	int i;
	extern byte tx2opc[];
	printf("tx2opc[]\n");
	for (i = 0; i < n; i++)
		printf("%02X%c", tx2opc[i], (i % 16 == 15) ? '\n' : ' ');
	if (i % 16 != 0)
		putchar('\n');
}

void dumpTx2QDetail(int n)
{
	int i;
	extern byte tx2opc[], tx2Aux1b[], tx2Aux2b[];
	extern word tx2op1[], tx2op2[], tx2op3[], tx2Auxw[];
	printf("tx2opc[]\n");
	printf("pos opc op1  op2  op3  a1b a2b Auxw\n");
	for (i = 0; i <= n; i++)
		printf("%3d %02X  %04X %04X %04X  %02X  %02X %04X\n",
			i, tx2opc[i], tx2op1[i], tx2op2[i], tx2op3[i],
			tx2Aux1b[i], tx2Aux2b[i], tx2Auxw[i]);
}


void dumpMem(char *file, word start, word end)
{
	FILE *fp;

	if ((fp = fopen(file, "a")) == NULL) {
		printf("can't append to file %s\n", file);
		return;
	}
	byte *s = (byte *)address(start & 0xfff0);
	int i;
	for (i = start & 0xfff0; i <= end; i++) {
		if (i%16 == 0)
			fprintf(fp, "%04X ", i);
		fprintf(fp, "%02X%c", *s++, (i % 16 == 15) ? '\n' : ' ');
	}
	putc('\n', fp);
	fclose(fp);
}
