ITOOLS ?= ../..
OBJ = obj
LST = lst
SRC = src
LIB = lib


# map src for the pex include files
# lower level makefiles need to unexport ISIS_F1
export ISIS_F1=$(SRC)
# we used a customised verify here
#NOVERIFY = T

TARGETS := chklod Clean compar consol dcopy dsort else endif errs exit ganef\
	genpex goto hdback if last latest loop lower merg80 merg86 mrkobj note\
	pack passif relab rescan return sort stopif unpack upper wait\
	which xlate2

# note pause was not precompiled in the original toolbox so a copy
# has been added
# also source for bltsym.asm was missing so recreated
include $(ITOOLS)/tools/isis.mk

REF = $(call ipath,toolbox,2.0)

# the list below controls whether the freshly compiled library or
# the original one shipped with the toolbox is used
# due to limitations in cusp2 the new compiled library cannot be
# used if a byte level exact file is neeed for others routines
# that compile differently are not used

cusp2.lib := $(call ipath,toolbox,2.0)/cusp2.lib
#cusp2.lib := $(LIB)/cusp2.lib
#cusp5.lib := $(call ipath,toolbox,2.0)/cusp5.lib
cusp5.lib := $(LIB)/cusp5.lib
#cusp6.lib := $(call ipath,toolbox,2.0)/cusp6.lib
cusp6.lib := $(LIB)/cusp6.lib
#cusp8.lib := $(call ipath,toolbox,2.0)/cusp8.lib
cusp8.lib := $(LIB)/cusp8.lib
#fnames.lib := $(call ipath,toolbox,2.0)/fnames.lib
fnames.lib := $(LIB)/fnames.lib
#sys.lib := $(call ipath,toolbox,2.0)/sys.lib
sys.lib := $(LIB)/sys.lib
#unixio.lib := $(call ipath,toolbox,2.0)/unixio.lib
unixio.lib := $(LIB)/unixio.lib
#util.lib := $(call ipath,toolbox,2.0)/util.lib
util.lib := $(LIB)/util.lib



LIBS = cusp2.lib cusp5.lib cusp6.lib cusp8.lib fnames.lib\
       sys.lib unixio.lib util.lib

SUBDIRS := $(basename $(LIBS))

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

lib/cusp2.lib: cusp2/cusp2.lib  | $(LIB) ; cp $^ $@
lib/cusp5.lib: cusp5/cusp5.lib  | $(LIB) ; cp $^ $@
lib/cusp6.lib: cusp6/cusp6.lib  | $(LIB) ; cp $^ $@
lib/cusp8.lib: cusp8/cusp8.lib  | $(LIB) ; cp $^ $@
lib/fnames.lib: fnames/fnames.lib  | $(LIB) ; cp $^ $@
lib/sys.lib: sys/sys.lib  | $(LIB) ; cp $^ $@
lib/unixio.lib: unixio/unixio.lib  | $(LIB) ; cp $^ $@
lib/util.lib: util/util.lib  | $(LIB) ; cp $^ $@


#.INTERMEDIATE: $(addprefix $(OBJ)/,$(addsuffix .rel,$(TARGETS)))
$(OBJ)/chklod.rel: $(call objdir,chklod.obj cinvok.obj cmovem.obj cgreat.obj csort2.obj cdirec.obj) $(fnames.lib) $(util.lib) $(sys.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/clean.rel: $(OBJ)/clean.obj $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/compar.rel: $(call objdir,compar.obj tail.obj out.obj m3.obj divide.obj) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/consol.rel: $(OBJ)/consol.obj $(cusp5.lib) $(plm80.lib) $(system.lib)
	$(call link,$@,$^)

$(OBJ)/dcopy.rel: $(OBJ)/dcopy.obj $(system.lib) $(cusp5.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/dsort.rel: $(OBJ)/dsort.obj $(cusp6.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/else.rel: $(OBJ)/else.obj $(cusp8.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/endif.rel: $(OBJ)/endif.obj $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/errs.rel: $(OBJ)/errs.obj $(util.lib) $(sys.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/exit.rel: $(OBJ)/exit.obj $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/ganef.rel: $(OBJ)/ganef.obj $(cusp2.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/genpex.rel: $(OBJ)/genpex.obj $(fnames.lib) $(util.lib) $(sys.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/goto.rel: $(OBJ)/goto.obj $(cusp8.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/hdback.rel: $(call objdir,hdback.obj hdfils.obj) $(util.lib) $(plm80.lib) $(system.lib)
	$(call link,$@,$^)

$(OBJ)/if.rel: $(OBJ)/if.obj $(cusp8.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/last.rel: $(OBJ)/last.obj $(util.lib) $(sys.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/latest.rel: $(OBJ)/latest.obj $(util.lib) $(sys.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/loop.rel: $(OBJ)/loop.obj $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/lower.rel: $(OBJ)/lower.obj $(unixio.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/merg80.rel: $(call objdir,merg80.obj bltsym.obj sort2.obj) $(fnames.lib) $(util.lib) $(sys.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/merg86.rel: $(call objdir,merg86.obj bsym86.obj sort2.obj) $(fnames.lib) $(util.lib) $(sys.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/mrkobj.rel: $(call objdir,mrkobj.obj minvok.obj mfiler.obj) $(fnames.lib) $(util.lib) $(sys.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/note.rel: $(OBJ)/note.obj $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/pack.rel: $(call objdir,pack.obj packer.obj) $(cusp6.lib) $(plm80.lib) $(system.lib)
	$(call link,$@,$^)

$(OBJ)/passif.rel: $(call objdir,passif.obj cli.obj errmsg.obj func.obj report.obj tokens.obj) $(util.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/relab.rel: $(OBJ)/relab.obj $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/rescan.rel: $(OBJ)/rescan.obj $(plm80.lib) $(system.lib)
	$(call link,$@,$^)

$(OBJ)/return.rel: $(OBJ)/return.obj $(cusp8.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/sort.rel: $(OBJ)/sort.obj $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/stopif.rel: $(OBJ)/stopif.obj $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/unpack.rel: $(call objdir,unpack.obj unpakr.obj) $(cusp6.lib) $(plm80.lib) $(system.lib)
	$(call link,$@,$^)

$(OBJ)/upper.rel: $(OBJ)/upper.obj $(util.lib) $(sys.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/wait.rel: $(OBJ)/wait.obj $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/which.rel: $(OBJ)/which.obj $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

$(OBJ)/xlate2.rel: $(OBJ)/xlate2.obj $(util.lib) $(sys.lib) $(system.lib) $(plm80.lib)
	$(call link,$@,$^)

# extensions to default rules
clean::
	-@for d in $(SUBDIRS); do $(MAKE) -C $$d clean; done;

# as the clean name clashes use an upper case first letter to map the file
Clean: $(OBJ)/clean.rel
	$(call locate,$@,$^,$(PURGE))

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

