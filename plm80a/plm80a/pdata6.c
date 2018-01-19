#include "plm.h"
byte b7AD9;
byte b7ADA;
byte listing;
byte listOff;
byte codeOn;
word errNum;
word w7AE0;
word STMTNum;
bool b7AE4 = true;
word w7AE5;
word lineNo;
word w7AE9;
word w7AEB;
word offLastCh = 0;
word offCurCh = 0;
byte b7AF1 = true;
byte b7AF2 = true;
byte lstLineLen;
byte lstbuf[130];
byte srcbuf[2048];

