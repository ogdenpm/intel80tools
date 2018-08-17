#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <string.h>

#include "unidsk.h"


struct {
    int checksum;
    char *os;
} osMap[] = {
    24037, "ISIS 2.2",
    43778, "ISIS 3.4",
    57037, "ISIS 4.0",
    63738, "ISIS 4.1",
    40129, "ISIS 4.2",
    21906, "ISIS 4.2w",
    41763, "ISIS 4.3",
    27410, "ISIS 4.3w",
    13328, "PDS 1.0",
    10768, "PDS 1.1",
    40937, "TEST 1.0",
    43740, "TEST 1.1",
    53568, "OSIRIS 3.0",
    0, "NONE"
};

char *osFormat[] = { "UNKNOWN", "ISIS II SD", "ISIS II DD", "ISIS III", "ISIS IV" };


bool Dblookup(isisDir_t *entry, char *path, char **alt) {
    return false;
}


void mkRecipe(char *name, isisDir_t  *isisDir, char *comment, int diskType)
{
    FILE *fp;
    FILE *lab;
    char recipeName[_MAX_PATH];
    label_t label;
    char dbPath[_MAX_PATH];
    char *alt;
    int i;

    strcpy(recipeName, "@");
    strcat(recipeName, name);

    if ((fp = fopen(recipeName, "wt")) == NULL) {
        fprintf(stderr, "can't create recipe file\n");
        return;
    }
    if (*comment) {
        while (*comment) {
            fputs("# ", fp);
            while (*comment && *comment != '\n')
                putc(*comment++, fp);
            if (*comment)
                putc(*comment++, fp);
        }
    }
    else
        fprintf(fp, "# %s\n", name);

    if ((lab = fopen("ISIS.LAB", "rb")) == 0 || fread(&label, sizeof(label), 1, lab) != 1)
        fputs("label:\nversion:\n", fp);
    else {
        fputs("label:", fp);
        if (label.name[0]) {
            fprintf(fp, " %.6s", label.name);
            if (label.name[6])
                fprintf(fp, "-%.3s", label.name + 6);
        }
        fputs("\nversion:", fp);
        if (label.version[0])
            fprintf(fp, " %.2s", label.version);
    }

    fprintf(fp, "\nformat: %s\n", osFormat[diskType]);     // the disk format
    if (lab && diskType == ISIS_SD && strncmp(label.fmtTable, "1<6", 3) ||  // nonstandard skew suffix
        diskType == ISIS_DD && strncmp(label.fmtTable, "145", 3))
        fprintf(fp, "skew: %.3s\n", label.fmtTable);

    for (i = 0; osMap[i].checksum; i++)
        if (osMap[i].checksum == osChecksum)
            break;

    fprintf(fp, "os: %s\nFiles:\n", osMap[i].os);

    for (int i = 0; i < MAXDIR && isisDir[i].name[0]; i++) {
        fprintf(fp, "%s,", isisDir[i].name);
        if (isisDir[i].attrib & 0x80) putc('F', fp);
        if (isisDir[i].attrib & 4) putc('W', fp);
        if (isisDir[i].attrib & 2) putc('S', fp);
        if (isisDir[i].attrib & 1) putc('I', fp);

        alt = NULL;
        if (isisDir[i].len == 0)
            strcpy(dbPath, "ZERO");
        else if (strcmp(isisDir[i].name, "ISIS.DIR") == 0 ||
            strcmp(isisDir[i].name, "ISIS.LAB") == 0 ||
            strcmp(isisDir[i].name, "ISIS.MAP") == 0 ||
            strcmp(isisDir[i].name, "ISIS.FRE") == 0)
            strcpy(dbPath, "AUTO");
        else if (!Dblookup(&isisDir[i], dbPath, &alt)) {
            strcpy(dbPath, "./");
            strcat(dbPath, isisDir[i].name);
        }

        fprintf(fp, ",%d,%d,%s\n", isisDir[i].len, isisDir[i].checksum, dbPath);
        // print the alternative names if available
        // alt is a concatanated set of c strings terminated with an additional 0 to mark the end
        if (alt) {
            while (strlen(alt)) {
                int llen = fprintf(fp, (*dbPath == '.') ? "# consider %s\n" : "# also %s\n", alt);
                alt = strchr(alt, 0) + 1;
                while (*alt && strlen(alt) + llen < MAXLINE - 1) {
                    llen += fprintf(fp, " %s", alt);
                    alt = strchr(alt, 0) + 1;
                }
                putc('\n', fp);
            }

        }
    }
    fclose(fp);

}


