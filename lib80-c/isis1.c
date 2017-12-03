#include "isis1.h"

void Close(address conn, address statusP)
{
	ISIS(ICLOSE, &conn);
} /* Close */

void Delete(address pathP, address statusP)
{
    ISIS(IDELETE, &pathP);
} /* Delete */

void Error(address errorNum)
{
    address junk;
    junk = &junk;
    ISIS(IERROR, &ErrorNum);
} /* Error */

void Exit()
{
    address tmp;
    tmp = &tmp;
    ISIS(IEXIT, &tmp);
} /* Exit */


