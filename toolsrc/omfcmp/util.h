#pragma once

void *xmalloc(size_t len);
void *xrealloc(void *buf, size_t len);
void *xcalloc(size_t len, size_t size);
int pstrEqu(byte *s, byte *t);
int pstrCmp(byte *s, byte *t);