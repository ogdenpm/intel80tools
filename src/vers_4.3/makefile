# path to root of build tree
ROOT := ../..
TARGETS := vers
PEXFILE:=vers.pex

include $(ROOT)/tools/isis.mk

REF = $(call ipath,isis,4.3)

# build options
LOCATEFLAGS:=SYMBOLS LINES
PLMFLAGS:=DEBUG CODE
LINKFLAGS:=

objs =	vers.obj

all:: $(TARGETS)

vers: vers.rel
	$(call locate,$@,$^,code(3680H) name(vers) stacksize(44) purge)

.INTERMEDIATE: vers.rel
vers.rel: $(objs)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))
