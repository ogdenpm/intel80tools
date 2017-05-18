ROOT = ../..
OBJ = obj
LST = lst
SRC = src
LIB = lib


# map src for the pex include files
# lower level makefiles need to unexport ISIS_F1
export ISIS_F1=$(SRC)
# we used a customised verify here
NOVERIFY = T

TARGETS := chklod Clean compar consol dcopy dsort else endif errs exit ganef\
	genpex goto hdback if last latest loop lower merg80 merg86 mrkobj note\
	pack passif pause relab rescan return sort stopif unpack upper wait\
	which xlate2

# note pause was not precompiled in the original toolbox so a copy
# has been added
# also source for bltsym.asm was missing so recreated
include $(ROOT)/tools/isis.mk

REF = $(call ipath,toolbox,1.0)

# the list below controls whether the freshly compiled library or
# the original one shipped with the toolbox is used
# due to limitations in cusp2 the new compiled library cannot be
# used if a byte level exact file is neeed for others routines
# that compile differently are not used

#cusp1.lib := $(call ipath,toolbox,1.0)/cusp1.lib
cusp1.lib := $(LIB)/cusp1.lib
cusp2.lib := $(call ipath,toolbox,1.0)/cusp2.lib
#cusp2.lib := $(LIB)/cusp2.lib
#cusp3.lib := $(call ipath,toolbox,1.0)/cusp3.lib
cusp3.lib := $(LIB)/cusp3.lib
#cusp4.lib := $(call ipath,toolbox,1.0)/cusp4.lib
cusp4.lib := $(LIB)/cusp4.lib
#cusp5.lib := $(call ipath,toolbox,1.0)/cusp5.lib
cusp5.lib := $(LIB)/cusp5.lib
#cusp6.lib := $(call ipath,toolbox,1.0)/cusp6.lib
cusp6.lib := $(LIB)/cusp6.lib
#cusp7.lib := $(call ipath,toolbox,1.0)/cusp7.lib
cusp7.lib := $(LIB)/cusp7.lib
#fnames.lib := $(call ipath,toolbox,1.0)/fnames.lib
fnames.lib := $(LIB)/fnames.lib
#isis3.lib := $(call ipath,toolbox,1.0)/isis3.lib
isis3.lib := $(LIB)/isis3.lib
#pff.lib := $(call ipath,toolbox,1.0)/pff.lib
pff.lib := $(LIB)/pff.lib
#sys1.lib := $(call ipath,toolbox,1.0)/sys1.lib
sys1.lib := $(LIB)/sys1.lib
#sys2.lib := $(call ipath,toolbox,1.0)/sys2.lib
sys2.lib := $(LIB)/sys2.lib
#unixio.lib := $(call ipath,toolbox,1.0)/unixio.lib
unixio.lib := $(LIB)/unixio.lib
#util1.lib := $(call ipath,toolbox,1.0)/util1.lib
util1.lib := $(LIB)/util1.lib
#util2.lib := $(call ipath,toolbox,1.0)/util2.lib
util2.lib := $(LIB)/util2.lib
#util3.lib := $(call ipath,toolbox,1.0)/util3.lib
util3.lib := $(LIB)/util3.lib



LIBS = cusp1.lib cusp2.lib cusp3.lib cusp4.lib cusp5.lib\
       cusp6.lib cusp7.lib fnames.lib isis3.lib pff.lib\
       sys1.lib sys2.lib unixio.lib util1.lib util2.lib\
       util3.lib

SUBDIRS := $(basename $(LIBS))

PLM80 = 3.1
PLMFLAGS = CODE
LOCATEFLAGS = SYMBOLS PUBLICS
PURGE = purge

all::
	for d in $(SUBDIRS); do $(MAKE) -C $$d || exit 1; done
	$(MAKE) $(TARGETS)

$(LIB):
	mkdir -p $@

%: $(OBJ)/%.rel
	$(call locate,$@,$^,$(PURGE))

# to avoid conflict with the standard clean target - con make by creating target Clean
# as make is case sensitive this works
.INTERMEDIATE: cleanx

Clean: $(OBJ)/clean.rel
	$(call locate,cleanx,$^,$(PURGE))
	mv cleanx Clean

lib/cusp1.lib: cusp1/cusp1.lib  | $(LIB) ; cp $^ $@
lib/cusp2.lib: cusp2/cusp2.lib  | $(LIB) ; cp $^ $@
lib/cusp3.lib: cusp3/cusp3.lib  | $(LIB) ; cp $^ $@
lib/cusp4.lib: cusp4/cusp4.lib  | $(LIB) ; cp $^ $@
lib/cusp5.lib: cusp5/cusp5.lib  | $(LIB) ; cp $^ $@
lib/cusp6.lib: cusp6/cusp6.lib  | $(LIB) ; cp $^ $@
lib/cusp7.lib: cusp7/cusp7.lib  | $(LIB) ; cp $^ $@
lib/fnames.lib: fnames/fnames.lib  | $(LIB) ; cp $^ $@
lib/isis3.lib: isis3/isis3.lib  | $(LIB) ; cp $^ $@
lib/lib.lib: lib/lib.lib  | $(LIB) ; cp $^ $@
lib/lst.lib: lst/lst.lib  | $(LIB) ; cp $^ $@
lib/obj.lib: obj/obj.lib  | $(LIB) ; cp $^ $@
lib/pff.lib: pff/pff.lib  | $(LIB) ; cp $^ $@
lib/src.lib: src/src.lib  | $(LIB) ; cp $^ $@
lib/sys1.lib: sys1/sys1.lib  | $(LIB) ; cp $^ $@
lib/sys2.lib: sys2/sys2.lib  | $(LIB) ; cp $^ $@
lib/unixio.lib: unixio/unixio.lib  | $(LIB) ; cp $^ $@
lib/util1.lib: util1/util1.lib  | $(LIB) ; cp $^ $@
lib/util2.lib: util2/util2.lib  | $(LIB) ; cp $^ $@
lib/util3.lib: util3/util3.lib  | $(LIB) ; cp $^ $@


#.INTERMEDIATE: $(addprefix $(OBJ)/,$(addsuffix .rel,$(TARGETS)))

$(OBJ)/chklod.rel:  $(call objdir,chklod.obj cinvok.obj cmovem.obj cgreat.obj csort2.obj cdirec.obj) \
	            $(fnames.lib) $(util2.lib) $(sys2.lib) $(isis3.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/Clean.rel: $(OBJ)/clean.obj $(isis3.lib) $(cusp3.lib) $(sys1.lib) $(util3.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/compar.rel: $(call objdir,compar.obj tail.obj out.obj m3.obj divide.obj) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/consol.rel: $(OBJ)/consol.obj $(cusp4.lib) $(plm80.lib) $(system.lib)
	$(call link,$@,$^)

$(OBJ)/dcopy.rel: $(OBJ)/dcopy.obj $(system.lib) $(cusp5.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/dsort.rel: $(OBJ)/dsort.obj $(cusp6.lib) $(cusp3.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/else.rel: $(call objdir,else.obj seq.obj) $(cusp7.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/endif.rel: $(OBJ)/endif.obj $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/errs.rel: $(OBJ)/errs.obj $(cusp2.lib) $(util2.lib) $(sys2.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/exit.rel: $(OBJ)/exit.obj $(cusp7.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^ $(exitLib))

$(OBJ)/ganef.rel: $(OBJ)/ganef.obj $(cusp2.lib) $(util2.lib) $(sys2.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

# genpex did not have a build script in the original toobox
$(OBJ)/genpex.rel: $(OBJ)/genpex.obj $(fnames.lib) $(util1.lib) $(system.lib) $(sys2.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/goto.rel: $(call objdir,goto.obj seq.obj) $(cusp7.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/hdback.rel: $(call objdir,hdback.obj files.obj) $(cusp2.lib) $(plm80.lib) $(system.lib)
	$(call link,$@,$^)

$(OBJ)/if.rel: $(call objdir,if.obj seq.obj) $(cusp7.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/last.rel: $(OBJ)/last.obj $(cusp2.lib) $(util2.lib) $(sys2.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/latest.rel: $(OBJ)/latest.obj $(cusp3.lib) $(util3.lib) $(sys1.lib) $(isis3.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/loop.rel: $(OBJ)/loop.obj $(pff.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/lower.rel: $(OBJ)/lower.obj $(cusp3.lib) $(sys1.lib) $(util3.lib)\
	          $(unixio.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/merg80.rel: $(call objdir,merg80.obj bltsym.obj sort2.obj) $(cusp2.lib)\
		   $(util2.lib) $(sys2.lib) $(system.lib) $(plm80.lib) $(fnames.lib)
	$(call link,$@,$^)

$(OBJ)/merg86.rel: $(call objdir,merg86.obj bsym86.obj) $(fnames.lib)\
	           $(OBJ)/sort2.obj $(util2.lib) $(sys2.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/mrkobj.rel: $(call objdir,mrkobj.obj minvok.obj mfiler.obj) $(fnames.lib) $(util2.lib) $(sys2.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/note.rel: $(OBJ)/note.obj $(cusp7.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/pack.rel: $(call objdir,pack.obj packer.obj) $(cusp6.lib) $(plm80.lib) $(system.lib)
	$(call link,$@,$^)

$(OBJ)/passif.rel: $(call objdir,main.obj cli.obj errmsg.obj func.obj report.obj tokens.obj)\
	           $(util3.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/pause.rel: $(OBJ)/pause.obj $(cusp2.lib) $(util2.lib) $(pff.lib) $(sys2.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/relab.rel: $(OBJ)/relab.obj $(pff.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/rescan.rel: $(OBJ)/rescan.obj $(plm80.lib) $(system.lib)
	$(call link,$@,$^)

$(OBJ)/return.rel: $(OBJ)/return.obj $(cusp7.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/sort.rel: $(OBJ)/sort.obj $(cusp3.lib) $(sys1.lib) $(util3.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/stopif.rel: $(OBJ)/stopif.obj $(cusp2.lib) $(util2.lib) $(sys2.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/unpack.rel: $(call objdir,unpack.obj unpakr.obj) $(cusp6.lib) $(plm80.lib) $(system.lib)
	$(call link,$@,$^)

$(OBJ)/upper.rel: $(OBJ)/upper.obj $(cusp6.lib) $(util3.lib) $(sys1.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/wait.rel: $(OBJ)/wait.obj $(cusp7.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/which.rel: $(OBJ)/which.obj $(isis3.lib) $(sys1.lib) $(cusp3.lib) $(util3.lib) $(plm80.lib) 
	$(call link,$@,$^)

$(OBJ)/xlate2.rel: $(OBJ)/xlate2.obj $(util2.lib) $(sys2.lib) $(isis3.lib) $(plm80.lib)
	$(call link,$@,$^)

# extensions to default rules

$(CLEAN)::
	-@for d in $(SUBDIRS); do $(MAKE) -C $$d clean; done;

# special verify
# firstly makes sure the latest sources are extracted
# then invokes verify on the subdirectories which avoids separate invocation of make all
# from the top level makefile
# finally makes sure the main targets are built and then verifies them
verify: .extract verify_subs verify_targets

verify_subs:
	@for d in $(SUBDIRS); do $(MAKE) -C $$d verify || exit 1; done;

verify_targets: 
	$(MAKE) $(TARGETS)
	@for f in $(TARGETS); do $(COMPARE) $$f $(REF)/$$f || exit 1; done;

