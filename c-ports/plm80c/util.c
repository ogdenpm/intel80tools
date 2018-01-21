#include "plm.h"
#include <stdio.h>
#include <stdlib.h>
#define LOWMEM  0x613F      // plm lowest value of MEMORY
#define HIGHMEM 0xf7ff

static pointer memory;
offset_t MEMORY;

void initMem()
{
    if ((memory = (pointer)malloc(HIGHMEM - LOWMEM + 1)) == NULL) {
        fprintf(stderr, "can't allocate memory\n");
        exit(2);
    }
}

offset_t MemCk()
{
    return HIGHMEM;
}

offset_t ptr2Off(pointer p)
{
    if (p == 0)     // allow NULL
        return 0;
    if (p < memory || memory + HIGHMEM < p) {
        fprintf(stderr, "out of range memory ref %p\n", p);
        exit(3);
    }
    return (offset_t)(p - memory + LOWMEM);
}

pointer off2Ptr(offset_t off)
{
    if (off == 0)       // allow NULL
        return 0;
    if (off < LOWMEM || HIGHMEM < off) {
        fprintf(stderr, "out of range offset %04X\n", off);
        exit(4);
    }
    return memory + off - LOWMEM;
}