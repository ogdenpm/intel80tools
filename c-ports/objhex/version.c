#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include "Generated/version.h"

// use the following function declaration in the main code
void showVersion(FILE *fp, bool full);

#ifdef GIT_APPID
#define APPNAME GIT_APPID
#else
#define APPNAME GIT_APPDIR
#endif

void showVersion(FILE *fp, bool full) {

    fputs(APPNAME " " GIT_VERSION, fp);
#ifdef _DEBUG
    fputs(" {debug}", fp);
#endif
    fputs("  (C)" GIT_YEAR " Mark Ogden\n", fp);
    if (full) {
        fprintf(fp, "%d bit target - Git: %s [%.10s]", (int)sizeof(void *) * CHAR_BIT, GIT_SHA1, GIT_CTIME);
#if GIT_BUILDTYPE == 2
        fputs(" +uncommitted files", fp);
#elif GIT_BUILDTYPE == 3
        fputs(" +untracked files", fp);
#endif
        fputc('\n', fp);
    }
}
