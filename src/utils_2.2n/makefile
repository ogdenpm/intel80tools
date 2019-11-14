# path to root of build tree
ROOT := ../..
TARGETS := objhex
PEXFILE:=utils_2.2n.pex

include $(ROOT)/tools/isis.mk

REF = $(call ipath,utils,2.2n)

# build options
LOCATEFLAGS:=SYMBOLS LINES
PLMFLAGS:=DEBUG CODE
LINKFLAGS:=

objs =	objhex.obj dblank.obj dlimit.obj filerr.obj forupp.obj numout.obj\
	path.obj seq.obj mvasci.obj reperr.obj upcase.obj wdlmt.obj

all::
	$(MAKE) $(TARGETS)

objhex: objhex.rel
	$(call locate,$@,$^,code(3680H) name(objhex) stacksize(86) purge)

.INTERMEDIATE: objhex.rel
objhex.rel: $(objs)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))
