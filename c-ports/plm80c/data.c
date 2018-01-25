#include "plm.h"

offset_t topMem;
offset_t botMem;
offset_t botInfo;
offset_t topInfo;
offset_t topSymbol;
offset_t botSymbol;
offset_t curSymbolP;
offset_t curInfoP;
word offNxtCmdChM1;
word LEFTMARGIN;
word localLabelCnt;
word srcFileIdx;
offset_t hashChainsP; // offset is to pointer to array of offsets
word blockDepth;
offset_t localLabelsP;
offset_t w381E;
offset_t helpersP;
offset_t w3822;
word linesRead;
word programErrCnt;
word procCnt;
word w382A = 0;
word dsegSize = 0;
word csegSize = 0;
word objBlk;
word objByte;
word srcFileTable[60];
file_t srcFil;
file_t lstFil;
file_t objFile;
file_t conFile;
file_t tx1File;
file_t tx2File;
file_t atFile;
file_t nmsFile;
file_t xrfFile;
file_t ixiFile;
word procChains[35];
word procInfo[255];
word blk1Used = 400;
word blk2Used = 400;
offset_t w3C34 = 0x9F00;
word blkSize1 = 0xC400;
word blkSize2 = 0xA400;
byte srcStemLen;
bool standAlone = true;
bool IXREFSet = true;
bool PRINTSet = true;
bool OBJECTSet = true;
bool debugFlag = false;
bool unexpectedEOF = false;
bool haveModule = false;
byte fatalErrorCode = 0;
byte pad3C43 = 1;
offset_t w3C44 = 0xA000;
byte CONTROLS[8];
byte pad_3C4E[2];
byte srcStemName[10];
bool debugSwitches[26];
offset_t cmdLineP;
offset_t startCmdLineP;
byte overlay[7][FILE_NAME_LEN] = { ":F0:PLM80 .OV0 ", ":F0:PLM80 .OV1 ", ":F0:PLM80 .OV2 ", ":F0:PLM80 .OV3 ",
								   ":F0:PLM80 .OV4 ", ":F0:PLM80 .OV5 ", ":F0:PLM80 .OV6 "};
byte ixiFileName[FILE_NAME_LEN];
byte lstFileName[FILE_NAME_LEN];
byte objFileName[FILE_NAME_LEN];
word pageNo = 0;
byte b3CF2;
pointer lBufP = &b3CF2;	
word lChCnt = 0;
word lBufSz = 0;
bool lfOpen = false;
byte linLft = 0;
byte b3CFB, b3CFC, b3CFD;
byte col = 0;
byte b3CFF = 0;
byte tWidth = 0;
byte TITLELEN = 1;
byte PAGELEN = 60;
byte PWIDTH = 120;
byte margin = 0xFF;
byte DATE[9];
byte plm80Compiler[] = "PL/M-80 COMPILER    ";
byte TITLE[60] = " ";
//word ISIS = 0x40;
word REBOOTVECTOR = 0;

byte tblOffsets[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			        1, 1, 1, 1, 1, 1, 1, 1, 2, 2};
byte tblBitFlags[] = {0x80, 0x40, 0x20, 0x10, 0x10, 8, 8, 4, 2, 1,
				 0x80, 0x40, 0x20, 0x10, 8, 4, 2, 1, 0x80, 0x40};

byte intVecNum = 8;
word intVecLoc = 0;
bool hasErrors = false;
//byte overlay6[]  = ":F0:PLM80 ";
//byte ov6[] = ".OV6 ";
byte version[] = "X000";
//byte pad3DA1;
//byte invokeName[] = ":F0:PLM80 ";
//byte ov0[] =  ".OV0 ";

