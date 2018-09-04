#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <memory.h>
#include <ctype.h>
#include "flux.h"
#include "zip.h"


int debug = 0;      // debug level
int histLevels = 0;
bool showSectorMap = false;
char curFile[_MAX_PATH + _MAX_FNAME + _MAX_EXT + 3];    // core file name + possibly [zip file entry]
/*
 * Display error message and exit
 */
void error(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);
    exit(1);
}

void logger(int level, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    if (debug >= level) {
        printf("File %s - ", curFile);
        vprintf(fmt, args);
    }
}






_declspec(noreturn) void usage() {
    error("usage: idd2imd [-d[n]] -h[n] [zipfile|rawfile]+\n"
        "-d sets debug. n is optional level 0,1,2,3\n"
        "-h displays flux histogram. n is optional number of levels\n"
        "-s display sector mapping\n"
        "No IMD file is created for non zip files\n");
}
int main(int argc, char **argv) {
    int arg;
    int optCnt, optVal;
    char optCh;

    for (arg = 1; arg < argc && (optCnt = sscanf(argv[arg], "-%c%d", &optCh, &optVal)); arg++) {
        switch (tolower(optCh)) {
        case 'd':
            debug = optCnt == 2 ? optVal : MINIMAL; break;
        case 'h':
            histLevels = (optCnt == 2 && optVal > 5) ? optVal : 10; break;
        case 's':
            showSectorMap = true; break;
        default:
            usage();
        }
    }
    if (arg == argc)
        usage();

    for (; arg < argc; arg++) {
        struct zip_t *zip;
        byte *buf;
        size_t bufsize;
        char dir[_MAX_DIR];
        char drive[_MAX_DRIVE];
        char fname[_MAX_FNAME];
        char ext[_MAX_EXT];
        char imdFile[_MAX_PATH];
        FILE *fp;

        _splitpath_s(argv[arg], drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
        _makepath_s(imdFile, _MAX_PATH, drive, dir, fname, ".imd"); // create the default name
        sprintf(curFile, "%s%s", fname, ext);

        if (_stricmp(ext, ".raw") == 0) {
            if ((fp = fopen(argv[arg], "rb")) == NULL)
                logger(ALWAYS, "Can't open %s\n");
            else {
                resetIMD();
                fseek(fp, 0, SEEK_END);
                bufsize = ftell(fp);
                rewind(fp);
                buf = (byte *)xalloc(NULL, bufsize);
                if (fread(buf, bufsize, 1, fp) != 1)
                    logger(ALWAYS, "Failed to load %s\n");
                else {
                    readFluxBuffer(buf, bufsize);         // load in the flux data from buffer extracted from zip file
                    flux2track();
                    if (histLevels)
                        displayHist(histLevels);
                }
                fclose(fp);
            }
        }
        else if (_stricmp(ext, ".zip") != 0)
            usage();
        else if ((zip = zip_open(argv[arg], 0, 'r')) == NULL)
            logger(ALWAYS, "Can't open\n");
        else {
            resetIMD();
            int n = zip_total_entries(zip);
            for (int i = 0; i < n; i++) {
                zip_entry_openbyindex(zip, i);
                if (!zip_entry_isdir(zip)) {
                    size_t len;
                    int trk, sec;
                    const char *name = zip_entry_name(zip);
                    if ((len = strlen(name)) >= 8 && sscanf(name + len - 8, "%2d.%d.raw", &trk, &sec) == 2 && sec == 0 && trk < 77) {
                        sprintf(curFile, "%s%s[%s]", fname, ext, name);
                        bufsize = (size_t)zip_entry_size(zip);
                        if (!(buf = (byte *)calloc(1, bufsize)))
                            error("Fatal Error - Out of memory\n");

                        zip_entry_noallocread(zip, (void *)buf, bufsize);
                        readFluxBuffer(buf, bufsize);         // load in the flux data from buffer extracted from zip file
                        flux2track();
                        if (histLevels)
                            displayHist(histLevels);
                        free(buf);
                    }
                }
                zip_entry_close(zip);
            }
            zip_close(zip);
            WriteImgFile(imdFile, "Created by idd2imd\n");
        }
    }
    return 0;
}

