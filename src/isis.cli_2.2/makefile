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

all::
	$(MAKE) $(TARGETS)

isis.cli: cli.rel
	$(call locate,$@,$^,code(3680h) name(CLI) stacksize(70) purge)


monitr.obj: PLMFLAGS=code nooptimize debug

.INTERMEDIATE: cli.rel cli.obj filerr.obj reperr.obj
cli.rel: $(objs)
	$(call link,$@,$^ $(system.lib,3.1))

# note the three object files need special treatment
# the compiled files have plm80.lib library
# code fragments attached. The fixomf.pl utility is
# used to remove the references to the public plm80.lib
# functions post linkage, this prevents other code
# using it.
# It is possible that the code was originally compiled
# using an older, possibly pre-release version of the PL/M compiler.
cli.obx: cli.obj p0095.obj p0100.obj p0101.obj
	$(call link-nocheck,$@,$^)
	perl $(ROOT)/tools/fixomf.pl -p $@

filerr.obx: filerr.obj p0095.obj p0101.obj
	$(call link-nocheck,$@,$^)
	perl $(ROOT)/tools/fixomf.pl -p $@

reperr.obx: reperr.obj p0100.obj p0101.obj
	$(call link-nocheck,$@,$^)
	perl $(ROOT)/tools/fixomf.pl -p $@
