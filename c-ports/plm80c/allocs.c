#include "plm.h"

offset_t AllocSymbol(word spc)
{
	offset_t tmp;

	if ((tmp = botSymbol - spc) < topInfo )
		FatalError(ERR83);
	return (botSymbol = tmp);
} /* AllocSymbol() */

