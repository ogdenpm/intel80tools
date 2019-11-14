ROOT = ../..
TARGETS = isis.cli
PEXFILE=cli.pex
include $(ROOT)/tools/isis.mk

REF = $(call ipath,isis,4.2)

PLMFLAGS = code optimize

objs=	cli.obj dblank.obj dlimit.obj filerr.obj forupp.obj monitr.obj seq.obj\
	reperr.obj upcase.obj wdlmt.obj 

all::
	$(MAKE) $(TARGETS)

isis.cli: cli.rel
	$(call locate,$@,$^,code(3680h) name(CLI) stacksize(66) purge)

.INTERMEDIATE: cli.rel
cli.rel: $(objs)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

