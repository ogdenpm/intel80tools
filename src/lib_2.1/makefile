# path to root of build tree
ITOOLS ?= ../..
TARGETS := lib
PEXFILE:=lib.pex

include $(ITOOLS)/tools/isis.mk

REF = $(call ipath,lib,2.1)

# build options
LOCATEFLAGS:=SYMBOLS LINES
PLMFLAGS:=DEBUG CODE
LINKFLAGS:=

objs =	lib.obj lib1.obj isis1.obj isisa.obj isis2.obj lib3.obj lib4.obj

all::
	$(MAKE) $(TARGETS)

lib: lib.rel
	$(call locate,$@,$^,code(3680H) name(lib) stacksize(90) purge)

.INTERMEDIATE: lib.rel
lib.rel: $(objs)
	$(call link,$@,$^ $(plm80.lib))
