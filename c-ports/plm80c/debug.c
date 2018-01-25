#include "plm.h"
#include <stdio.h>

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
     for (p = botInfo + (infoMode == 0  ? 2 : procInfo[1]); p < topInfo; p += InfoP(p)->len)
         showInfo(p);

}
void showInfo(offset_t off)
{
    info_t *info;
    sym_t *sym;
    lit_t *lit;
    pstr_t *ps;

    info = InfoP(off);
    printf("len = %d", info->len);
    switch (info->type) {
    case 0: printf(", LIT"); break;
    case 1: printf(", LABEL"); break;
    case 2: printf(", BYTE"); break;
    case 3: printf(", ADDRESS"); break;
    case 4: printf(", STRUCT"); break;
    case 5: printf(", PROC"); break;
    case 6: printf(", BUILTIN"); break;
    case 7: printf(", MACRO"); break;
    case 8: printf(", UNKNOWN"); break;
    case 9: printf(", TEMP"); break;
    default: printf(", OOPS type = %d\n", info->type);
    }
    switch (symMode) {
    case 0: printf(", sym = %04X", topInfo - info->sym); break;
    case 1: sym = SymbolP(topSymbol - info->sym);
            printf(", sym = %.*s", sym->name[0], &sym->name[1]);
            break;
    case 2: ps = PstrP(topSymbol - 1 - info->sym);
            printf(", sym = %.*s", ps->len, ps->str);
            break;
    }
    printf(", scope = %04X", info->scope);
    printf(", ilink = %04X", info->ilink);
    switch (info->type) {
    case LIT_T:
        lit = LitP(off);
        if (symMode == 0)
            printf(", lit = %04X", lit->litAddr);
        else {
            ps = PstrP(lit->litAddr + 1);
            printf(", lit = %.*s", ps->len, ps->str);
        }
        break;
    case LABEL_T: case BYTE_T: case ADDRESS_T: case STRUCT_T:
    case PROC_T:
        printf(", flags =  %02X %02X %02X", info->flag[0], info->flag[1], info->flag[2]);
        break;
    case BUILTIN_T:
        printf(", builtin = %s, paramCnt = %d, data type = %d", builtinStr[info->flag[0]], info->flag[1], info->flag[2]);
        break;
    case MACRO_T:
        printf(", flags =  %02X %02X", info->flag[0], info->flag[1]);
        break;
    case TEMP_T:
        printf(", flag =  %02X", info->flag[0]);
        break;
    case UNK_T:
        break;
    }
    if (LABEL_T <= info->type && info->type <= PROC_T) {
        printf(", extId = %d", info->extId);
        if (info->type >= BYTE_T) {
            printf(", dim = %d", info->dim);
            printf(", baseoff = %04X", info->baseoff);
            printf(", parent = %04X", info->parent);
        }
    }
    if (info->type == PROC_T) {
        printf(", dtype = %d", info->dtype);
        printf(", intno = %d", info->intno);
        printf(", pcnt = %d", info->pcnt);
        printf(", procId = %d", info->procId);
    }
    putchar('\n');

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
    "T2_15", "T2_16", "T2_17", "T2_DOUBLE", "T2_ADDB",
    "T2_MINUSSIGN", "T2_STAR", "T2_SLASH", "T2_MOD", "T2_AND",
    "T2_OR", "T2_XOR", "T2_BASED", "T2_BYTEINDEX", "T2_WORDINDEX",
    "T2_MEMBER", "T2_UNARYMINUS", "T2_NOT", "T2_LOW", "T2_HIGH",
    "T2_ADDRESSOF", "T2_PLUS", "T2_MINUS", "T2_38", "T2_39",
    "T2_40", "T2_41", "T2_42", "T2_43", "T2_44",
    "T2_45", "T2_46", "T2_47", "T2_48", "T2_49",
    "T2_50", "T2_51", "T2_52", "T2_53", "T2_54",
    "T2_55", "T2_56", "T2_TIME", "T2_STKBARG", "T2_STKWARG",
    "T2_DEC", "T2_STORE", "T2_OUTPUT", "T2_63", "T2_STKARG",
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
    "T2_170", "T2_171", "T2_VARIABLE", "T2_NUMBER", "T2_BIGNUMBER",
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