#include "plm.h"
#include <stdio.h>

FILE *Fopen(char *isisPath, char *mode);

int symMode = 1;
int infoMode = 0;



 char *builtinStr[] = {
    "CARRY", "DEC", "DOUBLE", "HIGH", "INPUT", "LAST", "LENGTH",
    "LOW", "MOVE", "OUTPUT", "PARITY", "ROL", "ROR", "SCL", "SCR",
    "SHL", "SHR", "SIGN", "SIZE", "STACKPTR", "TIME", "ZERO"
};

 void dumpAllInfo()
 {
     offset_t p;
     for (p = botInfo + (infoMode == 0 ? 2 : procInfo[1]); p < topInfo; p += InfoP(p)->len) {
         showInfo(p, stdout);
         putchar('\n');
     }

}
void showInfo(offset_t off, FILE *fp)
{
    info_t *info;
    sym_t *sym;
    lit_t *lit;
    pstr_t *ps;

    info = InfoP(off);
    fprintf(fp,"len = %d", info->len);
    switch (info->type) {
    case 0: fprintf(fp,", LIT"); break;
    case 1: fprintf(fp,", LABEL"); break;
    case 2: fprintf(fp,", BYTE"); break;
    case 3: fprintf(fp,", ADDRESS"); break;
    case 4: fprintf(fp,", STRUCT"); break;
    case 5: fprintf(fp,", PROC"); break;
    case 6: fprintf(fp,", BUILTIN"); break;
    case 7: fprintf(fp,", MACRO"); break;
    case 8: fprintf(fp,", UNKNOWN"); break;
    case 9: fprintf(fp,", TEMP"); break;
    default: fprintf(fp,", OOPS type = %d\n", info->type);
    }
    switch (symMode) {
    case 0: fprintf(fp,", sym = %04X", topInfo - info->sym); break;
    case 1: sym = SymbolP(topSymbol - info->sym);
            fprintf(fp,", sym = %.*s", sym->name[0], &sym->name[1]);
            break;
    case 2: ps = PstrP(topSymbol - 1 - info->sym);
            fprintf(fp,", sym = %.*s", ps->len, ps->str);
            break;
    }
    fprintf(fp,", scope = %04X", info->scope);
    fprintf(fp,", ilink = %04X", info->ilink);
    switch (info->type) {
    case LIT_T:
        lit = LitP(off);
        if (symMode == 0)
            fprintf(fp,", lit = %04X", lit->litAddr);
        else {
            ps = PstrP(lit->litAddr + 1);
            fprintf(fp,", lit = %.*s", ps->len, ps->str);
        }
        break;
    case LABEL_T: case BYTE_T: case ADDRESS_T: case STRUCT_T:
    case PROC_T:
        fprintf(fp,", flags =  %02X %02X %02X", info->flag[0], info->flag[1], info->flag[2]);
        break;
    case BUILTIN_T:
        fprintf(fp,", builtin = %s, paramCnt = %d, data type = %d", builtinStr[info->flag[0]], info->flag[1], info->flag[2]);
        break;
    case MACRO_T:
        fprintf(fp,", flags =  %02X %02X", info->flag[0], info->flag[1]);
        break;
    case CONDVAR_T:
        fprintf(fp,", flag =  %02X", info->flag[0]);
        break;
    case UNK_T:
        break;
    }
    if (LABEL_T <= info->type && info->type <= PROC_T) {
        fprintf(fp,", extId = %d", info->extId);
        if (info->type >= BYTE_T) {
            fprintf(fp,", dim = %d", info->dim);
            fprintf(fp,", baseoff = %04X", info->baseoff);
            fprintf(fp,", parent = %04X", info->parent);
        }
    }
    if (info->type == PROC_T) {
        fprintf(fp,", dtype = %d", info->dtype);
        fprintf(fp,", intno = %d", info->intno);
        fprintf(fp,", pcnt = %d", info->pcnt);
        fprintf(fp,", procId = %d", info->procId);
    }
}

void dumpBuf(file_t *fp)
{
    printf("%.6s: %.16s\n", fp->sNam, fp->fNam);
    for (int i = 0; i < fp->actual; i++)
        printf("%02X%c", fp->bufP[i], i % 16 == 15 ? '\n' : ' ');
    putchar('\n');
}

void copyFile(pointer src, pointer dst) // handles isis src file name
{
    byte buffer[2048];
    word actual, status;
    word srcfd;
    FILE *dstfp;

    Open(&srcfd, src, READ_MODE, 0, &status);
    dstfp = fopen(dst, "wb");
    if (status != 0 || dstfp == NULL) {
        fprintf(stderr, "copyFile(%s, %s) fatal error\n", src, dst);
        Exit();
    }
    
    while (1) {
        Read(srcfd, buffer, sizeof(buffer), &actual, &status);
        if (actual == 0 || status != 0)
            break;
        fwrite(buffer, 1, actual, dstfp);
    }
    Close(srcfd, &status);
    fclose(dstfp);
}

char *tx2NameTable[] = {
    "T2_LT", "T2_LE", "T2_NE", "T2_EQ", "T2_GE",
    "T2_GT", "T2_ROL", "T2_ROR", "T2_SCL", "T2_SCR",
    "T2_SHL", "T2_SHR", "T2_JMPFALSE", "T2_13", "T2_14",
    "T2_15", "T2_16", "T2_17", "T2_DOUBLE", "T2_PLUSSIGN",
    "T2_MINUSSIGN", "T2_STAR", "T2_SLASH", "T2_MOD", "T2_AND",
    "T2_OR", "T2_XOR", "T2_BASED", "T2_BYTEINDEX", "T2_WORDINDEX",
    "T2_MEMBER", "T2_UNARYMINUS", "T2_NOT", "T2_LOW", "T2_HIGH",
    "T2_ADDRESSOF", "T2_PLUS", "T2_MINUS", "T2_38", "T2_39",
    "T2_40", "T2_41", "T2_42", "T2_43", "T2_44",
    "T2_45", "T2_46", "T2_47", "T2_48", "T2_49",
    "T2_50", "T2_51", "T2_52", "T2_53", "T2_54",
    "T2_55", "T2_56", "T2_TIME", "T2_STKBARG", "T2_STKWARG",
    "T2_DEC", "T2_COLONEQUALS", "T2_OUTPUT", "T2_63", "T2_STKARG",
    "T2_65", "T2_66", "T2_67", "T2_68", "T2_MOVE",
    "T2_70", "T2_RETURNBYTE", "T2_RETURNWORD", "T2_RETURN", "T2_74",
    "T2_75", "T2_76", "T2_77", "T2_78", "T2_79",
    "T2_80", "T2_81", "T2_82", "T2_83", "T2_84",
    "T2_85", "T2_86", "T2_87", "T2_88", "T2_89",
    "T2_90", "T2_91", "T2_92", "T2_93", "T2_94",
    "T2_95", "T2_96", "T2_97", "T2_98", "T2_99",
    "T2_100", "T2_101", "T2_102", "T2_103", "T2_104",
    "T2_105", "T2_106", "T2_107", "T2_108", "T2_109",
    "T2_110", "T2_111", "T2_112", "T2_113", "T2_114",
    "T2_115", "T2_116", "T2_117", "T2_118", "T2_119",
    "T2_120", "T2_121", "T2_122", "T2_123", "T2_124",
    "T2_125", "T2_126", "T2_127", "T2_128", "T2_129",
    "T2_ADDW", "T2_BEGMOVE", "T2_CALL", "T2_CALLVAR", "T2_134",
    "T2_PROCEDURE", "T2_LOCALLABEL", "T2_CASELABEL", "T2_LABELDEF", "T2_INPUT",
    "T2_GO_TO", "T2_JMP", "T2_JNC", "T2_JNZ", "T2_SIGN",
    "T2_ZERO", "T2_PARITY", "T2_CARRY", "T2_DISABLE", "T2_ENABLE",
    "T2_HALT", "T2_STMTCNT", "T2_LINEINFO", "T2_MODULE", "T2_SYNTAXERROR",
    "T2_TOKENERROR", "T2_EOF", "T2_LIST", "T2_NOLIST", "T2_CODE",
    "T2_NOCODE", "T2_EJECT", "T2_INCLUDE", "T2_ERROR", "T2_164",
    "T2_165", "T2_166", "T2_167", "T2_168", "T2_169",
    "T2_170", "T2_171", "T2_IDENTIFIER", "T2_NUMBER", "T2_BIGNUMBER",
    "T2_175", "T2_176", "T2_177", "T2_178", "T2_179",
    "T2_180", "T2_STACKPTR", "T2_SEMICOLON", "T2_OPTBACKREF", "T2_CASE",
    "T2_ENDCASE", "T2_ENDPROC", "T2_LENGTH", "T2_LAST", "T2_SIZE",
    "T2_BEGCALL"
};


char *tx2Name(byte op)
{
    if (op >= 191) {
        fprintf(stderr, "tx2 op %d out of range\n", op);
        Exit();
    }

    return tx2NameTable[op];
}


char *lexItems[] = {
    "L_LINEINFO", "L_SYNTAXERROR", "L_TOKENERROR", "L_LIST", "L_NOLIST",
    "L_CODE", "L_NOCODE", "L_EJECT", "L_INCLUDE", "L_STMTCNT",
    "L_LABELDEF", "L_LOCALLABEL", "L_JMP", "L_JMPFALSE", "L_PROCEDURE",
    "L_SCOPE", "L_END", "L_DO", "L_DOLOOP", "L_WHILE",
    "L_CASE", "L_CASELABEL", "L_IF", "L_STATEMENT", "L_CALL",
    "L_RETURN", "L_GO", "L_GOTO", "L_SEMICOLON", "L_ENABLE",
    "L_DISABLE", "L_HALT", "L_EOF", "L_AT", "L_INITIAL",
    "L_DATA", "L_IDENTIFIER", "L_NUMBER", "L_STRING", "L_PLUSSIGN",
    "L_MINUSSIGN", "L_PLUS", "L_MINUS", "L_STAR", "L_SLASH",
    "L_MOD", "L_COLONEQUALS", "L_AND", "L_OR", "L_XOR",
    "L_NOT", "L_LT", "L_LE", "L_EQ", "L_NE",
    "L_GE", "L_GT", "L_COMMA", "L_LPAREN", "L_RPAREN",
    "L_PERIOD", "L_TO", "L_BY", "L_INVALID", "L_MODULE",
    "L_XREFUSE", "L_XREFDEF", "L_EXTERNAL"
};


static getWord(FILE *fp)
{
    int cl = getc(fp);
    int ch = getc(fp);
    if (cl == EOF || ch == EOF) {
        fprintf(stderr, "premature EOF in getWord\n");
        return EOF;
    }
    return ch * 256 + cl;
}

void DumpLexStream() // to be used after Start1
{
    FILE *fp;
    FILE *fpout;
    int c;
    int w1, w2, w3;
    sym_t *sym;
    char inc[18];

    if ((fp = Fopen(tx1File.fNam, "rb")) == NULL) {
        fprintf(stderr, "can't open lex stream\n");
        return;
    }
    if ((fpout = fopen("lex.dmp", "w")) == NULL) {
        fprintf(stderr, "can't create lex.dmp\n");
        fclose(fp);
        return;
    }
    while ((c = getc(fp)) != EOF) {
        if (c > L_EXTERNAL)
            fprintf(fpout, "Invalid lex item %d\n", c);
        else {
            fprintf(fpout, "%s", lexItems[c]);
            switch (c) {
            case L_LINEINFO:
                w1 = getWord(fp);
                w2 = getWord(fp);
                w3 = getWord(fp);
                fprintf(fpout, " line %d, stmt %d, blk %d", w1, w2, w3);
                break;
            case L_SYNTAXERROR:
                fprintf(fpout, " %d", getWord(fp));
                break;
            case L_TOKENERROR:
                w1 = getWord(fp);
                sym = SymbolP(getWord(fp));
                if (sym == NULL)
                    fprintf(fpout, " %d -NULL-", w1);
                else
                    fprintf(fpout, "%d %.*s", w1, sym->name[0], &sym->name[1]);
                break;
            case L_STRING:
                w1 = getWord(fp);
                fprintf(fpout, " %d '", w1);
                while (w1-- > 0 && (c = getc(fp)) != EOF)
                    putc(c, fpout);
                putc('\'', fpout);
                break;
            case L_NUMBER: case L_STMTCNT: case L_LABELDEF: case L_LOCALLABEL: case L_JMP: case L_JMPFALSE:
            case L_SCOPE: case L_CASELABEL:
                w1 = getWord(fp);
                if (c != L_SCOPE)
                    fprintf(fpout, " %d", w1);
                if (c == L_SCOPE || (c == L_NUMBER && w1 > 9))
                    fprintf(fpout, " [%04X]", w1);
                break;
            case L_IDENTIFIER:
                sym = SymbolP(getWord(fp));
                if (sym == NULL)
                    fprintf(fpout, " -NULL-");
                else
                    fprintf(fpout, " %.*s", sym->name[0], &sym->name[1]);
                break;
            case L_AT: case L_INITIAL: case L_DATA:
                //w1 = getWord(fp);
                //fprintf(fpout, " info [%04X]", w1);
                //break;
            case L_PROCEDURE:
            case L_XREFUSE: case L_XREFDEF: case L_EXTERNAL:
                w1 = getWord(fp);
                if (w1 == 0)
                    fprintf(fpout, " -NULL-");
                else {
                    putc(' ', fpout);
                    showInfo(w1 + botInfo, fpout);
                }
                break;
            case L_INCLUDE:
                fread(inc + 12, 1, 6, fp);
                fread(inc + 6, 1, 6, fp);
                fread(inc, 1, 6, fp);
                fprintf(fpout, " %.16s", inc);
                break;
            }
            putc('\n', fpout);
        }
    }
    fclose(fp);
    fclose(fpout);
}
