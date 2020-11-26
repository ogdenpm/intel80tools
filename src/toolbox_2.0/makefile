ITOOLS ?= ../..
OBJ = obj
LST = lst
SRC = src
export LIB = lib


# map src for the pex include files
# lower level makefiles need to unexport ISIS_F1
export ISIS_F1=$(SRC)
# we used a customised verify here
NOVERIFY = T

# to avoid conflict with the standard clean target - con make by creating target Clean
TARGETS := chklod Clean compar consol dcopy dsort else endif errs exit ganef\
	genpex goto hdback if last latest loop lower merg80 merg86 mrkobj note\
	pack passif relab rescan return sort stopif unpack upper wait\
	which xlate2

# note pause was not precompiled in the original toolbox so a copy
# has been added
# also source for bltsym.asm was missing so recreated
include $(ITOOLS)/tools/isis.mk

REF = $(call ipath,toolbox,2.0)

libs = $(addprefix $(LIB)/,$(notdir $1))

LIBS = cusp2.lib cusp5.lib cusp6.lib cusp8.lib fnames.lib\
       sys.lib unixio.lib util.lib

SUBDIRS := $(basename $(LIBS))

PLMFLAGS = CODE
LOCATEFLAGS = SYMBOLS PUBLICS
PURGE = purge

all:: | $(LIB)
	for d in $(SUBDIRS); do $(MAKE) -C $$d || exit 1; done
	$(MAKE) $(TARGETS)

$(LIB):
	mkdir -p $@

%: $(OBJ)/%.rel
	$(call locate,$@,$^,$(PURGE))


$(OBJ)/chklod.rel: $(call objdir,chklod.obj cinvok.obj cmovem.obj cgreat.obj csort2.obj cdirec.obj)\
   				   $(call libs,fnames.lib util.lib sys.lib)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/clean.rel: $(OBJ)/clean.obj
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/compar.rel: $(call objdir,compar.obj tail.obj out.obj m3.obj divide.obj)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/consol.rel: $(OBJ)/consol.obj $(LIB)/cusp5.lib
	$(call link,$@,$^ $(plm80.lib) $(system.lib))

$(OBJ)/dcopy.rel: $(OBJ)/dcopy.obj $(system.lib) $(LIB)/cusp5.lib
	$(call link,$@,$^ $(plm80.lib))

$(OBJ)/dsort.rel: $(OBJ)/dsort.obj $(LIB)/cusp6.lib
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/else.rel: $(OBJ)/else.obj $(LIB)/cusp8.lib
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/endif.rel: $(OBJ)/endif.obj
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/errs.rel: $(OBJ)/errs.obj $(call libs,util.lib sys.lib)
	$(call link,$@,$^ $(plm80.lib))

$(OBJ)/exit.rel: $(OBJ)/exit.obj
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/ganef.rel: $(OBJ)/ganef.obj $(LIB)/cusp2.lib
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/genpex.rel: $(OBJ)/genpex.obj $(call libs,fnames.lib util.lib sys.lib)
	$(call link,$@,$^ $(plm80.lib))

$(OBJ)/goto.rel: $(OBJ)/goto.obj $(LIB)/cusp8.lib
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/hdback.rel: $(call objdir,hdback.obj hdfils.obj) $(LIB)/util.lib
	$(call link,$@,$^ $(plm80.lib) $(system.lib))

$(OBJ)/if.rel: $(OBJ)/if.obj $(LIB)/cusp8.lib
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/last.rel: $(OBJ)/last.obj $(call libs,util.lib sys.lib)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/latest.rel: $(OBJ)/latest.obj $(call libs,util.lib sys.lib)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/loop.rel: $(OBJ)/loop.obj
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/lower.rel: $(OBJ)/lower.obj $(LIB)/unixio.lib
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/merg80.rel: $(call objdir,merg80.obj bltsym.obj sort2.obj) $(call libs,fnames.lib util.lib sys.lib)
	$(call link,$@,$^ $(plm80.lib))

$(OBJ)/merg86.rel: $(call objdir,merg86.obj bsym86.obj sort2.obj) $(call libs,fnames.lib util.lib sys.lib)
	$(call link,$@,$^ $(plm80.lib))

$(OBJ)/mrkobj.rel: $(call objdir,mrkobj.obj minvok.obj mfiler.obj) $(call libs,fnames.lib util.lib sys.lib)
	$(call link,$@,$^ $(plm80.lib))

$(OBJ)/note.rel: $(OBJ)/note.obj
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/pack.rel: $(call objdir,pack.obj packer.obj) $(LIB)/cusp6.lib
	$(call link,$@,$^ $(plm80.lib) $(system.lib))

$(OBJ)/passif.rel: $(call objdir,passif.obj cli.obj errmsg.obj func.obj report.obj tokens.obj) $(LIB)/util.lib
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/relab.rel: $(OBJ)/relab.obj
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/rescan.rel: $(OBJ)/rescan.obj
	$(call link,$@,$^ $(plm80.lib) $(system.lib))

$(OBJ)/return.rel: $(OBJ)/return.obj $(LIB)/cusp8.lib
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/sort.rel: $(OBJ)/sort.obj
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/stopif.rel: $(OBJ)/stopif.obj
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/unpack.rel: $(call objdir,unpack.obj unpakr.obj) $(LIB)/cusp6.lib
	$(call link,$@,$^ $(plm80.lib) $(system.lib))

$(OBJ)/upper.rel: $(OBJ)/upper.obj $(call libs,util.lib sys.lib)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/wait.rel: $(OBJ)/wait.obj
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/which.rel: $(OBJ)/which.obj
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/xlate2.rel: $(OBJ)/xlate2.obj $(call libs,util.lib sys.lib)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

# extensions to default rules
clean::
	-@for d in $(SUBDIRS); do $(MAKE) -C $$d clean; done;

# as the clean name clashes use an upper case first letter to map the file
Clean: $(OBJ)/clean.rel
	$(call locate,$@,$^,$(PURGE))

# special verify
# firstly makes sure the latest sources are built
# then verify all of the subdirectories
# finally verify the main targets 
verify: all
	@for d in $(SUBDIRS); do $(MAKE) -C $$d verify || exit 1; done;
	@for f in $(TARGETS); do $(COMPARE) $$f $(REF)/$$f || exit 1; done;

