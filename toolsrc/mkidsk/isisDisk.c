#include "mkIsisDisk.h"

direct_t *directory;


byte *bitMap;
int baseGroup = 0;              // where to start search
int spt = 52;
byte *disk;
unsigned diskSize;
int sectorSize = 128;
int cntDirSectors = CNTDIRSECTORS;
int cntBitMapSectors;
int binHdrBlk;


word AllocSector() {
    int track, sector, i;

    while (bitMap[baseGroup] == 0xff) // will always stop on last slot which is never fully used
        ++baseGroup;

    // find the lowest group with a free sector
    for (sector = baseGroup * 8, i = 0x80; bitMap[baseGroup] & i; sector++, i >>= 1)
        ;
    track = sector / spt;           // tracks start at 0
    sector = sector % spt + 1;      // sectors start at 1
    if (track >= 77)                // make sure we are not off end of disk!!
        exit(1);                    // disk full
    bitMap[baseGroup] |= i;     // set sector is used

    return BLOCK(track, sector);   // return track & sector
}

void ReserveSector(word trkSec) {
    int lsec = BLKTRK(trkSec) * spt + BLKSEC(trkSec) - 1;
    bitMap[lsec / 8] |= 0x80 >> (lsec % 8);
}


byte *GetSectorLoc(word trkSec) {

    return disk + (BLKTRK(trkSec) * spt + BLKSEC(trkSec) - 1) * sectorSize;

}

word NewZeroSector(word trkSec) {
    if (trkSec == 0)
        trkSec = AllocSector();
    else
        ReserveSector(trkSec);
    byte *secp = GetSectorLoc(trkSec);  // get its in memory location
    memset(secp, 0, sectorSize);        // clear out sector
    return trkSec;
}

word *NewHdr(word trkSec) {
    return (word *)GetSectorLoc(NewZeroSector(trkSec));
}




void FormatDisk(int type) {
    diskSize = TRACKS * SECTORSIZE * (type == ISIS_SD ? SDSECTORS : DDSECTORS);
    if ((disk = (byte *)malloc(diskSize)) == NULL) {
        fprintf(stderr, "fatal error out of memory\n");
        exit(1);
    }
    memset(disk, FMTBYTE, diskSize);            // initial format
                                                // now set up some of the global variables
    sectorSize = SECTORSIZE;
    diskType = type;
    if (type == ISIS_SD) {
        spt = SDSECTORS;
        cntBitMapSectors = SDBITMAP_SIZE;
        binHdrBlk = SDBINHDR;
    } else {
        spt = DDSECTORS;
        cntBitMapSectors = DDBITMAP_SIZE;
        binHdrBlk = DDBINHDR;
    }
    bitMap = GetSectorLoc(ISISMAP_DAT);         // location of ISIS.MAP data
}

void SetLinks(word *hdr, int  startSlot, int cnt, int trkSec) {
    for (int i = 0; i < cnt; i++) {
        hdr[startSlot + i] = trkSec + i;
        ReserveSector(trkSec + i);
    }
}


bool NameToIsis(byte *isisName, char *name) {
    // convert name to isis internal format
    for (int i = 0; i < 6; i++) {
        if (isalnum(*name))
            isisName[i] = (byte)toupper(*name++);
        else
            isisName[i] = 0;
    }
    if (*name == '.')
        name++;
    for (int i = 6; i < 9; i++) {
        if (isalnum(*name))
            isisName[i] = (byte)toupper(*name++);
        else
            isisName[i] = 0;
    }
    return *name == 0 && *isisName;
}

direct_t *Lookup(char *name, bool autoName) {
    byte isisName[9];
    if (!NameToIsis(isisName, name)) {
        fprintf(stderr, "illegal ISIS name %s\n", name);
        exit(1);
    }
    direct_t *firstFree = NULL;                     // used to mark new entry insert point
    for (int i = 0; i < CNTDIRSECTORS * 8; i++)
        if (directory[i].use != INUSE) {
            if (firstFree == NULL && autoName)
                firstFree = &directory[i];
            if (directory[i].use == NEVUSE)
                break;
        } else if (memcmp(directory[i].file, isisName, 9) == 0)
            return &directory[i];
        if (firstFree)
            memcpy(firstFree->file, isisName, 9);
        return firstFree;
}



direct_t *MakeDirEntry(char *name, int attrib, int eofCnt, int blkCnt, int hdrBlk) {
    direct_t *dir = Lookup(name, true);
    if (dir == NULL) {
        fprintf(stderr, "too many directory entries - trying to create %s\n", name);
        exit(1);
    }
    if (dir->use == INUSE) {
        fprintf(stderr, "file %s already exists\n", name);
        exit(1);
    }
    dir->use = INUSE;
    dir->attrib = attrib;
    dir->eofCnt = eofCnt;
    dir->blkCnt = blkCnt;
    dir->hdrBlk = hdrBlk;
    return dir;
}


void CopyFile(char *isisName, char *srcName, int attrib) {
    direct_t *dir;
    int blkCnt = 0;
    int hdrIdx;
    word *hdr;
    byte buf[SECTORSIZE];      // used as staging area for data read
    int actual;
    FILE *fp;
    int curHdrBlk;


    if (strcmp(srcName, "AUTO") == 0) {
       dir = Lookup(isisName, false);
       if (attrib != 0 && dir->attrib != attrib)
            dir->attrib = attrib;
        return;
    }
    if (strcmp(srcName, "ZERO") == 0) {     // empty file
        if ((dir = Lookup(isisName, false)) == NULL)  // create dir entry if it doesn't exist
            dir = MakeDirEntry(isisName, attrib, 0, 0, 0);
        return;
    }
    /* all ISIS system files except ISIS.CLI are given the right attributes in WriteDirectory
    make sure it has the correct attributes */
    if (_stricmp(isisName, "ISIS.CLI") == 0 && attrib == 0)
        attrib = FMT | SYS | INV;

    if ((fp = fopen(srcName, "rb")) == NULL) {
        fprintf(stderr, "%s can't find source file %s\n", isisName, srcName);
        return;
    }
    if ((dir = Lookup(isisName, false)) == NULL)  // create dir entry if it doesn't exist
        dir = MakeDirEntry(isisName, attrib, 0, 0, 0);
    else if (attrib)
        dir->attrib = attrib;
 
    if (dir->hdrBlk == 0)
        dir->hdrBlk = NewZeroSector(0);
    hdr = (word *)GetSectorLoc(dir->hdrBlk);
    curHdrBlk = dir->hdrBlk;
    while (1) {
        for (hdrIdx = 2; hdrIdx < 64; hdrIdx++) {
            if ((actual = (int)fread(buf, 1, SECTORSIZE, fp)) == 0)
                break;
            blkCnt++;
            if (hdr[hdrIdx] == 0)
                hdr[hdrIdx] = actual == SECTORSIZE ? AllocSector(0) : NewZeroSector(0);
            memcpy(GetSectorLoc(hdr[hdrIdx]), buf, actual);
            if (actual != SECTORSIZE)
                break;
        }
        if (actual != SECTORSIZE)
            break;

        if (hdr[1] == 0) {     // see if forward link exists
            hdr[1] = NewZeroSector(0);
            word *nxtHdr = (word *)GetSectorLoc(hdr[1]);
            nxtHdr[0] = curHdrBlk;
        } 
        curHdrBlk = hdr[1];
        hdr = (word *)GetSectorLoc(curHdrBlk);
    }
    if (blkCnt > dir->blkCnt) {
        dir->blkCnt = blkCnt;
        dir->eofCnt = (actual == 0) ? SECTORSIZE : actual;
    }
    fclose(fp);


}


// lay down the initial directory info
// FormatDisk must have been called before
void WriteDirectory() {
    // make sure bitMap is clear
    memset(bitMap, 0, cntBitMapSectors * SECTORSIZE);
    // setup linkage for ISIS.T0
    word *hdr = NewHdr(ISIST0_HDR);
    SetLinks(hdr, 2, ISIST0_SIZE, ISIST0_DAT);
    // setup linkage for ISIS.LAB
    hdr = NewHdr(ISISLAB_HDR);
    if (diskType == ISIS_SD)
        SetLinks(hdr, 2, ISISLAB_SDSIZE, ISISLAB_DAT);
    else {
        SetLinks(hdr, 2, ISISLAB_DDSIZE, ISISLAB_DAT);
        SetLinks(hdr, 2 + ISISLAB_DDSIZE, ISISLAB_DDSIZEA, ISISLAB_DATA);
    }
    // setup linkage for ISIS.DIR
    hdr = NewHdr(ISISDIR_HDR);
    SetLinks(hdr, 2, CNTDIRSECTORS, ISISDIR_DAT);
    // set up linkage for ISIS.MAP
    hdr = NewHdr(ISISMAP_HDR);
    SetLinks(hdr, 2, cntBitMapSectors, ISISMAP_DAT);
    // now write the initial ISIS.DIR
    directory = (direct_t *)GetSectorLoc(ISISDIR_DAT);
    memset(directory, 0, CNTDIRSECTORS * SECTORSIZE);  // initialise to 0
    for (int i = 0; i < CNTDIRSECTORS * 8; i++)    // set the usage flags
        directory[i].use = NEVUSE;
    MakeDirEntry("ISIS.DIR", FMT | INV, 128, CNTDIRSECTORS, ISISDIR_HDR);
    MakeDirEntry("ISIS.MAP", FMT | INV, 128, cntBitMapSectors, ISISMAP_HDR);
    MakeDirEntry("ISIS.T0", FMT | INV, 128, ISIST0_SIZE, ISIST0_HDR);
    MakeDirEntry("ISIS.LAB", FMT | INV, 128, diskType == ISIS_SD ? ISISLAB_SDSIZE : ISISLAB_DDSIZE + ISISLAB_DDSIZEA, ISISLAB_HDR);
    if (hasSystem) {
        // finally setup empty linkage for ISIS.BIN
        NewHdr(binHdrBlk);
        MakeDirEntry("ISIS.BIN", FMT | SYS | INV, 128, 0, binHdrBlk);
    }
}

void WriteLabel() {
    // iniitlise the supplementary label info
    if (label.fmtTable[0] == 0)
        if (diskType == ISIS_SD)
            InitFmtTable(1, 12, 6);
        else
            InitFmtTable(1, 4, 5);
    label.crlf[0] = '\r';
    label.crlf[1] = '\n';
    memcpy(GetSectorLoc(ISISLAB_DAT), &label, sizeof(label));
}
