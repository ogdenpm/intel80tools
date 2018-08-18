/* writeImage.c     (c) by Mark Ogden 2018

DESCRIPTION
    part of mkidsk
    Writes the disk image format to a file, applying interlave as requested
    Portions of the code are based on Dave Duffield's imageDisk sources

MODIFICATION HISTORY
    17 Aug 2018 -- original release as mkidsk onto github
    18 Aug 2018 -- added copyright info

*/
#include "mkIsisDisk.h"
#include <time.h>

enum {
    IMG, IMD
};
byte smap[TRACKS][DDSECTORS];


void BuildSMap(char *skews) {
    word curSkew = 0;
    word curSec = 0;
    byte sectors;
    byte i, interTrackSkew;


    if (diskType == ISIS_DD) {	/* DD */
        sectors = 52;
        interTrackSkew = 7;
    } else {
        sectors = 26;
        interTrackSkew = 4;
    }

    for (int curTrack = 0; curTrack < TRACKS; curTrack++) {
        if (!skews) {
            for (i = 1; i <= sectors; i++)
                smap[curTrack][i - 1] = i;      // no sector mapping
        } else {
            if (curTrack < 3) {                 // track 0, 1 and 2 restart skew and intertrack skew
                curSkew = *skews++ - '0';
                curSec = 0;
            }
            for (i = 1; i <= sectors; i++) {
                curSec = (curSec + curSkew) % sectors;
                while (smap[curTrack][curSec] != 0)
                    curSec = ++curSec % sectors;
                smap[curTrack][curSec] = i;
            }
            if (interTrackInterleave)
                curSec += interTrackSkew;
            else
                curSec = 0;
        }
    }
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

void WriteImgFile(char *fname, char *skews, char *comment) {
    FILE *fp;
    char *fmtExt;
    int fmt;
    byte *sector;

    if ((fp = fopen(fname, "wb")) == NULL) {
        fprintf(stderr, "cannot create %s\n", fname);
        exit(1);
    }
    BuildSMap(skews);
    fmtExt = strrchr(fname, '.');
    fmt = _stricmp(fmtExt, ".img") == 0 ? IMG : IMD;
    

    if (fmt == IMD)
        WriteIMDHdr(fp, comment);
    for (int track = 0; track < TRACKS; track++) {
        if (fmt == IMD) {
            putc(0, fp);        // mode
            putc(track, fp);    // cylinder
            putc(0, fp);        // head
            putc(spt, fp);      // sectors in track
            putc(0, fp);        // sector size - 128
            fwrite(smap[track], 1, spt, fp);    // sector numbering map
        }
        for (int secNum = 0; secNum < spt; secNum++) {
            sector = GetSectorLoc(BLOCK(track, smap[track][secNum]));
            if (fmt == IMD) {
                if (SameCh(sector)) {
                    putc(2, fp);
                    putc(*sector, fp);
                } else {
                    putc(1, fp);
                    fwrite(sector, 1, SECTORSIZE, fp);
                }
            } else
                fwrite(sector, 1, SECTORSIZE, fp);
        }
    }

    fclose(fp);
}


