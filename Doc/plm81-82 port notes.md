The notes below capture the key changes made as part of the port of the PLM81 and PLM82 code from Fortran 66 to modern C.

Whilst more changes may be made, the current changes represent a stable release.

Note the pretty.pl tool which processes the listing file to show source and interspersed code has been updated in line with the changes to the compiler.

# External Impacting changes (pass 1)

The following are the key externally impacting changes, hopefully most people will not have any negative impact

- For the pass1, the file to compile is now specified as a command line option and intermediate files are generated from its name. Additionally the command line can specify key control options, rather than relying on $ control lines.

  ```
  Usage : plm81 [-a] [-b] [-l nn] [-g] [-m] [-p] [-s nn] [-t n] [-u] [-w nn] srcfile
  Where
  -a       debug: show production analysis
  -b       debug: show stack dump
  -l nn    set line count - default 0 i.e. first line is 1
  -g       debug: show generated intermediate code
  -m       turn off dump symbol table
  -p       turn off source code listing
  -s n     debug: show symbol information 0 (default) none, 1 symbols, 2 symbols & data
  -t n     set tab expansion 1 <= n <= 8. Expand tabs with spaces so column is a multiple of n
  -u       force upper case in strings
  -w nn    set output line width (min 72) - default 132
  srcfile  is the source file, of the form prefix.ext e.g. m80.plm
           intermediate files prefix.lst, prefix.pol, prefix.sym are created
           if the srcfile does not have an extension, .plm is added
  
  Also supports single arguments -v, -V for version info and -h for help
  ```

- Support for the $ control lines has been modified to align with the options above.
  - Intermediate file options J, K, U, V, W are no longer needed for their original purpose as the intermediate file format has changed.
  - Support for Left Margin and Right Margin has been removed, so L and R are no longer used for their original purpose. To use very old PLM decks, external tools will need to apply the left/right margin clipping.
    Note the line length limit is now 256 characters. Strings can continue across lines and the '\n' is ignored, otherwise the '\n' terminates a token. Identifiers are truncated to 32 significant characters.
  - The input file is now replaced by the source file command line option and when the control $I is used it now takes a file name, without spaces and opens this as an include file, e.g. I=myincludefile.plm.
  - Using stdin as an input is no longer supported implicitly. The corresponding T option is no longer used for its original purpose.
  - The cryptic C and D options now use the freed L and W options.
  - The control options to list current control values are no longer supported.
  - The input file now supports ascii characters 0x00-0xff, however
    - '\0' and '\r' are ignored (useful when compiling under unix/linux with \r\n source files)
    - '\n' is used as a line terminator
    - '\t' is expanded with spaces. The -t/$T option value is used add spaces to the next column which is a multiple of the $T value. By default $T=1 which mimics the original behaviour of a single space.
      Warning it is not recommended to use control characters or characters with the top bit set inside a PL/M string.
    - As lower case letters are now supported, to retain compatibility with older code, the -u/$U option can be used to map lower case to upper case characters
  
- Error messages are now descriptive text.

- A fatal error terminates without trying to process any further. The original recover hacks were not reliable and could generate garbage.
  Note A 100 error threshold will also trigger a fatal error.

# External Impacting changes (pass 2)

The pass 2 of the compiler plm82 no longer uses a configuration file with $ controls, instead all options are set using command line. Like pass 1, the file to process is part of the command line and should match the name used in pass 1. The intermediate files are auto named and use a binary file format.

```
Usage: plm82 [-a n] [-d nn] [-f] [-g n] [-l nn] [-m] [-n] [-o] [-s n] [-v nn] [-x] [-# nn] plmfile
Where
-a n       debug - set analysis level != 0 show state, >= 2 show registers
-f         disable code dump at finish
-g n       trace - 0 no trace, 1 lines vs locs, 2 full interlist- default 1
-l nn      start machine code generation at location nn
-m         turn off symbol map
-n         write emitter trace
-o         enable the additional V4 load and arith immediate with 0 optimisations
-s n       debug - write symbol info. 0 none, != 0 address , >= 2 detailed info
-v nn      set first page of RAM
-w nn      set output width, min 72 default 120
-x         disable hex file
-@ nn      stack handling. 0 system determined, 1 user specified, > 1 stack size
plmfile    is the same source file name used in plm81 of the form prefix.ext
           intermediate files prefix.lst, prefix.pol and prefix.sym are used
           optionally prefix.cfg can be used to hold plm82 configuration flags
           prefix.lst is updated with pass2 output added and prefix.hex is created
           Note the .pol and .sym files are deleted if pass 2 is successful
           
Also supports single arguments -v, -V for version info and -h for help
```

- Internally pass 2 no longer uses the restricted character set and as noted, the interface to pass 1 uses a binary file and not the base 32 encoded text file.
- The v4 optimisations related to
  - converting adi 0, sui 0, xri 0 and ori 0 to ora a, ani 0 to xra a.
  - optimising mvi x,nn, followed by mvi x,mm, to remove the first instruction
  - optimising consecutive mvi opcodes that load a register pair into an single lxi opcode
- There is no longer an option to print the intermediate file text as the files are now binary
- Error message are now descriptive
- If there are no errors the intermediate files are deleted.
- The pass 2 listing is appended to the same file as the pass 1 listing. A minor issue is that if pass 2 fails and is rerun, it will append again. In practice this is unlikely to be an issue as unless the pass 2 is modified (i.e. during its development), the same result will occur. Rerunning pass 1 will reset the file if necessary.
- Support for BNPF has been removed.
- The full 64k of memory space can now be used.

# Intermediate File Changes

The pol file and sym file are now binary files.

On error the intermediate .pol and .sym files are deleted as they are not usable.

# Major Internal Changes (pass 1)

There are two major areas of changes vs. the original Fortran code

- File I/O

  - The Fortran unit numbering for files is replaced by file names. The intermediate files are now binary and don't use base32 encoding
  - The internal character set mapping is no longer used. It now assumes ascii chars. Related packed string arrays have been converted to C strings.
  - The handling of line input and $ control line processing has been simplified.
  - In most cases the W option is handled explicitly with long input lines being wrapped as needed. The only area this may break is for very long error messages which are not wrapped.
    Note. The space used for Fortran line printer control is no longer emitted.
  - Scanning of tokens has been simplified.

- Symbol Table (pass 1)
  This was a major structural change as the symbol table was used in multiple ways. Additionally the indexing in to the table was inconsistent making it hard to understand. The replacement code uses a single structure for each symbol, with an adjunct table for the symbol names to avoid a variable size structure. 

  Items that built down from the top of the original symbol array have been moved, specifically

  - Literally definitions, also uses the symbol table string array
  - Initial Data
  - Assignment lists
  - Identifier lists

  Some of the debug information has been modified to reflect the new symbol table.

# Major Internal Changes (pass 2)

- File I/O

  - The Fortran unit numbering for files is replaced by file names. The intermediate files are now binary and don't use base32 encoding.
  - The internal character set mapping is no longer used. It now assumes ascii chars. Related packed string arrays have been converted to C strings.
  - Support for $ controi lines has been removed. Options can now be set on the command line.
  - In most cases the -w option is handled explicitly with long input lines being wrapped as needed. The only area this may break is for very long error messages which are not wrapped.
    Note. The space used for Fortran line printer control is no longer emitted.
- The code generation for multiply and divide has been simplified.

Pending changes relate to splitting the symbol and information tables and cleaning up some of the packing/unpacking of the information table.

```
Updated: 17-Jul-2025
```

