#include "plm.h"

void Backup(loc_t *locP, word cnt)
{ /* Backup() the block/byte pos by cnt bytes */

    word byt = cnt % 128;

    locP->blk -= cnt / 128;
    if (byt > locP->byt) {
        locP->blk--;
        locP->byt += 128 - byt;
    } else
        locP->byt -= byt;
}

