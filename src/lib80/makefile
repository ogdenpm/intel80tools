# path to root of build tree
ROOT := ../..
TARGETS := lib
PEXFILE:=lib.pex
KEEP = 1
# build options
LOCATEOPT:=SYMBOLS LINES
PLMFOPT:=DEBUG
LINKOPT:=

include $(ROOT)/tools/isistools.mk

# set default tools
COMPARE := $(ROOT)/tools/diffbin


OBJ =	lib.obj lib1.obj isis1.obj isisa.obj isis2.obj lib3.obj lib4.obj

all: $(TARGETS)

.PHONY: verify rebuild

lib: $(OBJ)
	$(call link,lib.rel,$^ $(call tools-v40,plm80.lib))
	$(call locate,$@,lib.rel,code(3680H) name(lib) stacksize(90) purge)
	@del lib.rel
