#include "plm.h"
byte Strncmp(pointer s1, pointer s2, byte cnt)
{
	while (cnt != 0) {
		if (*s1 < *s2 )
			return 1;
		if (*s1 > *s2 )
			return 2;
		cnt = cnt - 1;
		s1++; s2++;
	}
	return 0;
} /* Strncmp() */
