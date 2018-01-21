#include "plm.h"

static byte b4222[] = {
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0xE, 1, 1, 1, 1,
	1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 2, 1, 1, 1,
	1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 6, 1, 0, 0, 0,
	0, 3, 0, 9, 9, 8, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 9, 0xB, 0xB, 0x81,
	0x99, 0x91, 0x91, 0x91, 0x81, 0x89, 0x89, 0x89, 0x8A, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x90, 0xA1, 0xA3, 0xA0, 0xA1, 0xA2,
	0xB0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA3, 0xA3, 0,
	0, 0, 0, 0, 0, 0, 0, 0x41, 0x41, 0x41, 0x40, 0, 0, 0, 0,
	0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xD8, 0x41, 0x41, 0x41,
	0xC0};

// lifted to file scope
static byte cfCode;
static word itemArgs[4];


static void Sub_4396()
{
    word i;
    if (itemArgs[1] > 0 || itemArgs[2] == 0 )
        itemArgs[3] = itemArgs[0];
    else {
        itemArgs[3] = itemArgs[2];
        itemArgs[2] = 0;
    }
    for (i = itemArgs[0]; i <= itemArgs[3]; i++) {
        Sub_6550();
        w7AE5 = i;
        lineNo = itemArgs[1];
        w7AE9 = itemArgs[2];
        GetSourceLine_6();
    }
}

static void Sub_4400()
{
    errNum = itemArgs[0];
    w7AE0 = 0;
    STMTNum = w7AEB;
    EmitError();
}


static void Sub_4416()
{
    errNum = itemArgs[0];
    w7AE0 = itemArgs[1];
    STMTNum = w7AEB;
    EmitError();
}


static void Sub_442C()
{
    errNum = itemArgs[0];
    w7AE0 = itemArgs[1];
    STMTNum = itemArgs[2];
    EmitError();
}


static void Sub_4442()
{
    byte name[19];

    switch (cfCode - 0x9d) {
    case 0: listOff = false; break;
    case 1: listOff = true; break;
    case 2: codeOn = PRINT; break;
    case 3: codeOn = false; break;
    case 4: if (listing )
                NewPageNextChLst();
            break;
    case 5:
            Sub_6550();
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
	if (cfCode != 0xa2 )
		Fread(&tx2File, (pointer)itemArgs, Shl(b4222[cfCode] & 3,1));
	if (cfCode == 0x98 )
		Sub_4396();
	else if (cfCode == 0x97 )
		w7AEB = itemArgs[0];
	else if (cfCode == 0x9A )
		Sub_4400();
	else if (cfCode == 0x9B )
		Sub_4416();
	else if (cfCode == 0xA3 )
		Sub_442C();
	else if (0x9D <= cfCode && cfCode <= 0xA2 )
		Sub_4442();
	else if (cfCode == 0x9C )
		b7AE4 = false;
}
