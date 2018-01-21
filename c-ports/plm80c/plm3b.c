#include "plm.h"

void WriteRec(pointer recP, byte arg2b)
{
    word p;
    wpointer lenP;
    byte crc;
    word cnt;

    lenP = &((rec_t *)recP)->len;
	if (*lenP > 0 && OBJECT ) {
        crc = 0;
        p = 0;
        *lenP = *lenP + arg2b + 1;
        cnt = *lenP + 2;
        while (p < cnt) {
            crc = crc - recP[p];
            p = p + 1;
        }
        recP[cnt] = crc;	/* insert checksum */
        Fwrite(&objFile, recP, cnt + 1);
	}
	*lenP = 0;
}



void RecAddByte(pointer recP, byte arg2b, byte arg3b)
{
    wpointer lenP;

	lenP = &((rec_t *)recP)->len;
	((rec_t *)recP)->val[*lenP + arg2b] = arg3b;
	*lenP = *lenP + 1;
}



void RecAddWord(pointer arg1w, byte arg2b, word arg3w)
{
	RecAddByte(arg1w, arg2b, Low(arg3w));
	RecAddByte(arg1w, arg2b, High(arg3w));
}
