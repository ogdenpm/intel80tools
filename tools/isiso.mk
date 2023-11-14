# shared makefile configuration for isis tool builds

# convert \ to /, remove trailing directory separator and set to . if blank
# $(call fixpath,path)
fixpath = $(if $1,$(subst |,,$(subst /|,,$(subst \,/,$(strip $1))|)),.)

# normalise ITOOLS path
ITOOLS := $(call fixpath,$(ITOOLS))
export _ITOOLS=$(ITOOLS)
#
# make sure bash and other unix tools are on the path
PATH := $(ITOOLS)/unix;$(PATH)
SHELL := bash.exe
COMPARE ?= $(ITOOLS)/tools/omfcmp

# default directories to . if not set
SRC := $(call fixpath,$(SRC))
LST := $(call fixpath,$(LST))
OBJ := $(call fixpath,$(OBJ))

# ISIS disk usage - :F0: will default to current dir
export ISIS_F0 ?= ./

# the none ISIS build tools
ISIS:=$(ITOOLS)/thames -m -T
PLMPP:=$(ITOOLS)/tools/plmpp
ASM80X:=perl $(ITOOLS)/tools/asm80x.pl
NGENPEX:=$(ITOOLS)/tools/ngenpex
MKDEPEND:=perl $(ITOOLS)/tools/makedepend.pl
OBJBIN:=$(ITOOLS)/tools/obj2bin
HEXOBJ:=$(ITOOLS)/tools/hexobj
PLM81:=$(ITOOLS)/tools/plm81
PLM82:=$(ITOOLS)/tools/plm82

# macros to create the path & full file names of an isis file
# usage: $(call ipath,file[,version])
ipath = $(ITOOLS)/itools/$(strip $1)$(if $2,/$(strip $2))
# 
# usage: $(call ifile,file[,version])
ifile = $(call ipath,$1,$2)/$(strip $1)

# set default tool versions if none given
PLM80 ?= 4.0
ASM80 ?= 4.1
LIB80 ?= 2.1
LINK80 ?= 3.0
LOCATE80 ?= 3.0
FORT80 ?= 2.1
ASM48 ?= 4.2
PLM86 ?= 2.1
ASM86 ?= 2.1
LINK86 ?= 1.3
LOC86 ?= 1.3

PLMFLAGS ?= code
PLM86FLAGS ?= code

# the standard libraries
plm80.lib := $(call ifile,plm80.lib)
system.lib,4.0 := $(call ipath,utils,2.2n)/system.lib
system.lib,3.1 := $(call ipath,isis,3.4)/system.lib
system.lib,3.0 := $(call ipath,isis,2.2)/system.lib
system.lib = $(system.lib,$(PLM80))
fpal.lib,2.1 := $(call ifile,fpal.lib,2.1)
fpal.lib,2.0 := $(call ifile,fpal.lib,2.0)
fpal.lib = $(fpal.lib,2.1)

# macros to map versions to real files

# work out the master source file
_masterfile:=$(firstword $(wildcard *_all.src))

## macros used for the build
# useful make escapes
comma := ,
space := $  

# all but the last word $1
notlast = $(wordlist 2,$(words $1),dummy $1)
# create a comma separated list from $1
mklist = $(subst $(space),$(comma),$(strip $1))

# macros to add common directory prefix to list $1
objdir = $(addprefix $(OBJ)/,$(notdir $1))
srcdir = $(addprefix $(SRC)/,$(notdir $1))
lstdir = $(addprefix $(LST)/,$(notdir $1))

# macros to generate suitable output file names from $1
lin = $(LST)/$(basename $(notdir $(firstword $1))).lin
map = $(LST)/$(basename $(notdir $(firstword $1))).map
lst = $(LST)/$(basename $(notdir $(firstword $1))).lst

vpath %.plm $(SRC)
vpath %.asm $(SRC)

## the generic build commands
# $(call plm80,objfile,srcfile[,target specific options])
define plm80
  $(if $(PEXFILE),$(NGENPEX) $(PEXFILE) $2)
  @$(ISIS) $(if $(PEXFILE),,-MF .deps/$1.d) $(call ifile,plm80,$(PLM80)) $2 "object($1)"\
	  $(if $(PLMFLAGS), "$(PLMFLAGS)")$(if $3, "$3")
endef
#  $(if $(PEXFILE),$(NGENPEX) $(PEXFILE) $2,$(MKDEPEND) $1 $2)
#  @$(ISIS) $(call ifile,plm80,$(PLM80)) $2 "object($1)"\
#	  $(if $(PLMFLAGS), "$(PLMFLAGS)")$(if $3, "$3")

# $(call asm80,objfile,srcfile[,target specific options])
#  @$(ISIS) $(call ifile,asm80,$(ASM80)) $2 "object($1)"\
#  
define asm80
  asm80 $2 "object($1)"\
	  $(if $(ASMFLAGS), "$(ASMFLAGS)")$(if $3, "$3")
endef

# $(call asm80x,objfile,srcfile[,target specific options])
# asm80x.pl is a wrapper around asm80 to provide support for long variable
# names, the lst file and the obj file are also modified to make sure the
# long variable names are reflected in these..
# under the covers only asm80 v4.1 is used
define asm80x
  @$(ASM80X) $2 "object($1)" $(if $(ASMFLAGS), "$(ASMFLAGS)")$(if $3, "$3")
endef

# $(call asm48,objfile,srcfile[,target specific options])
define asm48
  @$(ISIS) $(call ifile,asm48,$(ASM48)) $2 \
	  "object($1)"$(if $(ASM48FLAGS), "$(ASM48FLAGS)")$(if $3, "$3")
endef

# $(call fort80,objfile,srcfile[,target specific options])
define fort80
  @$(ISIS) $(call ifile,fort80,$(FORT80)) $2 "print($(call lst,$2))"\
	  "workfiles(:f0:)" "object($1)"$(if $(FTNFLAGS), "$(FTNFLAGS)")$(if $3, "$3")
endef


# standard link
# $(call link,relocfile,objs[,target specific options])
define link
  @$(ISIS) $(call ifile,link,$(LINK80)) "$(call mklist,$2)" to $1 map\
	  "print($(call lin,$1))"$(if $(LINKFLAGS), "$(LINKFLAGS)")$(if $3, "$3")
endef

# link with no check for unresolved
# $(call link-nocheck,relocfile,objs[,target specific options])
define link-nocheck
  @$(ISIS) -u $(call ifile,link,$(LINK80)) "$(call mklist,$2)" to $1 map\
	  "print($(call lin,$1))"$(if $(LINKFLAGS), "$(LINKFLAGS)")$(if $3, "$3")
endef

# standard locate
# $(call locate,target,relocfile[,target specific options])
define locate
  @$(ISIS) $(call ifile,locate,$(LOCATE80)) $2 to $1\
	  "print($(call map,$2))"$(if $(LOCATEFLAGS), "$(LOCATEFLAGS)")$(if $3, "$3") 
endef

# locate with no check for unsatisfied
# $(call locate-nocheck,target,relocfile[,target specific options])
define locate-nocheck
  @$(ISIS) -u $(call ifile,locate,$(LOCATE80)) $2 to $1\
	  "print($(call map,$2))"$(if $(LOCATEFLAGS), "$(LOCATEFLAGS)")$(if $3, "$3") 
endef

# locate allowing overlaps
# $(call locate-overlaps,target,relocfile[,target specific options])
define locate-overlaps
  @$(ISIS) -o $(call ifile,locate,$(LOCATE80)) $2 to $1\
	  "print($(call map,$2))"$(if $(LOCATEFLAGS), "$(LOCATEFLAGS)")$(if $3, "$3") 
endef

# limited version of locate to remove symbols - does not produce map file
# $(call rm-symbols,target,source)
define rm-symbols
  @$(ISIS) $(call ifile,locate,$(LOCATE80)) $2 to $1 PURGE
endef

# $(call lib,target,objects)
define lib
  rm -fr $1
  @$(ISIS) $(call ifile,lib,$(LIB80)) "&&"\
  create $1 "&&"\
  add $(if $(filter-out 0 1,$(words $2)),$(call mklist,$(call notlast,$2))$(comma)"&")\
  $(lastword $2) to $1 "&&"\
  exit
endef

# $(call plm86,objfile,srcfile[,target specific options])
define plm86
  $(if $(PEXFILE),$(NGENPEX) $(PEXFILE) $2)
  @$(ISIS) $(if $(PEXFILE),,-MF .deps/$1.d) $(call ifile,plm86,$(PLM86)) $2 "object($1)"\
	  $(if $(PLM86FLAGS), "$(PLM86FLAGS)")$(if $3, "$3")
endef

# $(call asm86,objfile,srcfile[,target specific options])
#
define asm86
  @$(ISIS) $(call ifile,asm86,$(ASM86)) $2 \
	  "object($1)"$(if $(ASM86FLAGS), "$(ASM86FLAGS)")$(if $3, "$3")
endef

# predefined rules
$(OBJ)/%.obj: %.plm  | $(OBJ) $(LST)
	$(call plm80,$@,$<)

$(OBJ)/%.obj: %.asm  | $(OBJ) $(LST)
	$(call asm80,$@,$<)

$(OBJ)/%.obj: %.f | $(OBJ) $(LST)
	$(call fort80,$@,$<)

$(OBJ)/%.obj: %.asmx | $(OBJ) $(LST)
	$(call asm80x,$@,$<)

$(OBJ)/%.hex: %.a48 | $(OBJ) $(LST)
	$(call asm48,$@,$<)

$(OBJ)/%.o86: %.plm | $(OBJ) $(LST)
	$(call plm86,$@,$<)

$(OBJ)/%.o86: %.asm  | $(OBJ) $(LST)
	$(call asm86,$@,$<)


# common targets
.PHONY: all rebuild distclean
# all is default first rule
# if master file format is being used add rules to extract the source
# user rules for all will be in the calling makefile
ifdef _masterfile
all:: .extract

.extract: $(_masterfile) | $(OBJ) $(LST)
	perl $(ITOOLS)/tools/unpack.pl
	touch .extract
else
all::
endif

# rules to make sure directories exist
$(sort $(OBJ) $(LST)): ; mkdir -p $@

ifneq '$(NOVERIFY)' 'T'
.PHONY: verify
verify: all
	$(if $(NOVERIFY), $(info skipping $(NOVERIFY)))
	$(eval _verify := $(filter-out $(NOVERIFY),$(TARGETS)))
	$(if $(_verify),\
	  $(if $(REF),\
	    $(if $(filter 1,$(words $(_verify))),\
	      @$(COMPARE) $(_verify) $(REF)/$(notdir $(_verify)),\
	      @for f in $(_verify) ; do $(COMPARE) $$f $(REF)/`basename $$f` || exit 1; done),\
	    $(info verify failed - REF variable not set) exit 1))
endif

rebuild: distclean
	$(MAKE) all

.PHONY: clean
CLEANFILES += %.hex %.bin %.obj %.lst %.ipx %.abs %.lin %.map
clean::
	-rm -fr $(filter-out ., $(OBJ) $(LST))
	$(eval _cleanup_ =  $(filter-out  $(PROTECT) $(REF) $(TARGETS), $(filter $(CLEANFILES), $(shell ls $(if $(filter-out .,$(SRC)),$(SRC)/*,*)))))
	-$(if $(_cleanup_), rm -f $(_cleanup_)) 

distclean:: clean
ifdef _masterfile
	$(eval _cleanup_ =  $(filter-out $(PROTECT) $(REF) $(_masterfile) mk makefile,$(shell ls)))
	-rm -fr $(_cleanup_) .extract
else
	-rm -fr $(TARGETS)
endif
ifndef PEXFILE
	-rm -fr .deps
endif

ifndef PEXFILE
include $(wildcard .deps/*.d)
endif


.DELETE_ON_ERROR:
