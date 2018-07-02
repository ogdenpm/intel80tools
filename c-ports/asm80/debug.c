#include "asm80.h"

static char *opNames[] = {
    "BEGIN", "\\r", "(", ")", "*", "+", ",", "-", "unary +", "/", "unary -",
    "EQ", "LT", "LE", "GT", "EE", "NE", "NOT", "AND", "OR", "XOR",
    "MOD", "SHL", "SHR", "HIGH", "LOW", "DB", "DW", "DS", "EQU", "SET",
    "ORG", "END", "IF", "ELSE", "ENDIF", "LXI", "REG16", "LDSTAX", "ARITH", "IMM8",
    "MVI", "INRDCR", "MOV", "IMM16", "SINGLE", "RST", "ASEG", "CSEG", "DSEG",
    "PUBLIC", "EXTRN", "NAME", "STKLN", "MACRO", "MACROBODY", "ENDM", "EXITM",
    "MACRONAME", "IRP", "IRPC", "ITERPARAM", "REPT", "LOCAL", "OPTVAL", "NUL"
};


void ShowYYType() {
    if (yyType < sizeof(opNames) / sizeof(*opNames))
        printf(" %i:%s\n", yyType, opNames[yyType]);
    else
        printf(" %i:%02X\n", yyType, yyType);
}
void DumpSymbols(byte tableId)
{
    char token[7];

    tokensym_t *s, *e;
    s = symTab[tableId];
    e = endSymTab[tableId];
    if (s == e)
        printf("no data for symtab[%d]\n", tableId);
    else {
        printf("symtab[%d]\n", tableId);
        while (s < e) {
            UnpackToken(s->tok, token);
            token[6] = 0;
            printf("tok = \"%s\", line/val = %04X, type = %02X, flags = %02X\n", token, s->addr, s->type, s->flags);

            s++;
        }
    }
}

void DumpOpStack()
{
    printf("OpStack:");
    for (int i = 0; i <= opSP; i++) {
        if (opStack[i] < sizeof(opNames) / sizeof(*opNames))
            printf(" %i:%s", i, opNames[opStack[i]]);
        else
            printf(" %i:%02X", i, opStack[i]);
    }
    printf("\n");
}

void DumpTokenStackItem(int i, bool pop)
{
    char token[7];
    tokensym_t *s;

    if (i == 0 && pop)
        printf("^");
    else
        printf("%1d", pop ? i - 1 : i);

    if (tokenSize[i] == 4) {
        UnpackToken((wpointer)tokStart[i], token);
        printf(" %6.6s", token);
    }
    else if (tokenSize[i] == 1)
        printf(" %6d", *(pointer)tokStart[i]);
    else if (tokenSize[i] == 2)
        printf(" %6d", *(wpointer)tokStart[i]);
    else
        printf(" %6.*s", tokenSize[i], tokStart[i]);
    printf("  %02X   %02X  %3d  %3d", tokenType[i], tokenAttr[i], tokenSize[i], tokenSymId[i]);
    s = tokenSym.stack[i];
    if (s /*&& (symTab[TID_SYMBOL] <= s && s <= endSymTab[TID_SYMBOL] || symTab[TID_MACRO] <= s && s <= endSymTab[TID_MACRO]) */) {
        UnpackToken(s->tok, token);
        printf("   %6.6s %04X   %02X   %02X\n", token, s->addr, s->type, s->flags);
    }
    else
        printf("\n");
}
void DumpTokenStack(bool pop)
{
    char token[7];
    token[6] = 0;

    printf("TokenStack:\n");
    printf("  Token  Type Attr Size  Id | Sym    Addr  Type Flags\n");
    DumpTokenStackItem(0, pop);
    for (int i = tokenIdx; i > 0; i--)
        DumpTokenStackItem(i, pop);

}