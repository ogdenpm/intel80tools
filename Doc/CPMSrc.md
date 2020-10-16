# CPM Applications

The cpmsrc directory tree contains my decompilation work for a number of CPM applications. At the top of the tree is a makefile that can be used to build all of the applications.

Note there are additional CPM applications, specifically the Whitesmith's development tools, in my github repository [ws21](https://github.com/ogdenpm/ws21).

Specific subdirectories are noted below. Both the lib_1.1 and link_1.31 directories use my packed source format. See [misc.md](misc.md) for details.

### basice

Although not a decompilation, this directory contains the source for basic-e v2.1 and the runtime interpreter v2.3. It also contains a document that describes the challenge on getting this to compile to byte level matching, despite having access to the source

### lib_1.1

This is a decompilation of Digital Research's LIB tool version 1.1 back to plm source.

### link_1.31

This is a decompilation of Digital Research's LINK tool version 1.31 back to plm source.

### ml80

This is my reconstruction of the ml81 high level assembler, which has four separate .com files

- m81 - a macro processor
- l81 - the lexical analyser
- l82 - the code generator
- l83 - a simple linker

Historically I decompiled this many years ago, but I subsequently found a copy of the original thesis that described the assembler. I used the thesis to update my decompilation to reflect variable names etc. Note the thesis version varies slightly from the published versions.

From various CPM archives I have identified two variants of the m81.com file and two variants of the l82.com file. The files in the patchFiles subdirectory allow the variants to be created. Each contains descriptive information on what the patches do.

For  l82.com one of the patches was a significant manual patch. I have included details, but also added a new plm source file l82fix.plm that creates a clean PL/M 80 file with the same functional changes.

Note the files were compiled with the Fortran based PL/M 80 compiler, however the version used  appears to one somewhere between the V2 & V4 versions available on the internet. In particular patches from V4 needed to be added to the V2 variant to make it work. The plm81 & plm82 files in the tools directory have the appropriate patches applied.

### tex

This is a decompilation of various versions of Digital Research's TEX text processor. In particular versions 1.0, 1.2 and 2.1. In addition there is a 2.1a version that includes fixes I have made.

The readme.txt file in the directory provides more details on the various versions.

Note in addition to the  usual makefile, the directory also build.ninja to allow the ninja build tool to be used. This was largely an experiment to see whether moving to ninja would be better than using makefiles.

------

```
Updated by Mark Ogden 16-Sep-2020
```