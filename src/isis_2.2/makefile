# path to root of build tree
ITOOLS ?= ../..
TARGETS := isis.bin isis.t0 isis.cli format
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

cliobjs=	cli.obx dblank.obj dlimit.obj filerr.obx forupp.obj monitr.obj\
			reperr.obx scnblk.obj seq.obj upcase.obj numout.obx

formatobjs = format.obx mask.obj absio.obj clrbuf.obj dblank.obj diskio.obj\
			 dlimit.obj filerr.obx fmttrk.obx forupp.obj path.obx reperr.obx\
			 scnblk.obj setblk.obx upcase.obj unpath.obj wrdir.obx\
			 mvasci.obj numout.obx 

formatlst = format.lst mask.lst absio.lst clrbuf.lst dblank.lst diskio.lst\
			 dlimit.lst filerr.lst fmttrk.lst forupp.lst path.lst reperr.lst\
			 scnblk.lst setblk.lst upcase.lst unpath.lst wrdir.lst\
			 mvasci.lst numout.lst

isis.cli: cli.rel
	$(call locate,$@,$^,code(3680h) name(CLI) stacksize(70) purge)

# this is a hack as the binary has MAPDCB as an external, but it is not used
# initially format.abs is built which represents the normal format build
# this is then linked with fake.obj which contains the extern ref
# finally the patches & file split are applied using fixobj
# to create an identical image
format.abs: format.rel
	$(call locate,$@,$^,code(3680h) name(FORMAT) stacksize(102) purge)

%: %.abs fake.obj %.patch
	$(call link-nocheck,$@,$*.abs fake.obj)
	$(ITOOLS)/tools/fixobj -p $*.patch $@ $@


monitr.obj: PLMFLAGS=code nooptimize debug

.INTERMEDIATE: cli.rel cli.obj filerr.obj reperr.obj

cli.rel: $(cliobjs)
	$(call link,$@,$^ $(system.lib,3.1))

format.rel: $(formatobjs)
	$(call link-nocheck,$@,$^ $(system.lib,3.1))


# note the several object files need special treatment
# the compiled files have plm80.lib library
# code fragments attached. The fixobj utility is
# used to remove the references to the public plm80.lib
# functions post linkage, this prevents other code
# using it.
# It is possible that the code was originally compiled
# using an older, possibly pre-release version of the PL/M compiler.
%.obx: %.obj
	$(call link-nocheck,$@,$^ $(plm80.lib))
	$(ITOOLS)/tools/fixobj -l $@ $@

cli.obx: cli.obj p0095.obj p0100.obj
	$(call link-nocheck,$@,$^ $(plm80.lib))
	$(ITOOLS)/tools/fixobj -l $@ $@

filerr.obx: filerr.obj p0095.obj
	$(call link-nocheck,$@,$^ $(plm80.lib))
	$(ITOOLS)/tools/fixobj -l $@ $@

reperr.obx: reperr.obj p0100.obj
	$(call link-nocheck,$@,$^ $(plm80.lib))
	$(ITOOLS)/tools/fixobj -l $@ $@

format.obx: format.obj p0019.obj p0095.obj
	$(call link-nocheck,$@,$^ $(plm80.lib))
	$(ITOOLS)/tools/fixobj -l $@ $@

clean::
	rm -fr *.hex *.prn *.omf *.pol *.sym *.obx tmp0000*

print: all
	perl $(ITOOLS)/tools/relst.pl format format.map $(formatlst)
	perl $(ITOOLS)/tools/rebase.pl format.lst 0 3680 >format.prn
