#include "omfcmp.h"

file_t *newFile(char *fn)
{
    FILE *fp;
    long fileSize;
    file_t *fi = NULL;

    if ((fp = fopen(fn, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", fn);
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    if ((fileSize = ftell(fp)) <= 0)
        fprintf(stderr, "error reading %s\n", fn);
    else {
        rewind(fp);
        fi = (file_t *)xmalloc(sizeof(file_t));
        fi->image = (byte *)xmalloc(fileSize);

        if (fread(fi->image, 1, fileSize, fp) != fileSize) {
            fprintf(stderr, "error reading %s\n", fn);
            free(fi->image);
            free(fi);
            fi = NULL;
        }
        else {
            fi->size = fileSize;
            fi->name = fn;
        }
    }
    fclose(fp);
    return fi;
}
