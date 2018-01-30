#include "plm.h"
static byte copyright[] = "[C] 1976, 1977, 1982 INTEL CORP";

jmp_buf exception;

static void Sub_3EDF()
{
	if (unexpectedEOF)
		SyntaxError(ERR87);	/* MISSING 'end' , end-OF-FILE ENCOUNTERED */
	WriteLineInfo();
	WrByte(L_EOF);
	Sub_4119();
	TellF(&srcFil, (loc_t *)&srcFileTable[srcFileIdx + 8]);
	Backup((loc_t *)&srcFileTable[srcFileIdx + 8], offLastCh - offCurCh);
	CloseF(&srcFil);
} /* Sub_3EDF() */


static void Sub_3F23()
{
	InitF(&srcFil, "SOURCE", (pointer)&srcFileTable[srcFileIdx]);
	OpenF(&srcFil, 1);
	SeekF(&srcFil,  (loc_t *)&srcFileTable[srcFileIdx + 8]);
	offCurCh = offLastCh;
	if (offNxtCmdChM1 != 0)
		while (cmdLineP != 0) {
			DoControl(offNxtCmdChM1 + ByteP(cmdLineP));
			offNxtCmdChM1 = 2;
			cmdLineP = CmdP(cmdLineP)->link;
		}
	offNxtCmdChM1 = 0;
	curProcData = &procData.w;
	inChrP = "\n" - 1;
	blockDepth = 1;
	procChains[1] = 0;
	GNxtCh();
} /* Sub_3F23() */

static void Sub_3FAD()
{
	Sub_3F23();
	Sub_6F00();
} /* Sub_3FAD() */


word Start0()
{
	if (setjmp(exception) == 0) {
		state = 20;	/* 9B46 */
		Sub_3FAD();
	}
	Sub_3EDF();		/* exception goes to here */
    return 1; // Chain(overlay[1]);
}
