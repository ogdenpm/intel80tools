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

Another change has been the introduction of a new plm preporcessor to support
plm v3.x files. This has allowed me to radically reduce the source file set
for asm80 with the makefile auto generating different files from a common
source using the pre-processor. 

Some additional tools and soruce have now been included along with the source
to some of the tools I have written. Key changes are
relst.pl, delib.pl, ngenpex.exe, plmpp.exe, plm80lib and systemlib source.

What's here
plm80v3\*.*	this is the plm80 version 3 compiler that is needed for
		some of the modules !! The source files have the .pl3 extension

plm80v4\*.*	basically most of the plm80 version 4 compiler and utilities
		taken from the intel emulator package

isis.exe	the intel isis emulator for msdos - no longer used
isis.doc	and its documentation

thames.exe	John Elliott's intel isis emulator used in the build tree as isis.exe does
		not work on 64bit windows. It contains the fixes I submitted to John
		Another change is that now debug level 4 gives out a little
		more detail on memory changes. This was to help me to trace
		the intel code.

nmake.exe	the microsoft nmake utility taken from the microsoft web site
nmake.err


mkdepend.pl	perl script to auto generate dependency information in the makefile
		cd to where the makefile is and run this script
		Note there are some minor limitations on the script
		1) it assumes the tools files are on :f1: and will map this drive
		   to the relevant tools directory $(V4) for .plm and .asm and
		   $(V3) for .pl3 files. No other extensions are processed
		2) lines of the format objfile.obj: srcfile.(plm|asm|pl3) are
		   processed but multiple objfile.obj before the : are not nor
		   are additional source dependencies, although nmake will handle
		   these
		3) avoid target files of format $(F1)file as the $(F1) will depend on
		   the tools used

unpack.pl	perl script to unpack the combined source files I use personally
		as the master files as it allows easier global search / replace.
		The script only unpacks changed files - note no longer calls
		mkdepend.pl to update the makefile. Makefiles modified to
		handle this
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

		
src\clean.bat	cleans out the source tree of recreatable files
src\mkall.bat	makes all the files in the source tree and checks they
		match the intel binaries

Note: each of the source directories listed below as an associated makefile.
To use this change to the directory and enter one of the following commands

..\..\nmake		- builds the files
..\..\nmake verify	- builds the files and verifies with the intel binary
..\..\nmake clean	- cleans the directory
..\..\nmake vclean	- cleans additional files - not supported on all makefiles
..\..\nmake uclean	- cleans everything apart from the packed source file
			  where used
..\..\nmake gitprep	- runs uclean and then unpacks files stored in git -
			  not used in all cases


src\asm80\*.*	plm & asm recreated source for asm80
		note asm80_all.plm is the packed source file I use to edit the code
		as it makes global find / replace a lot easier.
		It is a packed file of all of the source. The unpack.pl
		utility is used to unpack to individual files
		Recent change is that the listing and object files are now
		created in separate directories. This makes the makefile more complex
		but makes finding files easier. Additionally debug versions of
		the application files are create in the obj directory and the
		map files in the list directory now have the local symbol detail.

src\lib80\*.*	plm & asm recreated source for lib80
		note the file lib80_all.plm is the file I personally use to edit
		the code. See comments above

src\link80\*.*	plm & asm recreated source for link and link.ovl
		note the file link80_all.plm is the file I personally use to edit
		the code. See comments above

src\loc80\*.*	plm & asm recreated source for locate
		note loc80_all.plm is the packed source file I use to edit the code
		see comments above

src\common\*.*	contains small plm & asm files and associated include files
		used by several of the plm builds.
		All the plm makefiles refer to this directory and for the isis
		emulator it is treated as simulated drive :f3:
		The makefile in this directory now will build all the files
		however the individual plm makefiles will build any changed files
		they need as well

src\plm\*.*	recreated plm source for the plm80 file
src\plm0\*.*	recreated plm & asm source for plm80.ov0
src\plm1\*.*	recreated plm for plm80.ov1
src\plm2\*.*	recreated plm for plm80.ov2
src\plm3\*.*	recreated plm for plm80.ov3
src\plm4\*.*	recreated plm for plm80.ov4
src\plm5\*.*	recreated plm for plm80.ov5
src\plm6\*.*	recreated plm for plm80.ov6



src\isis41\*.*	files to build isis4.1
		the source files are packed in isis41_all.plm use unpack.pl to
		extract them or the m.bat file - the other files are
		1) isis.abs - the reference isis.abs used to check the build
		2) m.bat - simple batch file to unpack and build

src\isist0\*.*	files to build isis4.1 boot file
		the source files are packed in isist0_all.plm use unpack.pl
		extact them or the m.bat file - the other files are
		1) isis.t0.ref - reference file to check build
		2) m.bat - simple batch file to unpack and build

src\isis.cli\*.* files to build isis4.1 cli file

src\kermit\*.*	files to build kermit for isis

src\tex\*.*	files to build tex v2.0 and v2.1 for cpm also a patched version
                of v2.1 that fixes a bug in printing text only

tools\*.*	tools I wrote to help me decompile / build the files
  delib.pl	tool to split a library out into individual object files
		usage: delib.pl libraryfile targetdir
		note it uses the module name as the filename and this may not
		be a valid isis filename 
  diffbin.exe	a small utility to compare intel files with respect to final
		image load in memory. Originally created as I left debug
		symbols, however it is needed to verify that the link has built
		correctly. Simple binary compare cannot be used for these
		because:
		1) link has a absolute segment load split into two parts that
		   I cannot recreate since locate joins them together.
		2) link.ovl has a start record that locate refuses to create
		   since it's not a main module
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