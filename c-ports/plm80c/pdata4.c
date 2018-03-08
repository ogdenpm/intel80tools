#include "plm.h"

//byte helperModId; made into local var
//byte endHelperId;
bool listing;
bool listOff;
bool codeOn;
byte locLabStr[32];		/* used to hold symbol name */
err_t errData;
bool bo812B = true;
word baseAddr;
byte cfCode;
word w812F;
word stmtCnt;
word blkCnt;
word stmtNo;
//word offLastCh = 0;  already defined in plm0a.c
//word offCurCh = 0;  already defined in plm0a.c
bool linePrefixChecked = true;
bool linePrefixEmitted = true;
byte lstLineLen;
byte lstLine[130];
byte rec6_4[1024] = {6, 0, 0, 1};
byte rec22[1022] = {0x22, 0, 0, 3};
byte rec24_1[1022] = {0x24, 0, 0, 1, 3};    // initialise for ov3 (code seg), main4.c updates to 0x24 0, 0, 2, 3
// byte rec24_2[104] = {0x24, 0, 0, 3, 3};  // use the larger array from plm3a.c
// byte rec24_3[104] = {0x24, 0, 0, 4, 3};  // use the larger array from plm3a.c
byte rec20[1022] = {0x20, 0, 0, 3};
byte rec8[1024] = {8, 0, 0, 1};
byte rec4[sizeof(rec4_t)] = {4, 4, 0, 0, 1};
byte b9692;
byte helperId;
byte helperStr[] = "\0@P    :";
byte b969C;
byte b969D;
pointer w969E;
word wValAry[4];
pointer sValAry[4];
byte b96B0[38];
byte b96D6;
word w96D7;
byte curExtId;
byte commentStr[41] = "\0; ";
byte line[81];  // pstr
byte opByteCnt;
byte opBytes[3];
byte dstRec;
//byte srcbuf[640]; use larger version in pdata6.c
//byte tx1buf[640]; use larger version in plm0a.c
//byte objbuf[640]; use larger buff in plm3a.c
//byte lstbuf[640]; use larger version in main5.c
