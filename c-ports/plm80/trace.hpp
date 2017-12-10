// $Id: trace.hpp,v 1.1 2003/10/04 21:08:48 Mark Ogden Exp $
extern char *tx2Names[];
void traceTx1(void *buf, word len);
extern byte tx1ItemLengths[];
void traceRdTx1(byte tx1type, byte *buf, word len);
void traceTx2(word cnt, byte *buf, byte len);

void dumpInfo(info_pt p);
void dumpSym(symbol_pt p);
void dumpTx2Q(int n);
void dumpTx2QDetail(int n);
void dumpMem(char *file, word start, word end);

