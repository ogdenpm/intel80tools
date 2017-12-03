#include <stdbool.h>
typedef unsigned char byte;
typedef unsigned short address;
typedef byte *pointer;

#define	ICLOSE	1
#define	IDELETE	2
#define	IERROR	12
#define	IEXIT	9
void Isis(byte type, address parameterPtr);
