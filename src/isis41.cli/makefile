ROOT = ../..
TARGETS = cli
REF = ref
PEXFILE=cli.pex
include $(ROOT)/tools/isis.mk

ISISTOOLS=$(V31)

PLMFFLAGS=code optimize

objs=	cli.obj dblank.obj dlimit.obj filerr.obj forupp.obj monitr.obj seq.obj\
	reperr.obj upcase.obj wdlmt.obj 

all:: $(TARGETS)

cli: cli.rel
	$(call locate,$@,$^,code(3680h) name(CLI) stacksize(68) purge)

.INTERMEDIATE: cli.rel
cli.rel: $(objs)
	$(call link,$@,$^ $(call v31dir,system.lib plm80.lib))

