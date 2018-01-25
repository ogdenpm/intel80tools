#include "plm.h"
static byte copyRight[] = "[C] 1976, 1977, 1982 INTEL CORP";

static byte stateMain;

// FatalError is shared between passes
byte currentModule = 0xff;		// 0xff for main 0-6 for each overlay


void FatalError_main(byte err)
{
    if (err == ERR83)
        Fatal("DYNAMIC STORAGE OVERFLOW", 24);
    Fatal("UNKNOWN FATAL ERROR", 19);
    Exit();
}

word Start()
{
	stateMain = 10;
	Sub_40AC();
	stateMain = 15;
	Sub_4767();
    return 0;   // Chain(invokeName);
}
