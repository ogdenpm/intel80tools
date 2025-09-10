# path to root of build tree
ITOOLS ?= ../..
TARGETS := monasm.bin
REF= ref
include $(ITOOLS)/tools/isis.mk

MONSRC = sdkmon.src common.inc utilit.inc discha.inc xqt.inc monfun.inc
ADMSRC = sdkadm.src common.inc asm.inc asma.inc sdkdsm.inc

export ISIS_F1=.

all:: $(TARGETS)
monasm.bin: sdkmon.bin sdkadm.bin
	$(call abstool,$@,-le000 $^,monasm.patch)



# build options

%.bin : %.o51
	$(call abstool,$@,$^)

%.o51: %.src
	$(call asm51,$@,$<,DATE(8,12,81) EP DB SB)


sdkmon.o51: common.inc utilit.inc discha.inc xqt.inc monfun.inc
sdkadm.o51: common.inc asm.inc asma.inc sdkdsm.inc

clean::
	rm -f *.o51
