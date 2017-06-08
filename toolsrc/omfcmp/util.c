#include "omfcmp.h"

void *xmalloc(size_t len)
{
    void *result = malloc(len);
    if (result == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    return result;
}

void *xrealloc(void *buf, size_t len)
{
    void *result = (void *)realloc(buf, len);
    if (result == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    return result;
}

void *xcalloc(size_t len, size_t size)
{
    void *result = calloc(len, size);
    if (result == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    return result;
}


int pstrEqu(byte *s, byte *t)
{
    return (*s == *t && strncmp(s + 1, t + 1, *s) == 0);
}

int pstrCmp(byte *s, byte *t)
{
    int cmp;
    if ((cmp = strncmp(s + 1, t + 1, *s <= *t ? *s : *t)) == 0)
        cmp = *s - *t;
    return cmp;
}