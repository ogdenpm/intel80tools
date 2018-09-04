#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "flux.h"

#define HIST_MAX  168

// private definitions
enum {
    OVL16 = -2, PAD = -3
};

#define FLUXCHUNKSIZE 400000UL        // default chunk size for flux stream
#define ARRAYCHUNKSIZE  20          // default chunk size for stream and index info
#define DEFAULT_8_BYTE_TIME 3075        // 64 bits sample count @ standard flux clock rate of 24027428.5714285Hz
#define ONEUS       24

/* private types for managing the data from the flux stream */
typedef struct _flux {
    struct _flux *link;
    unsigned blkId;
    int cnt;
    int flux[FLUXCHUNKSIZE];
} flux_t;


typedef struct {
    long streamPos;
    long sampleCnt;
    long indexCnt;
} index_t;

typedef struct {
    long streamPos;
    long transferTime;
} stream_t;


/*** input buffer management ***/
// varaibles used for input buffer management
static byte *inBuf;
static size_t inSize;
static size_t inIdx;

void initInput(byte *buf, size_t size) {
    inBuf = buf;
    inSize = size;
    inIdx = 0;
}
// get next byte from input
int get1() {
    if (inIdx < inSize)
        return inBuf[inIdx++];
    return EOF;
}

// read in 2 byte little edian word
int get2() {
    int cl;
    int ch;

    if ((cl = get1()) == EOF || (ch = get1()) == EOF)
        return EOF;
    return (ch << 8) + cl;
}

// read in 4 byte little edian word
long get4() {
    int cl;
    int ch;

    if ((cl = get2()) == EOF || (ch = get2()) == EOF)
        return EOF;
    return (ch << 16) + cl;
}

// check at least given number of bytes left

bool atLeast(int size) {
    return inIdx + size < inSize;
}

size_t inPos() {
    return inIdx;
}

// skip input bytes
void skip(int size) {       // skip forward size bytes or if size < 0 skip to end of input
    if (size >= 0)
        inIdx += size;
    else
        inIdx = inSize;
}

/*
    The core flux processing functions
    and data definitions
*/

static flux_t *fluxData;        // the list of flux data blocks
static flux_t *curBlk;          // current block being processed
static index_t *indexArray;     // the array of info block
static int indexArrayCnt;       // the count of index info records
static int indexArraySize;      // the current size of indexArray 
static stream_t *streamArray;   // stream info block equivalent of above
static int streamArrayCnt;
static int streamArraySize;


static size_t fluxRead;           // total number of flux transitions read
static size_t fluxPos;            // current flux transition

static long sampleTime;         // tracks the sample flux time since start of track
static size_t endTrack;         // flux index for end of track
static long byteX8Time = DEFAULT_8_BYTE_TIME;         //timing fo 8 bytes used to support variances in disk rotation

/*
    seek to pos in flux stream, updating fluxPos and curBlk as required
    returns true if ok else false
*/
static bool fluxSeek(size_t pos) {
    if (pos >= fluxRead)
        return false;
    fluxPos = pos;

    if (curBlk->blkId != fluxPos / FLUXCHUNKSIZE) {     // does curBlk need updating
        if (curBlk->blkId > fluxPos / FLUXCHUNKSIZE)    // if moving backwards start search from first block
            curBlk = fluxData;
        while (curBlk->blkId != fluxPos / FLUXCHUNKSIZE)    // find the correct block
            if (curBlk->link)
                curBlk = curBlk->link;
            else {
                logger(ALWAYS, "fluxSeek beyond end\n");
                return false;
            }
    }
    return true;
}

// seeks to the start of a full block and returns streamPos of next block
// or 0 if no full block or seek error
// if blk is -ve it sets up to read everything
size_t seekBlock(int blk) {
    sampleTime = 0;                         // reset sample time
    if (blk < 0 || blk >= indexArrayCnt - 1)
        return 0;
    if (!fluxSeek(indexArray[blk].streamPos))
        return 0;
    return endTrack = indexArray[blk + 1].streamPos;
}

/* utility function to return current position in flux stream */
size_t where() {
    return fluxPos;
}

/* utility function to return current sampleTime of last flux transition
   after adding in the current flux val or setting to zero if val -ve
*/
long when(int val) {
    if (val >= 0)
        return sampleTime += val;
    else
        return sampleTime = 0;
}

int x8ByteTime() {
    return byteX8Time;
}

// memory allocator with out of memory detection
void *xalloc(void *blk, size_t size) {
    void *p;
    if (blk == NULL) {          // no pre-existing memory
        if (p = malloc(size))
            return p;
    }
    else
        if (p = realloc(blk, size))
            return p;
    error("Out of memory\n");
}

// free up all memory - not realy needed as system will clear
void freeMem() {
    flux_t *p, *q;

    for (p = fluxData; p; p = q) {
        q = p->link;
        free(p);
    }
    free(streamArray);
    free(indexArray);
}


/*
    The Flux read engine
*/
// reset the flux machine for the next track

void resetFlux() {
    for (flux_t *p = fluxData; p; p = p->link)  // reset all counts in the allocated blocks
        p->cnt = 0;
    fluxPos = fluxRead = 0;
    indexArrayCnt = 0;
    streamArrayCnt = 0;

}


// add a new flux transition entry, allocating more memory if needed

void addFlux(int val) {
    flux_t *p;


    if (!fluxData || (curBlk->cnt == FLUXCHUNKSIZE && !curBlk->link)) {
        p = (flux_t *)xalloc(NULL, sizeof(flux_t));
        p->cnt = 0;
        p->link = NULL;
        if (!fluxData) {
            fluxData = curBlk = p;
            p->blkId = 0;
        }
        else {
            p->blkId = curBlk->blkId + 1;
            curBlk->link = p;
            curBlk = p;
        }
    }
    curBlk->flux[curBlk->cnt++] = val;
    fluxRead++;
}

// add another stream Info entry allocating more memory if needed
void addStreamInfo(long streamPos, long transferTime) {
    if (streamArrayCnt == streamArraySize) {
        streamArray = (stream_t *)xalloc(streamArray, (streamArraySize + ARRAYCHUNKSIZE) * sizeof(stream_t));
        streamArraySize += ARRAYCHUNKSIZE;
    }
    stream_t *p = &streamArray[streamArrayCnt++];
    p->streamPos = streamPos;
    p->transferTime = transferTime;
}

// add another index block entry allocating more memory if needed
void addIndexBlock(long streamPos, long sampleCnt, long indexCnt) {
    if (indexArrayCnt == indexArraySize) {
        indexArray = (index_t *)xalloc(indexArray, (indexArraySize + ARRAYCHUNKSIZE) * sizeof(index_t));
        indexArraySize += ARRAYCHUNKSIZE;
    }
    index_t *p = &indexArray[indexArrayCnt++];
    p->streamPos = streamPos;
    p->sampleCnt = sampleCnt;
    p->indexCnt = indexCnt;
}




// handle flux Out Of Band info
void oob() {
    unsigned here = (unsigned)inPos();
    int type = get1();
    int size = get2();

    if (type != 0xd && (size < 0 || !atLeast(size))) {
        logger(ALWAYS, "EOF in OOB block @ %lX\n", here);
        skip(-1);
        return;
    }

    switch (type) {
    case 0:
        logger(MINIMAL, "Invaild OOB - skipped\n");
        skip(size);
        break;
    case 1:
    case 3:
        if (size != 8) {
            logger(MINIMAL, "bad OOB block type %d size %d - skipped\n", type, size);
            skip(size);
        }
        else if (type == 1) {
            unsigned streamPos = get4();
            unsigned transferTime = get4();

            logger(VERBOSE, "StreamInfo block, Stream Position = %lu, Transfer Time = %lu\n", streamPos, transferTime);
            addStreamInfo(streamPos, transferTime);
        }
        else {
            long streamPos = get4();
            long resultCode = get4();
            if (resultCode != 0)
                logger(ALWAYS, "Read error %ld %s\n", resultCode, resultCode == 1 ? "Buffering problem" :
                    resultCode == 2 ? "No index signal" : "Unknown error");
        }
        break;
    case 2:
        if (size != 12) {
            logger(MINIMAL, "bad OOB block type %d size %d - skipped\n", type, size);
            skip(size);
        }
        else {
            unsigned streamPos = get4();
            unsigned sampleCounter = get4();
            unsigned indexCounter = get4();

            logger(VERBOSE, "Index block, Stream Position = %lu, Sample Counter = %lu, Index Counter = %lu\n", streamPos, sampleCounter, indexCounter);

            addIndexBlock(streamPos, sampleCounter, indexCounter);
        }
        break;
    case 4:

        logger(VERBOSE, "KFInfo block:\n");
        if (debug >= VERBOSE) {
            int c;
            while (size-- > 0 && (c = get1()) != EOF)
                if (c)
                    putchar(c);
                else
                    putchar('\n');
        }
        else

            skip(size);

        break;
    case 0xd:
        skip(-1);           // force to end of input stream
        return;
    default:
        logger(MINIMAL, "unknown OOB block type = %d @ %lX\n", type, here);
        skip(size);
    }
}


/*
    Process the flux stream
*/
size_t readFluxBuffer(byte *buf, size_t bufsize) {
    int c;
    initInput(buf, bufsize);
    resetFlux();
    addIndexBlock(0, 0, 0);         // mark start of all data

    while ((c = get1()) != EOF) {
        if (c >= 0xe || (c < 7 && (c = get1()) != EOF) || (c == 0xc && (c = get2()) != EOF))
            addFlux(c);
        else
            switch (c) {
            case 0xa: get1(); addFlux(PAD);    // NOP3
            case 0x9: get1(); addFlux(PAD);    // NOP2
            case 0x8:         addFlux(PAD);    // NOP1
                break;
            case 0xb: addFlux(OVL16);
                break;
            case 0xd:
                oob();
                break;
            default: error("coding error case %d\n", c);
            }
    }
    return fluxRead;
}


int getNextFlux() {
    int val;
    int ovl16 = 0;

    while (1) {
        if (fluxPos % FLUXCHUNKSIZE == 0 && !fluxSeek(fluxPos) || fluxPos >= fluxRead)
            return END_FLUX;

        if (endTrack && fluxPos >= endTrack)
            return END_BLOCK;

        val = curBlk->flux[fluxPos++ % FLUXCHUNKSIZE];

        if (val == OVL16)
            ovl16 += 0x1000;
        else if (val >= 0)                  // valid flux . Note PAD is ignored
            return val + ovl16;
    }
}


void displayHist(int levels)
{
    int histogram[HIST_MAX + 1];
    int cols[HIST_MAX + 1];
    char line[HIST_MAX + 2];        // allow for 0 at end
    int val;
    int blk = 1;
    int maxHistCnt = 0;
    int maxHistVal = 0;
    int outRange = 0;
    double sck = 24027428.5714285;

    /* read all of the flux data */
    memset(histogram, 0, sizeof(histogram));

    while (seekBlock(blk++)) {
        while ((val = getNextFlux()) != END_FLUX && val != END_BLOCK) {
            if (val > maxHistVal)
                maxHistVal = val;
            if (val > HIST_MAX)
                outRange++;
            else if (++histogram[val] > maxHistCnt)
                maxHistCnt++;
        }
    }
    if (maxHistCnt == 0) {
        logger(ALWAYS, "No histogram data\n");
        return;
    }

    for (int i = 0; i <= HIST_MAX; i++)
        cols[i] = (2 * levels * histogram[i] + maxHistCnt / 2) / maxHistCnt;

    int lowVal, highVal;
    for (lowVal = 0; lowVal < ONEUS / 2 && cols[lowVal] == 0; lowVal++)
        ;
    for (highVal = HIST_MAX; highVal >= ONEUS * 5 + ONEUS / 2 && cols[highVal] == 0; highVal--)
        ;

    printf("max flux value = %.1fus, flux values > %.1fus = %d\n", maxHistVal * 1000000.0 / sck, HIST_MAX * 1000000.0 / sck, outRange);
    printf("x axis range = %.1fus - %.1fus, y axis range 0 - %d\n", lowVal * 1000000.0 / sck, highVal * 1000000.0 / sck, maxHistCnt);
    puts("\nHistogram\n"
        "---------");
    int fillch;
    for (int i = levels * 2; i > 0; i -= 2) {
        line[highVal + 1] = fillch = 0;
        for (int j = highVal; j >= lowVal; j--) {
            if (cols[j] >= i - 1) {
                line[j] = cols[j] >= i ? '#' : '+';
                fillch = ' ';
            }
            else
                line[j] = fillch;
        }
        puts(line + lowVal);
    }
    for (int i = lowVal; i <= highVal; i++)
        putchar('-');
    putchar('\n');
    for (int i = lowVal; i <= (highVal / ONEUS) * ONEUS; i++)
        if (i % ONEUS == 0)
            i += printf("%dus", i / ONEUS) - 1;
        else
            putchar(' ');
    putchar('\n');


}


