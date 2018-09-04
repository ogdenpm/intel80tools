#pragma once
#include "zip.h"

typedef unsigned char byte;
typedef unsigned short word;
#define MAXTRACKS       80
#define MAXSECTORS      52
#define SECTORSIZE      128
#define MAXTRACKSIZE    10417       // max data based on 10417 bytes per unformatted track


enum {
    END_BLOCK = -1,
    END_FLUX = -2,
 };

enum {
    HASID = 1,
    HASDATA = 2
};

typedef struct _sector {
    struct _sector *next;
    int offset;                         // offset to start of sector in bytes from index mark
    byte id;                            // the sector id
    byte *buf;                          // pointer to the sector data or null if not present
} sector_t;

typedef struct {
    byte fmt;
    byte spt;
    int size;                            // sector size
    byte smap[MAXSECTORS];               // physical slot to sector map
    bool hasData[MAXSECTORS];            // in physical slot order
    byte track[MAXTRACKSIZE]; // in physical slot order
} imd_t;


enum {      // bit combinations returned by nextBits
    BIT0,
    BIT0M,  // special 0 bit used in marker
    BIT1,
    BIT1S,  // special to note bit 1 on which resync happend
    BIT00,
    BIT01,
    BITEND, // no more bits
    BITBAD,  // illegal flux pattern

    BITSTART,   // used to signal start of bitstream for bitLogging
    BITFLUSH,    // used to flush bitstream for bitLogging
    BITFLUSH_1   // variant that flushes all but last bit (used for marker alignment)

};


enum {      // disk format types
    UNKNOWN_FMT,
    FM,
    MFM,
    M2FM
};

#define MINSAMPLE   20000
#define SAMPLESCALER 500

extern int debug;
extern bool showSectorMap;

extern char curFile[];

#define LINELEN 80
enum {
    ALWAYS = 0, MINIMAL, VERBOSE, VERYVERBOSE
};

int nextBitsM2FM();
size_t seekBlock(int blk);
void resetFlux();
void freeMem();
size_t where();
long when(int val);
void *xalloc(void *buf, size_t size);
size_t readFluxBuffer(byte *buf, size_t bufsize);

int x8ByteTime();
void addIMD(int track, imd_t *trackPtr);
void WriteImgFile(char *fname, char *comment);
_declspec(noreturn) void error(char *fmt, ...);
void logger(int level, char *fmt, ...);
void displayHist(int levels);
void resetIMD();
void flux2track();
int getNextFlux();

int bitLog(int bits);
int guessFormat();