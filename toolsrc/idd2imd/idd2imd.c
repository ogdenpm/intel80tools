#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <memory.h>
#include <ctype.h>
#include "flux.h"
#include "zip.h"

#define INDEX_HOLE_MARK         0
#define ID_ADDRESS_MARK         0xe
#define INDEX_ADDRESS_MARK      0xc
#define DATA_ADDRESS_MARK       0xb
#define DELETED_ADDRESS_MARK    0x8

// byte lengths
#define NOMINAL_LEADIN          46
#define GAP_LEN                 28
#define ID_LEN                  7
#define DATA_LEN                131
#define SECTOR_LEN              (GAP_LEN + ID_LEN + GAP_LEN + DATA_LEN)
#define JITTER_ALLOWANCE        28

int debug = 0;      // debug level
int histLevels = 0;
bool showSectorMap = false;
/*
 * Display error message and exit
 */
void error(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);
    exit(1);
}

void logger(int level, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    if (debug >= level)
        vprintf(fmt, args);
}



int byte2Time(int cnt) {  // return estimated time of cnt bytes
    return (cnt * x8ByteTime() + 4) / 8;
}


int getSlot(int marker) {
    static long indexTime, lastSectorTime, lastSlot;
    long time = when();
    int initialSlot = lastSlot;

    switch (marker) {
    case INDEX_HOLE_MARK:
        indexTime = lastSectorTime = lastSlot = 0;
        return 0;
    case INDEX_ADDRESS_MARK:
        indexTime = time;
        return 0;
    case ID_ADDRESS_MARK:
        if (indexTime == 0)
            indexTime = NOMINAL_LEADIN * x8ByteTime() / 8;     // we didn't have an index marker assume one
        if (lastSectorTime == 0) {                         // first id block seen
            if (time - indexTime < byte2Time(GAP_LEN + JITTER_ALLOWANCE)) {
                lastSectorTime = time;
                return lastSlot = 0;
            }
            else {
                lastSectorTime = byte2Time(GAP_LEN + 1) + indexTime;        // estimated time after id marker
                lastSlot = 0;
            }
        }
        while (time - lastSectorTime > byte2Time(SECTOR_LEN - JITTER_ALLOWANCE)) {
            lastSectorTime += byte2Time(SECTOR_LEN);
            lastSlot++;
        }
        lastSectorTime = time;
        break;
    case DELETED_ADDRESS_MARK:
    case DATA_ADDRESS_MARK:
        if (indexTime == 0)
            indexTime = NOMINAL_LEADIN * x8ByteTime() / 8;      // we didn't have an index marker assume one
        if (lastSectorTime == 0) {                          // we didn't have a sector id marker
            if (time - indexTime < byte2Time(GAP_LEN + ID_LEN + GAP_LEN + JITTER_ALLOWANCE)) {
                lastSlot = 0;
                lastSectorTime = time - byte2Time(GAP_LEN + ID_LEN);    // set an effective time;
                return lastSlot;
            }
            else {
                lastSectorTime = byte2Time(GAP_LEN + 1) + indexTime;        // estimated time after id marker
                lastSlot = 0;
            }
        }
        while (time - lastSectorTime > byte2Time(ID_LEN + GAP_LEN + JITTER_ALLOWANCE)) {
            lastSectorTime += byte2Time(SECTOR_LEN);
            lastSlot++;
        }
        lastSectorTime = time - byte2Time(GAP_LEN + ID_LEN);
        break;
    default:
        error("marker %d parameter error in getSlot\n", marker);

    }
    if (lastSlot - initialSlot > 3)
        logger(MINIMAL, "warning more than 3 sectors missing\n");
    if (lastSlot >= DDSECTORS)
        error("physical sector (%d) >= %d\n", lastSlot, DDSECTORS);
    return lastSlot;
}



bool crcCheck(byte * buf, unsigned length) {
    byte x;
    unsigned short crc = 0;
    if (length < 2)
        return false;
    while (length-- > 2) {
        x = crc >> 8 ^ *buf++;
        x ^= x >> 4;
        crc = (crc << 8) ^ (x << 12) ^ (x << 5) ^ x;
    }

    return crc == (buf[0] * 256 + buf[1]);      // check with recorded crc
}

enum {
    BAD_DATA = 256,
};

// read data into buffer and check crc  ok
// returns bytes read + marker if CRC error or bad flux transition
int getData(int marker, byte *buf, bool cbit) {
    int val = 0;
    int bitCnt = cbit ? 1 : 0;
    int toRead;
    int byteCnt = 0;

    switch (marker) {
    case ID_ADDRESS_MARK:
        toRead = 7;
        break;
    case DATA_ADDRESS_MARK:
    case DELETED_ADDRESS_MARK:
        toRead = 131;
        break;
    case INDEX_ADDRESS_MARK:
        return 0;
    default:
        return 0;
    }

    buf[byteCnt++] = marker;        // record the marker

    do {
        switch (nextBits()) {
        case BIT0: val <<= 1; bitCnt++; break;
        case BIT1: val = (val << 1) + 1; bitCnt++; break;
        case BIT00: val <<= 2; bitCnt += 2; break;
        case BIT01: val = (val << 2) + 1; bitCnt += 2; break;

        case BIT0M:         // marker bit illegal in data
        case BIT1S:         // had to resync in data which shouldn't happen
        case BITEND:        // ran out of flux data
        case BITBAD:        // corrupt flux data
            bitLog(BITFLUSH);   // flush every thing
            return byteCnt + BAD_DATA;
        }
        if (bitCnt >= 8) {
            buf[byteCnt++] = bitCnt == 8 ? val : val >> 1;
            bitCnt -= 8;
        }

    } while (byteCnt < toRead);
    bitLog(bitCnt ? BITFLUSH_1 : BITFLUSH);

    return crcCheck(buf, byteCnt) ? byteCnt : byteCnt + BAD_DATA;    // flag if crc error
}

#define BYTES_PER_LINE  16

void dumpBuf(byte *buf, int length) {
    char text[BYTES_PER_LINE + 1];

    if (debug < VERYVERBOSE)
        return;

    for (int i = 0; i < length; i += BYTES_PER_LINE) {
        printf("%04x:", i);
        for (int j = 0; j < BYTES_PER_LINE; j++)
            if (i + j < length) {
                printf(" %02X", buf[i + j]);
                if (' ' <= buf[i + j] && buf[i + j] <= '~')
                    text[j] = buf[i + j];
                else
                    text[j] = '.';
            }
            else {
                printf(" --");
                text[j] = 0;
            }
        text[BYTES_PER_LINE] = 0;
        printf(" |%s|\n", text);
    }
}



imd_t curTrack;

int getMarker() {
    int bits;

    while ((bits = nextBits()) != BITEND) {          // read this track
    /* look for the marker start signature 00MM1 where M is special marker specific 0 */
        if (bits == BIT00 && nextBits() == BIT0M && nextBits() == BIT0M && nextBits() == BIT1)
            switch (nextBits()) {
            case BIT1:                              // 00MM11                     
                if ((bits = nextBits()) == BIT00) { // 00MM1100 - index address mark
                    bitLog(BITFLUSH);
                    return INDEX_ADDRESS_MARK;
                }
                else if (bits == BIT1 && nextBits() == BIT00) {
                    bitLog(BITFLUSH_1);
                    return ID_ADDRESS_MARK;         // 00MM1110 (0) id address mark + preread of next 0 bit
                }
                break;
            case BIT01:
                if (nextBits() == BIT1) {             // 00MM1011 - data address mark
                    bitLog(BITFLUSH);
                    return DATA_ADDRESS_MARK;
                }
                break;
            case BIT00:                             // 00MM1000 (0) deleted address mark
                if (nextBits() == BIT00) {
                    bitLog(BITFLUSH);
                    return DELETED_ADDRESS_MARK;
                }
            }
    }
    return INDEX_HOLE_MARK;
}



void flux2track(byte *buf, size_t bufsize, const char *fname) {
    byte dataBuf[131];
    int track = -1, sector;
    int blk = 0;
    int slot;
    int i;
    int marker;
    int missingIdCnt, missingSecCnt;
    byte secToSlot[DDSECTORS + 1];
    int len;

    readFluxBuffer(buf, bufsize);         // load in the flux data from buffer extracted from zip file

    memset(secToSlot, DDSECTORS, sizeof(secToSlot));
    memset(&curTrack, 0, sizeof(curTrack));

    while (1) {
        if (seekBlock(blk) == 0) {
            if (track == -1) {
                printf("File %s - No Id markers\n", fname);
                return;
            }
            addIMD(track, &curTrack, fname);
            return;
        }
        slot = getSlot(INDEX_HOLE_MARK);                    // initialise the pyhsical sector logic

        while ((marker = getMarker()) != INDEX_HOLE_MARK) {          // read this track
            slot = getSlot(marker);
            switch (marker) {
            case INDEX_ADDRESS_MARK:
                logger(VERBOSE, "Blk %d - Index Address Mark\n", blk + 1);
                break;
            case ID_ADDRESS_MARK:
                if ((len = getData(marker, dataBuf, true)) & BAD_DATA) {
                    logger(curTrack.smap[slot] ? VERBOSE : MINIMAL, "Block %d - Id corrupt for physical sector %d\n", blk + 1, slot);
                    dumpBuf(dataBuf + 1, (len & 0xff) - 1);
                }
                else {
                    logger(VERBOSE, "Block %d - Sector Id - Track = %d, Sector = %d at physical sector %d\n", blk + 1, dataBuf[1], dataBuf[3], slot);
                    dumpBuf(dataBuf + 1, len - 1);
                    if (track != -1 && track != dataBuf[1]) {
                        logger(ALWAYS, "Block %d - Multiple track Ids %d and %d - skipping track", track, dataBuf[1]);
                        return;
                    }
                    track = dataBuf[1];
                    sector = dataBuf[3];
                    if (sector == 0 || sector > DDSECTORS) {
                        logger(ALWAYS, "Block %d - Ignoring invalid sector number %d at physical sector %d\n", blk + 1, sector, slot);
                        continue;
                    }

                    if (curTrack.smap[slot] && curTrack.smap[slot] != sector) {
                        logger(ALWAYS, "Block %d - Pyhsical sector %d already used by sector %d ignoring allocation to sector %d\n",
                            blk + 1, slot, curTrack.smap[slot], sector);
                        continue;
                    }

                    if (secToSlot[sector] != slot && secToSlot[sector] != DDSECTORS) {   // physical is 0 to DDSECTORS - 1
                        logger(ALWAYS, "Block %d - Sector %d already allocated to pyhsical sector %d ignoring allocation to physical sector %d\n",
                            blk + 1, curTrack.smap[slot], slot, sector);
                        continue;

                    }
                    curTrack.smap[slot] = sector;
                    secToSlot[sector] = slot;
                }
                break;
            case DELETED_ADDRESS_MARK:
                logger(ALWAYS, "Block %d - Deleted Data Marker seen at pyhsical sector %d\n", blk + 1, slot);
            case DATA_ADDRESS_MARK:
                if ((len = getData(marker, dataBuf, marker == DELETED_ADDRESS_MARK)) & BAD_DATA) {
                    logger(curTrack.smap[slot] ? VERBOSE : MINIMAL, "Block %d - Data corrupt for physical sector %d\n", blk + 1, slot);
                    dumpBuf(dataBuf + 1, (len & 0xff) - 1);
                }
                else {
                    if (curTrack.smap[slot])
                        logger(VERBOSE, "Block %d - Data for sector %d at physical sector\n", blk + 1, curTrack.smap[slot], slot);
                    else
                        logger(VERBOSE, "Block %d - Data for physical sector %d\n", blk + 1, slot);

                    dumpBuf(dataBuf + 1, len - 1);

                    if (marker == DATA_ADDRESS_MARK) {
                        if (!curTrack.hasData[slot]) {
                            memcpy(curTrack.track + slot * SECTORSIZE, dataBuf + 1, 128);
                            curTrack.hasData[slot] = true;
                        }
                        else if (memcmp(curTrack.track + slot * SECTORSIZE, dataBuf + 1, 128) != 0)
                            logger(ALWAYS, "Block %d - Pyhsical sector %d has different valid sector data\n", blk + 1, slot);
                    }
                }
                break;
            }

        }
        missingIdCnt = missingSecCnt = 0;
        for (i = 0; i < DDSECTORS; i++) {
            if (!curTrack.smap[i])
                missingIdCnt++;
            if (!curTrack.hasData[i])
                missingSecCnt++;
        }
        ;
        if (missingIdCnt == 0 && missingSecCnt == 0) {
            addIMD(track, &curTrack, fname);
            return;
        }
        logger(MINIMAL, "File %s - missing %d ids and %d sectors after trying blk %d\n", fname, missingIdCnt, missingSecCnt, ++blk);
    }
}


_declspec(noreturn) void usage() {
    error("usage: idd2imd [-d[n]] -h[n] [zipfile|rawfile]+\n"
        "-d sets debug. n is optional level 0,1,2,3\n"
        "-h displays flux histogram. n is optional number of levels\n"
        "-s display sector mapping\n"
        "No IMD file is created for non zip files\n");
}
int main(int argc, char **argv) {
    int arg;
    int optCnt, optVal;
    char optCh;

    for (arg = 1; arg < argc && (optCnt = sscanf(argv[arg], "-%c%d", &optCh, &optVal)); arg++) {
        switch (tolower(optCh)) {
        case 'd':
            debug = optCnt == 2 ? optVal : MINIMAL; break;
        case 'h':
            histLevels = (optCnt == 2 && optVal > 5) ? optVal : 10; break;
        case 's':
            showSectorMap = true; break;
        default:
            usage();
        }
    }
    if (arg == argc)
        usage();

    for (; arg < argc; arg++) {
        struct zip_t *zip;
        byte *buf;
        size_t bufsize;
        char dir[_MAX_DIR];
        char drive[_MAX_DRIVE];
        char fname[_MAX_FNAME];
        char ext[_MAX_EXT];
        char imdFile[_MAX_PATH];
        FILE *fp;

        _splitpath_s(argv[arg], drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
        _makepath_s(imdFile, _MAX_PATH, drive, dir, fname, ".imd"); // create the default name

        if (_stricmp(ext, ".raw") == 0) {
            if ((fp = fopen(argv[arg], "rb")) == NULL)
                error("can't open %s\n", argv[arg]);
            resetIMD();
            fseek(fp, 0, SEEK_END);
            bufsize = ftell(fp);
            rewind(fp);
            buf = (byte *)xalloc(NULL, bufsize);
            if (fread(buf, bufsize, 1, fp) != 1)
                error("failed to load %s\n", argv[arg]);
            else {
                logger(MINIMAL, "procesing %s\n", argv[arg]);
                flux2track(buf, bufsize, argv[arg]);
                if (histLevels)
                    displayHist(histLevels);
            }
            fclose(fp);
        }
        else if (_stricmp(ext, ".zip") != 0)
            usage();
        else if ((zip = zip_open(argv[arg], 0, 'r')) == NULL)
            error("can't open %s\n", argv[arg]);
        else {
            resetIMD();
            int n = zip_total_entries(zip);
            for (int i = 0; i < n; i++) {
                zip_entry_openbyindex(zip, i);
                if (!zip_entry_isdir(zip)) {
                    size_t len;
                    int trk, sec;
                    const char *name = zip_entry_name(zip);
                    if ((len = strlen(name)) >= 8 && sscanf(name + len - 8, "%2d.%d.raw", &trk, &sec) == 2 && sec == 0 && trk < 77) {
                        logger(MINIMAL, "procesing %s\n", name);
                        bufsize = (size_t)zip_entry_size(zip);
                        if (!(buf = (byte *)calloc(1, bufsize)))
                            error("out of memory\n");
                        zip_entry_noallocread(zip, (void *)buf, bufsize);
                        flux2track(buf, bufsize, name);
                        if (histLevels)
                            displayHist(histLevels);
                        free(buf);
                    }
                }
                zip_entry_close(zip);
            }
            zip_close(zip);
            WriteImgFile(imdFile, "Created by idd2imd\n");
        }
    }
    return 0;
}

