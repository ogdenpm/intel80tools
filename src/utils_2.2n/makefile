# path to root of build tree
ITOOLS ?= ../..
TARGETS := objhex hexobj
PEXFILE:=utils_2.2n.pex
.PRECIOUS: hexobj.rel
include $(ITOOLS)/tools/isis.mk

REF = $(call ipath,utils,2.2n)

# build options
LOCATEFLAGS:=SYMBOLS LINES
PLMFLAGS:=DEBUG CODE
LINKFLAGS:=

objhexObj =	objhex.obj dblank.obj dlimit.obj filerr.obj forupp.obj numout.obj\
	path.obj seq.obj mvasci.obj reperr.obj upcase.obj wdlmt.obj

hexobjObj =	hexobj.obj dblank.obj dlimit.obj filerr.obj forupp.obj\
	path.obj scanin.obj seq.obj mvasci.obj reperr.obj upcase.obj wdlmt.obj\
   	numout.obj 

all::
	$(MAKE) $(TARGETS)

objhex: objhex.rel
	$(call locate,$@,$^,code(3680H) name(objhex) stacksize(86) purge)

hexobj: hexobj.rel
	$(call locate,$@,$^,code(3680H) MAP name(hexobj) stacksize(94) purge)

.INTERMEDIATE: objhex.rel hexobj.rel
objhex.rel: $(objhexObj)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

hexobj.rel: $(hexobjObj)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

