The ml80 suite was originally compiled with an unknown version of
Intel's Fortran based PL/M 80 compiler.
Two versions of this compiler are in circulation but neither matches the
version used on ml80.
After some investigation, by disabling two of the optimisations introduced in
the V4 fortran compiler, I have been able to create a version that works.
Note I actually created my own C versions of the compiler, by translating the
fortran to ratfor and then into C. It is still a work in progress and I haven't released
the source yet but potentially the original fortan code could be patched if required.

The specific optimisations that I disabled are
	adi,0, sui,0, xri,0 and ori,0 optimisations are disabled
	and,0 optimisation is disabled

Below are the list of patch files that help put the same junk into unitialised data areas
and apply manual patches found in the original distribution files
If you wish to a particular patch file copy it from the patchFiles directory to the the same directory
as the source files and rename it to {plmfile}.patch
e.g. the patch file for m81 is m81.patch

In the list below * items are the default installed patch files
m81
m81.patch*	used to create copy of ref/m81.com	(CPMUG04)
	hlt ei nulled out to fall through to reboot.
	The two instructions at 1A40H are swapped but the effect is the same
	Some of the uninitialised data area has garbage in the original, 0 in the generated.

m81a.patch	used to create copy of ref/m81a.com	(CPMUG36)
	hlt ei nulled out to fall through to reboot.
	The two instructions at 1A40H are swapped but the effect is the same
	Some of the uninitialised data area has garbage in the original, 0 in the generated.

m81.minpatch	minimal patch to get the file to work
	hlt ei nulled out to fall through to reboot.

l81
l81.patch*	used to create copy of ref/l81.com	(CPMUG04)
	hlt ei nulled out to fall through to reboot.
	Some of the uninitialised data area has garbage in the original, 0 in the generated. Could patch.
	Extra garbage padding at end of file in original

l81.minpatch	minimal patch to get the file to work
	hlt ei nulled out to fall through to reboot.


l82.patch*	used to create copy of ref/l82.com	(CPMUG04)
	TTOP set to 30H vs.  32H
	Extra garbage padding at end of file in original.

L82a.patch	used to create copy of ref/m81a.com	(CPMUG36)
	large number of manually applied patches that allow original to be recreated
	the patch file has more detailed information
	One set patches fixes a critical bug, the others are mainly to allow for larger program size

Note the file l82fix.plm applies the fixes to the source code, but due how the compiler optimises
address loads it is not possible to get byte alignment.
Unless byte matching is required it is recommended to use the l82fix.com file



l83.patch*	used to create copy of ref/l83.com	(CPMUG04)
	Code is the same apart from two spurious NOPs that shifts a small block of code in READCH by one location.
	The files are realigned bu adding a FIX DATA(0) statement.
	Note the compiled code adds a RET at the end of the while 1, an alternative is to replace the while 1
	with explicit gotos and put two fix bytes.
Note this patch is not needed for functionality, you can delete it from the source directory
if do not need byte level matching

