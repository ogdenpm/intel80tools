#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>

enum {
    ALL, ODD, EVEN
};

#define	WORD(n)	(rec[n] + rec[n+1]*256)


#define MAXREC	0x10000
#define MAXROM  0x10000
uint8_t rec[MAXREC];               // holds record being processed
uint8_t rom[MAXROM];               // larget ROM supported
unsigned base = 0xffffffff;     // base of rom - sentinal value used to flag no base offset specified
unsigned toprom = 0;            // top of rom useage
unsigned botrom = ~0;

unsigned int reclen;
int rectype;
unsigned int recptr;

#pragma pack(push, 1)
typedef struct {        // omf286 header
    uint8_t tspace[4];  // uint32_t
    char date[8];
    char time[8];
    char creator[41];
    uint8_t gdtl[2];    // uint16_t
    uint8_t gdtb[4];    // uint32_t
    uint8_t idtl[2];    // uint16_t
    uint8_t idtb[4];    // uint32_t
    uint8_t tss[2];     // uint16_t
    uint8_t location[4];     // start of data info
    uint8_t debugloc[4];     // start of debug info
    uint8_t lastloc[4];      // end of file (excluding crc)
    uint8_t nextpart[4];     // will be 0
    uint8_t res[4];
} loader_hdr;
#pragma pack(pop)

inline uint16_t word(uint8_t *buf) {
#ifdef SAFECONVERT
    return buf[0] + (buf[1] >> 8);
#else
    return *(uint16_t *)buf;
#endif
}

inline uint32_t dword(uint8_t *buf) {
#ifdef SAFECONVERT
    return buf[0] + (buf[1] >> 8) + (buf[2] >> 16) + (buf[3] >> 24);
#else
    return *(uint32_t *)buf;
#endif
}
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
    if (addr - base < botrom)
        botrom = addr - base;

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
    uint8_t crc = rectype + reclen % 256 + reclen / 256;
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
    if (dword(hdr.debugloc) != 0)                           // ignore debug info
        reclen = dword(hdr.debugloc) - dword(hdr.location);
    else
        reclen = dword(hdr.lastloc) - dword(hdr.location);  // exclude crc
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

void usage() {
    fprintf(stderr, "usage: aomf2bin option* infile [outfile | -o odd_outfile | -e even_outfile]+\n"
            "   supported options are\n"
            "   -b address - sets base address of rom\n"
            "   -p         - pads image to eprom boundary\n"
            "   -z         - sets uninitialsed data to 0 instead of 0xff\n");
    exit(1);

}

int main(int argc, char **argv) {
    FILE *fp;
    unsigned char magic;
    bool loaded = false;
    bool pad = false;
    int arg;
    int fill = 0xff;

    for (arg = 1; arg < argc; arg++) {
        if (strcmp(argv[arg], "-b") == 0 && arg + 1 < argc) {
            char tailch;
            int itemcnt = sscanf(argv[++arg], "%X%c", &base, &tailch);
            if (itemcnt == 0 || (itemcnt == 2 && toupper(tailch) != 'H')) {
                fprintf(stderr, "invalid base address %s\n", argv[arg]);
                usage();
            }
        } else if (strcmp(argv[arg], "-p") == 0)
            pad = true;
        else if (strcmp(argv[arg], "-z") == 0)
            fill = 0;
        else
            break;
    }
    if (arg >= argc || *argv[arg] == '-')
        usage();
    if ((fp = fopen(argv[arg], "rb")) == NULL) {
        fprintf(stderr, "can't open input file %s\n", argv[arg]);
        exit(1);
    }
    memset(rom, fill, MAXROM);      // set default uninitialised

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
        printf("Base=%08X Load Range=%08X-%08X\n", base, base + botrom, base + toprom - 1);
        if (pad) {                  // if padding set toprom to size to match an eprom (2^n k)
            unsigned i;
            for (i = 256; i < toprom; i <<= 1)
                ;
            toprom = i;
        }
        for (arg++; arg < argc; arg++) {
            if (strcmp(argv[arg], "-o") == 0) {       // odd bytes
                select = ODD;
                arg++;
            } else if (strcmp(argv[arg], "-e") == 0) {    // even bytes
                select = EVEN;
                arg++;
            } else if (*argv[arg] == '-')
                usage();
            else
                select = ALL;
            if (arg == argc) {
                fprintf(stderr, "missing filename on command line\n");
                usage();
            }
            if (!dump(select, argv[arg]))         // generate bin file
                return 1;
        }
        return 0;
    }
    return 1;
}











