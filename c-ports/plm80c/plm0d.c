#include "plm.h"

static void GetSrcLine()
{
    WriteLineInfo();
    inChrP = lineBuf;
    InGetC();
    if (*inChrP == ISISEOF)	/* eof */
        return;
    trunc = false;
    RSrcLn();
    *inChrP = '\r';
    inChrP = inChrP + 1;
    *inChrP = '\n';
    inChrP = lineBuf;
    linfo.lineCnt = linfo.lineCnt + 1;
    linfo.stmtCnt = linfo.blkCnt = 0;
    lineInfoToWrite = true;
}




static void GetCodeLine()
{
    pointer startOfLine;

    while (1) {
        GetSrcLine();
        if (*inChrP != ISISEOF) {
            startOfLine = inChrP + LEFTMARGIN - 1;
            while (inChrP < startOfLine) {
                if (*inChrP == '\r')
                    return;
                inChrP = inChrP + 1;
            }
            if (skippingCOND)
                DoCondCompile(inChrP);
            else if (*inChrP == '$') {
                WrByte(L_STMTCNT);
                WrWord(0);
                if (trunc) {
                    SyntaxError(ERR86);	/* LIMIT EXCEEDED: SOURCE LINE length() */
                    trunc = false;
                }
                DoControl(inChrP);
            } else {
                isNonCtrlLine = true;
                return;
            }
        }
        else if (srcFileIdx == 0) {
            if (ifDepth != 0)
                SyntaxError(ERR188);	/* MISPLACED RESTORE OPTION */
            inChrP = "/*'/**/EOF   ";
            return;
        } else {
            CloseF(&srcFil);
            srcFileIdx = srcFileIdx - 10;
            InitF(&srcFil, "SOURCE", (pointer)&srcFileTable[srcFileIdx]);
            OpenF(&srcFil, 1);
            SeekF(&srcFil, (loc_t *)&srcFileTable[srcFileIdx + 8]);
            ocurch = olstch;
        }
    }
}

void GetLin()
{
	word tmp;

	if (macroDepth != 0) {
		tmp = curInfoP;
		curInfoP = curMacroInfoP;
		SetType(0);
		curMacroInfoP = ptr2Off(macroPtrs[macroDepth + 1]);
		inChrP = macroPtrs[macroDepth];
		macroDepth = macroDepth - 2;
		curInfoP = tmp;
	}
	else
		GetCodeLine();
}

