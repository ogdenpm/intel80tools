# shared makefile configuration for isis tool builds

# convert \ to /, remove trailing directory separator and set to . if blank
# $(call fixpath,path)
fixpath = $(if $1,$(subst |,,$(subst /|,,$(subst \,/,$(strip $1))|)),.)

# normalise ROOT path
ROOT := $(call fixpath,$(ROOT))
#
# make sure bash and other unix tools are on the path
PATH := $(ROOT)/unix;$(PATH)
SHELL := bash.exe
COMPARE ?= fc /b
# nul device /dev/null on unix
null := nul


# set up the ISIS environment
V40:=$(ROOT)/plm80v40
V31:=$(ROOT)/plm80v31
V30:=$(ROOT)/plm80v30

# default directories to . if not set
SRC := $(call fixpath,$(SRC))
LST := $(call fixpath,$(LST))
OBJ := $(call fixpath,$(OBJ))
#  directory for reference files, default version 4 tools
REF := $(call fixpath,$(if $(REF),$(REF),$(V40)))

# ISIS disk usage - :F0: will default to current dir
export ISIS_F0 ?= ./

# the none ISIS build tools
ISIS:=$(ROOT)/thames -m
PLMPP:=$(ROOT)/tools/plmpp
NGENPEX:=$(ROOT)/tools/ngenpex
MKDEPEND:=perl $(ROOT)/tools/makedepend.pl

ISISTOOLS := $(V40)
# ISIS tools - all show mapped command line on stderr
ASM = $(ISISTOOLS)/asm80
PLM = $(ISISTOOLS)/plm80
LINK = $(ISISTOOLS)/link
LIB = $(ISISTOOLS)/lib
LOCATE = $(ISISTOOLS)/locate

# work out the master source file to protect
MASTERFILE:=$(wildcard *_all.plm)

## macros used for the build
# useful make escapes
comma := ,
space :=
space +=

# all but the last word $1
notlast = $(wordlist 2,$(words $1),dummy $1)
# create a comma separated list from $1
mklist = $(subst $(space),$(comma),$1)

# macros to add common directory prefix to list $1
objdir = $(addprefix $(OBJ)/,$(notdir $1))
srcdir = $(addprefix $(SRC)/,$(notdir $1))
lstdir = $(addprefix $(LST)/,$(notdir $1))
v30dir = $(addprefix $(V30)/,$(notdir $1))
v31dir = $(addprefix $(V31)/,$(notdir $1))
v40dir = $(addprefix $(V40)/,$(notdir $1))

# macros to generate suitable output file names from $1
lnk = $(LST)/$(basename $(notdir $1)).lnk
map = $(LST)/$(basename $(notdir $1)).map
lst = $(LST)/$(basename $(notdir $1)).lst

vpath %.plm $(SRC)
vpath %.asm $(SRC)

## the generic build commands
# $(call plm,objfile,srcfile[,target specific options])
define plm
  $(if $(PEXFILE),$(NGENPEX) $(PEXFILE) $2,$(MKDEPEND) $1 $2)
  @$(ISIS) $(PLM) $2 "print($(call lst,$2))" "object($1)"$(if $(PLMFLAGS), "$(PLMFLAGS)")$(if $3, "$3")
endef

# $(call asm,objfile,srcfile[,target specific options])
define asm
  @$(ISIS) $(ASM) $2 "print($(call lst,$2))" "object($1)"$(if $(ASMFLAGS), "$(ASMFLAGS)")$(if $3, "$3")
endef

# standard link
# $(call link,relocfile,objs[,target specific options])
define link
  @$(ISIS) $(LINK) "$(call mklist,$2)" to $1 map "print($(call lnk,$1))"$(if $(LINKFLAGS), "$(LINKFLAGS)")$(if $3, "$3")
endef

# link with no check for unresolved
# $(call link-nocheck,relocfile,objs[,target specific options])
define link-nocheck
  @$(ISIS) -u $(LINK) "$(call mklist,$2)" to $1 map "print($(call lnk,$1))"$(if $(LINKFLAGS), "$(LINKFLAGS)")$(if $3, "$3")
endef

# standard locate
# $(call locate,target,relocfile[,target specific options])
define locate
  @$(ISIS) $(LOCATE) $2 to $1 "print($(call map,$2))"$(if $(LOCATEFLAGS), "$(LOCATEFLAGS)")$(if $3, "$3") 
endef

# locate with no check for unsatisfied
# $(call locate-nocheck,target,relocfile[,target specific options])
define locate-nocheck
  @$(ISIS) -u $(LOCATE) $2 to $1 "print($(call map,$2))"$(if $(LOCATEFLAGS), "$(LOCATEFLAGS)")$(if $3, "$3") 
endef

# locate allowing overlaps
# $(call locate-overlaps,target,relocfile[,target specific options])
define locate-overlaps
  @$(ISIS) -o $(LOCATE) $2 to $1 "print($(call map,$2))"$(if $(LOCATEFLAGS), "$(LOCATEFLAGS)")$(if $3, "$3") 
endef

# limited version of locate to remove symbols - does not produce map file
# $(call rm-symbols,target,source)
define rm-symbols
  @$(ISIS) $(LOCATE) $2 to $1 PURGE
endef

# $(call lib,target,objects)
define lib
  rm -fr $1
  @$(ISIS) $(LIB) "&&"\
  create $1 "&&"\
  add $(if $(filter-out 0 1,$(words $2)),$(call mklist,$(call notlast,$2))$(comma)"&")\
  $(lastword $2) to $1 "&&"\
  exit
endef

# predefined rules
$(OBJ)/%.obj: %.plm  | $(OBJ) $(LST)
	$(call plm,$@,$<)

$(OBJ)/%.obj: %.asm  | $(OBJ) $(LST)
	$(call asm,$@,$<)

# common targets
.PHONY: all rebuild clean distclean
# all is default first rule
# if master file format is being used add rules to extract the source
# user rules for all will be in the calling makefiles
ifdef MASTERFILE
all:: .extract

.extract: $(MASTERFILE) | $(OBJ) $(LST)
	perl $(ROOT)/unpack.pl
	touch .extract
else
all::
endif

# rules to make sure directories exist
.: ;

ifneq '$(OBJ)' '.'
$(OBJ): ; mkdir -p $(OBJ)
endif
ifneq '$(LST)' '.'
$(LST): ; mkdir -p $(LST)
endif

# special handling of windows fc /b command for compare
ifndef OWNVERIFY
verify: all
	$(if $(filter fc,$(firstword $(COMPARE))),\
	@cmd /c "for %i in ($(TARGETS)) do fc /b $(subst /,\,%i $(REF)/%i)",\
	@for f in $(TARGETS); do echo comparing files $$f and $(REF)/$$f && $(COMPARE) $$f $(REF)/$$f; done)
endif

rebuild: distclean all

## housekeeping rules
clean::
	-$(if $(filter-out .,$(OBJ)),rm -fr $(OBJ),rm -fr *.obj *.abs) 2>$(null)
	-$(if $(filter-out .,$(LST)),rm -fr $(LST),rm -fr *.lst *.lnk *.map) 2>$(null)
ifndef PEXFILE
	-rm -fr .deps 2>$(null)
else
	-rm -fr $(SRC)/*.ipx
endif

distclean:: clean 
ifdef MASTERFILE
	-rm -fr $(filter-out mk makefile $(REF) $(MASTERFILE) $(PROTECT),$(shell ls)) $(TARGETS) .extract 2>$(null)
else
	-rm $(TARGETS) 2>$(null)
endif

ifndef PEXFILE
include $(wildcard .deps/*.d)
endif


.DELETE_ON_ERROR:
