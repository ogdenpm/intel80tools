#include "omfcmp.h"
#include "omf.h"

enum {
    UNUSED = 0,
    USED = 1,
    HASINFO = 2,
    CHECKED = 4,
    ERROR = 8,
    WARNING = 16
};

typedef struct {
    word cnt;
    word size;
    void *items;
} vector_t;

typedef struct {
    word cnt;
    word size;
    struct _fixup {
        word segId;   // > 255 then external fixup
        byte *name;   // used for external fixup
        word addr;
        byte type;
    } * items;
} fixup_t;

typedef struct {
    word cnt;
    word size;
    struct _public {
        word addr;
        byte *name;
    } *items;
} public_t;


typedef struct {
    word cnt;
    word size;
    struct _extern {
        byte *name;
        byte status;
    } *items;
} extern_t;

typedef struct {
    word cnt;
    word size;
    struct _content {
        word addr;
        word length;
        byte *image;
    } *items;
} content_t;


typedef struct {
    byte *name;
    word length;
    byte align;
    byte status;
    public_t publics;
    content_t contents;
    fixup_t fixups;
} seg_t;


typedef struct {
    char *name;
    byte compiler[2];
    extern_t externals;
    byte maxSeg;
    seg_t segs[256];
} module_t;


int returnCode = 0;
word lastContentAddr;
byte lastContentSeg;


void deleteModule(module_t *mod);

void printPstr(byte *s)
{
    printf("%.*s", *s, s + 1);
}

void printPstrPair(byte *s, byte *t)
{
    printf("%.*s : %.*s", *s, s + 1, *t, t + 1);
}

void checkAllocItem(vector_t *vec, size_t size)
{
    if (vec->size == 0) {
        vec->size = INITIALCHUNK;
        vec->items = xmalloc(vec->size * size);
    }
    else if (vec->cnt >= vec->size) {
        vec->size *= 2;
        vec->items = xrealloc(vec->items, vec->size  * size);
    }
     
}

void addPublic(public_t *pub, byte *name, word addr)
{
    checkAllocItem((vector_t *)pub, sizeof(struct _public));
    pub->items[pub->cnt].name = name;
    pub->items[pub->cnt++].addr = addr;
}

void addExternal(extern_t *ext, byte *name)
{
    checkAllocItem((vector_t *)ext, sizeof(struct _extern));
    ext->items[ext->cnt].name = name;
    ext->items[ext->cnt++].status = UNUSED;
}

void addFixup(fixup_t *fix, int id, byte *name, word addr, byte type)
{
    checkAllocItem((vector_t *)fix, sizeof(struct _fixup));

    fix->items[fix->cnt].segId = id;
    fix->items[fix->cnt].name = name;
    fix->items[fix->cnt].addr = addr;
    fix->items[fix->cnt++].type = type;
}

void addContent(content_t *con, word addr, word length, byte *image)
{
    checkAllocItem((vector_t *)con, sizeof(struct _content));

    lastContentAddr = con->items[con->cnt].addr = addr;
    con->items[con->cnt].length = length;
    con->items[con->cnt++].image = image;
}



/* print out the binary differences*/
void diffBinary(omf_t *left, omf_t *right)
{
    int i;

    printf("%s : %s ===Binary difference===\n", left->name, right->name);

    

    for (i = 0; i < left->size && i < right->size; i++) {
        if (left->image[i] != right->image[i]) {
            printf("%04X: %02X %02X\n", i, left->image[i], right->image[i]);
            returnCode = 1;
        }
    }
    if (left->size > right->size) {
        printf("%s is longer\n", left->name);
        returnCode = 1;
    }
    else if (left->size < right->size) {
        printf("%s is longer\n", right->name);
        returnCode = 1;
    }
}


int findSegByName(module_t *mod, byte *name)
{
    int i;

    for (i = mod->maxSeg; i >= 5; i--) {
        if (mod->segs[i].name && pstrEqu(mod->segs[i].name, name))
            return i;
    }
    return -1;
}



void modhdr(omf_t *omf, module_t *mod)
{
    seg_t *seg;
    byte segId;

    mod->name = getName(omf);
    mod->compiler[0] = getByte(omf);
    mod->compiler[1] = getByte(omf);
    while (!atEndOfRecord(omf)) {
        segId = getByte(omf);
        if (segId > mod->maxSeg) mod->maxSeg = segId;
        seg = &mod->segs[segId];
        seg->length = getWord(omf);
        seg->align = getByte(omf);
        seg->status |= USED;
    }
}

void content(omf_t *omf, module_t *mod)
{
    seg_t *seg;
    word addr;
    word length;

    lastContentSeg = getByte(omf);
    if (lastContentSeg > mod->maxSeg) mod->maxSeg = lastContentSeg;

    seg = &mod->segs[lastContentSeg];
    seg->status |= HASINFO;
    addr = getWord(omf);
    length = omf->lengthRec - 4;

    addContent(&seg->contents, addr, length, omf->image + omf->pos);

}


void pubnam(omf_t *omf, module_t *mod)
{

    seg_t *seg;
    byte segId;
    word addr;

    segId = getByte(omf);
    if (segId > mod->maxSeg) mod->maxSeg = segId;
    seg = &mod->segs[segId];
    seg->status |= HASINFO;

    while (!atEndOfRecord(omf)) {
        addr = getWord(omf);
        addPublic(&seg->publics, getName(omf), addr);
        getByte(omf);
    }
}

void extnam(omf_t *omf, module_t *mod)
{
    while (!atEndOfRecord(omf)) {
        addExternal(&mod->externals, getName(omf));
        getByte(omf);
    }
}

void extref(omf_t *omf, module_t *mod)
{
    word extId;
    seg_t *seg = &mod->segs[lastContentSeg];

    byte type = getByte(omf);
    while (!atEndOfRecord(omf)) {
        extId = getWord(omf);
        addFixup(&seg->fixups, 256, mod->externals.items[extId].name, getWord(omf), type);
        mod->externals.items[extId].status |= USED;
    }
}

void reloc(omf_t *omf, module_t *mod)
{
    seg_t *seg = &mod->segs[lastContentSeg];

    byte type = getByte(omf);
    while (!atEndOfRecord(omf))
        addFixup(&seg->fixups, lastContentSeg, mod->segs[lastContentSeg].name, getWord(omf), type);
}
void intseg(omf_t *omf, module_t *mod)
{
    word segId;
    seg_t *seg = &mod->segs[lastContentSeg];

    segId = getByte(omf);
    byte type = getByte(omf);
    while (!atEndOfRecord(omf))
        addFixup(&seg->fixups, segId, mod->segs[segId].name, getWord(omf), type);
}

void common(omf_t *omf, module_t *mod)
{
    byte segId;
    while (!atEndOfRecord(omf)) {
        segId = getByte(omf);
        if (segId > mod->maxSeg) mod->maxSeg = segId;
        mod->segs[segId].name = getName(omf);
        mod->segs[segId].status |= HASINFO;
    }
}

module_t *newModule(omf_t *omf)
{
    module_t *mod;

    mod = (module_t *)xcalloc(1, sizeof(module_t));
    mod->segs[0].name = "\3ABS";
    mod->segs[1].name = "\4CODE";
    mod->segs[2].name = "\4DATA";
    mod->segs[3].name = "\5STACK";
    mod->segs[4].name = "\6MEMORY";
    mod->segs[255].name = "\2//";


    seekRecord(omf, 0);
    while (!omf->error) {
        switch (getRecord(omf)) {
        case MODHDR:
            modhdr(omf, mod);
            break;
        case MODEND:
            return mod;
        case CONTENT:
            content(omf, mod);
            break;
        case PUBNAM:
            pubnam(omf, mod);
            break;
        case EXTNAM:
            extnam(omf, mod);
            break;
        case EXTREF:
            extref(omf, mod);
            break;
        case RELOC:
            reloc(omf, mod);
            break;
        case INTSEG:
            intseg(omf, mod);
            break;
        case COMMON:
            common(omf, mod);
            break;
        case LINNUM: case PARENT: case LOCNAM:
            break;
        default:
            omf->error = 1;
        }
    }
    deleteModule(mod);
    return NULL;
   
}

void deleteModule(module_t *mod)
{
    int i;

    for (i = 0; i <= mod->maxSeg; i++) {
        if (mod->segs[i].status) {
            if (mod->segs[i].fixups.size)
                free(mod->segs[i].fixups.items);
            if (mod->segs[i].publics.size)
                free(mod->segs[i].publics.items);
            if (mod->segs[i].contents.size)
                free(mod->segs[i].contents.items);
        }
    }
    if (mod->externals.size)
        free(mod->externals.items);

    free(mod);
}


void printSegHeader(seg_t *ls, seg_t *rs, int status)
{
    if ((ls->status & (ERROR | WARNING)) == 0)
        printf("\n%.*s:\n", ls->name[0], ls->name + 1);
    ls->status |= status;
    rs->status |= status;
}

int cmpPublic(const struct _public *a, const struct _public *b)
{
 
    int cmp;
    if ((cmp = pstrCmp(a->name, b->name)) == 0)
        cmp = a->addr - b->addr;
    return cmp;
}



void diffPublics(module_t *lm, byte lseg, module_t *rm, byte rseg)
{
    int i, j;
    int cmp;
    public_t *lp, *rp;
    seg_t *ls, *rs;

    ls = &lm->segs[lseg];
    rs = &rm->segs[rseg];


    lp = &ls->publics;
    rp = &rs->publics;



    qsort(lp->items, (size_t)lp->cnt, sizeof(public_t), &cmpPublic);
    qsort(rp->items, (size_t)rp->cnt, sizeof(public_t), &cmpPublic);

    i = j = 0;
    while (i < lp->cnt || j < rp->cnt) {
        if (i == lp->cnt)
            cmp = 1;
        else if (j == rp->cnt)
            cmp = -1;
        else
            cmp = pstrCmp(lp->items[i].name, rp->items[j].name);
        if (cmp < 0) {
            printSegHeader(ls, rs, ERROR);
            printPstrPair(lp->items[i].name, "\x06------");
            printf(" - Public missing\n");
            i++;
        }
        else if (cmp > 0) {
            printSegHeader(ls, rs, ERROR);
            printPstrPair("\x06------",rp->items[j].name);
            printf(" - Public missing\n");
            j++;
        }
        else {
            if (lp->items[i].addr != rp->items[j].addr) {
                printSegHeader(ls, rs, ERROR);
                printPstrPair(lp->items[i].name, rp->items[j].name);
                printf(" - Public addresses different %04X : %04X\n", lp->items[i].addr, rp->items[j].addr);
            }
            i++; j++;
        }
    }
}


int cmpContent(const struct _content *a, const struct _content *b)
{

    return a->addr - b->addr;
}




#define MAXRUN  8

void emitRun(int addr, int run, short *lrun, short *rrun)
{
    if (run == 0)
        return;
    printf("%04X", addr);
    for (int i = 0; i < run; i++)
        if (lrun[i] < 0)
            printf(" --");
        else
            printf(" %02X", lrun[i]);
    printf(" :");
    for (int i = 0; i < run; i++)
        if (rrun[i] < 0)
            printf(" --");
        else
            printf(" %02X", rrun[i]);
    putchar('\n');
}

void diffContent(module_t *lm, byte lseg, module_t *rm, byte rseg)
{
    seg_t *ls, *rs;
    content_t *lc, *rc;
    int lcb, rcb;       // index into contents blocks
    int laddr, raddr;  // base address of current block
    int li, ri;         // index into current block
    int run = 0;        // count of different items used to format
    int arun = -1;       // start address of difference run
    short lrun[MAXRUN], rrun[MAXRUN]; // the run values -ve for missing


    ls = &lm->segs[lseg];
    rs = &rm->segs[rseg];
    lc = &ls->contents;
    rc = &rs->contents;

    /* sort content */
    qsort(lc->items, (size_t)lc->cnt, sizeof(content_t), &cmpContent);
    qsort(rc->items, (size_t)rc->cnt, sizeof(content_t), &cmpContent);

    lcb = rcb = 0;
    laddr = raddr = -1;
    while (lcb < lc->cnt || rcb < rc->cnt) {
        if (laddr < 0 && lcb < lc->cnt) {
            laddr = lc->items[lcb].addr;
            li = 0;
        }
        if (raddr < 0 && rcb < rc->cnt) {
            raddr = rc->items[rcb].addr;
            ri = 0;
        }
        if (run && (laddr < 0 || arun + run != laddr + li) && (raddr < 0 || arun + run != raddr + ri)) {
            emitRun(arun, run, lrun, rrun);
            run = 0;
        }
        if (raddr < 0 || (laddr >= 0 && laddr + li < raddr + ri)) {
            printSegHeader(ls, rs, ERROR);
            if (run == 0)
                arun = laddr + li;
            lrun[run] = lc->items[lcb].image[li];
            rrun[run++] = -1;
            li++;
        }
        else if (laddr < 0 || (raddr >= 0 && laddr + li > raddr + ri)) {
            printSegHeader(ls, rs, ERROR);
            if (run == 0)
                arun = raddr + ri;
            lrun[run] = -1;
            rrun[run++] = rc->items[rcb].image[ri];
            ri++;
        }
        else if (lc->items[lcb].image[li] != rc->items[rcb].image[ri]) {
            printSegHeader(ls, rs, ERROR);
            if (run == 0)
                arun = laddr + li;
            lrun[run] = lc->items[lcb].image[li];
            rrun[run++] = rc->items[rcb].image[ri];
            li++; ri++;
        }
        else {
            emitRun(arun, run, lrun, rrun);
            run = 0;
            li++; ri++;
        }

        if (run == MAXRUN) {
            emitRun(arun, run, lrun, rrun);
            run = 0;
        }
        if (laddr >= 0 && li >= lc->items[lcb].length) {
            lcb++;
            laddr = -1;
        }
        if (raddr >= 0 && ri >= rc->items[rcb].length) {
            rcb++;
            raddr = -1;
        }

    }
    emitRun(arun, run, lrun, rrun);
}

int cmpFixup(const struct _fixup *a, const struct _fixup *b)
{
    int cmp;
    if ((cmp = a->addr - b->addr) == 0 && (cmp = a->segId < 256 ? a->segId - b->segId : pstrCmp(a->name, b->name)) == 0)
        cmp = a->type - b->type;
    return cmp;
}

void printFixup(struct _fixup *fix)
{
    if (fix->type == 1)
        printf("low ");
    else if (fix->type == 2)
        printf("high ");
    if (fix->segId < 256)
        printf("(%.*s)", fix->name[0], fix->name + 1);
    else
        printf("\"%*s\"", fix->name[0], fix->name + 1);
}

void diffFixups(module_t *lm, byte lseg, module_t *rm, byte rseg)
{
    seg_t *ls, *rs;
    fixup_t *lf, *rf;

    int li, ri;        // index into fixups

    ls = &lm->segs[lseg];
    rs = &rm->segs[rseg];
    lf = &ls->fixups;
    rf = &rs->fixups;

    /* sort content */
    qsort(lf->items, (size_t)lf->cnt, sizeof(struct _fixup), &cmpFixup);
    qsort(rf->items, (size_t)rf->cnt, sizeof(struct _fixup), &cmpFixup);

    li = ri = 0;
    while (li < lf->cnt || ri < rf->cnt) {
        if (ri >= rf->cnt || (li < lf->cnt && lf->items[li].addr < rf->items[ri].addr)) {
            printSegHeader(ls, rs, ERROR);
            printf("%04X: ", lf->items[li].addr);
            printFixup(&lf->items[li++]);
            printf(" : ------ - Fixup missing\n");
        }
        else if (li >= lf->cnt || (ri < rf->cnt && lf->items[li].addr > rf->items[ri].addr)) {
            printSegHeader(ls, rs, ERROR);
            printf("%04X: ------ :", rf->items[ri].addr);
            printFixup(&rf->items[ri++]);
            printf(" - Fixup missing\n");
        }
        else if (lf->items[li].segId != rf->items[ri].segId ||
            (lf->items[li].segId > 255 && !pstrEqu(lf->items[li].name, rf->items[ri].name)) ||
            lf->items[li].type != rf->items[ri].type) {
            printSegHeader(ls, rs, ERROR);
            printf("%04X ", lf->items[li].addr);
            printFixup(&lf->items[li++]);
            printf(" : ");
            printFixup(&rf->items[ri++]);
            printf(" - Fixup different\n");
        }
        else {
            li++;
            ri++;
        }
    }
}




int diffExternals(module_t *lm, module_t *rm, int result)
{
    int i, j;

    /* check that any unused externals match */
    for (i = 0; i < lm->externals.cnt; i++) {
        for (j = 0; j < rm->externals.cnt; j++)
            if (!(rm->externals.items[j].status & CHECKED) && pstrEqu(lm->externals.items[i].name, rm->externals.items[j].name))
                break;

        if (j == rm->externals.cnt) {
            if (result == 1) printf("\nExternals:\n");
            printPstrPair(lm->externals.items[i].name, "\x06------");
            printf(" - External missing\n");
            result = 0;
        } else
            rm->externals.items[j].status |= CHECKED;
        lm->externals.items[i].status |= CHECKED;
    }
    for (j = 0; j < rm->externals.cnt; j++)
        if (!(rm->externals.items[j].status & CHECKED)) {
            if (result == 1) printf("\nExternals:\n");
            printPstrPair("\06------", rm->externals.items[j].name);
            printf(" - External missing\n");
            result = 0;
            rm->externals.items[j].status |= CHECKED;
        }
    return result;
}


void printCompiler(int compiler, int version)
{
    switch (compiler) {
    case 1: printf("PLM_%d.%d", version / 16, version % 16); return;
    case 2: printf("FORT_%d.%d", version / 16, version % 16); return;
    case 0: if (version == 0) return;
    default: printf("%02X_%02X", compiler, version);
    }
}

int diffModule(module_t *lm, module_t *rm)
{
    int i, j;
    int result = 1;

    printf(" %.*s ", lm->name[0], lm->name + 1);
    printCompiler(lm->compiler[0], lm->compiler[1]);
    if (!pstrEqu(lm->name, rm->name)) {
        printf(" : %.*s", rm->name[0], rm->name + 1);
        result = 0;
    }

    if (lm->compiler[0] != rm->compiler[0] || lm->compiler[1] != rm->compiler[1]) {
        printf(result ? " : " : " ");
        printCompiler(rm->compiler[0], rm->compiler[1]);
    }
    if (!result)
        printf(" - Module names different\n");


    for (i = 0; i <= lm->maxSeg; i++) {
        if (lm->segs[i].status) {
            j = (i < 5 || i == 255) ? i : findSegByName(rm, lm->segs[i].name);
            if (j < 0 || rm->segs[j].status == 0) {
                  if (result) {
                    putchar('\n');
                    result = 0;
                }
                printPstrPair(lm->segs[i].name, "\x06------");
                printf(" - Segment missing\n");
                lm->segs[i].status |= ERROR + CHECKED;
            }
            else {
                if (lm->segs[i].length != rm->segs[j].length) {
                    printSegHeader(&lm->segs[i], &rm->segs[j], ERROR);
                    printf("%04X : %04X - Segment sizes different\n", lm->segs[i].length, rm->segs[j].length);
                }
                diffPublics(lm, i, rm, j);
                diffContent(lm, i, rm, j);
                diffFixups(lm, i, rm, j);


                lm->segs[i].status |= CHECKED;
                rm->segs[j].status |= CHECKED;
            }

            if (lm->segs[i].status & ERROR)
                result = 0;
        }
    }
    for (j = 0; j <= rm->maxSeg; j++)
        if (rm->segs[i].status && (rm->segs[j].status & CHECKED) == 0) {
            printPstrPair("\x06------", rm->segs[j].name);
            printf(" - Segment missing\n");
            rm->segs[j].status |= ERROR + CHECKED;
            result = 0;
        }



    if (!diffExternals(lm, rm, result))
        result = 0;

    return result;

}






void cmpModule(omf_t *lomf, omf_t *romf)
{
    module_t *lm, *rm;

    if ((lm = newModule(lomf)) == NULL || (rm = newModule(romf)) == NULL) {
        diffBinary(lomf, romf);
        if (lm) deleteModule(lm);
        return;
    }
    printf("%s : %s", lomf->name, romf->name);

    
    if (diffModule(lm, rm))
        printf(" *** Equivalent\n");
    else {
        putchar('\n');
        returnCode = 1;
    }
    deleteModule(lm);
    deleteModule(rm);
}





int main(int argc, char **argv)
{
    file_t *left, *right;


    if (argc != 3) {
        fprintf(stderr, "usage: %s file1 file2\n", argv[0]);
        exit(0);
    }
    if ((left = newFile(argv[1])) == NULL || (right = newFile(argv[2])) == NULL)
        exit(1);

    /* do a fast check to see if identical */
    if (left->size == right->size && memcmp(left->image, right->image, left->size) == 0) {
        printf("%s and %s are identical\n", left->name, right->name);
        return 0;
    }

    if (left->image[0] == LIBHDR && right->image[0] == LIBHDR)
        cmpLibrary(left, right);
    else if (left->image[0] == MODHDR && right->image[0] == MODHDR)
        cmpModule(newOMF(left, -1, 0, left->size), newOMF(right, -1, 0, right->size));
    else
        diffBinary(newOMF(left, -1, 0, left->size), newOMF(right, -1, 0, right->size));
    return returnCode;
}
