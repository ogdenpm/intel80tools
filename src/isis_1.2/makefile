# path to root of build tree
ITOOLS ?= ../..
TARGETS := asm80 asmb asmb12 attrib batch copy delete dir \
		   edit format hexbin isis.bin isis.cli isis.t0 load rename \
		   type upm

include $(ITOOLS)/tools/isis.mk

REF = $(call ipath,isis,1.2)
ISIST0PATCH=isis.t0_1.2.patch	# for uninitialised data
ISISBINPATCH=isis.bin_1.2.patch	# for start overrite

isis.bin.hex : PLM2OPT := -i1 -e0

all:: 
	$(MAKE) $(TARGETS)

isis.t0: isis.t0.hex 
	$(call abstool,$@,$^,$(ISIST0PATCH))

%: %.hex
	$(call abstool,$@,-i $^)

%: %.obj	# asm80 converts .asm files to .obj files
	$(call abstool,$@,-i $^)
	
%.hex %.prn: %.plm
	$(PLM81) $^
	$(PLM82) $(PLM2OPT) $^
	@perl $(ITOOLS)/tools/pretty.pl $*.lst $*.prn
	rm -fr $*.pol $*.sym $*.lst

clean::
	rm -fr *.hex *.prn *.pol *.sym 
