/* mkIsisDisk.c     (c) by Mark Ogden 2018

DESCRIPTION
    Creates ISIS II format disk images in .imd or .img format using a recipe file
    Supports:
    * SD and DD formats and will apply both sector and track interleave as required
    * non standard values for disk formatting byte
    Portions of the code are based on Dave Duffield's imageDisk sources

MODIFICATION HISTORY
    17 Aug 2018 -- original release as mkidsk onto github
    18 Aug 2018 -- updated to use non standard skew information and disk formatting byte

LIMITATIONS
    There are three cases where mkidsk will not create an exact image, although physical
    disks create from these images should perform without problem.
    1) The disk used for the original image had files added and deleted. In this case
       the residual data from the deleted files will be missing in the image.
       Also deleted files make cause some files be created in different isis.dir slots
       and with different link block information.
    2) The default disk images are created without interleave and using 0xc7 as the
       initialisation byte for formating.
       Sector interleave according to the ISIS.LAB information can be applied as can
       the inter track interleave used in the isis formatting utilities.
       Also the 0xC7 byte can be changed e.g. to 0xe5 or 0.
       Despite this flexibility for the .imd disks there may be differences because IMD
       sometimes incorrectly detects the first sector. Use Dave Duffield's MDU utility
       to see this.
    3) If the comment information is changed, the IMD files will be different although
       disks created will not.

 NOTES
    This version relies on visual studio's pack pragma to force structures to be byte
    aligned.
    An alternative would be to use byte arrays and index into these to get the relevant
    data via macros or simple function. This approach would also support big edian data

TODO
    Add support for ISIS III, this depends on understanding the format of ISIS.LAB and
    ISIS.FRE for ISIS III


RECIPE file format
The following is the current recipe format, which has change a little recently
with attributes split out and new locations behaviour

initial comment lines
    multiple comment lines each beginning a # and a space
    these are included in generated IMD files and if the first comment starts with IMD
    then this is used as the signature otherwise a new IMD signature is generated with
    the current date /time
information lines which can occur in any order, although the order below is the default
and can be separated with multiple comment lines starting with #. These comments are ignored
    label: name[.|-]ext     Used in ISIS.LAB name has max 6 chars, ext has max 3 chars
    version: nn             Up to 2 chars used in ISIS.LAB
    format: diskFormat      ISIS II SD, ISIS II DD or ISIS III
    skew:  skewInfo         optional non standard skew info taken from ISIS.LAB. Rarely needed
    os: operatingSystem     operating system on disk. NONE or ISIS ver, PDS ver, OSIRIS ver
marker for start of files
    Files:
List of files to add to the image in ISIS.DIR order. Comment lines starting with # are ignored
each line is formated as
    IsisName,attributes,len,checksum,srcLocation
where
    isisName is the name used in the ISIS.DIR
    attibutes are the file's attributes any of FISW
    len is the file's length
    checksum is the file's checksum
    location is where the file is stored or a special marker as follows
    AUTO - file is auto generated and the line is optional
    DIR - file was a listing file and not saved - depreciated
    ZERO - file has zero length and is auto generated
    ZEROHDR - file has zero length but header is allocated eofcnt will be set to 128
    *text - problems with the file the text explains the problem. It is ignored
    path - location of file to load.
           if path begins with a . then is is relative to current directory e.g. ./VT100.MAC
           else it is relative to the file repository e.g. Intel80/aedit_1.0/vt100.mac

*/

#include "mkIsisDisk.h"


char comment[MAXCOMMENT];
char root[_MAX_PATH];
byte diskType = ISIS_DD;
label_t label;
bool hasSystem = true;
bool interleave = false;
bool interTrackInterleave = false;
byte formatCh = FMTBYTE;                     // default format character -e overrides

char *special[] = { "AUTO", "DIR", "ZERO", "ZEROHDR", NULL };


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
        }
        else if ((cnt = Match(line, "format:"))) {
            s = line + cnt;
            if ((Match(s, "ISIS II SD")))
                diskType = ISIS_SD;
            else if ((Match(s, "ISIS II DD")))
                diskType = ISIS_DD;
            else {
                fprintf(stderr, "Unsupported disk format %s\n", s);
                exit(1);
            }
        } else if ((cnt = Match(line, "skew:"))) {      // we have non standard skew
            s = line + cnt;
            if ('1' <= s[0] && s[0] <= 52 + '0' &&
                '1' <= s[1] && s[1] <= 52 + '0' &&
                '1' <= s[2] && s[2] <= 52 + '0')
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
                fprintf(stderr, "%s - skipped because %s\n", line, path + 1);
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
        "usage: mkidsk [options]* [-]recipe [diskname][.fmt]\n"
        "where\n"
        "recipe     file with instructions to build image. Optional - prefix if name starts with @\n"
        "diskname   generated disk image name - defaults to recipe without leading @\n"
        "fmt        disk image format either .img or imd - defaults to " EXT "\n"
        "\noptions are\n"
        "-f[nn]     override C7 format byte with E5 or hex value specified by nn\n"
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
    int fmtCh;

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
        case 'f':
            if (sscanf(argv[i] + 2, "%x", &fmtCh) != 1)
                fmtCh = ALT_FMTBYTE;
            formatCh = (byte)fmtCh;
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