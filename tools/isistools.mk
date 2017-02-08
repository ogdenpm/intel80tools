# shared makefile configuration for isis tool builds
# default directories to . if not set
SRCDIR ?=.
LSTDIR ?=.
OBJDIR ?=.

# ISIS disk usage
HOMEDSK:=F0
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

# ISIS tools
ASM=$(ISIS) :$(TOOLDSK):asm80
PLM=$(ISIS) :$(TOOLDSK):plm80
LINK=$(ISIS) :$(TOOLDSK):link
LOCATE=$(ISIS) :$(TOOLDSK):locate

# set up the ISIS environment
V40:=$(ROOT)/plm80v4
V31:=$(ROOT)/plm80v3
V30:=$(ROOT)/plm80v30
# and the thames ISIS_Fx environment
export ISIS_$(HOMEDSK) := $(CURDIR)
export ISIS_$(SRCDSK) := $(SRCDIR)
export ISIS_$(LSTDSK) := $(LSTDIR)
export ISIS_$(OBJDSK) := $(OBJDIR)
export ISIS_$(V30DSK) := $(V30)
export ISIS_$(V31DSK) := $(V31)
export ISIS_$(V40DSK) := $(V40)

# macros used for the build
space= 
space+=
comma=,
# $(call towin,filelist)	convert unix file names to windows
towin = $(subst /,\,$1)

# $(call objdsk,filelist)	set isis obj disk for all of the files
objdisk = $(addprefix :$(OBJDSK):,$(notdir $1))
# $(call objdir,filelist)	set path to objdir for all files
objdir = $(addprefix $(OBJDIR)/,$(notdir $1))

# $(call srcdsk,filelist)	set isis src disk for all of the files
srcdisk = $(addprefix :$(SRCDSK):,$(notdir $1))
# $(call srcdir,filelist)	set path to srcdir for all files
srcdir = $(addprefix $(SRCDIR)/,$(notdir $1))

# $(call lstdsk,filelist)	set isis lst disk for all of the files
lstdisk = $(addprefix :$(LSTDSK):,$(notdir $1))
# $(call lstdir,filelist)	set path to lstdir for all files
lstdir = $(addprefix $(LSTDIR)/,$(notdir $1))

# $(call tooldisk,filelist)	set isis tool disk for all of the files
tooldisk = $(addprefix :$(TOOLDSK):,$(notdir $1))

# $(call mapdisk,filelist)	convert list of full file paths into isis disk versions - not tools dirs
# no path, passes though i.e. drive F0
# this allows options to be included in the list and pass through unchanged
mapdisk = $(strip $(subst $(CURDIR)/,$(HOMEDSK),\
            $(subst ./,$(HOMEDSK),\
              $(subst $(OBJDIR)/,:$(OBJDSK):,\
                $(subst $(LSTDIR)/,:$(LSTDSK):,\
                  $(subst $(SRCDIR)/,:$(SRCDSK):,$1))))))

# $(call link-nocheck,relocfile,objs)
define link-nocheck
  $(file >$(basename $1).in)
  $(foreach O,$(wordlist 2,1000,$2),$(file >>$(basename $1).in,,$O&))
  $(file >>$(basename $1).in,to $(call objdisk,$1) map print($(call lstdisk,$(basename $1).lin)) $(LINKOPT))
  $(call towin,$(LINK)) $(firstword $2) "&" <$(basename $1).in >nul
  @del $(basename $1).in
endef

define link
  $(call link-nocheck,$1,$2)
  @find $(LSTDIR)\$(basename $1).lin "LINK MAP" >nul || (del $(OBJDIR)\$1 && set ERRORLEVEL=1)
endef


# $(call locate-nocheck,target,relocfile,options)
define locate-nocheck
  $(call towin,$(LOCATE)) $(call objdisk,$2) to $(call mapdisk,$1) PRINT($(call  lstdisk,$(basename $2).map)) $3 $(LOCATEOPT) >nul
  @del $(OBJDIR)\$2
endef

define locate
  $(call locate-nocheck,$1,$2,$3)
  @find $(LSTDIR)\$(basename $2).map "UNSATISFIED EXTERNAL" >nul && (del $(call towin,$1) && set ERRORLEVEL=1) || set ERRORLEVEL=0
endef

# the generic build commands
$(OBJDIR)/%.obj: %.plm
	$(NGENPEX) $(call mapdisk,$(PEXFILE) $<)
	$(PLM) $(call mapdisk,$<) CODE PRINT($(call lstdisk,$*.lst)) OBJECT($(call objdisk,$*.obj)) $(PLMOPT)
	-@del $(OBJDIR)\$*.ipx >nul
	@find $(LSTDIR)\$*.lst "     0 PROGRAM ERROR(S)" >nul || (del $(call towin,$@) & set ERRORLEVEL=1)

# a variant of the above to support generated source in a separate object directory
ifneq '$(OBJDIR)' '.'
$(OBJDIR)/%.obj: $(OBJDIR)/%.plm
	$(NGENPEX) $(call mapdisk,$(PEXFILE) $<)
	$(PLM) $(call mapdisk,$<) CODE PRINT($(call lstdisk,$*.lst)) OBJECT($(call objdisk,$*.obj)) $(PLMFLAGS)
	-@del $(OBJDIR)\$*.ipx >nul
	@find $(LSTDIR)\$*.lst "     0 PROGRAM ERROR(S)" >nul || (del $(call towin,$@) & set ERRORLEVEL=1)
# and to create a release version of an application
%: $(OBJDIR)/%
	$(LOCATE) $(call mapdisk,$< to $@) PURGE
endif

$(OBJDIR)/%.obj: %.asm
	$(ASM) $(call mapdisk,$<) PRINT($(call lstdisk,$*.lst)) OBJECT($(call objdisk,$*.obj)) $(ASMFLAGS)
	@find $(LSTDIR)\$*.lst "ASSEMBLY COMPLETE,   NO ERRORS" >nul || (del $(call towin,$@) & set ERRORLEVEL=1)

