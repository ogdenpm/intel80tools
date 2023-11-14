 have recently made a number of significant changes to the source files to
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

10-May 2017 added several isis tools and isis versions including a very early
version of isis (v2.2) which has the isis.t0 file written in the fortran based
plm80. Due the the increase in isis versions, system.lib, binobj, hexobj and
objhex now reside in the relevant isis directory.

*NOTE*
Although there are multiple versions of system.lib available the later ones
all appear to be backward compatible so unless there is a need to byte match
a tool, the latest version located in itools/utils_2.2 can be used; this is
the one that was with the pl/m 80 v4.0 build freely shared by intel.
Also note that so far I have only located one version of plm80.lib.

17-May 2017 added the ISIS toolbox to the repository, including decompilations
of all of the libraries.

22-May 2017 added v2.0 of the ISIS toolbox.

8-Jun-2017 reworked thames to emulate 8080, this allows support of basic. Added
various new binaries and a new version of relst.pl (see new  details below)

10-Oct 2017 Added -i option to thames to allow files with invalid crc to load
i.e. Cobol80, Added altmap application to isis 4.2w in itools.
As part of my work to convert the asm80 source to C I have reworked the asm80
4.1 source to reflect usage of address types as follows
	word	 var is numeric
	pointer	 var contains the address of a variable. 
	apointer var contains the address of a word variable.
	address	 var is used as both a word and a pointer
Note I also corrected a naming error on asm80_4.1_all.src (I had accidently
named it asm80_4.2_all.src)
The first C port of asm80 is now in the asm80-c directory. But be aware it
currently ignores :Fx: specifications and will load files from the current
directory only. If you compile the code you will get warnings about the pack
pragma. These are safe to ignore, once I get a chance I will clean them up.

28-Nov-2017 Added isis 4.0 decompilations for isis.bin and isis.t0
Note I have done more work on the C port of asm80 but put on a separate feature
branch. Will merge back in once I feel enough work has been done. This branch
includes fixes and adds drive specifier support. If you are interested
check out feature/asm80

10-Dec-2017 Added C ports of asm80 v4.1, lib v2.1, link v3.0, locate v3.0.
	    The command line is as per the ISIS versions and you need to set
	    environment variables of form ISIS_Fn=dir to support :Fx: drive
	    prefixes. As a convienence for all but plm80 :F0: defaults to the
	    current directory.
	    I have also added my old plm80 port to C++. It supports :Fx:
	    prefixes, but :F0: map to current directory and other :Fn: prefixes
	    map to ./fn. At somepoint I will convert to the format used for
	    the other tools.
	    To clean up the directories all the C ports are now under c-ports
	    and managed via a single solution

15-Mar-2018 Added newer C port of plm80.
        Added decompilations of isis.bin and isis.t0 for isis 2.2. The
        compilation of isis.bin required a fix to be applied to the old
        fortran compiler. I will release the corrected source shortly. In the
        meantime compiled plm81 and plm82 with larger symbol, macro and memory
        tables are in the tools directory. The fix in particular was to plm82
        which previously generated incorrect code.
        Added decompilations for objhex (2.2n and 4.3), hexobj (4.3), vers (4.3)
        binobj (3.4), help (ipds 1.1), submit (4.3)
        Added decompilations of isis.t0 and isis.bin for v4.3w.
        Minor fixes to various utilities and thames.
        Improved relst.pl and added pretty.pl that generates embedded plm /
        asm for the old fortran compiler

3-Oct-2019 further refinments of C ports including variable naming
	Added exe files for C ports to tools
	The C ports now compile under linux, although limited testing has been done
	Added asxref support for asm80
	Added note on msdos applications for 8086 build
	Updated readme to reflect re-organisation of code done some time ago

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
Update 17-May-2017   added isis toolbox v1.0
Update 18-May-2017   minor reorganisation of isis toolbox v1.0 to use nested packed files
		     standardised on packed files naming convention to
		     tool_ver_all.src
Update 8-Jun-2017    Updated thames to use 8080 emulation. This allows basic to be run
		     Added isis toolbox 2.0 and a new version of relst.pl
Update 28-Nov-2017   Added decompilations isis.bin and isis.t0 for isis 4.0
Update 10-Dec-2017   Added C ports
Update 15-Mar-2018  Updated this file to reflect recent additions. See notes
             	     earlier in the file.
Update 4-Oct-2019    Added notes re build environment, the c-port executables, the msdos 8086 programming
		     and documented some additional scripts. Also updated locations of small Intel apps