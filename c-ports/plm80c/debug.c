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