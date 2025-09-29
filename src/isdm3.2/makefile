# path to root of build tree
ITOOLS ?= ..\..
# location of the msdos build tools
# the raw built binaries
BIN_88 = 8825 8840 8845
BIN_86 = 8635
BIN_188 = 18848
BIN_186 = 18603a 18651
BIN_286 = 286100 28610a 3861xx 3863x 

# the derived prom names for 8 bit, 16 processors and 286
PROMS_8 = $(foreach p,$(BIN_88) $(BIN_188),$p.prm)
PROMS_16 = $(foreach p,$(BIN_86) $(BIN_186),$p.odd $p.evn)
PROMS_286 = $(foreach p,$(BIN_286),$p.odd $p.evn)

# all binaries and all proms
BINARIES := $(BIN_88) $(BIN_86) $(BIN_188) $(BIN_186) $(BIN_286)
TARGETS := $(PROMS_8) $(PROMS_16) $(PROMS_286)

# paths where the tools are
TOOLS=$(ITOOLS)\tools
MSDOS=$(ITOOLS)\msdos
UNIX=$(ITOOLS)\unix

# add unix tools to path to avoid defining any that are used
PATH:= $(UNIX);$(PATH)

ASM86 = $(MSDOS)\asm86
LINK86 =$(MSDOS)\link86
LOC86 = $(MSDOS)\loc86
ASM286 = $(MSDOS)\asm286
BND286 = $(MSDOS)\bnd286
BLD286 = $(MSDOS)\bld286
AOMF2BIN = $(TOOLS)\aomf2bin
LCNAMES = $(TOOLS)\lcnames

# defaults for locate
START=reset_8086
INIT=noinitcode
# overrides for 186
$(BIN_188) $(BIN_186) : START=montor_186
$(BIN_188) $(BIN_186) : INIT=
# defaults for PROM build
BASE=0F8000H
# overides for 286
$(PROMS_286) : BASE=0FF8000H

# macro to allow newline in make rules using $(\n)
define \n


endef

####################################################
# the top  level rule to make sure all is up to date
####################################################
all:
	$(MAKE)	$(TARGETS)

# the msdos applications create tmp files that are not unique
# so force all build to be done serially
.NOTPARALLEL:

# target to clean up files
.PHONY: clean distclean
clean:
	rm -fr *.lst *.mp? *.in *.o28 *.l28 *.o86 *.l86

distclean: clean
	rm -fr $(BINARIES) $(TARGETS)

rebuild: distclean all

verify:
	$(info "verify not implemented")

##################################
# 8088, 8086 and 80186 build rules
##################################
%.o86: %.a86 cnf1.mac cnf2.mac omf86.inc
	$(ASM86) $< object($@) macro(100) errorprint include(omf86.inc)
	$(LCNAMES) $@ $*.LST

%.l86: %.o86
	$(LINK86) $<,cnfsdm.lib,8087.lib to $@ print($*.mp1)
	$(LCNAMES) $@ $*.MP1

%: %.l86
	$(file >$*.in,addresses(classes( &$(\n)\
		data(400h), &$(\n)\
		code(0f8240h),&$(\n)\
		restart(0fff80h))) &$(\n)\
		start($(START)) bootstrap print($*.mp2) $(INIT))
	-$(LOC86) $< ^& <$*.in
	$(LCNAMES) $@ $*.MP2
# comment out next line if you need to check the loc86 command file
	@rm $*.in

###################
# 80286 build rules
###################
%.o28: %.a28 cnf1.mac cnf2.mac omf286.inc
	-$(ASM286) $< object($@) pagewidth(132) macro(100) include(omf286.inc)
	$(LCNAMES) $@ $*.LST

%.l28: %.o28
	$(BND286) $<,cnfsdm.lb2 object($@) print($*.mp1) name($*) noload
	$(LCNAMES) $@ $*.MP1

%: %.l28 cnf286.bld
	$(BLD286) $< object($@) print($*.mp2) buildfile(cnf286.bld) bootload
	$(LCNAMES) -v $@ $*.MP2

# rule to build single PROM cf. prom88.csd
%.PRM: %
	$(AOMF2BIN) -b $(BASE) $< $@


# rule to build odd / even PROMS cf. prom86.csd
%.odd %.evn: %
	$(AOMF2BIN) -b $(BASE) $< -o $*.odd -e $*.evn


# force dependency match
8825.prm: 8825
8840.prm: 8840
8845.prm: 8845

18848.prm: 18848

8635.odd 8635.evn: 8635

18603a.odd 18603a.evn: 18603a
18651.odd 18651.evn: 18651

286100.odd 286100.evn: 286100
28610a.odd 28610a.evn: 28610a
3861xx.odd 3861xx.evn: 3861xx
3863x.odd 3863x.evn: 3863x

