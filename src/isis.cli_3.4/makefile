ROOT = ../..
TARGETS = isis.cli
PEXFILE=cli.pex
include $(ROOT)/tools/isis.mk

REF = $(call ipath,isis,3.4)
PLM80 := 3.1

PLMFFLAGS=code optimize debug
LOCATEFLAGS=symbols publics

objs=	cli.obj dblank.obj dlimit.obj filerr.obj forupp.obj monitr.obj\
	reperr.obj seq.obj upcase.obj wdlmt.obj numout.obj 

all::
	$(MAKE) $(TARGETS)

isis.cli: cli.rel
	$(call locate,$@,$^,code(3680h) name(CLI) stacksize(74) purge)

.INTERMEDIATE: cli.rel
cli.rel: $(objs)
	$(call link,$@,$^ $(system.lib,3.1) $(plm80.lib))

