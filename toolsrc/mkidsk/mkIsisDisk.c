#include "mkIsisDisk.h"

#define MAXLINE 512
#define ROOT "E:/OneDrive/Intel/"        // my local copy of the repository - to use if recipe is not in .../Intel/diskindex/
#define DISKINDEX   "diskIndex"
#define EXT ".imd"                      // default extension and hence format
#define MAXCOMMENT	4096				// super large comment

char comment[MAXCOMMENT];
char root[_MAX_PATH];
byte diskType = ISIS_DD;
label_t label;
bool hasSystem = true;
bool interleave = false;
bool interTrackInterleave = false;


char *special[] = { "AUTO", "DIR", "ZERO", NULL };


void InitFmtTable(byte t0Interleave, byte t1Interleave, byte interleave) {
    label.fmtTable[0] = t0Interleave + '0';
    label.fmtTable[1] = t1Interleave + '0';
    for (int i = 2; i < TRACKS; i++)
        label.fmtTable[i] = interleave + '0';
}

int Match(char *s, char *ref) {
    char *start = s;
    while (*ref && toupper(*s) == toupper(*ref)) {
        if (isblank(*ref))
            while (isblank(*++s))
                s++;
        else
            s++;
        ref++;
    }
    if (*ref || (*s && !isblank(*s)))
        return 0;
    while (isblank(*s))
        s++;
    return (int) (s - start);
}

void ParseRecipeHeader(FILE *fp) {
    char line[MAXLINE];
    char *s;
    int cnt;
    int c;
    bool pastComment = false;

    while (fgets(line, MAXLINE, fp)) {
        s = line;
        if (*s == '#') {
            if (!pastComment) {
                if (*++s == ' ')
                    s++;
                if (strlen(comment) + strlen(s) < MAXCOMMENT)	// ignore if comment too long
                    strcat(comment, s);
            }
            continue;
        }
        pastComment = true;

        if (s = strchr(line, '\n'))
            *s = 0;
        else if (strlen(line) == MAXLINE - 1) {
            fprintf(stderr, "line too long %s\n", line);
            while ((c = getc(fp)) != EOF && c != '\n')
                ;
            if (c == EOF)
                return;
            else
                continue;
        }
        if (Match(line, "Files:"))
            return;
        if (Match(line, "#"))
            continue;
        if ((cnt = Match(line, "label:"))) {
            s = line + cnt;
            for (int i = 0; i < 6; i++)
                if (isalnum(*s))
                    label.name[i] = toupper(*s++);
            if (*s == '.' || *s == '-')
                s++;
            for (int i = 6; i < 9; i++)
                if (isalnum(*s))
                    label.name[i] = toupper(*s++);
        } else if ((cnt = Match(line, "version:"))) {
            s = line + cnt;
            for (int i = 0; i < 2; i++)
                if (isalnum(*s) || *s == '.')
                    label.version[i] = toupper(*s++);
        } else if ((cnt = Match(line, "format:"))) {
            s = line + cnt;
            if ((cnt = Match(s, "ISIS II SD")))
                diskType = ISIS_SD;
            else if ((cnt = Match(s, "ISIS II DD")))
                diskType = ISIS_DD;
            else {
                fprintf(stderr, "Unsupported disk format %s\n", s);
                exit(1);
            }
            s += cnt;        // skip the core format info
            
            if ('1' <= s[0] && s[0] <= 26 + '0' &&
                '1' <= s[1] && s[1] <= 26 + '0' &&
                '1' <= s[2] && s[2] <= 26 + '0') // we have user specified skew
                InitFmtTable(s[0] - '0', s[1] - '0', s[2] - '0');
        } else if (Match(line, "os: NONE"))
            hasSystem = false;
    }
}

char *ParseIsisName(char *isisName, char *src) {
    while (isblank(*src))       // ignore leading blanks
        src++;
    for (int i = 0; i < 6 && isalnum(*src); i++)
        *isisName++ = toupper(*src++);
    if (*src == '.') {
        *isisName++ = *src++;
        for (int i = 0; i < 3 && isalnum(*src); i++)
            *isisName++ = toupper(*src++);
    }
    *isisName = '\0';
    return src;
}

char *ParseAttributes(char *src, int *attributep) {
    int attributes = 0;
    while (*src && *src != ',') {
        switch (toupper(*src)) {
        case 'I': attributes |= INV; break;
        case 'W': attributes |= WP; break;
        case 'S': attributes |= SYS; break;
        case 'F': attributes |= FMT; break;
        default:
            fprintf(stderr, "Warning bad attribute %c\n", *src);
        }
        src++;
    }
    *attributep = attributes;
    return src;
}

// the ParsePath return codes
enum {
    PATHOK = 1,
    DIRLIST = 2,
    IGNORE = 3,
    TOOLONG = 4

};


void ParsePath(char *path, char *src, char *isisName) {
    char *s;

    if ((s = strchr(src, ',')))    // remove any additional field
        *s = 0;

    while (isblank(*src))           // remove any leading space;
        src++;

    if (!*src) {
        strcpy(path, "*missing path");
        return;
    }

    for (int i = 0; special[i]; i++) {
        if (strcmp(src, special[i]) == 0) {
            strcpy(path, src);
            return;
        }

    }
    if (*src == '*') {				// special entry
        strcpy(path, strlen(src) < _MAX_PATH ? src : "*bad special path");
        return;
    }

    if (*src == '.') {				// local relative path
        strcpy(path, strlen(src) < _MAX_PATH ? src : "*path too long");
        return;
    }


    if (strlen(root) + strlen(src) >= _MAX_PATH) {
        strcpy(path, "*path too long");
        return;
    }
    strcpy(path, root);
    strcat(path, src);
}

void ParseFiles(FILE *fp) {
    char line[MAXLINE];
    char isisName[11];
    char path[_MAX_PATH];
    int attributes;
    char *s;
    int c;

    while (fgets(line, MAXLINE, fp)) {
        if (line[0] == '#')
            continue;

        if (s = strchr(line, '\n'))
            *s = 0;
        else if (strlen(line) == MAXLINE - 1) { // if full line then we overran - otherwise unterminated last line
            fprintf(stderr, "%s - line truncated\n", line);
            while ((c = getc(fp)) != EOF && c != '\n')
                ;
            continue;
        }


        s = ParseIsisName(isisName, line);                      // get the isisFile
        if (isisName[0] == '\0' || *s != ',') {
            if (*s)
                fprintf(stderr, "%s - invalid ISIS file name\n", line);
            else
                fprintf(stderr, "%s - missing attributes\n", line);
            continue;
        }
        s = ParseAttributes(++s, &attributes);
        if (*s != ',') {
            fprintf(stderr, "%s - missing file length\n", line);
            continue;
        }

        if ((s = strchr(s + 1, ',')) && (s = strchr(s + 1, ','))) { // path pointer
            ParsePath(path, s + 1, isisName);
            if (*path == '*')
                fprintf(stderr, "%s - %s\n", line, path + 1);
            else if (strcmp(path, "DIR") == 0)
                printf("%s - DIR listing not created\n", line);
            else
                CopyFile(isisName, path, attributes);
        }
        else
            fprintf(stderr, "%s - missing path information\n", line);
    }

}

void usage() {
    static int shown = 0;
    if (shown)
        return;
    shown++;
    printf(
        "mkIsisDisk [options]* [-]recipe [diskname][.fmt]\n"
        "where\n"
        "recipe     file with instructions to build image. Optional - prefix is name starts with @\n"
        "diskname   generated disk image name - defaults to recipe without leading @\n"
        "fmt        disk image format either .img or imd - defaults to " EXT "\n"
        "\noptions are\n"
        "-h         displays this help info\n"
        "-i         interleave disk image\n"
        "-t         apply inter track interleave\n"
    );
}

void main(int argc, char **argv) {
    FILE *fp;
    int i;
    char *recipe;
    char outfile[_MAX_PATH + 4] = "";
    char *s;
    char *diskname;

    for (i = 1; i < argc; i++) {
        if (argv[i][0] != '-' || argv[i][1] == '@')
            break;
        switch (tolower(argv[i][1])) {
        default:
            fprintf(stderr, "unknown option %s\n", argv[i]);
        case 'h': usage(); break;
        case 'i':
            interleave = true;
            break;
        case 't':
            interTrackInterleave = true;
            break;
        }
    }
    if (i != argc - 1  &&  i != argc - 2) {                 // allow for recipe and optional diskname
        usage();
        exit(1);
    }
    recipe = argv[i][0] == '-' ? argv[i] + 1 : argv[i];    // allow - prefix to @filename for powershell
    if (strlen(recipe) >= _MAX_PATH) {
        fprintf(stderr, "recipe name too long\n");
        exit(1);
    }
    diskname = ++i == argc - 1 ? argv[i] : EXT;            // use disk and ext if given else just assume default fmt
    if (*diskname != '.') {                            // we have a file name
        if (strlen(diskname) >= _MAX_PATH) {           // is it sensible?
            fprintf(stderr, "disk name too long\n");
            exit(1);
        }
        strcpy(outfile, diskname);
    } else {
        strcpy(outfile, *recipe == '@' ? recipe + 1 : recipe);      // apply the default name;
    }
    if ((s = strrchr(diskname, '.')) && (_stricmp(s, ".imd") == 0 || _stricmp(s, ".img") == 0)) {
        if (s == diskname)                           // we had .fmt only so add
            strcat(outfile, s);
    } else
        strcat(outfile, EXT);                          // we didn't have ext

    if (strlen(outfile) >= _MAX_PATH) {
        fprintf(stderr, "disk name too long\n");
        exit(1);
    }

    // probe to find root - currently this code is windows only
    if (_fullpath(root, recipe, _MAX_PATH)) {
        s = strchr(root, 0);                        // to end of path;
        while (s > root && *s != '\\')
            s--;
        *s-- = 0;                                   // remove \ file name
        while (s >= root && *s != '\\')
            s--;
        if (_stricmp(s + 1, DISKINDEX) == 0)
            *s = 0;
        else
            *root = 0;
    }
    if (!*root)                                 //  make sure we have a root
        strcpy(root, ROOT);
    strcat(root, "/");

    fp = fopen(recipe, "rt");
    ParseRecipeHeader(fp);
    FormatDisk(diskType);
    WriteDirectory();
    WriteLabel();
    ParseFiles(fp);
    fclose(fp);
    WriteImgFile(outfile, interleave ? label.fmtTable : NULL, comment);



}