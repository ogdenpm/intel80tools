/* recipe.c     (c) by Mark Ogden 2018

DESCRIPTION
    Generates the recipe file for ISIS II and ISISI III disks.
    Part of unidsk

MODIFICATION HISTORY
    17 Aug 2018 -- original release as unidsk onto github
    18 Aug 2018 -- added attempted extraction of deleted files for ISIS II/III
    19 Aug 2018 -- Changed to use environment variable IFILEREPO for location of
                   file repository. Changed location name to use ^ prefix for
                   repository based files, removing need for ./ prefix for
                   local files
    20 Aug 2018 -- replaced len, checksum model with full sha1 checksum
                   saves files with lower case names to avoid conflict with
                   special paths e.g. AUTO, ZERO, DIR which became a potential
                   issue when ./ prefix was removed. Lower case is also compatible
                   with the thames emulator and the repository file names
    21 Aug 2018 -- Added support for auto looking up files in the repository
                   the new option -l or -L supresses the lookup i.e. local fiiles


TODO
    Review the information generated for an ISIS IV disk to see if it is sufficient
    to allow recreation of the original .imd or .img file
    Review the information generated for an ISIS III disk to see it is sufficient to
    recreate the ISIS.LAB and ISIS.FRE files.

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
    sha1 checksum of the file stored in base64
    location is where the file is stored or a special marker as follows
    AUTO - file is auto generated and the line is optional
    DIR - file was a listing file and not saved - depreciated
    ZERO - file has zero length and is auto generated
    ZEROHDR - file has zero length but header is allocated eofcnt will be set to 128
    *text - problems with the file the text explains the problem. It is ignored
    path - location of file to load a leading ^ is replaced by the repository path

if you haave ISIS files files named AUTO, DIR or ZERO please prefix the path with ./, or use lower case
for local files.

Note unidsk creates special comment lines as follows

if there are problems reading a file the message below is emitted before the recipe line
    # there were errors reading filename

if there were problems reading a delete file the message
    # file filename could not be recovered
For other deleted files the recipe line is commented out and the file is extracted to a file
with a leading # prefix

If there are aliases in the repository these are shown in one or more comments after the file
    # also path [path]*
If there is a match with the ISIS name then only alises with the same ISIS name will match
otherwise all files with the same checksum will be listed
The primary use of this is to change to prefered path names for human reading

*/


#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <string.h>

#include "unidsk.h"


struct {
    char *checksum;
    char *os;
} osMap[] = {
    "/YmatRu9vhzEjMhDylDjhykvqZo", "ISIS 2.2",
    "CDiOIW5+mmg+lMLUzBiKotg1Q58", "ISIS 3.4",
    "sRiKeMMS6BzoByW8JMfR602LkRc", "ISIS 4.0",
    "o3AUw0iiH+s7gzpbQng+QpJOpUU", "ISIS 4.1",
    "sWQzN17YlQdDPy7hXQc+2Htq9lA", "ISIS 4.2",
    "u/pkKs6Q7J2EBWs9znd6aTF7j3o", "ISIS 4.2w",
    "IT/NSStKstpz2wMCcHbGrEj0GJM", "ISIS 4.3",
    "oQXgd+7fmrQ/wH/Wfr9ptN4yK2s", "ISIS 4.3w",
    "6zffO/tlxHVojxlqs2BQvu31814", "PDS 1.0",
    "kr87fI+DkuGXQec3i2IG7S+8usI", "PDS 1.1",
    "DJ5TiAs5F4yNGlp7fzUjT/k5Zsk", "TEST 1.0",
    "7SzuQtZxju9XU/+ehbFOQ7W0tz8", "TEST 1.1",
    "yOeRj3n6yo8SYlu3Ne4L8Ci52BI", "OSIRIS 3.0",
    NULL, "NONE"
};

char *osFormat[] = { "UNKNOWN", "ISIS II SD", "ISIS II DD", "ISIS III", "ISIS IV" };





void mkRecipe(char *name, isisDir_t  *isisDir, char *comment, int diskType)
{
    FILE *fp;
    FILE *lab;
    char recipeName[_MAX_PATH];
    label_t label;
    char *dbPath;
    char *prefix;
    isisDir_t *dentry;


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

    if ((lab = fopen("isis.lab", "rb")) == 0 || fread(&label, sizeof(label), 1, lab) != 1)
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

    char *os;
    if (osIdx >= 0) {
        os = "UNKNOWN";       // default to unknown os
        for (int i = 0; osMap[i].checksum; i++)
            if (strcmp(osMap[i].checksum, isisDir[osIdx].checksum) == 0) {
                os = osMap[i].os;
                break;
            }
    }
    else
        os = "NONE";

    fprintf(fp, "os: %s\nFiles:\n", os);

    for (dentry = isisDir; dentry < &isisDir[MAXDIR] && dentry->name[0]; dentry++) {
        if (dentry->errors) {
            if (dentry->name[0] == '#') {           // if the file was deleted and has errors note it and skip
                fprintf(fp, "# file %s could not be recovered\n", dentry->name + 1);
                continue;
            }
            else {
                fprintf(fp, "# there were errors reading %s\n", dentry->name);
                strcpy(dentry->checksum, "CORRUPT");
            }
        }

        fprintf(fp, "%s,", dentry->name);
        if (dentry->attrib & 0x80) putc('F', fp);
        if (dentry->attrib & 4) putc('W', fp);
        if (dentry->attrib & 2) putc('S', fp);
        if (dentry->attrib & 1) putc('I', fp);

        prefix = "";
        if (dentry->len <= 0)
            strcpy(dentry->checksum, "NULL");

        if (strcmp(dentry->name, "ISIS.DIR") == 0 ||
            strcmp(dentry->name, "ISIS.LAB") == 0 ||
            strcmp(dentry->name, "ISIS.MAP") == 0 ||
            strcmp(dentry->name, "ISIS.FRE") == 0)
            dbPath = "AUTO";
        else if (dentry->len == 0)
            dbPath = "ZERO";
        else if (dentry->len < 0) {
            dbPath = "ZEROHDR";        // zero but link block allocated
            dentry->len = -dentry->len;
        }
        else if (dbPath = Dblookup(dentry))
            prefix = "^";
        else {
            dbPath = dentry->name;
            _strlwr(dbPath);                     // force to lower case, also avoids conflict with AUTO & ZERO, already used name
        }

        fprintf(fp, ",%s,%s%s", dentry->checksum, prefix, dbPath);
        // print the alternative names if available

        int llen = MAXOUTLINE + 1;
        while ((dbPath = Dblookup(NULL)) && *dbPath) {
            if (llen + strlen(dbPath) >= MAXOUTLINE - 1)
                llen = fprintf(fp, "\n# also");
            llen += fprintf(fp, " %s", dbPath);
        }
        putc('\n', fp);
    }
    fclose(fp);

}


