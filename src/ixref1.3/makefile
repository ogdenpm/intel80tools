# path to root of build tree
ROOT := ../..
TARGETS := ixref

# build options
LOCATEOPT:=SYMBOLS PUBLICS
PLMOPT:= DEBUG
LINKOPT:=

include $(ROOT)/tools/isistools.mk

# set default tools
#COMPARE := $(ROOT)/tools/diffbin
REFDIR:=$(V40)

PEXFILE:=ixref.pex

OBJ =	ixref.obj ixref2.obj ixref3.obj ixref4.obj ixref5.obj ixref6.obj ixref7.obj
IXIS = $(subst .obj,.ixi,$(OBJ))

all: $(TARGETS)

ixref: $(OBJ)
	$(call link,ixref.rel,$^ $(call tools-v40,system.lib plm80.lib))
	$(call locate,$@,ixref.rel,code(3880H) stacksize(126) purge)
	@del ixref.rel

# invokes ixref on itself. Note the library functions are shown as unresolved as
# their source is not included
ixref.ixo: all
	perl $(ROOT)/tools/mkisisdir.pl
	$(ISIS) ixref $(subst $(space),$(comma),$(IXIS)) print($@)
