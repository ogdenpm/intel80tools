# path to root of build tree
ROOT := ..\..
# the raw built binaries
# note files need to have the none extent name in upper case as gnu make is case sensitive
TARGETS_88 = 8825 8840 8845
TARGETS_86 = 8635
TARGETS_188 = 18848
TARGETS_186 = 18603A 18651
TARGETS_286 = 286100  28610A 3861XX  3863X 

# the derived proms
PROMS_88 = $(foreach P,$(TARGETS_88),$P.PRM)
PROMS_86 = $(foreach P,$(TARGETS_86),$P.ODD $P.EVN)
PROMS_188 = $(foreach P,$(TARGETS_188),$P.PRM)
PROMS_186 = $(foreach P,$(TARGETS_186),$P.ODD $P.EVN)
PROMS_286 = $(foreach P,$(TARGETS_286),$P.ODD $P.EVN)

# all binaries and all proms
TARGETS := $(TARGETS_88) $(TARGETS_86) $(TARGETS_186) $(TARGETS_286)
PROMS := $(PROMS_88) $(PROMS_86) $(PROMS_188) $(PROMS_186) $(PROMS_286)

# set up the dos emulator and build tools
MSDOS = $(ROOT)\msdos\msdos
ASM86 = $(MSDOS) $(ROOT)\msdos\asm86
LINK86 = $(MSDOS) $(ROOT)\msdos\link86
LOC86 = $(MSDOS) $(ROOT)\msdos\loc86
ASM286 = $(MSDOS) $(ROOT)\msdos\asm286
BND286 = $(MSDOS) $(ROOT)\msdos\bnd286
BLD286 = $(MSDOS) $(ROOT)\msdos\bld286
IPPS = $(MSDOS) $(ROOT)\msdos\ipps


# the top  level rule to make sure all is up to date
all: $(TARGETS) $(PROMS)

# target to clean up files
.PHONY: clean
clean:
	del $(TARGETS) $(PROMS) *.LST *.MP1 *.MP2 *.in

# below are the build runes

# build customisation for assembly
$(TARGETS_88) $(TARGETS_86) : START=reset_8086
$(TARGETS_88) $(TARGETS_86) : INIT=noinitcode
$(TARGETS_186) : START=montor_186
$(TARGETS_186) : INIT=
# build customisation for PROM creation
$(PROMS_86) $(PROMS_186): BASE=0F8000H
$(PROMS_286) : BASE=0FF8000H
$(PROMS_86) $(PROMS_186): OMF=86
$(PROMS_286): OMF=286

# 8088, 8086 and 80186 build rules
%.o86: %.a86
	$(ASM86) $< object($@) macro(100) errorprint include(omf86.inc)

%.l86: %.o86
	$(LINK86) $<,cnfsdm.lib,8087.lib to $@ print($*.mp1)

%: %.l86
	$(file >loc.in, addresses(classes(data(400h),code(0f8240h),restart (0fff80h))) &)
	$(file >>loc.in,start($(START)) bootstrap print($*.mp2) $(INIT))
	-$(LOC86) $< ^& <loc.in
	@del loc.in

# 80286 build rules
%.o28: %.a28
	-$(ASM286) $< object($@) pagewidth(132) macro(100) include(omf286.inc)

%.l28: %.o28
	$(BND286) $<,cnfsdm.lb2 object($@) print($*.mp1) name($*) noload

%: %.l28
	$(BLD286) $< object($@) print($*.mp2) buildfile(cnf286.bld) bootload

# rule to build single PROM
%.PRM: %
	-@del $@ 2>NUL
	$(file >ipps.in,initialize 86)
	$(file >>ipps.in,format $*($0F8000H) p)
	$(foreach O,3 1 1,$(file >>ipps.in,$O))
	$(file >>ipps.in,0 to $@)
	$(file >>ipps.in,)
	$(file >>ipps.in,exit)
	$(IPPS) <ipps.in
	@del ipps.in

# rule to build odd / even PROMS

%.ODD %.EVN : %
	-@del  $*.ODD $*.EVN 2>NUL
	$(file >ipps.in,initialize $(OMF))
	$(file >>ipps.in,format $*($(BASE)) p)
	$(foreach O,3 2 1,$(file >>ipps.in,$O))
	$(file >>ipps.in,0 to $*.EVN)
	$(file >>ipps.in,1 to $*.ODD)
	$(file >>ipps.in,)
	$(file >>ipps.in,exit)
	$(IPPS) <ipps.in
	@del ipps.in

