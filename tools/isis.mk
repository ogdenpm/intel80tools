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
COMPARE ?= $(ROOT)/tools/omfcmp

# default directories to . if not set
SRC := $(call fixpath,$(SRC))
LST := $(call fixpath,$(LST))
OBJ := $(call fixpath,$(OBJ))

# ISIS disk usage - :F0: will default to current dir
export ISIS_F0 ?= ./

# the none ISIS build tools
ISIS:=$(ROOT)/thames -m
PLMPP:=$(ROOT)/tools/plmpp
NGENPEX:=$(ROOT)/tools/ngenpex
MKDEPEND:=perl $(ROOT)/tools/makedepend.pl

# macros to create the path & full file names of an isis file
# usage: $(call ipath,file[,version])
ipath = $(ROOT)/itools/$(strip $1)$(if $2,_$(strip $2))
# 
# usage: $(call ifile,file[,version])
ifile = $(call ipath,$1,$2)/$(strip $1)

# set default tool versions if none given
PLM80 ?= 4.0
ASM80 ?= 4.1
LIB ?= 2.1
LINK ?= 3.0
LOCATE ?= 3.0
FORT80 ?= 2.1

PLMFLAGS ?= code

# the standard libraries
plm80.lib := $(call ifile,plm80.lib)
system.lib,4.0 := $(call ipath,utils,2.2)/system.lib
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
space :=
space +=

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
  $(if $(PEXFILE),$(NGENPEX) $(PEXFILE) $2,$(MKDEPEND) $1 $2)
  @$(ISIS) $(call ifile,plm80,$(PLM80)) $2 "print($(call lst,$2))"\
	  "object($1)"$(if $(PLMFLAGS), "$(PLMFLAGS)")$(if $3, "$3")
endef

# $(call asm80,objfile,srcfile[,target specific options])
define asm80
  @$(ISIS) $(call ifile,asm80,$(ASM80)) $2 "print($(call lst,$2))"\
	  "object($1)"$(if $(ASMFLAGS), "$(ASMFLAGS)")$(if $3, "$3")
endef

# $(call fort80,objfile,srcfile[,target specific options])
define fort80
  @$(ISIS) $(call ifile,fort80,$(FORT80)) $2 "print($(call lst,$2))"\
	  "workfiles(:f0:)" "object($1)"$(if $(FTNFLAGS), "$(FTNFLAGS)")$(if $3, "$3")
endef


# standard link
# $(call link,relocfile,objs[,target specific options])
define link
  @$(ISIS) $(call ifile,link,$(LINK)) "$(call mklist,$2)" to $1 map\
	  "print($(call lin,$1))"$(if $(LINKFLAGS), "$(LINKFLAGS)")$(if $3, "$3")
endef

# link with no check for unresolved
# $(call link-nocheck,relocfile,objs[,target specific options])
define link-nocheck
  @$(ISIS) -u $(call ifile,link,$(LINK)) "$(call mklist,$2)" to $1 map\
	  "print($(call lin,$1))"$(if $(LINKFLAGS), "$(LINKFLAGS)")$(if $3, "$3")
endef

# standard locate
# $(call locate,target,relocfile[,target specific options])
define locate
  @$(ISIS) $(call ifile,locate,$(LOCATE)) $2 to $1\
	  "print($(call map,$2))"$(if $(LOCATEFLAGS), "$(LOCATEFLAGS)")$(if $3, "$3") 
endef

# locate with no check for unsatisfied
# $(call locate-nocheck,target,relocfile[,target specific options])
define locate-nocheck
  @$(ISIS) -u $(call ifile,locate,$(LOCATE)) $2 to $1\
	  "print($(call map,$2))"$(if $(LOCATEFLAGS), "$(LOCATEFLAGS)")$(if $3, "$3") 
endef

# locate allowing overlaps
# $(call locate-overlaps,target,relocfile[,target specific options])
define locate-overlaps
  @$(ISIS) -o $(call ifile,locate,$(LOCATE)) $2 to $1\
	  "print($(call map,$2))"$(if $(LOCATEFLAGS), "$(LOCATEFLAGS)")$(if $3, "$3") 
endef

# limited version of locate to remove symbols - does not produce map file
# $(call rm-symbols,target,source)
define rm-symbols
  @$(ISIS) $(call ifile,locate,$(LOCATE)) $2 to $1 PURGE
endef

# $(call lib,target,objects)
define lib
  rm -fr $1
  @$(ISIS) $(call ifile,lib,$(LIB)) "&&"\
  create $1 "&&"\
  add $(if $(filter-out 0 1,$(words $2)),$(call mklist,$(call notlast,$2))$(comma)"&")\
  $(lastword $2) to $1 "&&"\
  exit
endef

# predefined rules
$(OBJ)/%.obj: %.plm  | $(OBJ) $(LST)
	$(call plm80,$@,$<)

$(OBJ)/%.obj: %.asm  | $(OBJ) $(LST)
	$(call asm80,$@,$<)

$(OBJ)/%.obj: %.f | $(OBJ) $(LST)
	$(call fort80,$@,$<)

# common targets
.PHONY: all rebuild distclean
# all is default first rule
# if master file format is being used add rules to extract the source
# user rules for all will be in the calling makefile
ifdef _masterfile
all:: .extract

.extract: $(_masterfile) | $(OBJ) $(LST)
	perl $(ROOT)/unpack.pl
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
	      @$(COMPARE) $(_verify) $(REF)/$(_verify),\
	      @for f in $(_verify) ; do $(COMPARE) $$f $(REF)/$$f || exit 1; done),\
	    $(info verify failed - REF variable not set) exit 1))
endif

rebuild: distclean all
# to allow the toolbox to creat the utility clean
# clean target is only defined here if the make command explicitly has
# it as a target
## housekeeping rules
ifeq ($(MAKECMDGOALS),clean)
.PHONY: clean
clean::
	-$(if $(filter-out .,$(OBJ)),rm -fr $(OBJ),rm -f *.obj *.abs) 
	-$(if $(filter-out .,$(LST)),rm -fr $(LST),rm -fr *.lst *.lin *.map) 
ifdef PEXFILE
	-rm -fr $(SRC)/*.ipx
endif
endif

distclean::
	-$(if $(filter-out .,$(OBJ)),rm -fr $(OBJ),rm -f *.obj *.abs) 
	-$(if $(filter-out .,$(LST)),rm -fr $(LST),rm -fr *.lst *.lin *.map)
ifdef _masterfile
	-rm -fr $(filter-out mk makefile $(REF) $(_masterfile) $(PROTECT),$(shell ls)) $(TARGETS) .extract 
else
	-rm -f $(TARGETS) 
endif
ifdef PEXFILE
	-rm -fr $(SRC)/*.ipx
else
	-rm -fr .deps
endif

ifndef PEXFILE
include $(wildcard .deps/*.d)
endif


.DELETE_ON_ERROR:
