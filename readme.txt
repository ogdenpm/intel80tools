These files are my attempts at decompiling the ISIS plm80 compiler and these
associated utilities link and locate.

*****************************************************************************
* WARNING: git stores crlf as lf in line with common usage. When cloning    *
*          files you need to make sure thar your local git is set to        *
*          re-expanded lf to crlf otherwise the build will fail             *
*          alternatively use an external utility to convert the text files  *
*****************************************************************************

During the course of decompiling the code I have generated variable names
that at the time seemed sensible. I have not systematically reviewed these
to see if they are logically correct, so please be aware of this when you
read the source code.

I have recently made a number of significant changes to the source files to
make it easier to make changes as I clean up the code.
The biggest is the move to use a tool I developed, ngenpex, based on the intel
toolbox tool genpex. This uses a database of public definitions that is used to
automatically generate an include file with the required external / literal / based
definitions.

Another change has been the introduction of a new plm preprocessor to support
plm v3.x files. This has allowed me to radically reduce the source file set
for asm80 with the makefile auto generating different files from a common
source using the pre-processor. 

Some additional tools and soruce have now been included along with the source
to some of the tools I have written. Key changes are
relst.pl, delib.pl, ngenpex.exe, plmpp.exe, plm80lib and systemlib source.

The May 2017 update includes a major rework of the build scripts to use gnu
make and a significant update to thames to support auto mapping of file paths
to drives and interception of isis error codes.
In addition the isis tool directories have been restructured to ease speficication
of various tool versions.

What's here
itools/*/*.*	the various isis tools each set specified under a sub
		directory of the format tool_version. Tools include
		asm80, plm80, fort80, pasc80, asm86, plm86
		lib, link, locate lib86, link86, loc86
		binobj, hexobj, objhex, conv86
		plm80.lib, system.lib, fpal.lib
		note the fort80 libraries and 8086 libraries have not
		yet been splitout.
		Additionally isis v3.4, 4.1 and 4.3 are included with
		the build tools noted above removed

thames.exe	This is a major update on John Elliott's intel isis emulator
thames32.exe	used in the build tree as isis.exe does not work on 64bit windows.
thamesSrc\*.*	I have included the source of the tool in thamesSrc where you will
		also find a markdown file thames.md that describes the changes
		thames.exe is the 64bit version thames32.exe is the 32bit version

make.exe	64bit and 32bit versions of the gnu make utility
make32.exe

unix\*.*	various unix like tools used for the build

unpack.pl	perl script to unpack the combined source files I use personally
		as the master files as it allows easier global search / replace.
		The script only unpacks changed files.
		Note the packed file format is Ctrl-L filename newline file content
		repeated for all files. The Ctrl-L separates the files
		Now supports multi level directory expand.

relst.pl	perl script that takes a listing file and a map file and
		creates new listing file with the addresses relocated to
		actual addresses. It does not change code addresses
		usage: relst.pl mapfile listfiles

rebase.pl	(depreciated by relst.pl but may be required if there are no public
		symbols in the listing file)
		perl script to allow mapping of addresses in plm listings to real
		addresses of the located code
		usage: rebase.pl listingfile listingaddr realaddr
		where listingaddr is an address in hex as shown in the listingfile
		and realaddr is the address it is located to
		the tool writes to the console so you most likely want to redirect
		to a file


		
src\makefile	makefile to manage all of the builds. Runs make in each of the
		subdirectories noted below. It supports the same common targets

src\makefile.md	markdown description of the makefile support for isis builds

Note: each of the source directories listed below contains a makefile.
To use this change to the directory and enter one of the following commands.

..\..\make all		- builds the files, the all is optional
..\..\make verify	- builds the files and verifies with the intel binary
..\..\make clean	- cleans the directory
..\..\make distclean	- cleans out files not needed to rebuild from source
..\..\make rebuild	- runs distclean followed by all


src\asm80_4.1\*.* plm & asm recreated source for asm80
		asm80_all.plm is a packed file of all of the source.

src\lib_2.1\*.*	plm & asm recreated source for lib80 v2.1
		lib_all.plm contains the packed source code.

src\link_3.0\*.* plm & asm recreated source for link and link.ovl v3.0
		link_all.plm contains the packed source code

src\locate_3.0\*.* plm & asm recreated source for locate v3.0
		loc80_all.plm contains the packed source code

src\plm_v4.0\*.* plm & asm recreated source for plm80 v4.0
		 plm_4.0_all.plm contains the packed source code.

src\isis_3.4\*.*	files to build isis 3.4
		 isis34_all.plm contains the packed source code.
		
src\isis.t0_3.4\*.* files to build isis 3.4 boot file
		 isist0_all.plm contains the packed source code.

src\isis_4.11\*.*	files to build isis 4.1
		 isis41_all.plm contains the packed source code.
		
src\isis.t0_4.1\*.* files to build isis 4.1 boot file
		 isist0_all.plm contains the packed source code.

src\isis.cli_4.1\*.* files to build isis 4.1 cli file
		 cli_all.plm contains the packed source code.

src\isis_4.3\*.*	files to build isis 4.3
		 isis43_all.plm contains the packed source code.
		
src\isis.t0_4.3\*.* files to build isis 4.3 boot file
		 isist0_all.plm contains the packed source code.

src\isis.cli_4.3\*.* files to build isis 4.3 cli file
		 cli43_all.plm contains the packed source code.

src\isis.ov0_4.3\*.* files to build isis 4.3 .ov0 file
		 isisov_all.plm contains the packed source code.

src\isisUtil_4.3\*.*	files to build isis 4.3 fixmap, format and idisk
		disk43_all.plm contains the packed source code

src\ixref_1.2\*.* files to build ixref 1.2
		 ixref_all.plm contains the packed source code.
		
src\ixref_1.3\*.* files to build ixref 1.3
		 ixref_all.plm contains the packed source code and the
		 isis.ov0 file needed to test it.

src\plm80.lib\*.* files to build plm80.lib for plm v4.0
		plm80lib_all.plm contains the packed source

src\system.lib_4.0\*.* files to build system.lib for plm v4.0
		systemlib_all.plm contains the packed source

src\kermit\*.*	files to build kermit for isis

src\tex\*.*	files to build tex v1.0, v1.2, v2.1 for cpm also a patched version
                of v2.1 that fixes a bug in printing text only

tools\*.*	tools I wrote to help me decompile / build the files

  isis.mk	include makefile script see src/makefile.md for details
		also look at the makefiles in the source tree

  delib.pl	tool to split a library out into individual object files
		usage: delib.pl libraryfile targetdir
		note it uses the module name as the filename and this may not
		be a valid isis filename.

  disIntelLib.exe
		a program that takes a library file and create assembly like
		files for each module.
		usage: disIntelLib library
		a directory is created with the name name as the library file
		name without the .lib. Individual files are created in the
		directory with a .asm, .plm or .for extension dependent on the
		compiler detected. For plm some of the plm data structures are
		emitted rather than assembler ones.

  dumpintel.exe dumps the contents of an omf file
		usage: dumpintel objectfile

  omfcmp.exe	a small utility to compare intel files with respect to final
		image load in memory. This is a significant update on the
		previous diffbin that now supports library comparison and will do
		a binary compare if the files are not in omf format.
  ngenpex.exe   My own implemenation of the intel software tools utility
		genpex. This fixes a number of issues with the original which
		I have included in the plm80v4 directory along with the
		original documentation.
		The tool takes a master database of public variables,
		literals, procedures, based and label declarations and a
		source plm file. It generates an included file with required
		external and literal definitions for the plm file to compile.
		See the genpex.txt file in plm80v4 for the main details. My
		changes are:
		1) Text in simple strings do not trigger defintions e.g. 'ERROR x'
		   does not cause the error procedure to be defined as an external
		2) The pex file allows a minus prefix to force prevent a match
		3) I have extended the procedure defintion to optionally allow
		   parameter names to be included by following the type indicator
		   with a space, the name and a comma or trailing )
		4) In genpex a trailing S indicates an array variable and a
		   (1) size is written to the ipx file. ngenpex optionally
		   allows a (size) where size can be number or a "literal",
		   this is written to the ipx file. This allows the plm size,
		   length and last functions to be used
		5) Local variable names do not trigger external definitions
		6) literal or variable declarations before the $include line
		   for the generated .ipx file take precedence
		7) Optionally ngenpex will emit public definitions in the plm
		   file into a .pub file to allow an initial pex database to be created.
		   Note it will also emit externals with a # prefix. This
		   allow you to cross check for missing / incorrect usage
 		usage: ngenpex pexfile sourcefile [-p]
		where the -p is optional and generates the .pub file noted above

		Note I have not yet included the source of ngenpex as I think
		some clean up is in order. If you find a problem let me know
		and I will see if there is a simple fix. I should also point
		out that it only works for plm80 source.

  isisc.exe	a utility to check the isis4.1 build is compatible with the reference
  isisu.exe	a utility to unpack the isis boot file to show the load addresses
		and length of each block - not needed for the build
  obj2bin.exe	convert omf85 to a binary file - used to create image for disassembly
  packisis.exe	convert omf85 to isis4.1 load format used for isis.bin i.e. remove
		the 06 header and the crc bytes
  patchbin.exe	a utility to apply patches to an existing .com file
                usage: patchbin patchfile comfile
                where
		format of patch file is
	        address xx yy ...
		where address is the cpm location (file offset + 0x100)
		and xx yy etc. are the new byte values.
		note all values are entered in hex.
		The latest version allows writing beyond the end of the
		comfile, which is useful for tail end padding of files.
  plmpp.exe	Implements the plm preprocessor for $IF $ELSE $ELSEIF and $END
		so that plm 3.x version files can include the preprocessor
		directives. 
		usage: plmpp [-f]* [-F] [-sVAR[=val][,VAR[=val]]*] [-rVAR[,VAR]*] [-o outfile] srcfile
		where
		-f -> each -f causes one level of include files to be flattend. 
		-F -> all include files are flattend
		-s -> same as plm SET on the command line () are omitted
		-r -> same as plm RESET on the command line () are omitted
		-o -> specifies the output file

  t0bootdiff	compares to bootfile files to show differences

  mkisisdir.pl	simple script to create a basic ISIS.DIR file based on files
		matching ??????.???. No link information is included and its
		main use is to support ixref
  makedepend.pl utility to generate a dependency file information.
		usage: perl makedepend.pl objfile srcfile
		ISIS_Fn environment variables need to be set for all of the
		include drives used. If there are any dependencies then
		a suitable .d file is created in the .deps directory.

toolsrc\*.*	source files for the above tools in visual studio 2015
		the underlying c and c++ source files can easily be found
		if you are using a different compiler

regards
Mark Ogden
07-Jan-2007
updated 1-Apr-2015
updated 6-Sep-2015
updated 24-Oct-2015
updated 3-Nov-2015 (added cli)
updated 19-Apr-2016 (added kermit, tex20, tex21 and patchbin tool)
updated 7-Oct-2016 (added warning on crlf expansion)
updated 1-Dec-2016 (added ixref)
updated 10-Dec-2016 (added asxref to the asm80 directory, stuctural changes to
		     the makefile for asm80)
updated 29-Jan-2017 (renamed asm80 files to be more consistent and simpified
		     makefile. Note may move to gnu make at some point to
		     simplifiy further)
updated 1-May-2017  (Updated thames to support auto unix file mapping and isis error handling
		     moved build to use gnu make including tools/isis.mk
		     restructured plm v4.0 tree
		     renamed several directories to be more consistent
		     added plm80v30
		     replaced diffbin by a more comprehensive comparison tool omfcmp
		     added decompilation of isis v3.4 and v4.3 files
		     added disIntelLib, dumpintel, makedepend.pl)
Update 8-May-2017    restructured isis tools directories and changed isis.mk
		     to support this. See src/makefile.md for info
