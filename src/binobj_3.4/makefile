# path to root of build tree
ROOT := ../..
TARGETS := binobj
PEXFILE:=binobj.pex

include $(ROOT)/tools/isis.mk

REF = $(call ipath,isis,3.4)

# build options
LOCATEFLAGS:=SYMBOLS LINES
PLMFLAGS:=DEBUG CODE
LINKFLAGS:=
PLM80 = 3.1

objs =	binobj.obj dblank.obj dlimit.obj filerr.obj forupp.obj\
	path.obj reperr.obj seq.obj upcase.obj wdlmt.obj mvasci.obj\
	numout.obj

all:: $(TARGETS)

binobj: binobj.rel
	$(call locate,$@,$^,code(3680H) name(binobj) stacksize(84) purge)

.INTERMEDIATE: binobj.rel
binobj.rel: $(objs)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))
