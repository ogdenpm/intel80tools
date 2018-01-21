#include "link.h"

bool Strequ(pointer s, pointer t, byte cnt)
{
	int i;

	for (i = 0; i < cnt; i++) {
		if (*s++ != *t++ )
			return false;
	}
	return true;
} /* Strequ() */
