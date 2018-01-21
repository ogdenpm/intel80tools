#include "plm.h"

static byte copyright[] = "(C) 1976, 1977, 1982 INTEL CORP";
jmp_buf cleanup;

word markedStSP;
word t2CntForStmt;
word curProcInfoP = {0};
word curStmtNum;
word varInfoOffset, varArrayIndex, varNestedArrayIndex, varVal;
byte regetTx1Item = false;
byte b88B3 = false;
linfo_t linfo;
tx1item_t tx1Item;
byte tx1Aux2;
byte tx1Aux1;
byte tx1RdBuf[512];
byte tx2Buf[512];
byte xrfBuf[512];
// byte atBuf[512]; use version in plm3a.c
byte b91C0;


static void Sub_3F19()
{
    CreatF(&tx1File, tx1RdBuf, 512, 1);
    OpenF(&tx2File, 3);
    CreatF(&tx2File, tx2Buf, 512, 2);
    if (XREF || IXREF || SYMBOLS)
    {
        OpenF(&xrfFile, 2);
        CreatF(&xrfFile, xrfBuf, 512, 2);
    }
    OpenF(&atFile, 3);
    CreatF(&atFile, atBuf, 16, 2);
    blockDepth = 1;
    procChains[1] = 0;
}

static void Sub_3F8B()
{
    if (b88B3) 
        WrTx2File((pointer)&linfo, 7);
    CloseF(&tx1File);
    DeletF(&tx1File);
    OpenF(&tx1File, 3);
    if (XREF || IXREF || SYMBOLS)
    {
        Fwrite(&xrfFile, "\0", 1);
        Fflush(&xrfFile);
        CloseF(&xrfFile);
    }
    WrAtFileByte(ATI_END);
    WrAtFileByte(ATI_EOF);
    Fflush(&atFile);
    Rewind(&atFile);
}

word Start1()
{

	if (setjmp(cleanup) == 0) {
		Sub_3F19();	/* create files and preload tx1 */
		Sub_6523();
	} else {
		/* here longjmp(cleanup, -1) */
		WrTx2ExtError(b91C0);
		while (tx1Item.type != L_EOF) {
			if (tx1Item.type == L_STMTCNT)
			{
				t2CntForStmt = 0;
				MapLToT2();
				curStmtNum = tx1Item.dataw[0];
			}
			GetTx1Item();
		}
	}
	Sub_3F8B();
	Sub_6EE0();

    if (hasErrors)
        return 6; // Chain(overlay[6]);
    else
        return 2; // Chain(overlay[2]);
}
