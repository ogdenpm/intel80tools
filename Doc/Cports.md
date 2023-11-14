# C ports of PL/M 80 & Fortran applications

The c-ports directory tree contains my ports of a number of PL/M 80 applications to C and my port of the old Fortran based PL/M 80 compiler.

Below are details of what is available, followed by some notes on my approach to porting the applications.

## Direct ports from PL/M source (Intel tool chain)

Earlier ports closely follow the decompiled PL/M source, however to make the tools more usable under Windows and Linux, major chunks of code have recently been rewritten. Additionally variable and function name changes and comments have not been reflected in the original decompiled code. This makes it a little harder to match the C port to the decompiled code.

Except where noted below, the tool usage is as per the Intel documentation, but as many of the Intel command options include parentheses, ampersands and single quotes, it may be necessary to escape them, e.g. using double quotes, to avoid interpretation by your OS shell.

### Common changes

#### Native filenames

All the tools now support native filenames including directory paths. There are however some minor restrictions noted below.

- Control characters cannot be included in a filename, even if quoted.
- Unless quoted a file name cannot contain a space, comma or a left or right parentheses.
- A quoted file name cannot contain an embedded quote. Only single quote is supported.
- Filenames can contain an optional :Fx: or :fx: prefix where x is a digit. If there is a corresponding environment variable ISIS_Fx then its value is used as a path prefix for the filename. The environment variable can contain any character as it is not checked.
- File name errors are now detected when the file is accessed and not when the name is parsed. Also the error message use the OS error descriptions and write to stderr.
- Although long file names are supported it is recommended to keep names reasonably short for readability. Likewise it is recommended to avoid spaces in file names, as these often cause problems with build tools.

Note ISIS devices e.g. :CO:, :CI: and :BB: are no longer supported, use the OS equivalents.

#### Command line

Command lines can now be as long as the OS supports. The ampersand (&) continuation is still supported if necessary and has been added to asm80.

Additionally commands invoked without arguments will take the arguments from stdin, allowing for command line redirection from a file e.g.

```
link <responsefile
```

Within the file, & can be used for continuation and an end of file will add an implicit end of line if needed. For lib an end of file will also cause an implicit exit.

Several of the Intel commands echo the command line in the listing file. To support longer file names, the break in long lines is now done slightly differently from the original code, to avoid splitting file names. This does however mean that file names longer than the page width will spill over the line boundary.

All the tools support three common standalone options

```
-v	print simple version information
-V	print extended version information
-h	print usage
```

#### Internal work files

All internal work files used with multi pass tools, have been eliminated. This change also allows parallel compilation. A temporary file is still used for building new libraries with lib, but it has the process id appended to it, to make it unique.

The change means that WORKFILE options are no longer needed and are ignored. The optional drive specifier for MACROFILE is also ignored.

#### Identifiers & Numbers

- The underbar (_) , is now a valid character in identifiers, aligning with later versions of PL/M. This includes identifiers used for module names.
- ASM80: The dollar ($) is now treated as a visual separator in identifiers
- ASM80 & PL/M80:  Both dollar and underbar are treated as visual separators for numbers.

### Application specific changes

#### asm80

The port of asm80 v4.1 and is based on asm80.ov4 which is the macro version of the assembler. I have seen this distributed as asm80 without the other overlays.

Note, for the ISIS assembler. the main asm80 just determines whether to run asm80.ov4 (macro support), asm80.ov5 (large memory support no macros) or to use overlays asm80.ov1, asm80.ov2 and asm80.ov3 for small memory support.

The following additional behaviour changes have been made to asm80

- MACROFILE and MOD85 are now the default. NOMACROFILE and NOMOD85 have been added to remove the reserved words if needed
- Labels and module name can now be up to 31 characters to align with PL/M-80.
  Note, as the assembler truncates identifiers, you may see errors reported for older code with identifiers longer than the six character limit, these will need to be fixed manually.
- The header lines have been modified to reflect that they are no longer ISIS-II and to enable long module names to be centred in the available space. Additionally, if there is room, the date and time of the assembly in the format [yyyy-mm-dd hh:mm] will also be included. The default header width is now 80 unless the page width is smaller.
- The Symbol Cross Reference now uses the common new page functionality so page numbers don't restart from 1
- Symbol and Cross Reference information that cross page boundaries how add a sub heading to indicate that the information is continued.
- Limits on the source line length have been removed, however for listing purposes very long lines are truncated.
- The maximum number of tokens on a line has been increased to 20. This allows longer db/dw statements.
- The combined length of nested macro arguments has been increased to 4096.
- PAGELENGTH and PAGEWIDTH can now be set up to a value of 65535, with 0 an alias for 65535. Existing lower limits still apply.
- For the auto generated default .lst and .obj files a check is made on whether the file name part is upper case only. If it is, then uppercase extensions .LST and .OBJ are used instead.
- The XREF processing is now built in and does not require temporary files.
- A new option MAKEDEPEND has been added to generate dependency information for use by make

```
MAKEDEPEND [(file)]
		   file defaults to .deps/{src}.d, where {src} is input name without extent
```

### lib, link & locate

Ports of lib v2.1, link v3.0 and locate v3.0

- Commas in lists that are enclosed in parentheses, are now optional
- Commas in file name lists are also optional with the list terminating with an option keyword. In the rare case that a file has the same name, add a ./ prefix.
  Note in LINK, PUBLICS does not break the list, however it is possible to include a file called publics by using ./publics
- LINK & LOCATE: New option EXTERNOK. This will treat unresolved externals as warnings and not delete the output file. The primary use of this is when building overlays.
- LOCATE: If the MEMORY segment is not the last segment, its size is truncated to avoid the overlap. 
- LOCATE: New option OVERLAPOK. This will treat overlapping memory segments as warnings and will not delete the output file. This option has been provided to replicate similar functionality provided by the thames emulator. Its use it not recommended. Historically it was used in two scenarios:
  - When using an absolute object file to patch code. It is now recommended to use the fixobj utility which is one of the tools found at https://github.com/ogdenpm/tool-src
  - To work around the segment size problem when MEMORY was not the last segment. This is now addressed, see above.
- LIB: Now optionally supports a single command line operation after which it will exit
- LIB: A new option INIT, which is like ADD, but ignores any existing library content. It is designed to provide a single command to rebuild a library file, rather than deleting an existing file, creating a new library and adding modules.

### plm80 (source in plm80c)

This is the C port of PL/M 80 v4.0 and is the preferred port, rather than the much older C++ port plm80 noted below. This is the one I am  adding commentary to as I understand the compiler internals.

When compiled it generates an executable plm80.exe

Note I kept the old directory naming to avoid messing up the git history.

- Limits have been increased, specifically
  - Max String length, including literals - 4096
  - Max Identifier strings - 2500 - unique identifier names
  - Max Identifier definition - 3000 - allows for identifiers reused in different scopes
  - Max Identifier references - 4000
  - Max Cases - 1000
  - Max Structure members - 33
  - Max Factored names in definition - 33
  - Max Unique Includes 40
- The DATE option now allows 10 characters, also if the (string) is omitted then todays date/time is used.
  For some reason the original date string entry allows nesting using  ( and ), given that the original maximum length was 9 characters, this does seem bizarre. The nesting is still supported.
- A new option MAKEDEPEND has been added to generate dependency information for use by make.
- XREF option, sorts the labels, alphabetically and unlike the ISIS PL/M80 implementation, where labels of the same name occur, these are sorted by earliest location seen.
- IXREF option, modified to support longer file names, specifically
  - Any directory component is ignored, this is consistent with the drive being skipped under ISIS.
  - For file names <= 10 chars, the format is as per PL/M80 V4.0, i.e. 10 character file name, padded with trailing spaces if required, followed by 9 dashes.
  - For longer file names, the file name is 16 characters; truncated or padded with spaces as required, followed by a 0 and two spaces. With ISIS IXREF, this will show the first 10 characters, two spaces and the remaining characters under the diskette name. The 0 prevents the diskette name dot being shown.
  - Additionally for longer names a new record is written at the end of the IXI file containing the full name. ISIS IXREF will report this as a bad record and ignore it, the new IXREF will use this as the full filename.
- INTVECTOR, RESET, SET and WORKFILES, still require commas as per Intel documentation. WORKFILES is however redundant.

### ixref

This is a port of ixref 1.3. Internally it is a major rewrite, to use an in memory sort, rather than merge file sort. It has also been modified to accept an additional record as noted in the PL/M80 comments above.

Simple wildcard file matching is supported to allow its use in response files and continuation lines. On the command line these may need quoting to avoid normal shell expansion.

As of 14-Nov-2023, the thames emulator auto generates an ISIS.LAB and ISIS.DIR file so that PL/M3.1 can use IXREF and the emulated versions of IXREF work, although the C port is preferred.

Note PL/M3.0 does not support IXREF.

## Other ports

### ml80, l81, l82, l83

Recently updated C ports of the ml80 assembler chain found under cpmsrc in the [Intel80Tools](https://github.com/ogdenpm/intel80tools) repository. The original port was done in July 2007. It has been updated to reflect the more recent comments added to the plm source.

```
Usage:	ml80 [-v|V] | file[.m80]	produces file.l80
		l81	 [-v|V] | file			uses file.l80, creates file.80p & file.80s
		l82  [-v|V] | file			uses file.(80p & 80s), creates file.(80c, 80d, 80r) 
	    l83  [-v|V] | [-s] [-l addr] file	uses file.(80c, 80d, 80r), creates file.com
The l83 options are
	-s			create symbol listing in file.prn
	-l addr		set load start address, defaults to 100H for CP/M
If the applications are built with the #define _TRACE then additional trace features are enabled, please see the source code.

The base filename is any valid valid OS filename (allowing for any .xxx extent) and can contain a directory prefix. All files will be read/written from the same directory, including any externally linked files identified in the source. Additonally externally linked files are limited to 29 characters (excluding the directory prefix).
For cases sensitive file systems, note the filename part is converted to lower case before usage.
```

### oldplm80 (source in plm80)

This was a very early translation from the plm v4.0 binaries to C++. It was done before I decompiled the source to PL/M. It is written in an old version of C++ and is very clunky. I have left it for historical reasons but now consider it to be obsolete. The  original port was done in 2007

If you compile the source it will now generate an executable oldplm80.exe

```
Usage is as per Intel documentation, however this old version uses a different drive to directory mapping approach.
:F1: maps to the current directory,  other :Fn: map to subdirectories named fn.
```

### plm81, plm82

These two programs are my port of the Fortran based PL/M compiler to C. Although based on the V2 compiler, some of the patches from V4 have been incorporated to fix invalid code generation.

In addition both compilers now accept the name.plm as a file to compile, the old way of using fort.n is also still supported. This change allows for parallel compilation. If plm82 requires $ config information this will be read from name.cfg is present or fort.1. It is no longer an error for neither file to exist.
Note name is not restricted to the ISIS 6 character alphanumeric limit.

When name.plm is specified, the intermediate files from plm81 are name.pol and name.sym, the file name.lst also contains the concatenation of the two listing files from plm81 and plm82, hence both should use the same name parameter or both omit it. Hex output is stored in name.hex.

The other change is that the message previously written to fort.1 by plm82 is now written to the console. This is message re-errors.

```
Usage: 	 plm81 file.plm					or	[mv | move] file.plm fort.2
										    plm81
										   	[mv | move] fort.16 fort.4
										    [mv | move] fort.17 fort.7
										    [mv | move] fort.12 fort.12a
		plm82 file.plm					    plm82
										    [mv | move] fort.17 file.hex
										    [cat fort.12a fort.12 >file.lst |
										     copy /Y fort.12a+fort.12 file.lst]
cleanup: [del|rm] file.pol file.sum 		[rm | del] fort.*

Note that file can be any valid OS filename prefix and a directory path is supported

plm81 and plm82 also support -v & -V which show version information and provide simple help
```

### binobj, hexobj & objhex

1. These are functionally equivalent ports of the corresponding ISIS tools. They support both the Intel invocation syntax and a more modern less verbose version.

Unlike the other tools these do not support ISIS drives, but do support unix/windows file path names.

Note due to memory constraints the ISIS binobj and hexobj would split very long content records. The ports do not need to do this.

```
Usage:	binobj [-v|V] | binfile [to] objfile
		hexobj [-v|V] | hexfile objfile [startaddr]
					  | hexfile to objfile [$] [start(startaddr)]
		objhex [-v|V] | objfile [to] hexfile

filenames can be any valid OS filename
```

## Notes on porting

The notes below were written several years ago and whilst they capture the approach I used and some  are still relevant, I have since made changes to improve the quality of the port. The key ones are

- I/O now uses stdio which makes the original application buffering code redundant.

- Command line handling has been improved to allow very long lines.

- Most tools now support native OS filenames and allow spaces instead of commas in lists.

- Heap management is now handled with native memory management, although there are some cases where indexed arrays are pre allocated, especially where the indexes are passed via external files.

- Data types are now better handled wrt. big/little endian support.

- Some unions of structures have been unwound into a single structure. For the original code these unions saved precious memory, however they added complexity and potentially  leaves memory unaligned. With modern systems memory is usually readily available, so such savings are not needed.

- For  memory saving reasons, global variables were quite common in the original code. Where I can determine the genuine scope of the variables, I prefer to convert the variables into locals and or pass as parameters.

- If possible I remove internal work files between overlays. This is done by writing directly to the final data structures or by using a virtual file internally. By doing this the applications can be invoked in parallel if needed.

- Occasionally I have had to split code due to C sequence point constraints. Common ones being

  - Parameter evaluation, which C allows in any order.

  - lhs and rhs evaluation order for binary operands.


```
For example
call func(i, i := i + 2);
will excute differently dependent on which parameter is evaluated first
likewise
a = b(i) xor b(i := i + 1);
will generate different results depending on whether the  lhs or rhs is evaluated first
```



#### Original Notes

Prior to porting it is helpful to make sure that the source code follows a  number of conventions as this makes it easier to spot variables vs. functions and helps with semi automation of the conversion. In particular

- All procedures follow the Pascal case naming convention. This makes functions stand out from arrays and simple variables.

- All variables follow camel case naming convention. Replacing $letter with Uppercase letter.

- As many basic data types mapped to one the following

  ```
  byte, word, dword, boolean, integer, pointer and address
  Here address is limited to when the variable holds both a word or pointer at different times
  I also use wpointer as a pointer to a word variable; common for system calls.
  ```

- If ngenpex is used, the pex file is helpful as this contains information that can be use to generate function prototypes, variable types, macros and typedefs

From this base point I use a mixture of bespoke perl scripts and global edits using vim and visual studio to create a base point for further analysis. This includes using support files e.g. command line parsing, I/O and  macros from previous ports.

The process is then iterative to get a version that compiles, followed by debugging to get a working version. This can include looking at the code generated from the original PL/M source to check the precise interpretation of complex conditionals.

Below are some of the typical issues that need to be addressed during the porting, which may be of interest to anyone contemplating their own ports.

### Variables and memory layout

Other than for trivial programs, it is unlikely that a single strategy can be used for variables and memory layout.

#### Simple variables

- byte, word, dword, boolean, integer can be mapped to uint8_t, uint16_t, uin32_t, bool, int16_t respectively. 
  If byte level access is needed to word, dword or integer values through based variables, the use of AT statements (effectively unions) or the writing of memory to disk, then it may be necessary to create accessor functions to handle the byte order mapping.

- pointer & wpointer types are more problematical as their size is dependent on the target processor. I handle this in two ways.

  1. Where possible I map to a standard C pointer, the type determined by the base variable usage of the pointer. Where the are different usage scenarios this may need to be a union or a void * pointer. Note wpointer maps to uint16_t *.
  2. Where the the pointer needs to be stored in a word size variable, a scenario typical in accessing heap type memory, I use accessor functions that map the pointer to / from an offset.

- Of the base types, address is the most complex and can be typically handled in two ways

  1. As a union of the numeric and pointer types. Where pointer can be implemented in either of the ways noted above. The challenge is determining the correct type to extract at each point in the program. Until more is known about the specific pointers I typically define a typedef for address as follows

     ```
     typedef union {
     	uint8_t b[2];	/* allow for byte pair */
     	uint16_t w;		/* word value */
     	uint16_t off;	/* a 16bit offset into a memory space */
     	uint8_t *p;		/* optional if direct C pointer can be used */
     					/* note not void * to allow for p++ */
     } address;
     ```

     

  2. As an intptr_t, assuming that there are no specific space / alignment requirements. Here specific casts will be needed for each pointer use.

#### Structures & Arrays

The main consideration in using structures and arrays is in constraints the C compiler places on data alignment and potentially on the data byte ordering for the scenarios outlined above.

If the size of a structure or array needs to match the PL/M size e.g. because it is written to disk or other code depends on the offsets of elements, then if the compiler has a suitable pragma to force byte alignment, this can be used, otherwise a byte array will need to be defined and suitable accessor functions created.

A variant of the above problem is that many PL/M programs assume variables are allocated in consecutive memory. Unfortunately this isn't guaranteed in C, so often the code / data will need to be reworked. For example the following take from isist0.plm v2.2

```
DECLARE MSG1(8) BYTE INITIAL(CR, LF, 'ERROR '),
        MSG2(3) BYTE, /* ERROR NUMBER GOES HERE */
        MSG3(9) BYTE INITIAL(' USER PC '),
        MSG4(4) BYTE, /* USER PC IN HEX GOES HERE */
        MSG5(2) BYTE INITIAL(CR, LF),
        MSG6(5) BYTE INITIAL('FDCC='),
        MSG7(4) BYTE, /* FDCC ERROR DATA GOES HERE */
        MSG8(2) BYTE INITIAL(CR, LF);
/*
The code fills in the strings at MSG2, MSG4 and MSG7 then writes out the whole string
*/
```

A couple of example solutions to this would be

1. Create a single MSG1 string and define the other MSGn variables as #define statements e.g.

   ```
   uint8_t MSG1[37];
   #define MSG2	(MSG1 + 8)
   ...
   ```

2. Use sprintf or similar to write the whole formatted string to MSG1 or to directly emit the string.

Note for static string variables, unless there are overriding requirements, I typically use a standard quoted string with it's extra trailing '\0' byte. Modern C's convention of concatenating adjacent strings is useful to create some of the longer tables.

#### At variables

AT is used in two ways in PL/M and typically needs different handling in C.

1. AT(absolute address) - this is used to make to a OS location. Although this could be handled in C by setting a pointer to the absolute address and dereferencing it, in reality it is more likely that any code that uses it will need to be re-written. As an example CP/M iobyte is often accessed this way but has no corresponding meaning in a C port.
2. AT(.var) - this is the way PL/M effectively handles unions. I use one of three approaches to handline this
   1. Create a union and either change the variable names or use #define to map them to a separate union variable.
   2. Use casts. This may be needed if the AT(.loc) is not to a simple variable
   3. Use accessor macros / functions. If the AT function is used to access high / low bytes then this may be necessary to handle byte ordering.

#### Based variables 

Based variables are how PL/M handles pointer dereferencing. However C pointer arithmetic isn't supported. I use four basic approaches to handling Based variables

1. Where a based variable is used in a simple way and the underlying pointer does is only dereferenced in one way, it is often easier to convert the pointer into a C pointer to the base type e.g.

   ```
   declare ptr pointer;
   declare wrd based ptr word;
   while (wrd <> 0)
   	ptr = ptr + 2;
   
   can be converted to 
   
   word *ptr;
   while (*ptr)
   	ptr++;
   	
   Note care needs to be taken with the increment of ptr to reflect C's pointer arithmetic
   ```

2. If a pointer has multiple ways of being dereferenced, then it can be cast into the appropriate type. Care will still be needed with any arithmetic with the underlying pointer, especially if the data alignment does not match PL/M. For this approach pointer can be declared as uint8_t * as this allows basic pointer arithmetic consistent with PL/M.

3. A variant of the above is to create a union of the types and use C -> to select the appropriate type. In this case the pointer would be declared as a  pointer to the union. Note the pointer may need to be cast to uint8_t * to support pointer arithmetic consistent with PL/M.

4. Use accessor macros/functions. This may be needed if byte ordering or special alignment is needed

#### Heap space

Many non trivial programs use the free space between MEMORY and the top of memory as heap space and have procedures that manage this space. The normal C equivalent would be malloc, however this will have native address pointers which may not be viable without major changes to the application code. I tend to follow the following approaches

- If the memory is a simple buffer or structure from MEMORY upwards. I pre-allocate the buffer either statically or via malloc.
- For true heap management I pre-allocate a memory image either statically or via malloc, which matches the heap size available to the program in ISIS. This is then accessed using accessor function that use offsets to read / write the data on the heap. This requires finding all instances of access to the heap and using the appropriate accessor function to either get the data or return a pointer to the data. An additional function is provided to convert a pointer into an offset.
  The advantage of this approach is that the data uses is a 16 bit offset value, i.e. the same size as the original used. Additionally if the base offset is correct, any offsets written to intermediate files will match the ones the native tools generate.

#### Address of parameters

For PL/M 80, unless a procedure is marked as re-entrant, the passed in parameters are copied into locally reserved consecutive memory locations. This allows the address to be taken of a passed in parameter. In C this may fail as the compiler may pass parameters in registers, or the data will be padded to stack variable boundaries. As an example from the PL/M 80 compiler, *note the data types follow  an earlier convention* 

```
WrTx2Item2Arg: PROCEDURE(arg1b, arg2w, arg3w) ADDRESS public;
    DECLARE arg1b BYTE, (arg2w, arg3w) ADDRESS;
    call Sub$4251(.arg1b, 5);
    return T2CntForStmt;
end;

Here the three variables are passed to Sub$4251 though the use of .arg1b.
The C code I used is

word WrTx2Item2Arg(byte arg1b, word arg2w, word arg3w)
{
#pragma pack(push, 1)
    struct { byte arg1; word arg2, arg3; } tmp = { arg1b, arg2w, arg3w };
#pragma pack(pop)
    Sub_4251((pointer)&tmp, 5);
    return t2CntForStmt;
}
```

### Conditional expressions

For conditional expressions PL/M uses the least significant bit of a value with 1 representing true and 0 false. Additionally PL/M does not have the short circuit evaluation options available in C i.e. && and ||.

Fortunately in many cases it is possible to translate PL/M expressions to use the C short circuit versions however each needs to be checked. Some key points to note are:

- Simple variables in conditionals e.g. 

  ```
  if var then
  ```

  If these variables are used as true/false in the application then no change is required, otherwise use

  ```
  if (var & 1) then
  ```

- Function invocation in  conditionals e.g. 

  ```
  if isalpha(c) or isnumeric(c) then
  ```

  If any of the functions have side effects then you will need to replace and / or with the non short circuit variants. Occasionally this can be avoided by re-arranging the order of the tests by putting non side effect functions at the end, allowing them to use the short circuit evaluation.

- Setting a value to the result of a conditional expression e.g. 

  ```
  test = a > b;
  ```

  All uses of test need to be reviewed. If test is only ever used as a boolean value, then the expression should be ok, if not use 

  ```
  test = a > b ? 0xff : 0;
  ```

   The default C would set true to 1 rather than 0xff.

  When setting a value additional care is needed to check whether the target is actually boolean. For example the following relies on how PL/M handles conditional and could be used to set the debugFlags to verboseFlags if debugLevel > 2, else clear debugFlags

  ```
  debugFlags = verboseFlags and debugLevel > 2;
  ```

  In C this can be represented by

  ```
  debugFlags = verboseFlags & (debugLevel > 2 ? 0xff : 0);
  or more natrually as
  debugFlags = debugLevel > 2 ? verboseFlags : 0;
  ```

Note it may be possible to replace some nested PL/M if statements by more idiomatic C short circuit code for example.

```
declare val based ptr byte;
if ptr <> 0 then
	if val <> 0 then
		...

can be replaced by
if (ptr && *(byte *)ptr)
	...

```

### 8 bit arithmetic

Unlike C, In PL/M 8 bit values are processed using 8 bit arithmetic and only promoted to 16 bit when involved with 16 bit numbers. Care needs to be taken to identify these cases and force the 8 bit calculation in C, for example by using a (uint8_t) cast.

Some examples, note the different handling for arrays when the compiler detects a index + fixed offset.

```
declare a address, b byte, c(257) byte, d byte;
a = -1;				/* sets a = 0xff */
a = -double(1);		/* sets a = 0xffff */
a = double(-1);		/* sets a = 0xff */
a = 255 + 1;		/* sets a = 0 */
b = 255;
d = 1;
a = b + 1;			/* sets a = 0 */
a = b + d;			/* sets a = 0 */
c(b + d) = 0;		/* sets c(0) = 0 */
					/* but !!! */
c(b + 1) = 0		/* sets c(256) = 0 - because the compiler adds 255 to .c(1) */
c(b := b + 1) = 0;	/* sets c(0) = 0 */
```

### Expression order

In C the order of evaluation of expressions is not guaranteed in all cases so simple PL/M may need to be split. Probably the most common example I have seen is to get a two byte word value from a data stream. In PL/M the following works

```
return getc() + getc() * 256;
```

In C the order of calling of the two getc() is not specified, so this has to be translated to

```
{
int c = getc();
return c + getc() * 256;
}
```

The same type of modification would be needed if the side effects of various calls needed to be done in order.

#### Nested procedures

As C does not support nested procedures, these need to be extracted and raised to file scope. For simple procedures it may be possible to define a macro to replace the code, undefining the macro after it is used. The preferred method however is to create a file level static function, possibly renamed to indicate that it was a nested procedure.

As nested procedures have access to their parent variables, these will need to be passed in either as value parameters if not modified or the addresses of the parameters if they modify their parent's data. The code will need to be changed to accommodate this change.

Potentially a single parameter change could be done via return value unless the nested procedure already returned a value.

#### Public labels

PL/M allows code to jump to known global locations and in doing so resetting the stack pointer. The C equivalent of this is longjmp. In general the modifications to use this are reasonably straight forward, however some careful analysis is required to make sure that longjmp is replicating the functionality of the PL/M label. Some minor code modifications may be needed to make this work correctly.

#### Chaining

As memory is no longer a major issue, I usually merge all overlays into a single application, with effectively a control switch that transfers control between the various pseudo overlays.

In addition to avoiding overlays this also allows for code sharing to be done, however it does involve extra work to make sure that functions and variable names are not in conflict and make sure that variables are initialised appropriately at the start of each pseudo overlay.

------

```
Updated by Mark Ogden 13-Nov-2023
```