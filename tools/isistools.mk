# shared makefile configuration for isis tool builds
# default directories to . if not set
SRCDIR ?=.
LSTDIR ?=.
OBJDIR ?=.

# work out the master source file to protect
MASTERFILE:=$(wildcard *_all.plm)

# compare tool default
COMPARE ?= fc /b
PURGE ?= PURGE

# ISIS disk usage
# current directory :F0:
SRCDSK:=F1
LSTDSK:=F2
OBJDSK:=F3
V30DSK:=F4
V31DSK:=F5
V40DSK:=F6

# the none ISIS build tools
ISIS:=$(ROOT)/thames
PLMPP:=$(ROOT)/tools/plmpp
NGENPEX:=$(ROOT)/tools/ngenpex

# set up the ISIS environment
V40:=$(ROOT)/plm80v40
V31:=$(ROOT)/plm80v31
V30:=$(ROOT)/plm80v30

ISISTOOLS := $(V40)
REFDIR ?= $(V40)

# ISIS tools
ASM=$(ISISTOOLS)/asm80
PLM=$(ISISTOOLS)/plm80
LINK=$(ISISTOOLS)/link
LIB=$(ISISTOOLS)/lib
LOCATE=$(ISISTOOLS)/locate

# and the thames ISIS_Fx environment
export ISIS_F0 := .
export ISIS_$(SRCDSK) := $(SRCDIR)
export ISIS_$(LSTDSK) := $(LSTDIR)
export ISIS_$(OBJDSK) := $(OBJDIR)
export ISIS_$(V30DSK) := $(V30)
export ISIS_$(V31DSK) := $(V31)
export ISIS_$(V40DSK) := $(V40)

## macros used for the build
# useful make escapes
comma := ,
space :=
space +=
lp := (

# $(call towin,filelist)	convert unix file names to windows
towin = $(subst /,\,$1)

rm = del $(call towin,$1)

notlast = $(wordlist 2,$(words $1),dummy $1)

# $(call objdir,filelist)	set path to objdir for all files
objdir = $(addprefix $(OBJDIR)/,$(notdir $1))

# $(call srcdir,filelist)	set path to srcdir for all files
srcdir = $(addprefix $(SRCDIR)/,$(notdir $1))

# $(call lstdir,filelist)	set path to lstdir for all files
lstdir = $(addprefix $(LSTDIR)/,$(notdir $1))
tools-v30 = $(addprefix $(V30)/,$(notdir $1))
tools-v31 = $(addprefix $(V31)/,$(notdir $1))
tools-v40 = $(addprefix $(V40)/,$(notdir $1))

lst-lin = $(call lstdir,$(basename $(notdir $1)).lin)
lst-map = $(call lstdir,$(basename $(notdir $1)).map)
lst-lst = $(call lstdir,$(basename $(notdir $1)).lst)
obj-rel = $(call objdir,$(basename $(notdir $1)).rel)

# $(call mapdisk,filelist)	convert list of full file paths into isis disk versions
# no path and current directory have no drive prefix i.e. drive F0
# options are passed through abd filenames in options are handled
mapdisk = $(strip $(subst !,,\
            $(subst !$(V40)/,:$(V40DSK):,\
              $(subst !$(V31)/,:$(V31DSK):,\
                $(subst !$(V30)/,:$(V30DSK):,\
                  $(subst !$(OBJDIR)/,:$(OBJDSK):,\
                    $(subst !$(LSTDIR)/,:$(LSTDSK):,\
                      $(subst !$(SRCDIR)/,:$(SRCDSK):,\
                        $(subst !./,,$(addprefix !,$(subst $(lp),$(lp)!,$1)))))))))))

# $(call echo,command,files,commandtail)
echo = $(info $(call mapdisk,$1) $(subst $(space),$(comma),$(call mapdisk,$2)) $(call mapdisk,$3))

# $(call responsefile,outfile,files,commandtail)
define responsefile
  $(foreach F,$(call mapdisk,$(call notlast,$2)),$(file >>$1,$F,&))
  $(file >>$1,$(call mapdisk,$(lastword $2) $3))
endef

# link will not produce a map file if there is an error
# $(call link-nocheck,relocfile,objs)
define link-nocheck
  $(call echo,$(LINK),$2,to $1 map print($(call lst-lin,$1)) $(LINKOPT))
  $(file >$(basename $1).in) $(call responsefile,$(basename $1).in,$2,to $1 map print($(call lst-lin,$1)) $(LINKOPT))
  @$(call towin,$(ISIS)) $(call mapdisk,$(LINK)) "&" <$(basename $1).in >_link.err
  $(if $(KEEP),,@del $(call towin,$(basename $1).in))
  @if not exist $(call towin,$1) (type _link.err & del _link.err & exit 1)
  @del _link.err
endef

# further check that there is
define link
  $(call link-nocheck,$1,$2)
  @find "UNRESOLVED EXTERNAL" $(call towin,$(call lst-lin,$1))  >nul &&\
	  ($(call towin,del $1 & echo link error see $(call lst-lin,$1)) & exit 1) || exit 0
endef

# $(call locate-nocheck,target,relocfile,options)
define locate-nocheck
  $(call echo,$(LOCATE),$2,to $1 print($(call lst-map,$2)) $3 $(LOCATEOPT))
  @$(call towin,$(ISIS)) $(call mapdisk,$(LOCATE) $2 to $1 PRINT($(call lst-map,$2)) $3 $(LOCATEOPT)) >_locate.err
  @if not exist $(call towin,$1) (type _locate.err & del _locate.err & exit 1)
  @del _locate.err
endef

define locate
  $(call locate-nocheck,$1,$2,$3)
  @find "UNSATISFIED EXTERNAL" $(call towin,$(call lst-map,$2)) >nul &&\
	  (del $(call towin,$1) & echo locate error see $(call lst-map,$2)) & exit 0
  @find "MEMORY OVERLAP" $(call towin,$(call lst-map,$2)) >nul && type $(call towin,$(call lst-map,$2)) & exit 0
  @if not exist $(call towin,$1) exit 1
endef

# $(call rm-symbols,target,source)
define rm-symbols
  $(info $(call mapdisk,$(LOCATE) $2 to $1) $(PURGE))
  @$(call towin,$(ISIS)) $(call mapdisk,$(LOCATE) $2 to $1) $(PURGE) >_purge.err 
  @if not exist $(call towin,$1) (type _purge.err & del _purge.err & exit 1)
  @del _purge.err
endef

# $(call lib,target,objects)
define lib
  $(call echo,$(LIB),create,$1)
  $(call echo,add,$2,to $1)
  $(call echo,exit)
  @del $1 2>nul
  $(file >lib.rsp,create $(call mapdisk,$1))
  $(file >>lib.rsp,add &)
  $(call responsefile,lib.rsp,$2,to $1)
  $(file >>lib.rsp,exit)
  @$(call towin,$(ISIS)) $(call mapdisk,$(LIB)) <lib.rsp >_lib.err
  $(if $(KEEP),,@del $(call towin,lib.rsp))
  @find "NO SUCH FILE" _lib.err >nul && (del $1 & type _lib.err) & exit 0
  @del _lib.err
  @if not exist $(call towin,$1) exit 1
endef

## the generic build commands
# the search paths
vpath %.plm $(SRCDIR) $(OBJDIR)
vpath %.plx $(SRCDIR)
vpath %.asm $(SRCDIR)
vpath %.obj $(OBJDIR)
vpath %.abs $(OBJDIR)
vpath %.rel $(OBJDIR)

$(OBJDIR)/%.obj: %.plm  | $(filter-out .,$(OBJDIR) $(LSTDIR))
	$(if $(PEXFILE),$(NGENPEX) $(call mapdisk,$(PEXFILE) $<),perl $(ROOT)/tools/makedepend.pl $@ $<)
	$(ISIS) $(call mapdisk,$(PLM) $< CODE PRINT($(call lst-lst,$<)) OBJECT($@) $(PLMOPT))
	-@del $(OBJDIR)\$*.ipx 2>nul
	@if not exist $(call towin,$@) exit 1

# a variant of the above to support generated source in a separate object directory
ifneq '$(OBJDIR)' '.'
  $(OBJDIR)/%.obj: $(OBJDIR)/%.plm  | $(filter-out .,$(OBJDIR) $(LSTDIR))
	$(if $(PEXFILE),$(NGENPEX) $(call mapdisk,$(PEXFILE) $<),perl $(ROOT)/tools/makedepend.pl $@ $<)
	$(ISIS) $(call mapdisk,$(PLM) $< CODE PRINT($(call lst-lst,$<)) OBJECT($@) $(PLMOPT))
	-@del $(OBJDIR)\$*.ipx >nul
	@if not exist $(call towin,$@) exit 1
endif

$(OBJDIR)/%.obj: %.asm  | $(filter-out .,$(OBJDIR) $(LSTDIR))
	$(ISIS) $(call mapdisk,$(ASM) $< PRINT($(call lst-lst,$<)) OBJECT($@) $(ASMOPT))
	@if not exist $(call towin,$@) exit 1

#	$(info $(call mapdisk,$(ASM) $< PRINT($(call lst-lst,$<)) OBJECT($@) $(ASMOPT)))
#	@if not exist $(call towin,$@) ($(call towin,type $*.err & del $*.err)  & exit 1)
#	@del $(call towin,$*.err)


#ifndef PEXFILE
#	perl $(ROOT)/tools/makedepend.pl $@ $<
#endif

# common targets
.PHONY: all
ifdef MASTERFILE
  all: .extract

  .extract: $(MASTERFILE) | $(filter-out .,$(OBJDIR) $(LSTDIR))
	perl $(ROOT)/unpack.pl
	date /t >.extract

  ifneq '$(OBJDIR)' '.'
    $(OBJDIR):
	mkdir $(OBJDIR)
  endif
  ifneq '$(LSTDIR)' '.'
    $(LSTDIR):
	mkdir $(LSTDIR)
  endif

 FORCE:
else
  all:
endif

verify: all
	cmd /c for %i in ($(TARGETS)) do $(call towin,$(firstword $(COMPARE))) $(wordlist 2,10,$(COMPARE)) %i $(call towin,$(REFDIR))\%i


rebuild: distclean all

## housekeeping rules
.PHONY: clean
clean:
	-$(if $(subst .,,$(OBJDIR)),rmdir /s /q $(OBJDIR),del /q *.obj *.abs) 2>nul
	-$(if $(subst .,,$(LSTDIR)),rmdir /s /q $(LSTDIR),del /q *.lst *.lin *.map) 2>nul
ifndef PEXFILE
	-rmdir /s /q .deps *.in 2>nul
endif

.PHONY: vclean

distclean: clean 
	-del /q $(TARGETS) 2>nul
ifdef MASTERFILE
  ifneq '$(SRCDIR)' '.'
	-rmdir /s /q $(SRCDIR) 2>nul
  endif
	-del /q $(filter-out mk makefile $(REFDIR) $(MASTERFILE),$(wildcard *)) .extract 2>nul
endif


ifndef PEXFILE
include $(wildcard .deps/*.d)
endif
