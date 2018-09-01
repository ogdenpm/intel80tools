/* writeImage.c     (c) by Mark Ogden 2018

DESCRIPTION
    part of mkidsk
    Writes the disk image format to a file, applying interlave as requested
    Portions of the code are based on Dave Duffield's imageDisk sources

MODIFICATION HISTORY
    17 Aug 2018 -- original release as mkidsk onto github
    18 Aug 2018 -- added copyright info

*/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include "flux.h"
enum {
    IMG, IMD
};
byte smap[TRACKS][DDSECTORS];


imd_t trackData[TRACKS];
#define RECOVERED    0x80        // flag for recovered sector id


void resetIMD() {
    memset(trackData, 0, sizeof(trackData));
}


void addIMD(int track, imd_t *trackPtr) {
    byte fmt[DDSECTORS];             // skew assignments - assuming starting at 1
    int offset;                      // offset to make fmt align to smap
    byte secToPhsMap[DDSECTORS];     // map of sectors (-1) to slots
    int missingIdCnt = 0;
    int missingSecCnt = 0;

    bool canRecover = true;

    if (track < 0 || track >= TRACKS) {
        logger(ALWAYS, "File %s - Track %d invalid\n", curFile, track);
        return;
    }
    if (trackData[track].smap[0]) {
        logger(ALWAYS, "File %s - Ignoring duplicate track %d\n", curFile, track);
        return;
    }
    /* see what sectors are present */
    memset(secToPhsMap, 0xff, sizeof(secToPhsMap));
    for (int i = 0; i < DDSECTORS; i++) {
        if (trackPtr->smap[i])
            secToPhsMap[trackPtr->smap[i] - 1] = i;
        else
            missingIdCnt++;
        if (!trackPtr->hasData[i])
            missingSecCnt++;
    }
    if (missingIdCnt) {
        int skew = -1;
        for (int i = 0; i < DDSECTORS - 1; i++)
            if (secToPhsMap[i] != 0xff && secToPhsMap[i + 1] != 0xff) {
                skew = (secToPhsMap[i + 1] - secToPhsMap[i] + DDSECTORS) % DDSECTORS;
                break;
            }
        if (skew < 0)
            canRecover = false;
        else {
            /* create a skew map for the track based at 1 */
            int slot = 0;

            memset(fmt, 0, sizeof(fmt));            // 
            for (int i = 1; i <= DDSECTORS; i++) {
                while (fmt[slot])
                    slot = (slot + 1) % DDSECTORS;
                fmt[slot] = i;
                slot = (slot + skew) % DDSECTORS;
            }

            for (slot = 0; trackPtr->smap[slot] == 0; slot++)       // find first slot with allocated sector
                ;

            for (offset = 0; fmt[offset] != trackPtr->smap[slot]; offset++)      // find this sector in fmt table
                ;
            offset -= slot;                                         // backup to align with first slot 
            /* check whether skew recovery possible */
            for (int i = 0; i < DDSECTORS; i++) {
                if (trackPtr->smap[i] && trackPtr->smap[i] != fmt[(i + offset) % DDSECTORS]) {
                    canRecover = false;
                    break;
                }
            }

        }
    }

    if (showSectorMap) {
        printf("File %s - Track %d Sector Mapping:\n", curFile, track);
        for (int i = 0; i < DDSECTORS; i++) {
            if (trackPtr->smap[i])
                printf("%02d ", trackPtr->smap[i]);
            else if (canRecover) {
                trackPtr->smap[i] = fmt[(i + offset) % DDSECTORS];
                printf("%02dr", trackPtr->smap[i]);
            }
            else
                printf("-- ");
        }
        putchar('\n');
        for (int i = 0; i < DDSECTORS; i++) {
            putchar(trackPtr->hasData[i] ? 'D' : 'X');
            putchar(' ');
            putchar(' ');
        }putchar('\n');
    }
    else if (missingIdCnt) {
        if (canRecover) {
            printf("File %s - track %d recovering sector ids:", curFile, track);
            for (int i = 0; i < DDSECTORS; i++, offset = (offset + 1) % DDSECTORS) {
                if (!trackPtr->smap[i])
                    printf(" %02d", fmt[(i + offset) % DDSECTORS]);
            }
            putchar('\n');
        }
        else {
            printf("File %s - track %d cannot recover sector ids:", curFile, track);
            for (int i = 0; i < DDSECTORS; i++, offset = (offset + 1) % DDSECTORS) {
                if (secToPhsMap[i] == 0xff)
                    printf(" %02d", i + 1);
            }
            putchar('\n');


        }
    }
    if (missingSecCnt) {
        if (missingIdCnt == 0 || canRecover) {
            printf("File %s - track %d missing data for sectors:", curFile, track);
            for (int i = 0; i < DDSECTORS; i++) {
                if (!trackPtr->hasData[i]) {
                    printf(" %02d", trackPtr->smap[i]);
                }
            }
            putchar('\n');
        }
        else
            printf("File %s - track %d not usable - missing %d sector Ids and %d data sectors\n", curFile, track, missingIdCnt, missingSecCnt);
    
    }
    if (missingIdCnt == 0 || canRecover)
        trackData[track] = *trackPtr;           /* structure copy */
}

void WriteIMDHdr(FILE *fp, char *comment) {
    struct tm *dateTime;
    time_t curTime;

    if (strncmp(comment, "IMD ", 4) != 0) {		// if comment does not have IMD header put one in
        time(&curTime);
        dateTime = localtime(&curTime);
        fprintf(fp, "IMD 1.18 %02d/%02d/%04d %02d:%02d:%02d\r\n", dateTime->tm_mday, dateTime->tm_mon, dateTime->tm_year + 1900,
            dateTime->tm_hour, dateTime->tm_min, dateTime->tm_sec);
    }

    while (*comment) {
        if (*comment == '\n')
            putc('\r', fp);
        putc(*comment++, fp);
    }
    putc(0x1A, fp);
}

bool SameCh(byte *sec) {
    for (int i = 1; i < SECTORSIZE; i++)
        if (sec[0] != sec[i])
            return false;
    return true;
}

void WriteImgFile(char *fname, char *comment) {
    FILE *fp;
    imd_t *trackPtr;
    byte *sector;

    if ((fp = fopen(fname, "wb")) == NULL)
        error("cannot create %s\n", fname);

    WriteIMDHdr(fp, comment);
    for (int track = 0; track < TRACKS; track++) {
        trackPtr = &trackData[track];
        if (trackPtr->smap[0] == 0)

//        if ((trackPtr = chkTrack(track)) == NULL)
            continue;
        putc(3, fp);        // mode
        putc(track, fp);    // cylinder
        putc(0, fp);        // head
        putc(DDSECTORS, fp);      // sectors in track
        putc(0, fp);        // sector size - 128
        fwrite(trackPtr->smap, 1, DDSECTORS, fp);    // sector numbering map

        for (int secNum = 0; secNum < DDSECTORS; secNum++) {
            if (trackPtr->hasData[secNum]) {
                sector = trackPtr->track + secNum * SECTORSIZE;
                if (SameCh(sector)) {
                     putc(2, fp);
                    putc(*sector, fp);
                } else {
                    putc(1, fp);
                    fwrite(sector, 1, SECTORSIZE, fp);
                }
            }
            else
                putc(0, fp);            // data not available
        }
    }

    fclose(fp);
}


