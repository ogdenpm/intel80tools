# common makefile elements included by isis.mk and cpm.mk
#
# convert \ to /, remove trailing directory separator and set to . if blank
# $(call fixpath,path)
fixpath = $(if $1,$(subst |,,$(subst /|,,$(subst \,/,$(strip $1))|)),.)

# normalise ITOOLS path
ITOOLS := $(call fixpath,$(ITOOLS))

export _ITOOLS=$(ITOOLS)
#
# make sure bash and other unix tools are on the path
PATH := $(ITOOLS);$(ITOOLS)/tools;$(ITOOLS)/unix;$(PATH)
SHELL = bash.exe
COMPARE ?= $(ITOOLS)/tools/omfcmp.exe

# default directories to . if not set
SRC := $(call fixpath,$(SRC))
LST := $(call fixpath,$(LST))
OBJ := $(call fixpath,$(OBJ))

# set Intel ASM extension if not set
IASM ?= asm

# ISIS disk usage - :F0: will default to current dir
export ISIS_F0 ?= ./

# shared build tools
ISIS:=thames -m -T
PLMPP:=plmpp
ASM80X:=perl $(ITOOLS)/tools/asm80x.pl
NGENPEX:=ngenpex
MKDEPEND:=perl $(ITOOLS)/tools/makedepend.pl
ABSTOOL:=abstool
HEXOBJ:=hexobj
PLM81:=plm81
PLM82:=plm82

# macros to create the path & full file names of an isis file
# usage: $(call ipath,file[,version])
ipath = $(ITOOLS)/itools/$(strip $1)$(if $2,/$(strip $2))
# 
# usage: $(call ifile,file[,version])
ifile = $(call ipath,$1,$2)/$(strip $1)
#
# usage: $(call prog,progname,progver[,cver])
prog = $(if $(filter-out $3,$2),$(ISIS) $(call ifile,$1,$2), $(ITOOLS)/tools/$1)

#usage $(call copt,flag,progname,progver,cver)
copt = $(if $(filter-out $3,$2),,$1)



# set default tool versions if none given
PLM80 ?= 4.0
ASM80 ?= 4.1
LIB80 ?= 2.1
LINK80 ?= 3.0
LOCATE80 ?= 3.0
FORT80 ?= 2.1

PLMFLAGS ?= code

# the standard libraries
plm80.lib := $(call ifile,plm80.lib)
system.lib,4.0 := $(call ipath,utils,2.2n)/system.lib
system.lib,3.1 := $(call ipath,isis,3.4)/system.lib
system.lib,3.0 := $(call ipath,isis,2.2)/system.lib
system.lib = $(system.lib,$(PLM80))
fpal.lib,2.1 := $(call ifile,fpal.lib,2.1)
fpal.lib,2.0 := $(call ifile,fpal.lib,2.0)
fpal.lib = $(fpal.lib,2.1)


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

publics = "publics($(call mklist,$(call objdir,$1)))"

# macros to generate suitable output file names from $1 adding .$2
mkname = $(LST)/$(basename $(notdir $(firstword $1))).$(strip $2)

vpath %.plm $(SRC)
vpath %.asm $(SRC)


## the generic build commands
# $(call plm80,objfile,srcfile[,target specific options])
define plm80
  $(if $(PEXFILE),$(NGENPEX) $(PEXFILE) $2)
  $(call prog,plm80,$(PLM80),4.0) $2 "object($1)" "print($(call mkname,$2,lst))" $(if $(PLMFLAGS), "$(PLMFLAGS)")$(if $3, "$3")
endef

# $(call asm80,objfile,srcfile[,target specific options])
#  
define asm80
  $(call prog,asm80,$(ASM80),4.1) $2 "object($1)" "print($(call mkname,$2,lst))" $(if $(ASMFLAGS), "$(ASMFLAGS)")$(if $3, "$3")
endef

# $(call abstool,target,options_and_inputs[,patchfile])
# if patch file is omitted but $(basename $1).patch exists use it
define abstool
  $(ABSTOOL) -o $1 $(if $3,-p $3,$(if $(wildcard $(basename $1).patch),-p $(basename $1).patch)) $2
endef

# $(call fort80,objfile,srcfile[,target specific options])
define fort80
  @$(ISIS) $(call ifile,fort80,$(FORT80)) $2 "print($(call mkname,$2,lst))"\
	  "workfiles(:f0:)" "object($1)"$(if $(FTNFLAGS), "$(FTNFLAGS)")$(if $3, "$3")
endef

# standard link
# $(call link,relocfile,objs[,target specific options])
define link
 $(call prog,link,$(LINK80),3.0)  $2 to $1 map "print($(call mkname,$1,lin))"$(if $(LINKFLAGS), "$(LINKFLAGS)")$(if $3, "$3")
endef
#
# link but where warnings are not treated as errors. Useful for overlay builds
# $(call link-warnok,relocfile,objs[,target specific options])
define link-externok
 $(call prog,link,$(LINK80),3.0) $2 to $1 $(call copt,EXTERNOK,$(LINK80),3.0) \
 map "print($(call mkname,$1,lin))" $(if $(LINKFLAGS), "$(LINKFLAGS)")$(if $3, "$3")
endef

# standard locate
# $(call locate,target,relocfile[,target specific options])
define locate
  $(call prog,locate,$(LOCATE80),3.0) $2 to $1 "print($(call mkname,$2,map))" $(if $(LOCATEFLAGS), "$(LOCATEFLAGS)")$(if $3, "$3") 
endef

# locate with unresolved externs allowed
# $(call locate-externok,target,relocfile[,target specific options])
define locate-externok
   $(call prog,locate,$(LOCATE80),3.0) $2 to $1 $(call copt,EXTERNOK,$(LOCATE80),3.0) \
   "print($(call mkname,$2,map))" $(if $(LOCATEFLAGS), "$(LOCATEFLAGS)")$(if $3, "$3") 
endef

# locate allowing overlaps
# $(call locate-overlaps,target,relocfile[,target specific options])
define locate-overlaps
   $(call prog,locate,$(LOCATE80),3.0) $2 to $1 $(call copt,OVERLAPOK,$(LOCATE80),3.0) \
   "print($(call mkname,$2,map))" $(if $(LOCATEFLAGS), "$(LOCATEFLAGS)")$(if $3, "$3") 
endef

# limited version of locate to remove symbols - does not produce map file
# $(call rm-symbols,target,source)
define rm-symbols
   $(call prog,locate,$(LOCATE80),3.0) $2 to $1 PURGE
endef

# $(call lib,target,objects)
define lib
  lib i $2 to '$1'
endef

# predefined rules
$(OBJ)/%.obj: %.plm  | $(OBJ) $(LST)
	$(call plm80,$@,$<)

$(OBJ)/%.obj: %.$(IASM)  | $(OBJ) $(LST)
	$(call asm80,$@,$<)

$(OBJ)/%.obj: %.f | $(OBJ) $(LST)
	$(call fort80,$@,$<)

ifndef NOOLDPLM
$(OBJ)/%.hex: %.plm
	$(PLM81) $^
	$(PLM82) $*
	@perl $(ITOOLS)/tools/pretty.pl $*.lst $*.prn
	@rm -fr $*.pol $*.sym $*.lst
endif

# common targets
.PHONY: all rebuild distclean fullverify
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
$(sort $(OBJ) $(LST)): ; mkdir $@

ifneq '$(NOVERIFY)' 'T'
.PHONY: verify
verify: all
	$(if $(NOVERIFY), $(info skipping $(NOVERIFY)))
	$(eval _verify := $(filter-out $(NOVERIFY),$(TARGETS)))
	$(if $(_verify),\
	  $(if $(REF),\
	    $(if $(filter 1,$(words $(_verify))),\
	      $(COMPARE) $(_verify) $(REF)/$(notdir $(_verify)),\
	      for f in $(_verify) ; do $(COMPARE) $$f $(REF)/$$f || exit 1; done),\
	    $(info verify failed - REF variable not set) exit 1))
endif

rebuild: distclean
	$(MAKE) all

fullverify: distclean
	$(MAKE) verify

.PHONY: clean
CLEANFILES += %.hex %.obj %.lst %.ipx %.abs %.lin %.map %.prn %.rel %.sym %.irl
clean::
	$(eval _cleanup_ =  $(wildcard $(filter-out ., $(OBJ) $(LST))))
	$(if $(_cleanup_), rm -fr $(_cleanup_))
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
