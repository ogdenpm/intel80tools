#include "plm.h"

static void InstallBuiltins()
{
    pointer p;

    p = builtins;
    while (*p != 0) {
        Lookup(p);
        CreateInfo(0, BUILTIN_T);
        SetBuiltinId(p[*p + 1]);
        SetParamCnt(p[*p + 2]);
        SetDataType(p[*p + 3]);
        p += *p + 4;
    }	
    Lookup("\x6MEMORY");
    CreateInfo(0, BYTE_T);
    SetInfoFlag(F_LABEL);
    SetInfoFlag(F_MEMORY);
    SetInfoFlag(F_ARRAY);
} /* InstallBuiltins() */


static void InstallKeywords()
{
    pointer p;

    p = keywords;
    while (*p != 0) {
        Lookup(p);
        SymbolP(curSymbolP)->infoP = 0xFF00 | p[*p + 1];
        p += *p + 2;
    }
} /* InstallKeywords() */


static void Sub_4845()
{
    word i;

    if (w3C44 > w3C34 )
        botMem = w3C44;
    else
        botMem = w3C34;
    botMem = botMem + 256;
    botSymbol = (topSymbol = (hashChainsP = (topMem + 1 - 64 * sizeof(offset_t))) - 1) + 1;
    topInfo = (botInfo = botMem) + 1;
    for (i = 0; i <= 63; i++)
        WordP(hashChainsP)[i] = 0;
    SetPageNo(1);
    localLabelCnt = 0;
    w382A = w382A + 1;
    procChains[0] = procChains[1] = blockDepth = 0;
} /* Sub_4845() */

void Sub_4767()
{
	Sub_4845();
	InstallKeywords();
	InstallBuiltins();
} /* Sub_4767() */

