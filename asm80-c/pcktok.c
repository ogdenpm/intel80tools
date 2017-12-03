#include "asm80.h"

/* packToken - packs the token pointed by tokStart, with length toksiz into 4 bytes
 packed version replaces original and toksize set to 4 bytes
*/
static byte Pack1(byte i)
{
	return  i >= tokenSize[0] ? 0 : tokStart[0][i] < 0x3f ? tokStart[0][i] - 0x2f : tokStart[0][i] - 0x34;
}

void PackToken()
{
	*(wpointer)tokPtr = (Pack1(0) * 40 + Pack1(1)) * 40 + Pack1(2);
	*(wpointer)(tokPtr + 2) = (Pack1(3) * 40 + Pack1(4)) * 40 + Pack1(5);
	tokenSize[0] = 4;
}
