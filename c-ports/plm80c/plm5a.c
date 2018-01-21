#include "plm.h"

static void Sub_557A()
{
    byte i, j, line[30];
    word actual;

	for (i = 0; i <= 25; i++) 
		debugSwitches[i] = false;

	if (! debugFlag)
		return;

	PrintStr("ENTER DEBUG SWITCHES\r\n*", 23);
	ReadF(&conFile, line, 30, &actual);
	if (actual != 0) {
		actual = actual - 1;
		for (i = 0; i <= actual; i++) {
			j = line[i];
			if ('A' <= j && j <= 'Z')
				debugSwitches[j - 'A'] = true;	
		}
	}
	PrintStr("BEGINNING PHASE EXECUTION\r\n", 27);
}


static byte Sub_561E(byte arg1b)
{
	if (debugSwitches[arg1b - 'A'])
		return true;
	else
		return false;
}
