#pragma once
#include "omfcmp.h"


typedef struct {
    char *name;
    int size;
    byte *image;
    int startRec;
    int lengthRec;
    int pos;
    int error;
} omf_t;


omf_t *newOMF(file_t *fi, char *mod, int start, int end);
void deleteOMF(omf_t *omf);

int seekRecord(omf_t *omf, int pos);
byte getByte(omf_t *omf);
byte *getName(omf_t *omf);
word getWord(omf_t *omf);
int getLoc(omf_t *omf);
int atEndOfRecord(omf_t *omf);
byte getRecord(omf_t *omf);