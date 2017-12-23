# path to root of build tree
ROOT := ../..
TARGETS := submit
PEXFILE:=submit.pex

include $(ROOT)/tools/isis.mk

REF = $(call ipath,isis,4.3)

# build options
LOCATEFLAGS:=SYMBOLS LINES
PLMFLAGS:=DEBUG CODE
LINKFLAGS:=

objs =	submit.obj dblank.obj dlimit.obj filerr.obj numout.obj\
	path.obj scanin.obj seq.obj trange.obj upcase.obj unpath.obj \
	mvasci.obj reperr.obj wdlmt.obj

all:: $(TARGETS)

submit: submit.rel
	$(call locate,$@,$^,code(3680H) name(submit) stacksize(102) purge)

.INTERMEDIATE: submit.rel
submit.rel: $(objs)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))
