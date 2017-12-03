#include "isis2.h"

void Open(address connP, address pathP, address access, address echo, address statusP)
{
    ISIS(IOPEN, &connP);
} /* Open */

void Read(address conn, address buffP, address count, address actualP, address statusP)
{
    ISIS(IREAD, &conn);
} /* Read */

void Rename(address oldP, address newP, address statusP)
{
    ISIS(IRENAME, &oldP);
} /* Rename */
void Seek(address conn, address mode, address blockP, address byteP, address statusP)
{
    ISIS(ISEEK, &conn);
} /* Seek */


void Write(address conn, address buffP, address count, address statusP)
{
    ISIS(IWRITE, &conn);
} /* Write */


