ROOT = ../..
TARGETS = cli
REF = ref
PEXFILE=cli.pex
include $(ROOT)/tools/isis.mk

ISISTOOLS=$(V31)

PLMFFLAGS=code optimize

OBJS=	cli.obj dblank.obj dlimit.obj filerr.obj forupp.obj monitr.obj seq.obj\
	reperr.obj upcase.obj wdlmt.obj 

all: $(TARGETS)

cli: $(OBJS)
	$(call link,cli.rel,$^ $(call v31dir,system.lib plm80.lib))
	$(call locate,$@,cli.rel,code(3680h) name(CLI) stacksize(68) purge)
	@rm cli.rel

