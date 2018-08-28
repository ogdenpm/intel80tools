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


void addIMD(int track, imd_t *trackPtr) {
    if (track < 0 || track >= TRACKS) {
        logger(ALWAYS, "track %d invalid\n", track);
        return;
    }
    trackData[track] = *trackPtr;           /* structure copy */
    if (showSectorMap) {
        printf("track %d Sector Mapping:\n", track);
        for (int i = 0; i < DDSECTORS; i++) {
            if (trackPtr->smap[i])
                printf("%02d", trackPtr->smap[i]);
            else
                printf("--");
            putchar(i == DDSECTORS - 1 ? '\n' : ' ');
        }
        for (int i = 0; i < DDSECTORS; i++) {
            putchar(trackPtr->hasData[i] ? 'D' : 'X');
            putchar(' ');
            putchar(i == DDSECTORS - 1 ? '\n' : ' ');
        }

    }
}

bool chkTrack(imd_t *trackPtr) {
    int noIdCnt;
    size_t slotUsed = 0, secUsed = 0;
    // see what sector ids are missing */
    noIdCnt = 0;
    for (int i = 0; i < DDSECTORS; i++) {
        if (!trackPtr->smap[i])
            noIdCnt++;
        else {
            slotUsed |= 1ULL << i;
            secUsed |= 1ULL << trackPtr->smap[i]; 
        }

    }

    if (noIdCnt == 0)
        return true;
    if (noIdCnt == DDSECTORS)       // no idea on sector mapping
        return false;
    if (noIdCnt == 1) {             // we have one missing sector Id so reasonably safe to recover
        int slot, id;
        for (slot = 0; slotUsed & 1; slot++, slotUsed >>= 1)
            ;
        for (id = 1; secUsed & 2; id++, secUsed >>= 1)
            ;
        logger(MINIMAL, "fixed missing sector id %d for slot %d\n", id, slot);
        trackPtr->smap[slot] = id;
        return true;
    }
    logger(ALWAYS, "recovery of multiple missing sector Ids not yet implemented - skipping track\n");

    return false;           /* for now fail if we have multiple missing sector Ids */
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
        if (!chkTrack(trackPtr))
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


