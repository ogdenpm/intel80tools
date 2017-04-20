# path to root of build tree
ROOT := ../..
TARGETS := lib
PEXFILE:=lib.pex
KEEP = 1
# build options
LOCATEFLAGS:=SYMBOLS LINES
PLMFLAGS:=DEBUG
LINKFLAGS:=

include $(ROOT)/tools/isis.mk

# set default tools
COMPARE := $(ROOT)/tools/diffbin


objs =	lib.obj lib1.obj isis1.obj isisa.obj isis2.obj lib3.obj lib4.obj

all:: $(TARGETS)

lib: lib.rel
	$(call locate,$@,$^,code(3680H) name(lib) stacksize(90) purge)

.INTERMEDIATE: lib.rel
lib.rel: $(objs)
	$(call link,$@,$^ $(V40)/plm80.lib)
