#include "asm80.h"

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
	for (int i = 1; i <= opSP; i++)
		printf(" %i:%02X", i, opStack[i]);
	printf("\n");
}

void DumpTokenStack()
{
	char token[7];
	token[6] = 0;

	tokensym_t *s;
	printf("TokenStack:\n");
	for (int i = 0; i <= tokenIdx; i++) {
		if (tokenSize[i] == 4) {
			UnpackToken((wpointer)tokStart[i], token);
			printf("%2d: start = %s", i, token);
		} else
			printf("%2d: Start = %02X", i, (unsigned)(tokStart[i] - lineBuf));
		s = tokenSym.stack[i];
		if (s && (symTab[TID_SYMBOL] <= s && s <= endSymTab[TID_SYMBOL] || symTab[TID_MACRO] <= s && s <= endSymTab[TID_MACRO])) {
				UnpackToken(s->tok, token);
				token[6] = 0;
				printf(" Sym (\"%s\" %04X %02X %02X)", token, s->addr, s->type, s->flags);
			}
			else
				printf(" Sym (%p)", s);
		printf(" Type = %02X, Attr = %02X, Size=%d, Id=%d\n", tokenType[i], tokenAttr[i], tokenSize[i], tokenSymId[i]);
	}
}