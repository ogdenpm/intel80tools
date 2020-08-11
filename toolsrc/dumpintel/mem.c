#include "omf.h"
/* string storage */
#define STRCHUNK   2048

typedef struct str {
    struct str *next;
    size_t pos;
    char buf[STRCHUNK];
} str_t;

static str_t strings;

static index_t indexLists[7];
uint8_t const *indexKeys = "lbesogt";     // string used to index int the list for names
// 


void resetStrings() {
    str_t *q;
    for (str_t *p = strings.next; p; p = q) {
        q = p->next;
        free(p);
    }
    strings.pos = 0;
}


char const *xstrdup(char const *s) {
    size_t len = strlen(s) + 1;
    str_t *p;
    for (p = &strings; len + p->pos > STRCHUNK; p = p->next)
        if (!p->next)
            p->next = (str_t *)calloc(1, sizeof(str_t));
    char *newstr = p->buf + p->pos;
    strcpy(newstr, s);
    p->pos += len;
    return newstr;
}

void *xmalloc(size_t size) {
    void *p = malloc(size);
    if (!p) {
        fprintf(stderr, "Fatal: out of memory\n");
        exit(1);
    }
    return p;
}

void *xrealloc(void *block, size_t size) {
    void *p = realloc(block, size);
    if (!p) {
        fprintf(stderr, "Fatal: out of memory\n");
        exit(1);
    }
    return p;
}

void *xcalloc(size_t count, size_t size) {
    void *p = calloc(count, size);
    if (!p) {
        fprintf(stderr, "Fatal: out of memory\n");
        exit(1);
    }
    return p;
}


void extendIndex(index_t *p, uint16_t size) {
    uint16_t oldSize = p->size;
    p->names = xrealloc((void *)p->names, size * sizeof(char *));
    p->size = size;
    if (size > oldSize)
        memset((void *)(p->names + oldSize), 0, (size - oldSize) * sizeof(char *));
}

index_t *initIndex(uint8_t idx, uint8_t *label, uint16_t index, uint16_t size) {
    index_t *p = selectIndex(idx);
    p->label = label;
    if (size == 0) {
        if (p->names)
            free((void *)p->names);
        p->names = NULL;
    } else {
        p->names = xrealloc((void *)p->names, size * sizeof(char *));
        memset((void *)p->names, 0, size * sizeof(char *));
    }
    p->size = size;
    p->index = index;
    return p;
}

index_t *selectIndex(uint8_t type) {
    char *s = strchr(indexKeys, type);
    if (s)
        return &indexLists[s - indexKeys];
    return NULL;
}

uint16_t getAllocatedIndex(uint8_t type) {
    index_t *pIndex = selectIndex(type);
    if (!pIndex)
        return 0xffff;
    return pIndex->index - 1;            // compensate as we store one higher
}
