ROOT = ../..
TARGETS = isis.cli
PEXFILE=cli.pex
include $(ROOT)/tools/isis.mk

REF = $(call ipath,isis,2.2)
PLM80 := 3.1

PLMFLAGS=code optimize debug
LOCATEFLAGS=symbols publics

objs=	cli.obx dblank.obj dlimit.obj filerr.obx forupp.obj monitr.obj\
	reperr.obx scnblk.obj seq.obj upcase.obj numout.obj p0029.obj

all:: $(TARGETS)

isis.cli: cli.rel
	$(call locate,$@,$^,code(3680h) name(CLI) stacksize(70) purge)


monitr.obj: PLMFLAGS=code nooptimize debug

.INTERMEDIATE: cli.rel
cli.rel: $(objs)
	$(call link,$@,$^ $(system.lib,3.1))

cli.obx: cli.obj p0095.obj p0100.obj p0101.obj
	$(call link-nocheck,$@,$^)
	perl $(ROOT)/tools/fixomf.pl -p $@

filerr.obx: filerr.obj p0095.obj p0101.obj
	$(call link-nocheck,$@,$^)
	perl $(ROOT)/tools/fixomf.pl -p $@

reperr.obx: reperr.obj p0100.obj p0101.obj
	$(call link-nocheck,$@,$^)
	perl $(ROOT)/tools/fixomf.pl -p $@
