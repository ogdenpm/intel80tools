# shared makefile configuration for isis tool builds

# convert \ to /, remove trailing directory separator and set to . if blank
# $(call fixpath,path)
fixpath = $(if $1,$(subst |,,$(subst /|,,$(subst \,/,$1)|)),.)

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
ISIS:=$(ROOT)/thames
PLMPP:=$(ROOT)/tools/plmpp
NGENPEX:=$(ROOT)/tools/ngenpex
MKDEPEND:=perl $(ROOT)/tools/makedepend.pl
ERRTEST:= perl $(ROOT)/tools/errtest.pl

ISISTOOLS := $(V40)
# ISIS tools - all show mapped command line on stderr
ASM=$(ISIS) -m $(ISISTOOLS)/asm80
PLM=$(ISIS) -m $(ISISTOOLS)/plm80
LINKU=$(ISIS) -m -u $(ISISTOOLS)/link
LINK=$(ISIS) -m $(ISISTOOLS)/link
	LIB=$(ISIS) -m $(ISISTOOLS)/lib
LOCATE=$(ISIS) -m $(ISISTOOLS)/locate
LOCATEU=$(ISIS) -m -u $(ISISTOOLS)/locate
LOCATEO=$(ISIS) -m -o $(ISISTOOLS)/locate

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
rel = $(OBJ)/$(basename $(notdir $1)).rel


## the generic build commands
# $(call plm,objfile,srcfile,extra options)
define plm
  $(if $(PEXFILE),$(NGENPEX) $(PEXFILE) $2,$(MKDEPEND) $1 $2)
  @$(PLM) $2 "print($(call lst,$2))" "object($1)" "$(PLMFLAGS)" "$3"
endef

# $(call asm,objfile,srcfile,extra options)
define asm
  @$(ASM) "$2 print($(call lst,$2)) object($1) $(ASMFLAGS) $3"
endef

$(OBJ)/%.obj: $(SRC)/%.plm  | $(filter-out .,$(OBJ) $(LST))
	$(call plm,$@,$<)

$(OBJ)/%.obj: $(SRC)/%.asm  | $(filter-out .,$(OBJ) $(LST))
	$(call asm,$@,$<)

# link with no check for unresolved
# $(call link-nocheck,relocfile,objs)
define link-nocheck
  @$(LINKU) "$(call mklist,$2)" to $1 map "print($(call lnk,$1))" "$(LINKFLAGS)"
endef

# standard link
define link
  @$(LINK) "$(call mklist,$2)" to $1 map "print($(call lnk,$1))" "$(LINKFLAGS)"
endef

# $(call locate-nocheck,target,relocfile,options)
define locate-nocheck
  @$(LOCATEU) $2 to $1 "print($(call map,$2))" "$(LOCATEFLAGS)" "$3"
endef

# locate allowing overlaps
define locate-overlaps
  @$(LOCATEO) $2 to $1 "print($(call map,$2))" "$(LOCATEFLAGS)" "$3" 
endef

define locate
  @$(LOCATE) $2 to $1 "print($(call map,$2))" "$(LOCATEFLAGS)" "$3" 
endef

# $(call rm-symbols,target,source)
define rm-symbols
  @$(LOCATE) $2 to $1 PURGE
endef

# $(call lib,target,objects)
define lib
  rm -fr $1
  @$(LIB) "&&"\
  create $1 "&&"\
  add $(if $(filter-out 0 1,$(words $2)),$(call mklist,$(call notlast,$2))$(comma)"&")\
  $(lastword $2) to $1 "&&"\
  exit
endef


# common targets
.PHONY: all
ifdef MASTERFILE
  all: .extract

  .extract: $(MASTERFILE) | $(filter-out .,$(OBJ) $(LST))
	perl $(ROOT)/unpack.pl
	touch .extract
else
  all:
endif


ifneq '$(OBJ)' '.'
  $(OBJ):
	mkdir $(OBJ)
endif
ifneq '$(LST)' '.'
  $(LST):
	mkdir $(LST)
endif

# special handling of windows fc /b command for compare
verify: all
	$(if $(filter fc,$(firstword $(COMPARE))),\
	@cmd /c "for %i in ($(TARGETS)) do fc /b $(subst /,\,%i $(REF)/%i)",\
	@for f in $(TARGETS); do echo comparing files $$f and $(REF)/$$f && $(COMPARE) $$f $(REF)/$$f; done)

rebuild: distclean all

## housekeeping rules
.PHONY: clean
clean:
	-$(if $(filter-out .,$(OBJ)),rm -fr $(OBJ),rm -fr *.obj *.abs) 2>$(null)
	-$(if $(filter-out .,$(LST)),rm -fr $(LST),rm -fr *.lst *.lnk *.map) 2>$(null)
ifndef PEXFILE
	-rm -fr .deps 2>$(null)
else
	-rm -fr *.ipx
endif

distclean: clean 
ifdef MASTERFILE
	-rm -fr $(filter-out mk makefile $(REF) $(MASTERFILE) $(PROTECT),$(shell ls)) $(TARGETS) .extract 2>$(null)
else
	-rm $(TARGETS) 2>$(null)
endif

ifndef PEXFILE
include $(wildcard .deps/*.d)
endif


.DELETE_ON_ERROR:
