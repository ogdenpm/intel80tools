#include "omfcmp.h"

library_t *newLibrary(omf_t *omf)		// returns the list of library modules and locations
{
    word count, i;
    long loc;
    library_t *library;

    seekRecord(omf, 0);					// rewind
    if (getRecord(omf) != LIBHDR || omf->lengthRec != 7)
        return NULL;

    count = getWord(omf);				// number of modules
    loc = getLoc(omf);					// offset to names record
    if (!seekRecord(omf, loc) || getRecord(omf) != LIBNAM || getRecord(omf) != LIBLOC
        || getRecord(omf) != LIBDIC || getRecord(omf) != EOFREC) {
        omf->error = 1;
        return NULL;
    }

    library = (library_t *)xmalloc(sizeof(library_t));
    library->names = (char **)xcalloc(count, sizeof(byte *));
    library->locations = (int *)xcalloc(count + 1, sizeof(int));

    library->count = count;
    library->current = 0;

    seekRecord(omf, loc);
    getRecord(omf);
    for (i = 0; i < count; i++)
        library->names[i] = getName(omf);
    getRecord(omf);
    for (i = 0; i < count; i++)
        library->locations[i] = getLoc(omf);
    library->locations[count] = loc;	// end marker for final module
    return library;
}


void deleteLibrary(library_t *lib)
{
    free(lib->names);
    free(lib->locations);
    free(lib);

}

void cmpLibrary(file_t *left, file_t *right)
{
    library_t *ll, *rl;
    omf_t *lo, *ro;
    int outOfOrder = 0;
    int i, j, k;

    lo = newOMF(left, NULL, 0, left->size);
    ro = newOMF(right, NULL, 0, right->size);

    if ((ll = newLibrary(lo)) == NULL || (rl = newLibrary(ro)) == NULL) {
        diffBinary(lo, ro);
        if (ll) deleteLibrary(ll);
        deleteOMF(lo);
        deleteOMF(ro);
        return;
    }
    k = 0;
    for (i = 0; i < ll->count; i++) {
        for (j = 0; j < rl->count; j++) {
            if (pstrEqu(ll->names[i], rl->names[k]))
                break;
            k = (k + 1) % rl->count;
        }
        if (j == rl->count) {
            printf("%.*s only in %s\n", ll->names[i][0], ll->names[i] + 1, left->name);
            returnCode = 1;
        }
        else {
            deleteOMF(lo);
            deleteOMF(ro);
            lo = newOMF(left, ll->names[i], ll->locations[i], ll->locations[i + 1]);
            ro = newOMF(right, rl->names[k], rl->locations[k], rl->locations[k + 1]);
            if (lo->size == ro->size && (lo->image, ro->image, lo->size) == 0)
                printf("%*s identical to %*s\n", lo->name[0], lo->name + 1, ro->name[0], ro->name + 1);
            else
                cmpModule(lo, ro);
        }
    }
    k = 0;
    for (i = 0; i < rl->count; i++) {
        for (j = 0; j < ll->count; j++) {
            if (pstrEqu(rl->names[i], ll->names[k]))
                break;
            k = (k + 1) % ll->count;
        }
        if (j == ll->count) {
            printf("%.*s only in %s\n", rl->names[i][0], rl->names[i] + 1, right->name);
            returnCode = 1;
        }
    }
    deleteOMF(lo);
    deleteOMF(ro);
    deleteLibrary(ll);
    deleteLibrary(rl);

}
