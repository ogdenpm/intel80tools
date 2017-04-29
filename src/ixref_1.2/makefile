# path to root of build tree
ROOT := ../..
TARGETS := ixref

# build options
LOCATEFLAGS:=SYMBOLS LINES
PLMFLAGS:=OPTIMIZE IXREF
LINKFLAGS:=

include $(ROOT)/tools/isis.mk

# set default tools
REF:=$(V31)

PEXFILE:=ixref.pex

objs =	ixref.obj ixref2.obj ixref3.obj ixref4.obj ixref5.obj ixref6.obj ixref7.obj
IXIS = $(subst .obj,.ixi,$(objs))

all:: $(TARGETS)

ixref: ixref.rel
	$(call locate,$@,$^,code(3880H) stacksize(136) purge)

.INTERMEDIATE: ixref.rel
ixref.rel: $(objs)
	$(call link,$@,$^ $(call v31dir,system.lib plm80.lib))

# invokes ixref on itself. Note the library functions are shown as unresolved as
# their source is not included
ixref.ixo: all
	perl $(ROOT)/tools/mkisisdir.pl
	$(ISIS) -m -E ixref $(call mklist,$(IXIS)) "print($@)"
