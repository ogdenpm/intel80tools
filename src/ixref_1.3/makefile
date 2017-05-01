# path to root of build tree
ROOT := ../..
TARGETS := ixref
PEXFILE:=ixref.pex

# build options
LOCATEFLAGS:=SYMBOLS PUBLICS
PLMFLAGS:= DEBUG IXREF
LINKFLAGS:=
PROTECT:= ISIS.OV0

include $(ROOT)/tools/isis.mk

# set default tools
REF:=$(V40)


objs =	ixref.obj ixref2.obj ixref3.obj ixref4.obj ixref5.obj ixref6.obj ixref7.obj
IXIS = $(subst .obj,.ixi,$(objs))

all:: $(TARGETS)

ixref: ixref.rel
	$(call locate,$@,$^,code(3880H) stacksize(126) purge)

.INTERMEDIATE: ixref.rel
ixref.rel: $(objs)
	$(call link,$@,$^ $(call v40dir,system.lib plm80.lib))

# invokes ixref on itself. Note the library functions are shown as unresolved as
# their source is not included
# note this needs a copy of isis.ov0 to work
ixref.ixo: all
	perl $(ROOT)/tools/mkisisdir.pl
	$(ISIS) -m -E ixref $(call mklist,$(IXIS)) "print($@)"
