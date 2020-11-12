# path to root of build tree
ITOOLS ?= ../..
TARGETS := isis.bin isis.t0 isis.cli
PEXFILE=isis.pex
include $(ITOOLS)/tools/isis.mk

REF = $(call ipath,isis,2.2)

all::
	$(MAKE) $(TARGETS)

# makefile elements for isis.bin and isis.t0 which use the old plm compiler
# note source is in isis.bin.plm and isis.t0.plm
%.omf: %.hex
	$(HEXOBJ) $^ $@

isis.bin: isis.bin.omf
	$(OBJBIN) -i $^ $@

isis.t0: isis.t0.omf isis.t0_2.2.patch
	$(OBJBIN) $^ $@

%.hex: %.plm $(CONFIG)
	$(PLM81) $^
	$(PLM82) $^
	@perl $(ITOOLS)/tools/pretty.pl $*.lst $*.prn
	rm -fr $*.pol $*.sym $*.lst


# elements for isis.cli - note intermediate is cli.rel to avoid conflict

PLM80 := 3.1

PLMFLAGS=code optimize debug
LOCATEFLAGS=symbols publics

objs=	cli.obx dblank.obj dlimit.obj filerr.obx forupp.obj monitr.obj\
	reperr.obx scnblk.obj seq.obj upcase.obj numout.obj p0029.obj
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
	perl $(ITOOLS)/tools/fixomf.pl -p $@

filerr.obx: filerr.obj p0095.obj p0101.obj
	$(call link-nocheck,$@,$^)
	perl $(ITOOLS)/tools/fixomf.pl -p $@

reperr.obx: reperr.obj p0100.obj p0101.obj
	$(call link-nocheck,$@,$^)
	perl $(ITOOLS)/tools/fixomf.pl -p $@


clean::
	rm -fr *.hex *.prn *.omf *.pol *.sym 

