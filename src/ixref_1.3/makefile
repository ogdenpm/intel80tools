# path to root of build tree
ROOT := ../..
TARGETS := ixref

# build options
LOCATEFLAGS:=SYMBOLS PUBLICS
PLMFLAGS:= DEBUG IXREF
LINKFLAGS:=
PROTECT:= ISIS.OV0

include $(ROOT)/tools/isis.mk

# set default tools
#COMPARE := $(ROOT)/tools/diffbin
REF:=$(V40)

PEXFILE:=ixref.pex

OBJ =	ixref.obj ixref2.obj ixref3.obj ixref4.obj ixref5.obj ixref6.obj ixref7.obj
IXIS = $(subst .obj,.ixi,$(OBJ))

all: $(TARGETS)

ixref: $(OBJ)
	$(call link,ixref.rel,$^ $(call v40dir,system.lib plm80.lib))
	$(call locate,$@,ixref.rel,code(3880H) stacksize(126) purge)
	@rm ixref.rel

# invokes ixref on itself. Note the library functions are shown as unresolved as
# their source is not included
# note this needs a copy of isis.ov0 to work
ixref.ixo: all
	perl $(ROOT)/tools/mkisisdir.pl
	$(ISIS) -m -E ixref $(call mklist,$(IXIS)) "print($@)"
