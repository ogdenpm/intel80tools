#pragma once

typedef struct {
    char *name;
    int size;
    byte *image;
} file_t;

file_t *newFile(char *fn);