#include <stdbool.h>
typedef unsigned char byte;
typedef unsigned short address;
typedef byte *pointer;

#define	IOPEN	0
#define	IREAD	3
#define	IRENAME	7
#define	ISEEK	5
#define	IWRITE	4
void Isis(byte type, address parameterPtr);
