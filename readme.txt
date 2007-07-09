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

isis.exe	the intel isis emulator for msdos
isis.doc	and its documentation

nmake.exe	the microsoft nmake utility taken from the microsoft web site
nmake.err

diffbin.exe	a small utility to compare intel files with respect to final
diffbin.cpp	image load in memory. Originally created as I left debug
		symbols, however it is needed to verify that the link has built
		correctly. Simple binary compare cannot be used for these
		because:
		1) link has a absolute segment load split into two parts that
		   I cannot recreate since locate joins them together.
		2) link.ovl has a start record that locate refuses to create
		   since it's not a main module

src\clean.bat	cleans out the source tree of recreatable files
src\mkall.bat	makes all the files in the source tree and checks they
		match the intel binaries

Note: each of the source directories listed below as an associated makefile.
To use this change to the directory and enter one of the following commands

..\..\nmake		- builds the files
..\..\nmake verify	- builds the files and verifies with the intel binary
..\..\nmake clean	- cleans the directory

src\link80\*.*	plm & asm recreated source for link and link.ovl	

src\loc80\*.*	plm & asm recreated source for locate

src\common\*.*	contains small plm & asm files and associated include files
		used by several of the plm builds.
		All the plm makefiles refer to this directory and for the isis
		emulator it is treated as simulated drive :f3:
		Note the makefile in this directory only supports clean
		and a do nothing entry

src\plm\*.*	recreated plm source for the plm80 file
src\plm0\*.*	recreated plm & asm source for plm80.ov0
src\plm1\*.*	recreated plm for plm80.ov1
src\plm2\*.*	recreated plm for plm80.ov2
src\plm3\*.*	recreated plm for plm80.ov3
src\plm4\*.*	recreated plm for plm80.ov4
src\plm5\*.*	recreated plm for plm80.ov5
src\plm6\*.*	recreated plm for plm80.ov6

src\asm80\*.*	plm & asm recreated source for asm80
		note asm80a.plm is a single file containing all the source
		the perl script save.pl will extract the individual files
		this was done to allow global name changes to be done easily
regards
Mark Ogden
07-Jan-2007
