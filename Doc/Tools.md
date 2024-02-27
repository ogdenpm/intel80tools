# Reverse engineering tools

The notes below document the tools I have developed for reverse engineering.

All script (perl & windows cmd) is available in the tools directory and except for ngenpex all of the application source code is available in the [c-ports](https://github.com/ogdenpm/c-ports) or [tool-src](https://github.com/ogdenpm/tool-src) repositories. The relevant repository is noted in the individual headings below.

The version management scripts (fileVer.cmd, install.cmd, revisions.cmd & version.cmd) are mastered in the [versionTools](https://github.com/ogdenpm/versionTools) repository which also contains details on how they work.

Note most tools now support being invoked with -v and -V. The lower case version shows simple version information, the uppercase version provides additional git information to help identify the version. Both of these should be the only option on the command line.

### abstool (tool-src)

This is a general purpose tool for converting between a number of absolute file formats and optionally applying patches. It subsumes obj2bin, which is now depreciated.

```
Usage:
abstool [-v|-V|-h] |  [-l addr] [-a|-a51|-a85|-a96|-h|-i] infile [[patchfile] outfile]
Where -v/-V   provide version information
      -h      shows this help, if it is the only option
      -l addr override the load address for binary images, default is 100H (CP/M)
      -a51    produce AOMF51 file, note no symbols or debug info
      -a|-a85 produce AOMF85 file, note no symbols or debug info
      -a96    produce AOMF96 file, note no symbols or debug info
      -h      produce Intel Hex file, note no symbols
      -i      produce Intel ISIS I bin file
File format can be AOMF51, AOMF85, AOMF96, Intel Hex, Intel ISIS I Bin or binary image
The last format specified is used, default is binary image
If outfile is omitted, only a summary of the infile is produced
```

The optional patch file has contains lines which are interpreted int one of two modes, PATCH and APPEND, with PATCH being the initial mode

Unless part of a string, blanks are ignored and a punctuation symbol ends line processing

In PATCH mode each line starts with a patch address followed by any number of patch data values.
In APPEND mode, only patch data values are supported; the patch address is implicit

Any number of meta token assignments (see below) can be interspersed between patch values

#### Patch data values

Patch data values can be either of the following

```
'APPEND'     switch to APPEND mode, rest of line is interpreted as per APPEND mode
value ['x' repeatCnt] where repeatCnt is a hex number and value is one of
	hexvalue
    'string'  C string escapes \a \b \f \n \r \t \v \' \" \\ \xnn and \nnn are supported
     -        set to uninitialised. (error in APPEND mode)
     =        leave unchanged i.e. skip the bytes. (error in APPEND mode)
```

#### Meta token assignments

```
metaToken ['='] value
where metaToken is one of
  TARGET  issues a warning if the specified target format is different
  SOURCE  issues a warning if the actual source file format is different
  LOAD    issues a warning if the actual load address is different
  START   set the start address if not set, else warn if source file start is different
  NAME    sets the name for AOMFxx formats otherwise ignored
  DATE    sets the date field for AOM96 otherwise ignored
  TRN     sets the TRN value for AOMFxx otherwise ignored. Error if invalid
  VER     sets the VER value for AOMF85 otherwise ignored
  MAIN    sets the MAIN module value for AOMF85 & AOMF96 (bit 0 only use)
  MASK    sets the MASK value f or AOMF51 (low 4 bits only)
and value is one of
  fileFormat, used for TARGET and SOURCE. Vaild values are
    AOMF51   - Intel absolute OMF for 8051
    AOMF85   - Intel absolute OMF for 8080/8085
    AOMF96   - Intel absolute OMF for 8096
    ISISBIN  - Intel ISIS I binary
    HEX      - Intel Hex
    IMAGE    - Binary Image
  string for NAME and DATE
  hex for other. Note LOAD and START are word values others are byte values
```

 The genpatch tool can be used to create patch files in the right format

 Note APPEND mode is needed to support output files that are not simple binary images. A normal patch would incorrectly include the extra data within the loaded imageaomf2bin.exe (tool-src)

This utility take an absolute omf85, omf86 or omf286 file and creates binary images suitable for a prom programmer. There is an ability to set the base address of the prom and, whether to pad to a prom boundary, with 0 or 0xff. Optionally separate files can be created for odd and even bytes

```
usage: aomf2bin -v | -V | option* infile [outfile | -o odd_outfile | -e even_outfile]+
   supported options are
   -b address - sets base address of rom
   -p         - pads image to eprom boundary
   -v / -V    - show version info and exit
   -z         - sets uninitialsed data to 0 instead of 0xff
```

### asm80.exe, locate.exe, lib.exe, link.exe, plm80.exe (c-ports)

C port of Intel's ISIS tool chain. The usage is as per Intel's documentation with native file name support and removal of some limitations. See [Cports.md](Cports.md) for more information, including how directories are mapped to drives.

### asm80x.pl [depreciated]

No longer needed as the newer c-port of asm80 supports long label names

### asmx.pl

An experimental perl wrapper adding long label names and structure support to asm80. This is not yet robust.

### binobj.exe (c-ports)

A port of Intel's binobj utility. This one supports windows/unix filenames and does not support ISIS drive mapping.

```
usage: binobj -v | -V | binfile [to] objfile
```

### delib.pl

Simple perl script to extract the contents of an Intel OMF85 or OMF86 library file into separate files.

```
usage: delib.pl library targetdir
```

### disIntelLib (tool-src)

A homegrown utility to auto disassemble an Intel omf85 library into individual files. During the disassembly, whether the original code was PL/M or ASM is noted and the extension named accordingly.

```
Usage: disIntelLib infile
```

### dumpomf (tool-src)

Dumps the detail of the content of omf85, omf51, omf96 and omf86 files. Interpretation of the various formats is per the intel specifications with some extensions for omf86. Due to lack of samples, limited testing has been done on omf96. This supersedes **dumpIntel** which has now been depreciated.

```
usage: dumpomf -v | -V | objfile [outputfile]
```

<div style="page-break-after: always; break-after: page;"></div>

### filever.cmd

This shows the revision of an individual file with respect to the current repository. It is primarily of use to show how many revisions there have been to script files.

```
usage: filever [-v] | [-q] file
where -v shows version information filever itself
	  -q supresss output if the file is not in git
```

### fixobj.exe (tool-src)

Supports modifying omf85 files to work around lack of historic / unreleased compilers that are currently not available.

```
Usage: fixobj [-(v|V)] |  [-h] [-l] [-n] [-p file] [-t(f|p|u)]  [-v hh] infile outfile
Where:
  -v | -V     shows version information - must be only option
  -h	      create missing segdefs in MODHDR for CODE..MEMORY
  -l          remove @Pnnnn library references
  -n          mark as a non main module
  -p file     parses the file for patch information. See below
  -tf         sets translator to FORT80
  -tp         sets translator to PLM80
  -tu         sets translator to Unspecified/ASM80
  -v hh       sets version to hh hex value
 outfile      outfile can be the same as infile to do inplace edits

Using the -p option supports more advanced patching
the file can contain multiple instances of the following line types
n [(a|c) addr]		  non main module with optional non compliant entry point
p addr [val]*         patch from addr onwards with the given hex values
                      addr is absolute for apps, else code relative
r oldname [newname]   renames public/external symbols from oldname to newname
                      names are converted to uppercase and $ is ignored
                      omitting newname deletes, only vaild for public
                      valid chars are ?@A-Z0-9 and length < 32
s addr                force split in record at absolute addr
the command line options with out leading - can also be used
text from # onwards is treated as a comment and blank lines are skipped
```

In addition to the documented options above, all record checksums are recalculated, with previously invalid ones being highlighted.

| **Option** | **Typical usage**                                            |
| ---------- | ------------------------------------------------------------ |
| **-h**     | Although PL/M emits seg size info in the MODHDR, the linkers omit these if the size is zero. Adding in libraries see below, causes the linker to remove this seg size information. The -h option forces the standard segments CODE, DATA, STACK and MEMORY to be included even if their size is zero. |
| **-l**     | This is used to allow PL/M v1.0 behaviour to be synthesised. This older version includes some of the library routines in the object files it creates, which the more recent compilers don't. Although it is possible to link the missing library routines, the public definitions of the plm80.lib routines that this creates causes conflicts when linking. The -l option strips the public definitions out of the  synthesised object module. |
| **-n**     | Some older applications are composed of separate applications joined together, however the Intel linker objects to linking two or more main modules. In principle converting the files to hex and  joining them would work, this option makes the task simpler by removing the main program flag from the MODEND record. See patch file notes for a more advanced version. |
| **-t?**    | These options allow the trn field of the MODHDR record to be set to flag the original file as being PL/M80, FORT80 or ASM80/Unspecified. One use of this is to reset the trn to PL/M80 when the -l option is used, as linking the library routines will reset the trn to ASM80/Unspecified. |
| **-v**     | This allows the version files of the MODHDR to be forced to a particular value. For example to make it look like the object file has been created by version 1.0 of the PL/M compiler |

#### -p patchfile

The patch file option is used when more complex modifications are needed to make an object file match an original version. Multiple -p options are allowed.

| Option | Typical Usage                                                |
| ------ | ------------------------------------------------------------ |
| n      | This performs the same basic operation as the -n command line option, however it also allows an entry point to be defined, with a \| c setting the seg id to ABS or CODE respectively and the address being the offset.<br />According to the OMF specification the entry info is ignored for non main modules and should be set to 0, however PLM v1.0 modules does not adhere to this standard, this option allows the PLM v1.0 behaviour to be mimicked. |
| **p**  | This is used to patch a file in cases where it is not possible to get known compilers to generate the same code.  It only patches defined content and cannot be used to set data or uninitialised areas. Additionally fixup information is not changed, so care is needed when patching non located modules to make sure than only fixed data or offsets are modified.<br />For absolute file **abstool** may be a better choice. |
| **r**  | There are two primary uses of this. One is to delete or mask public references in a more targeted manor than the -l option. The second is to rename between ASM80 short names and the compiler long names. |

| Option | Typical Usage                                                |
| ------ | ------------------------------------------------------------ |
| **s**  | Some historic files appear to have splits in longer OMF CONTENT records, possibly due to older linkers or small memory build machines. Although this split has no impact on the loaded image, this option is used to force a split, so that exact binary images can be created. The inverse is not needed as recent versions of link/locate can be used to join records. |

Note the -t, -v and patch file s option are for cosmetic changes, images will be equivalent with or without them.

Note **fixobj** is not able to resolve all differences between old files and those created by more recent tools, it does however allow creation of equivalent files. The key outstanding issue relates to problems when fixing the embedded library code that PLM v1.0 generates. In linking in the library functions, the linker does not emit the records in the same sequence, nor does it create the same the same record splits. Whilst this has no impact on subsequent use, it does mean that the files generated will not be a byte for byte match. The only resolution of this would be to write a bespoke linker.

### genpatch.exe (tool-src)

It compares two absolute files and generates patch information that **abstool** can use to generate files.

Supported absolute formats are AOMF51, AOMF85, AOMF96, ISIS BIN, Intel Hex and binary images.

```
usage: genpatch (-v | -V | -h)  | [-b addr]  infile targetfile [patchfile]
Where -v/-V provide version information
      -h       shows this help
      -l addr  set explicit load address for binary image files. Default 100H (CP/M)
File format can be AOMF51, AOMF85, AOMF96 Intel Hex, Intel ISIS I Bin or binary image
If patchfile is omitted then the patch data is output to stdout
```

### hexobj.exe (c-ports)

A port of Intel's hexobj utility. This one supports windows/unix filenames and does not support ISIS drive mapping. 

Consider using **abstool**, unless symbol loading is required.

```
Usage: hexobj -v | -V | hexfile objfile [startaddr]
Or:    hexobj hexfile TO objfile [$] [START ( startaddr ) ]
startaddr replaces the start address specified in the hexfile.
Intel style address formats are supported
```

### isisc.exe (tool-src) [depreciated]

Use **genpatch** which will show any differences.

### isisu.exe (tool-src)

Use **abstool** to convert ISIS I bin to AOMF85

### makedepend.pl [depreciated]

 This is now depreciated since thames and the c-port of plm80 allows dependencies to be created as part of the normal assembly / compilation similar to modern C compilers

### mkisisdir.pl

Simple utility to create an ISIS.DIR file to test ixref. It fills ISIS.DIR with all files in the current directory matching the pattern ??????.???. It only includes enough information to allow ixref to work and does not fill in all of the normal ISIS.DIR content.

```
mkisisdir.pl
```

### mkmake.pl [depreciated]

Simple perl script that does a partial job of translating an Intel .CSD build file into a makefile. It is depreciated since the makefile support has been significantly enhanced since this was written.

```
mkmake.pl
```

It processes all .CSD files in the current directory.

### ml80.exe, l81.exe, l82.exe, l83.exe (c-ports)

C ports of the CP/M high level assembler ml80 which is available under the cpmsrc/ml80

```
ml80 file[.ext]				.ext defaults to .m80
l81  file
l82	 file
l83  file
```

### ngenpex.exe (source currently private)

My own implementation of the intel software tools utility genpex. This fixes a number of issues with the original which I have included in the itools directory along with the original documentation.

The tool takes a master database of public variables,  literals, procedures, based and label declaration and a source plm file. It generates an included file with required external and literal definitions for the plm file to compile.

See the genpex.txt file in the itools directory for the main details. My changes are:

1. Text in simple strings are ignored e.g. 'ERROR x'  does not cause the error procedure to be defined as an external

2. The pex file allows a minus prefix to force prevent a match

3. Procedure definitions optionally allow  parameter names to be included by following the type indicator with a space, the name and a comma or trailing )

4. In genpex a trailing S indicates an array variable and a (1) size is written to the ipx file.
   ngenpex optionally allows a (size) where size can be number or a "literal", this is written to the ipx file. This allows the plm size, length and last functions to be used

5. Local variable names do not trigger external definitions

6. Literal or variable declarations before the $include line for the generated .ipx file take precedence

7. Optionally ngenpex will emit public definitions in the plm file into a .pub file to allow an initial pex database to be created.
   Note it will also emit externals with a # prefix. This allow you to cross check for missing / incorrect usage

   ```
   usage: ngenpex pexfile sourcefile [-p]
   where the -p is optional and generates the .pub file noted above
   ```

### obj2bin [replaced by abstool]

### objhex.exe (c-ports)

A port of Intel's objhex utility. This one supports windows/unix filenames and does not support ISIS drive mapping.

Consider using **abstool**, unless symbols are required in the hex file.

```
usage: objhex -v | -V | objfile [to] hexfile
```

### omfcmp.exe (tool-src)

This tool is designed to intelligently compare intel OMF85 files, however it will revert to comparing binary files.

```
Usage: omfcmp -v | -V | file1 file2
```

### pack.pl

Manage a packed source file.

Note by default the top level makefile is excluded, as this is usually used to extract the files automatically. Makefiles in sub-directories are included when pattern matched.

```
usage: pack.pl [-h] [-a pattern | -c pattern | -d pattern | -l | -u] [-f] [-m] [file]

where -h            prints simple help and exits
      -a pattern    add text files matching pattern - also updates changed files
      -A            as -a with implied pattern of *
      -c pattern    create new packed file from text files matching pattern
      -C            as -c with implied pattern of *
      -d pattern    remove text files matching pattern
      -l            list names of included files
      -u            update files in existing packed file 
      -f            files only no directories
      -m            for -a and -c include makefile in top level directory
      file          an optional target file - default is {curdir}_all.src

      default operation is -h

      patterns are case insensitive ? matches any char * matches any number of chars
      multiple patterns are separated by |
      [..] matches ranges of chars and spaces should not be escaped
      e.g. to match a file name with a space use "* *"
```

<div style="page-break-after: always; break-after: page;"></div>

### patchbin.exe [replaced by abstool]

### plmpp.exe (tool-src)

Only PL/M v4 supports a pre-processor. This utility provides a pre-processor for older versions of PL/M.

```
usage: plmpp -v | -V |  [-f] [-F] [-sVAR[=val]] [-rVAR] [-o outfile] srcfile
Where -f				- expands a level of include files, each -f does another level
	  -F				- expands all include files regardless of depth
	  -sVAR[=val]		- same as PL/M's SET(VAR[=val])
	  -rVAR				- same as PL/M's RESET(VAR)
	  -o outfile		- specifies the output file, otherwise outputs to stdout
```

### pretty.pl

plm81/plm82 compiler listing format does not interleave source and generated code. This utility processes the list file to interleave the source and code. It is not perfect because the interleave information is not 100% accurate. It is however reasonable.

```
usage: pretty.pl infile outfile
```

### rebase.pl

Updates a listing file from ASM80 or PLM80 to remap code address locations. It is largely replaced by relst.pl. but it may have some use in partial builds scenarios.

```
usage: rebase.pl lstfile lstaddr realaddr
lstaddr is an address in lst file and realaddr is the real address
all addesses are adjusted to reflect the offset
```

### relst-simple.pl (depreciated)

Takes a mapfile and a set of .lst files from a build and generated .prn files with the listing files updated to reflect the located addresses. relst.pl is the natural replacement

```
usage: relst.pl mapfile lstfiles
use the mapfile to adjust all of the specified listing files
the output files have the same name as each lstfile with a .prn extension added
```

### relst.pl

This utility takes listing files generated as part of the build along with the located file and the map file and created a new set of files with both the location addresses and code bytes updated to reflect the located locations.

```
usage: relst.pl locfile mapfile lstfiles
where  locfile is the located application
	   mapfile is the map file created during the build
	   lstfiles are the .lst files created during the build (ASM and PLM)
The generated files have the same name as the lstfile but with .lst replaced bu .prn
```

### repack.pl

Updated the packed source file (*directory*_all.src) in the current directory with changed file content.

```
repack.pl
```

### revisions.cmd (master in versionTools)

Shows revisions of non .exe files in the current directory with respect to the current repository.

Note external tools copies in from other repositories are likely to have different revisions numbers e.g. revisions.cmd, install.cmd and version.cmd come from the versionTools repository

```
usage: revisions [-v] | [-s] [-q]
where -v	shows revision information on revisions.cmd itself
	  -s    also shows files in immediate sub-directories
	  		Note, directories beginning . are skipped
      -q	skips files not in git
```

### unpack.exe (tool-src), unpack.pl

These two files support extracting files form a packed source file. The perl variant will not extract the file if the contents are unchanged, this helps with makefiles as it reduces the number of rebuilds.

```
Usage: unpack -v | -V | [-r] [file]
	   unpack.pl [-r] [file]
if file is not specified the default file is directory_all.src
where directory is current directory name
-r does a recursive unpack
```

------

```
Updated by Mark Ogden 7-Feb-2024
```

