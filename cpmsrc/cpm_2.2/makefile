# path to root of build tree
ITOOLS ?= ..\..
TARGETS = pip.com ed.com load.com stat.com submit.com dump.com sysgen.com \
	   ddt.com xsub.com movcpm.com asm.com

REF=ref
# as we mix Intel and CPM assemblers use a80 suffix for Intel assembler
IASM=a80
# don't include the OLD plm81/plm82 compiler to avoid plm->hex->com build
NOOLDPLM=T

include $(ITOOLS)/tools/cpm.mk

# default interface for CPM
CPMINT = os5trint.obj

# special case build rules
ed.obj stat.obj submit.obj: PLM80=3.1
ed.abs: STACK=STACKSIZE(80)
load.abs stat.abs submit.abs: CPMINT = ostrint.obj

.PHONY: all

%.abs: %.obj | interface
	$(call link,$*.irl,$^ $(CPMINT) $(plm80.lib))
	$(call locate,$@,$*.irl,CODE(100h) $(STACK) purge)
	rm $*.irl

%.bin: %.hex
	$(call abstool,$@,$^)


all::
	$(MAKE) $(TARGETS)

interface: ostrint.obj os5trint.obj

movcpm.bin: cpmove.bin os1boot.arel cpm.spr
	mkMovcpm $^ $@

movcpm.com: movcpm.bin 			# apply patch file
	$(call abstool,$@,$^)


cpm0.hex: os2ccp0.hex os3bdos0.hex os4bios0.hex
	wcat -o $@ $^

cpm1.hex: os2ccp1.hex os3bdos1.hex os4bios1.hex
	wcat -o $@ $^


asm.com: as0com.hex as1io.hex as2scan.hex as3sym.hex as4sear.hex as5oper.hex as6main.hex
	$(call abstool,$@,$^)

ed.com: ed.abs ed20pat.hex
	$(call abstool,$@,$^)

# join the two ddt files based at 0
ddt0.hex: ddt1asm0.hex ddt2mon0.hex
	wcat -o $@ $^

# repeat join for the files based at 100h
ddt1.hex: ddt1asm1.hex ddt2mon1.hex
	wcat -o $@ $^

ddt.com: ddt.spr ddt0mov.hex 
	$(call abstool,$@,$^)

xsub.com: xsub1.spr xsub0.hex
	$(call abstool,$@,$^)

clean::
	-rm -f *.bin *.spr *.cpm *.arel


# this is a pseudo .spr file as it is located abs but has relocatable link info into cpm
os1boot.arel: os1boot.hex os1bootr.hex
	genmod -a $^ $@
