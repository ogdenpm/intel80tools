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

# note the use of Clean as the target file as this clashes with the standard
# clean target
TARGETS := chklod Clean compar consol dcopy dsort else endif errs exit ganef\
	genpex goto hdback if last latest loop lower merg80 merg86 mrkobj note\
	pack passif pause relab rescan return sort stopif unpack upper wait\
	which xlate2

# note pause was not precompiled in the original toolbox so a copy
# has been added
# also source for bltsym.asm was missing so recreated
include $(ITOOLS)/tools/isis.mk

REF = $(call ipath,toolbox,1.0)

# simple macro to prepend lib dir to a list of libraries
libs = $(addprefix $(LIB)/,$(notdir $1))


LIBS = cusp1.lib cusp2.lib cusp3.lib cusp4.lib cusp5.lib\
       cusp6.lib cusp7.lib fnames.lib isis3.lib pff.lib\
       sys1.lib sys2.lib unixio.lib util1.lib util2.lib\
       util3.lib

SUBDIRS := $(basename $(LIBS))

PLM80 = 3.1
PLMFLAGS = CODE 
LOCATEFLAGS = SYMBOLS PUBLICS
PURGE = purge

all::  | $(LIB)
	for d in $(SUBDIRS); do $(MAKE) -C $$d || exit 1; done
	$(MAKE) $(TARGETS)

$(LIB):
	mkdir -p $@

%: $(OBJ)/%.rel
	$(call locate,$@,$^,$(PURGE))

# the besoke rules for linking each application with its required libraries

$(OBJ)/chklod.rel:  $(call objdir,chklod.obj cinvok.obj cmovem.obj cgreat.obj csort2.obj cdirec.obj) \
	                $(call libs,fnames.lib util2.lib sys2.lib isis3.lib)
	$(call link,$@,$^ $(plm80.lib))

$(OBJ)/clean.rel: $(OBJ)/clean.obj $(call libs,isis3.lib cusp3.lib sys1.lib util3.lib)
	$(call link,$@,$^ $(plm80.lib))

$(OBJ)/compar.rel: $(call objdir,compar.obj tail.obj out.obj m3.obj divide.obj)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/consol.rel: $(OBJ)/consol.obj $(LIB)/cusp4.lib
	$(call link,$@,$^ $(plm80.lib) $(system.lib))

$(OBJ)/dcopy.rel: $(OBJ)/dcopy.obj $(system.lib) $(LIB)/cusp5.lib
	$(call link,$@,$^ $(plm80.lib))

$(OBJ)/dsort.rel: $(OBJ)/dsort.obj $(call libs,cusp6.lib cusp3.lib)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/else.rel: $(call objdir,else.obj seq.obj) $(LIB)/cusp7.lib
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/endif.rel: $(OBJ)/endif.obj $(system.lib)
	$(call link,$@,$^ $(plm80.lib))

$(OBJ)/errs.rel: $(OBJ)/errs.obj $(call libs,cusp2.lib util2.lib sys2.lib)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/exit.rel: $(OBJ)/exit.obj $(LIB)/cusp7.lib
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/ganef.rel: $(OBJ)/ganef.obj $(call libs,cusp2.lib util2.lib sys2.lib)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

# genpex did not have a build script in the original toolbox
$(OBJ)/genpex.rel: $(OBJ)/genpex.obj $(call libs,fnames.lib util1.lib) $(system.lib) $(LIB)/sys2.lib
	$(call link,$@,$^ $(plm80.lib))

$(OBJ)/goto.rel: $(call objdir,goto.obj seq.obj) $(LIB)/cusp7.lib
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/hdback.rel: $(call objdir,hdback.obj files.obj) $(LIB)/cusp2.lib
	$(call link,$@,$^ $(plm80.lib) $(system.lib))

$(OBJ)/if.rel: $(call objdir,if.obj seq.obj) $(LIB)/cusp7.lib
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/last.rel: $(OBJ)/last.obj $(call libs,cusp2.lib util2.lib sys2.lib)
	$(call link,$@,$^ $(plm80.lib))

$(OBJ)/latest.rel: $(OBJ)/latest.obj $(call libs,cusp3.lib util3.lib sys1.lib isis3.lib) $(plm80.lib)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/loop.rel: $(OBJ)/loop.obj $(LIB)/pff.lib
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/lower.rel: $(OBJ)/lower.obj $(call libs,cusp3.lib sys1.lib util3.lib unixio.lib)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/merg80.rel: $(call objdir,merg80.obj bltsym.obj sort2.obj) $(call libs,cusp2.lib util2.lib sys2.lib)\
   			       $(system.lib) $(plm80.lib) $(LIB)/fnames.lib
	$(call link,$@,$^)

$(OBJ)/merg86.rel: $(call objdir,merg86.obj bsym86.obj) $(LIB)/fnames.lib\
	               $(OBJ)/sort2.obj $(call libs,util2.lib sys2.lib)
	$(call link,$@,$^ $(plm80.lib))

$(OBJ)/mrkobj.rel: $(call objdir,mrkobj.obj minvok.obj mfiler.obj) $(call libs,fnames.lib util2.lib sys2.lib)
	$(call link,$@,$^ $(plm80.lib))

$(OBJ)/note.rel: $(OBJ)/note.obj $(LIB)/cusp7.lib
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/pack.rel: $(call objdir,pack.obj packer.obj) $(LIB)/cusp6.lib
	$(call link,$@,$^ $(plm80.lib) $(system.lib))

$(OBJ)/passif.rel: $(call objdir,main.obj cli.obj errmsg.obj func.obj report.obj tokens.obj) $(LIB)/util3.lib
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/pause.rel: $(OBJ)/pause.obj $(call libs,cusp2.lib util2.lib pff.lib sys2.lib)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/relab.rel: $(OBJ)/relab.obj $(LIB)/pff.lib
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/rescan.rel: $(OBJ)/rescan.obj
	$(call link,$@,$^ $(plm80.lib) $(system.lib))

$(OBJ)/return.rel: $(OBJ)/return.obj $(LIB)/cusp7.lib
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/sort.rel: $(OBJ)/sort.obj $(call libs,cusp3.lib sys1.lib util3.lib)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/stopif.rel: $(OBJ)/stopif.obj $(call libs,cusp2.lib util2.lib sys2.lib)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/unpack.rel: $(call objdir,unpack.obj unpakr.obj) $(LIB)/cusp6.lib
	$(call link,$@,$^ $(plm80.lib) $(system.lib))

$(OBJ)/upper.rel: $(OBJ)/upper.obj $(call libs,cusp6.lib util3.lib sys1.lib)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/wait.rel: $(OBJ)/wait.obj $(LIB)/cusp7.lib
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

$(OBJ)/which.rel: $(OBJ)/which.obj $(call libs,isis3.lib sys1.lib cusp3.lib util3.lib)
	$(call link,$@,$^ $(plm80.lib))

$(OBJ)/xlate2.rel: $(OBJ)/xlate2.obj $(call libs,util2.lib sys2.lib isis3.lib)
	$(call link,$@,$^ $(plm80.lib))

# extensions to default rules
clean::
	-@for d in $(SUBDIRS); do $(MAKE) -C $$d clean; done;

# as the clean name clashes, use an upper case letter to map the file
Clean: $(OBJ)/clean.rel
	$(call locate,$@,$^,$(PURGE))


# special verify
# firstly makes sure the latest sources are built
# then verify all of the subdirectories
# finally verify the main targets 
verify: all
	@for d in $(SUBDIRS); do $(MAKE) -C $$d verify || exit 1; done;
	@for f in $(TARGETS); do $(COMPARE) $$f $(REF)/$$f || exit 1; done;

