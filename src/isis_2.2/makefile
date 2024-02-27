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

isis.bin: isis.bin.hex
	$(ABSBIN) -i $^ $@

isis.t0: isis.t0.hex isis.t0.patch
	$(ABSBIN) $^ $@

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


monitr.obj: PLMFLAGS=code nooptimize debug

.INTERMEDIATE: cli.rel format.rel cli.obj filerr.obj reperr.obj

cli.rel: $(cliobjs)
	$(call link,$@,$^ $(system.lib,3.1))

isis.cli: cli.rel
	$(call locate,$@,$^,code(3680h) name(CLI) stacksize(70) purge)


# format has one undefined but unused external MAPDCB
format.rel: $(formatobjs)
	$(call link-externok,$@,$^ $(system.lib,3.1))

format: format.rel format.patch
	$(call locate-externok,$@,format.rel,code(3680h) name(FORMAT) stacksize(102) purge)
	$(ITOOLS)/tools/fixobj -p format.patch $@ $@


# note the several object files need special treatment
# the compiled files have plm80.lib library
# code fragments attached. The fixobj utility is
# used to remove the references to the public plm80.lib
# functions post linkage, this prevents other code
# using it.
# It is possible that the version of the compiler used had not been
# modified to use plm80.lib. The current compilers would
# do this only if there are no externals and no publics

%.obx: %.obj
	-$(call link,$@,$^ $(plm80.lib))
	$(ITOOLS)/tools/fixobj -l $@ $@

cli.obx: cli.obj p0095.obj p0100.obj
	-$(call link,$@,$^ $(plm80.lib))
	$(ITOOLS)/tools/fixobj -l $@ $@

filerr.obx: filerr.obj p0095.obj
	-$(call link,$@,$^ $(plm80.lib))
	$(ITOOLS)/tools/fixobj -l $@ $@

reperr.obx: reperr.obj p0100.obj
	-$(call link,$@,$^ $(plm80.lib))
	$(ITOOLS)/tools/fixobj -l $@ $@

format.obx: format.obj p0019.obj p0095.obj
	-$(call link,$@,$^ $(plm80.lib))
	$(ITOOLS)/tools/fixobj -l $@ $@

clean::
	rm -fr *.hex *.prn *.omf *.pol *.sym *.obx

print: all
	perl $(ITOOLS)/tools/relst.pl format format.map $(formatlst)
	perl $(ITOOLS)/tools/rebase.pl format.lst 0 3680 >format.prn
