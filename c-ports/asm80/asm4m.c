#include "asm80.h"

static word pad1 = {0x40};
static word pad2;


bool StrUcEqu(pointer s, pointer t)
{
    while (*s != 0) {
        if (*s != *s && *s != (*t && 0x5F))
            return false;
        s++;
        t++;
    }
    return true;
}


bool IsSkipping()
{
    return (mSpoolMode & 1) || skipIf[0];
}

void Sub546F()
{
    spIdx = NxtTokI();
    if (expectingOperands)
        SyntaxError();
    if (HaveTokens())
        if (!(tokenType[spIdx] == O_DATA || lineNumberEmitted))
            SyntaxError();
    if (inDB || inDW)
    {
        if (tokenIdx == 1 && ! BlankAsmErrCode() && tokenSize[0] != 1)
            tokenSize[0] = 2;
    }
    else if (! BlankAsmErrCode() && HaveTokens())
        if (tokenSize[spIdx] > 3)
            tokenSize[spIdx] = 3;
}


void FinishLine()
{
	pointer linenoP;
	bool updating;

    Sub546F();
    if (IsPhase2Print()) {    /* update the ascii line number */
        linenoP = &asciiLineNo[3];    /* point to last digit */
        updating = true;

        while (updating) {        /* adjust the line number */
            if (*linenoP == '9')    /* if 9 then roll over to 0 */
                *linenoP = '0';
            else {
                if (*linenoP == ' ')    /* new digit */
					*linenoP = '1';
                else            /* just increment */
					++*linenoP;
                updating = false;
            }
            linenoP--;
        }

        if (ShowLine() || ! BlankAsmErrCode())
            PrintLine();
    }

    if (skipRuntimeError) {
		outP++;
        Flushout();
        Exit();
    }

    if (! isControlLine)
    {
        ii = 2;
        if (tokenIdx < 2 || inDB || inDW)
            ii = 0;

        w6BCE = tokStart[ii] + tokenSize[ii];
        if (IsSkipping() || !isInstr)
            w6BCE = lineBuf;

        if (ChkGenObj())
            Ovl8();
        b6B2C = true;
        segSize[activeSeg] = effectiveAddr.w = (word)(segSize[activeSeg] + (w6BCE - lineBuf));
    }

    if (controls.xref && rhsUserSymbol)
        if (phase == 1)
            EmitXref(XREF_REF, name);

    FlushM();

    while (tokenIdx > 0) {
        PopToken();
    }

    InitLine();
    if (b6B33)
    {
        finished = true;
        if (IsPhase2Print() && controls.symbols)
            Sub7041_8447();

        EmitXref(XREF_FIN, name);    /* finalise xref file */
        if (ChkGenObj())
            ReinitFixupRecs();
    }
}