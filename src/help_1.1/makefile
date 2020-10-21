# path to root of build tree
ITOOLS ?= ../..
TARGETS := help
PEXFILE:=help.pex

include $(ITOOLS)/tools/isis.mk

REF = $(call ipath,pds,1.1)

# build options
LOCATEFLAGS:=SYMBOLS LINES
PLMFLAGS:=DEBUG CODE
LINKFLAGS:=

objs =	help.obj pstreq.obj pseq.obj scanin.obj forupp.obj shwerr.obj\
		numout.obj upcase.obj

all::
	$(MAKE) $(TARGETS)

help: help.rel
	$(call locate,$@,$^,code(3680H) name(help) stacksize(68) purge)

.INTERMEDIATE: help.rel
help.rel: $(objs)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))
