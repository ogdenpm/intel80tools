#pragma once

typedef struct {
    int count;
    int current;
    byte **names;
    int *locations;
} library_t;

library_t *newLibrary(omf_t *omf);
void deleteLibrary(library_t *lib);
void cmpLibrary(file_t *left, file_t *right);

