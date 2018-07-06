#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
typedef unsigned char byte;

enum {
    ALL, ODD, EVEN
};

#define	WORD(n)	(rec[n] + rec[n+1]*256)


#define MAXREC	0x10000
#define MAXROM  0x10000
byte rec[MAXREC];               // holds record being processed
byte rom[MAXROM];               // larget ROM supported
unsigned base = 0xffffffff;     // base of rom - sentinal value used to flag no base offset specified
unsigned toprom = 0;            // top of rom useage

unsigned int reclen;
int rectype;
unsigned int recptr;

#pragma pack(push, 1)
typedef struct {        // omf286 header
    unsigned long total;
    char date[8];
    char time[8];
    char res1[55];
    unsigned long location;     // start of data info
    unsigned long debugloc;     // start of debug info
    unsigned long lastloc;      // end of file (excluding crc)
    unsigned long nextpart;     // will be 0
    char res3[4];
} loader_hdr;
#pragma pack(pop)


/*
    load a chunk of data into the prom area if in range base -> base + MAXROM
    if out of range print info about what has been skipped
*/
void loadChunk(unsigned offset, unsigned addr, unsigned len) {
    unsigned preskip  = 0, postskip;

    if (addr < base) {                      // got some data below start of rom
        preskip = addr + len < base ? len : base - addr;
        printf("skipping %06X-%06X\n", addr, addr + preskip - 1);
        len -= preskip;
        addr += preskip;
        offset += preskip;
    }
    if (len == 0)
        return;

    if (addr < base + MAXROM) {      // got some data in range of rom
        postskip = addr + len < base + MAXROM ? 0 : addr + len - (base + MAXROM);
        len -= postskip;
        memcpy(rom + addr - base, rec + offset, len);
        if (toprom < addr + len - base)
            toprom = addr + len - base;
        addr += len;
        len = postskip;
    }
    if (len)                        // got some data above end of rom
        printf("skipping %06X-%06X\n", addr, addr + len - 1);

}

/*
    process an omf 6 - 8085 CONTENT record
*/
bool omf06() {                  // 8085 CONTENT record
    unsigned addr;
    if (rec[0] != 0) {
        fprintf(stderr, "Non absolute segment in content record\n");
        return false;
    }
    addr = WORD(recptr + 1);
    if (base == 0xFFFFFFFF) {
        printf("setting base address to %04X\n", addr);
        base = addr;
    }
    loadChunk(3, addr, reclen - 3);     // 3 prefix bytes seg (0) & offset

    return true;

}


/*
    process an omf 84 - PEDATA record
*/
bool omf84() {                  // 8086 PEDATA record
    unsigned addr;

    addr = WORD(0) * 16 + rec[2];
    if (base == 0xFFFFFFFF) {
        printf("setting base address to %06X\n", addr);
        base = addr;
    }
    loadChunk(3, addr, reclen - 3);     // 3 prefix bytes frame & offset

    return true;

}

/*
    load an omf record into memory (rec), setting the global rectype, reclen and checking crc
*/
bool getrec(FILE *fp) {         // load a standard omf record into memory and check its crc
    if ((rectype = getc(fp)) == EOF) {
        fprintf(stderr, "missing end record\n");
        return false;
    }
    reclen = getc(fp);
    reclen += getc(fp) * 256;

    if (reclen > MAXREC) {
        fprintf(stderr, "Fatal: Record length > %d\n", MAXREC);
        return false;
    }
    if (fread(rec, 1, reclen, fp) != reclen) {
        fprintf(stderr, "Fatal: Premature EOF\n");
        return false;
    }
    byte crc = rectype + reclen % 256 + reclen / 256;
    for (unsigned int i = 0; i < reclen; i++)
        crc += rec[i];
    if (crc != 0) {
        fprintf(stderr, "corrupt record type %02X length %d\n", rectype, reclen);
        return false;
    }
    reclen--;           // exclude crc
    recptr = 0;
    return true;
}

/*
    dump the rom, mode determines whether all, odd or even bytes are used
*/
bool dump(int mode, char *file) {
    FILE *fp;
    if ((fp = fopen(file, "wb")) == NULL) {
        fprintf(stderr, "can't create file %s\n", file);
        return false;
    }
    for (unsigned i = (mode == ODD) ? 1 : 0; i < toprom; i += (mode == ALL) ? 1 : 2) {
        putc(rom[i], fp);
    }
    fclose(fp);
    return true;
}

/*
    process an aomf85 file - returns true if good load
*/
bool loadOmf85(FILE *fp) {
    while (!feof(fp) && getrec(fp))
        switch (rectype) {
        case 0x4: return true;
        case 0x6: if (!omf06()) return false; break;
        case 0x2: case 0x8: case 0xe: case 0x10: case 0x12: case 0x16: case 0x18: case 0x20:
            break;
        default: fprintf(stderr, "invalid aomf85 record type %02X\n", rectype);
            return false;
        }
    fprintf(stderr, "unexpected EOF\n");
    return false;

}

/*
    process an aomf86 file, returns true if good load
*/
bool loadOmf86(FILE *fp) {
    while (!feof(fp) && getrec(fp))
        switch (rectype) {
        case 0x84: if (!omf84()) return false;  break;
        case 0x8a: return true;  break;
        case 0x82: case 0x98: case 0x8e: case 0x88: case 0x90: case 0x7e:
        case 0x7a: case 0x7c: case 0x94: case 0x96: case 0x80:
            break;
        case 0x86:
            fprintf(stderr, "PIDATA not currently supported\n");
            return false;
        default: fprintf(stderr, "invalid aomf86 record type %02X\n", rectype);
            return false;
        }
    fprintf(stderr, "unexpected EOF\n");
    return false;
}

/*
    process an aomf286 file - returns true if good load
*/
bool loadOmf286(FILE *fp) {
    loader_hdr hdr;
    int reclen;
    unsigned addr;
    unsigned len;

    if (base == 0xFFFFFFFF)
        base = 0;
    getc(fp);       // waste file header byte
    if (fread(&hdr, sizeof(hdr), 1, fp) != 1) {
            fprintf(stderr, "bootloadable module header corrupt\n");
            return false;
        }
    if (hdr.debugloc != 0)                      // ignote debug info
        reclen = hdr.debugloc - hdr.location;
    else
        reclen = hdr.lastloc - hdr.location;    // exclude crc
    for (; reclen > 0; reclen -= 5 + len) {
            addr = getc(fp);                    // 3 byte load address
            addr += getc(fp) * 256;
            addr += getc(fp) * 256 * 256;
            len = getc(fp);                     // 2 byte length
            len += getc(fp) * 256;            

            if (fread(rec, 1, len, fp) != len) {    // load the record into memory to allow common processing
                fprintf(stderr, "omf286 premature eof\n");
                return false;
            }
            loadChunk(0, addr, len);            // extract into the rom image - no prefix bytes

        }
    return true;
}

int main(int argc, char **argv) {
    FILE *fp;
    unsigned char magic;
    bool loaded = false;

    if (argc < 3 || (strcmp(argv[2], "-b") == 0 && argc < 5)) {
        fprintf(stderr, "usage: aomf2bin infile [-b address] [outfile | -o odd_outfile | -e even_outfile]+\n");
        exit(1);
    }

    if (strcmp(argv[2], "-b") == 0) {
        char tailch;
        int itemcnt = sscanf(argv[3], "%X%c", &base, &tailch);
        if (itemcnt == 0 || (itemcnt == 2 && toupper(tailch) != 'H')) {
            fprintf(stderr, "error in base address %s\n", argv[3]);
            exit(1);
        }
    }
    if ((fp = fopen(argv[1], "rb")) == NULL) {
        fprintf(stderr, "can't open input file %s\n", argv[1]);
        exit(1);
    }
    memset(rom, 0xff, MAXROM);      // default uninitialised to 0xff

    magic = getc(fp);               // peek at the magic header byte
    ungetc(magic, fp);
    switch (magic) {
    case 0x2:  loaded = loadOmf85(fp); break;
    case 0x82: loaded = loadOmf86(fp); break;
    case 0xA2: loaded = loadOmf286(fp); break;
    default:
        fprintf(stderr, "unknown file format marker %02X\n", magic);
        fclose(fp);
        exit(1);
    }

    if (loaded) {                   // if good load generate the bin files
        int select;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-b") == 0) {       // skip -b option here
                i++;
                continue;
            }
            if (strcmp(argv[i], "-o") == 0) {       // odd bytes
                select = ODD;
                i++;
            } else if (strcmp(argv[i], "-e") == 0) {    // even bytes
                select = EVEN;
                i++;
            } else
                select = ALL;
            if (i == argc) {
                fprintf(stderr, "missing filename on command line\n");
                return 1;
            }
            if (!dump(select, argv[i]))         // generate bin file
                return 1;
        }
        return 0;
    }
    return 1;
}











