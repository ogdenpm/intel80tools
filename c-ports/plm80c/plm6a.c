#include "plm.h"

// lifted to file scope
static word itemArgs[4];


static void UpdateLineInfo()
{
    word i;
    if (itemArgs[1] > 0 || itemArgs[2] == 0 )
        itemArgs[3] = itemArgs[0];
    else {
        itemArgs[3] = itemArgs[2];
        itemArgs[2] = 0;
    }
    for (i = itemArgs[0]; i <= itemArgs[3]; i++) {
        EmitLinePrefix();
        lineCnt = i;
        stmtCnt = itemArgs[1];
        blkCnt = itemArgs[2];
        GetSourceLine();
    }
}

static void SyntaxError_6()
{
    errData.num = itemArgs[0];
    errData.info = 0;
    errData.stmt = stmtNo;
    EmitError();
}


static void TokenError_6()
{
    errData.num = itemArgs[0];
    errData.info = itemArgs[1];
    errData.stmt = stmtNo;
    EmitError();
}


static void Error_6()
{
    errData.num = itemArgs[0];
    errData.info = itemArgs[1];
    errData.stmt = itemArgs[2];
    EmitError();
}


void MiscControl() // was also in plm4a.c
{
    byte name[19];

    switch (cfCode) {
    case T2_LIST: listOff = false; break;
    case T2_NOLIST: listOff = true; break;
    case T2_CODE: codeOn = PRINT; break;
    case T2_NOCODE: codeOn = false; break;
    case T2_EJECT:
            if (listing )
                NewPageNextChLst();
            break;
    case T2_INCLUDE:
            EmitLinePrefix();
            TellF(&srcFil, (loc_t *)&srcFileTable[srcFileIdx + 8]);
            Backup((loc_t *)&srcFileTable[srcFileIdx + 8], offLastCh - offCurCh);
            srcFileIdx = srcFileIdx + 10;
            Fread(&tx2File, &name[13], 6);	/* Read() in name of include file */
            Fread(&tx2File, &name[6], 7);
            Fread(&tx2File, &name[0], 7);	/* overwrites the type byte */
            memmove((pointer)&srcFileTable[srcFileIdx], name, 16);
            CloseF(&srcFil);
            InitF(&srcFil, "SOURCE", &name[1]);
            OpenF(&srcFil, 1);
            offCurCh = offLastCh;	/* force Read() next Time	*/
            break;
    }
}


void Sub_42E7()
{

	itemArgs[0] = itemArgs[1] = itemArgs[2] = 0;
	Fread(&tx2File, &cfCode, 1);

	if (cfCode != T2_INCLUDE )
		Fread(&tx2File, (pointer)itemArgs, (b5124[cfCode] & 3) * 2);
	if (cfCode == T2_LINEINFO)
		UpdateLineInfo();
	else if (cfCode == T2_STMTCNT)
		stmtNo = itemArgs[0];
	else if (cfCode == T2_SYNTAXERROR)
		SyntaxError_6();
	else if (cfCode == T2_TOKENERROR)
		TokenError_6();
	else if (cfCode == T2_ERROR)
		Error_6();
	else if (T2_LIST <= cfCode && cfCode <= T2_INCLUDE)
		MiscControl();
	else if (cfCode == T2_EOF)
		b7AE4 = false;
}
