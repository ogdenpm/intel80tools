# path to root of build tree
ITOOLS ?= ../..
TARGETS := ixref
PEXFILE:=ixref.pex

# build options
LOCATEFLAGS:=MAP SYMBOLS PUBLICS
PLMFLAGS:=OPTIMIZE IXREF CODE DEBUG
LINKFLAGS:=

include $(ITOOLS)/tools/isis.mk

# set default tools
REF:=$(call ipath,ixref,1.2)


objs =	ixref.obj ixref2.obj ixref3.obj ixref4.obj ixref5.obj ixref6.obj ixref7.obj
IXIS = $(subst .obj,.ixi,$(objs))

all::
	$(MAKE) $(TARGETS)

ixref: ixref.rel
	$(call locate,$@,$^,code(3880H) stacksize(136) purge)

.INTERMEDIATE: ixref.rel
ixref.rel: $(objs)
	$(call link,$@,$^ $(system.lib,3.1) $(plm80.lib))

# invokes ixref on itself. Note the library functions are shown as unresolved as
# their source is not included
ixref.ixo: all
	$(ISIS) ixref $(call mklist,$(IXIS)) "print($@)"
