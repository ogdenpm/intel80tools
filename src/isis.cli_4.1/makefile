ROOT = ../..
TARGETS = isis.cli
PEXFILE=cli.pex
include $(ROOT)/tools/isis.mk

REF = $(call ipath,isis,4.1)
PLM80 := 3.1

PLMFFLAGS=code optimize

objs=	cli.obj dblank.obj dlimit.obj filerr.obj forupp.obj monitr.obj seq.obj\
	reperr.obj upcase.obj wdlmt.obj 

all:: $(TARGETS)

isis.cli: cli.rel
	$(call locate,$@,$^,code(3680h) name(CLI) stacksize(68) purge)

.INTERMEDIATE: cli.rel
cli.rel: $(objs)
	$(call link,$@,$^ $(system.lib,3.1) $(plm80.lib))

