# obj2bin
Convert aomf85 files to binary an optionally apply a patch file.

## Usage

**obj2bin** *binfile* *objfile* [*patchfile*]

converts *objfile* in aomf85 format to *binfile* and applies optional *patchfile*

Lines in the *patchfile* have the following format, where values are in hex and separated by white space

| Item             | meaning                                                      |
| ---------------- | ------------------------------------------------------------ |
| \# any text      | ignored                                                      |
| addr [byte]*     | fill locations from addr onward with the specified bytes     |
| start - end byte | fill the address range start to end inclusive with the specified byte value.<br />Whitespace around the dash is optional |

Blank lines are also ignored,

A common use for the patch file is to support code developed using the Intel 8085 toolset which allow uninitialised data and exact file lengths. This allows specific values to be specified for uninitialised values and pad files to different lengths, which may be needed to byte match historic files.

#### Note:

This code used to be part of Intel80Tools, however to support version control it has been extracted into a separate repository. The code is still present in Intel80Tools but as a subgit

#### See Also:

**amof2bin** for creation of binary images for PROMs from aomf85, aomf86 and aomf286

Mark Ogden 30-Apr-2020