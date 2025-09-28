# path to root of build tree
ITOOLS ?= ..\..
# location of the msdos build tools
MSDOS := $(ITOOLS)\msdos

# the raw built binaries
BIN_88 = 8825 8840 8845
BIN_86 = 8635
BIN_188 = 18848
BIN_186 = 18603A 18651
BIN_286 = 286100  28610A 3861XX  3863X 

# the derived prom names for 8 bit, 16 processors and 286
PROMS_8 = $(foreach P,$(BIN_88) $(BIN_188),$P.PRM)
PROMS_16 = $(foreach P,$(BIN_86) $(BIN_186),$P.ODD $P.EVN)
PROMS_286 = $(foreach P,$(BIN_286),$P.ODD $P.EVN)

# all binaries and all proms
BINARIES := $(BIN_88) $(BIN_86) $(BIN_188) $(BIN_186) $(BIN_286)
TARGETS := $(PROMS_8) $(PROMS_16) $(PROMS_286)

# paths to the build tools
ASM86 = $(MSDOS)\asm86
LINK86 =$(MSDOS)\link86
LOC86 = $(MSDOS)\loc86
ASM286 = $(MSDOS)\asm286
BND286 = $(MSDOS)\bnd286
BLD286 = $(MSDOS)\bld286
AOMF2BIN = $(ITOOLS)\tools\aomf2bin

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
	del *.lst *.mp? *.in *.o28 *.l28 2>NUL

distclean: clean
	del $(BINARIES) $(TARGETS)

rebuild: distclean all

verify:
	echo "verify not implemented"

##################################
# 8088, 8086 and 80186 build rules
##################################
%.O86: %.A86 cnf1.mac cnf2.mac
	$(ASM86) $< object($@) macro(100) errorprint include(omf86.inc)

%.L86: %.O86
	$(LINK86) $<,cnfsdm.lib,8087.lib to $@ print($*.mp1)

%: %.L86
	$(file >$*.IN,addresses(classes( &$(\n)\
		data(400h), &$(\n)\
		code(0f8240h),&$(\n)\
		restart(0fff80h))) &$(\n)\
		start($(START)) bootstrap print($*.mp2) $(INIT))
	-$(LOC86) $< ^& <$*.IN
	-rm $*.IN

###################
# 80286 build rules
###################
%.O28: %.A28 cnf1.mac cnf2.mac
	-$(ASM286) $< object($@) pagewidth(132) macro(100) include(omf286.inc)

%.L28: %.O28
	$(BND286) $<,cnfsdm.lb2 object($@) print($*.mp1) name($*) noload

%: %.L28
	$(BLD286) $< object($@) print($*.mp2) buildfile(cnf286.bld) bootload


# rule to build single PROM cf. prom88.csd
%.PRM: %
	$(AOMF2BIN) -b $(BASE) $< $@


# rule to build odd / even PROMS cf. prom86.csd
%.ODD %.EVN: %
	$(AOMF2BIN) -b $(BASE) $< -o $*.ODD -e $*.EVN


# force dependency match
8825.PRM: 8825
8840.PRM: 8840
8845.PRM: 8845

18848.ODD 18848.EVN: 18848

8635.ODD 8635.EVN: 8635

18603A.ODD 18603A.EVN: 18603A
18651.ODD 18651.EVN: 18651

286100.ODD 286100.EVN: 286100
28610A.ODD 28610A.EVN: 28610A
3861XX.ODD 3861XX.EVN: 3861XX
3863X.ODD 3863X.EVN: 3863X

