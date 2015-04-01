These files are my attempts at decompiling the ISIS plm80 compiler and these
associated utilities link and locate.

During the course of decompiling the code I have generated variable names
that at the time seemed sensible. I have not systematically reviewed these
to see if they are logically correct, so please be aware of this when you
read the source code.


What's here
plm80v3\*.*	this is the plm80 version 3 compiler that is needed for
		some of the modules !! The source files have the .pl3 extension

plm80v4\*.*	basically most of the plm80 version 4 compiler and utilities
		taken from the intel emulator package

isis.exe	the intel isis emulator for msdos - no longer used
isis.doc	and its documentation

thames.exe	John Elliott's intel isis emulator used in the build tree as isis.exe does
		not work on 64bit windows. It contains the fixes I submitted to John

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
		The script only unpacks changed files and runs the same code as
		mkdepend.pl to update the makefile
		Note the packed file format is Ctrl-L filename newline file content
		repeated for all files. The Ctrl-L separates the files

rebase.pl	perl script to allow mapping of addresses in plm listings to real
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


src\asm80\*.*	plm & asm recreated source for asm80
		note asm80_all.plm is the packed source file I use to edit the code
		as it makes global find / replace a lot easier.
		It is a packed file of all of the source. The unpack.pl
		utility is used to unpack to individual files

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

tools\*.*	tools I wrote to help me decompile / build the files
  diffbin.exe	a small utility to compare intel files with respect to final
		image load in memory. Originally created as I left debug
		symbols, however it is needed to verify that the link has built
		correctly. Simple binary compare cannot be used for these
		because:
		1) link has a absolute segment load split into two parts that
		   I cannot recreate since locate joins them together.
		2) link.ovl has a start record that locate refuses to create
		   since it's not a main module
  isisc.exe	a utility to check the isis4.1 build is compatible with the reference
  isisu.exe	a utility to unpack the isis boot file to show the load addresses
		and length of each block - not needed for the build
  obj2bin	convert omf85 to a binary file - used to create image for disassembly
  packisis	convert omf85 to isis4.1 load format used for isis.bin i.e. remove
		the 06 header and the crc bytes
  t0bootdiff	compares to bootfile files to show differences

toolsrc\*.*	source files for the above tools in visual studio 2013
		the underlying c and c++ source files can easily be found
		if you are using a different compiler

regards
Mark Ogden
07-Jan-2007
updated 1-Apr-2015
