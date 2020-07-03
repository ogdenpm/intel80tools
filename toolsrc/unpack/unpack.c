#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <direct.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_LINE    512

// nested files are added to the end of the list below
// this allows for multiple levels of nesting
typedef struct _nested {
    struct _nested *next;
    char *file;
} nested_t;
nested_t *nestedFiles = NULL;


void addNestedFile(char *file) {
    nested_t *p = malloc(sizeof(nested_t));
    p->next = NULL;
    p->file = strdup(file);
    if (nestedFiles == NULL)
        nestedFiles = p;
    else {
        nested_t *q;
        for (q = nestedFiles; q->next; q = q->next)
            ;
        q->next = p;
    }
}



char *defaultIn() {
    char path[_MAX_PATH + 1];
    static char src[_MAX_FNAME];
    if (_getcwd(path, _MAX_PATH + 1) == NULL) {
        fprintf(stderr, "problem finding current directory\n");
        exit(1);
    }
    char *parent = path;
    char *s;
    while (s = strpbrk(parent, "\\/:"))
        parent = s + 1;
    strcpy(src, parent);
    strcat(src, "_all.src");
    return src;
}

FILE *newFile(char *parent, char *fname) {
    char path[_MAX_PATH];
    FILE *fp;

    if (*fname == '/' || *fname == '\\')        // rooted filename
        strcpy(path, fname);
    else {
        strcpy(path, parent);                   // assume start directory is same as parent
        char *fpart = path;
        for (char *s = path; s = strpbrk(s, "\\/"); s++)
            fpart = s + 1;
        strcpy(fpart, fname);
    }

    if ((fp = fopen(path, "wt")) == NULL) {     // try to create
        // see if the problem is missing directories
        for (char *s = path; s = strpbrk(s, "\\/"); s++) {
            *s = 0;
            mkdir(path);
            *s = '/';
        }
        fp = fopen(path, "wt");
    }
    if (fp == NULL) {
        fprintf(stderr, "can't create %s\n", path);
        fp = stdout;
    }
    return fp;
}


void unpack(char *fname) {
    char line[MAX_LINE];
    char curfile[_MAX_PATH];
    bool isnested = false;
    FILE *fpin;
    FILE *fpout = stdout;
    *curfile = 0;
    if ((fpin = fopen(fname, "rt")) == NULL) {
        fprintf(stderr, "can't open %s\n", fname);
        return;
    }
    while (fgets(line, MAX_LINE, fpin)) {
        if (*line == '\f') {
            if (line[1] == '?') {
                if (!isnested) {
                    addNestedFile(curfile);
                    isnested = true;
                }
                putc('\f', fpout);
                fputs(line + 2, fpout);
            } else {
                if (fpout != stdout)
                    fclose(fpout);
                char *s;
                for (s = line + 1; s = strchr(s, '`'); s++)   // replace escaped spaces
                    *s = ' ';
                if (s = strchr(line + 1, '\n'))
                    *s = 0;
                strcpy(curfile, line + 1);
                fpout = newFile(fname, curfile);
                isnested = false;
            }
        } else
            fputs(line, fpout);
    }
    if (fpout != stdout)
        fclose(fpout);
    fclose(fpin);
}





int main(int argc, char **argv) {
    char *infile;
    bool recurse = false;

    while (--argc > 0 && **++argv == '-') {
        if (strcmp(*argv, "-r") == 0)
            recurse = true;
        else
            fprintf(stderr, "ignoring unknown option %s\n", *argv);
    }
    if (argc > 1) {
        fprintf(stderr, "usage: unpack [-r] [file]\n");
        exit(1);
    }
    if (argc == 1)
        infile = *argv;
    else
        infile = defaultIn();
    unpack(infile);
    if (recurse)
        while (nestedFiles) {   // rely on OS to clean up memory
            unpack(nestedFiles->file);
            nestedFiles = nestedFiles->next;
    }
}