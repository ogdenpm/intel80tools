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
V40:=$(ROOT)/plm80v4
V31:=$(ROOT)/plm80v3
V30:=$(ROOT)/plm80v30

ISISTOOLS := $(V40)

# ISIS tools
ASM=$(ISIS) $(call mapdisk,$(ISISTOOLS)/asm80)
PLM=$(ISIS) $(call mapdisk,$(ISISTOOLS)/plm80)
LINK=$(ISIS) $(call mapdisk,$(ISISTOOLS)/link)
LOCATE=$(ISIS) $(call mapdisk,$(ISISTOOLS)/locate)

# and the thames ISIS_Fx environment
export ISIS_F0 := .
export ISIS_$(SRCDSK) := $(SRCDIR)
export ISIS_$(LSTDSK) := $(LSTDIR)
export ISIS_$(OBJDSK) := $(OBJDIR)
export ISIS_$(V30DSK) := $(V30)
export ISIS_$(V31DSK) := $(V31)
export ISIS_$(V40DSK) := $(V40)

# macros used for the build
# $(call towin,filelist)	convert unix file names to windows
towin = $(subst /,\,$1)

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

# $(call mapdisk,filelist)	convert list of full file paths into isis disk versions
# no path and current directory have no drive prefix i.e. drive F0
# options are passed through abd filenames in options are handled
lp=(
mapdisk = $(strip $(subst !,,\
            $(subst !$(V40)/,:$(V40DSK):,\
              $(subst !$(V31)/,:$(V31DSK):,\
                $(subst !$(V30)/,:$(V30DSK):,\
                  $(subst !$(OBJDIR)/,:$(OBJDSK):,\
                    $(subst !$(LSTDIR)/,:$(LSTDSK):,\
                      $(subst !$(SRCDIR)/,:$(SRCDSK):,\
                        $(subst !./,,$(addprefix !,$(subst $(lp),$(lp)!,$1)))))))))))

# $(call link-nocheck,relocfile,objs)
define link-nocheck
  $(file >$(basename $1).in)
  $(foreach O,$(call mapdisk,$(wordlist 2,1000,$2)),$(file >>$(basename $1).in,,$O&))
  $(file >>$(basename $1).in,$(call mapdisk,to $1 map print($(call lst-lin,$1) $(LINKOPT))))
  $(call towin,$(LINK)) $(call mapdisk,$(firstword $2)) "&" <$(basename $1).in  >nul
  @del $(call towin,$(basename $1).in)
endef


define link
  $(call link-nocheck,$1,$2)
  @find $(call towin,$(call lst-lin,$1)) "LINK MAP" >nul || (del $(call towin,$1) && set ERRORLEVEL=1)
endef


# $(call locate-nocheck,target,relocfile,options)
define locate-nocheck
  $(call towin,$(LOCATE)) $(call mapdisk,$2 to $1 PRINT($(call lst-map,$2)) $3 $(LOCATEOPT)) >nul
endef

# $(call rm-symbols,target,source)
rm-symbols = $(LOCATE) $(call mapdisk,$2 to $1) $(PURGE) 

define locate
  $(call locate-nocheck,$1,$2,$3)
  @find $(call towin,$(call lst-map,$2)) "UNSATISFIED EXTERNAL" >nul && (del $(call towin,$1) && set ERRORLEVEL=1) || set ERRORLEVEL=0
endef

## the generic build commands
# the search paths
vpath %.plm $(SRCDIR) $(OBJDIR)
vpath %.plx $(SRCDIR)
vpath %.asm $(SRCDIR)
vpath %.obj $(OBJDIR)
vpath %.abs $(OBJDIR)
vpath %.rel $(OBJDIR)

$(OBJDIR)/%.obj: %.plm  | $(filter-out .,$(OBJDIR) $(LSTDIR) FORCE)
	$(NGENPEX) $(call mapdisk,$(PEXFILE) $<)
	$(PLM) $(call mapdisk,$< CODE PRINT($(call lst-lst,$<)) OBJECT($@) $(PLMOPT))
	-@del $(OBJDIR)\$*.ipx 2>nul
	@cmd /c if not exist $(call towin,$@) exit /b 1

# a variant of the above to support generated source in a separate object directory
ifneq '$(OBJDIR)' '.'
$(OBJDIR)/%.obj: $(OBJDIR)/%.plm  | $(filter-out .,$(OBJDIR) $(LSTDIR) FORCE)
	$(NGENPEX) $(call mapdisk,$(PEXFILE) $<)
	$(PLM) $(call mapdisk,$< CODE PRINT($(call lst-lst,$<)) OBJECT($@) $(PLMFLAGS))
	-@del $(OBJDIR)\$*.ipx >nul
	@cmd /c if not exist $(call towin,$@) exit /b 1
endif

$(OBJDIR)/%.obj: %.asm  | $(filter-out .,$(OBJDIR) $(LSTDIR) FORCE)
	$(ASM) $(call mapdisk,$< PRINT($(call lst-lst,$<)) OBJECT($@) $(ASMFLAGS))
	@cmd /c if not exist $(call towin,$@) exit /b 1


# common targets
.PHONY: all verify rebuild
all: .extract

.extract: $(MASTERFILE) | $(filter-out .,$(OBJDIR) $(LSTDIR) FORCE)
	perl $(call towin,$(ROOT)/unpack.pl)
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

verify: all
	cmd /c for %i in $(call towin,($(TARGETS)) do $(firstword $(COMPARE))) $(wordlist 2,10,$(COMPARE)) %i $(call towin,$(V40))\%i

rebuild: distclean verify



## housekeeping rules
.PHONY: clean
clean:
ifneq '$(OBJDIR)' '.'
	-rmdir /s /q $(OBJDIR) 2>nul
else
	-del /q *.obj *.abs 2>nul
endif
ifneq '$(LSTDIR)' '.'
	-rmdir /s /q $(LSTDIR) 2>nul
else
	-del /q *.lst *.lin *.map 2>nul
endif

.PHONY: distclean

distclean:: clean 
	-del /q $(TARGETS) 2>nul
ifdef MASTERFILE
  ifneq '$(SRCDIR)' '.'
	-rmdir /s /q $(SRCDIR) 2>nul
  endif
	-del /q $(filter-out mk makefile $(MASTERFILE),$(wildcard *)) .extract 2>nul
endif
	perl $(call towin,$(ROOT)/unpack.pl)
