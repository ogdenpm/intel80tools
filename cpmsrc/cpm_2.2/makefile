# path to root of build tree
ITOOLS ?= ..\..
BDOS = os2ccp.spr os2ccp.prl os3bdos.spr os3bdos.prl os1boot.hex os4bios.hex
APPS = pip.com ed.com load.com stat.com submit.com dump.com sysgen.com \
	   ddt.com xsub.com
TARGETS =  $(APPS) $(BDOS)

NOVERIFY = $(BDOS)
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

%.irl: %.obj | interface
	$(call link,$*.irl,$^ $(CPMINT) $(plm80.lib))

%.abs: %.irl
	$(call locate,$@,$^,CODE(100h) $(STACK) purge)

all::
	$(MAKE) $(TARGETS)

interface: ostrint.obj os5trint.obj

asm.com: as0com.hex as1io.hex as2scan.hex as3sym.hex as4sear.hex as5oper.hex as6main.hex
	$(call abstool,$@,$^)

ed.com: ed.abs ed20pat.hex
	$(call abstool,$@,$^)

ddt.spr: ddt1asm.rel ddt2mon.rel
	$(DRLINK) $@ = ddt1asm, ddt2mon [OS]

ddt.com: ddt.spr ddt0mov.hex 
	$(call abstool,$@,$^)

xsub.com: xsub1.spr xsub0.hex
	$(call abstool,$@,$^)

