# path to root of build tree
ITOOLS ?= ../..
TARGETS := format delete isis.cli attrib dir rename copy hexbin\
   		   isis.t0 isis.bin edit as80

include $(ITOOLS)/tools/isis.mk

REF = $(call ipath,isis,1.1)
ISIST0PATCH = isis.t0_1.1.patch

all:: $(TARGETS)


% : %.hex
	$(call abstool,$@,-i $^)

isis.t0: isis.t0.hex
	$(call abstool,$@,$^,$(ISIST0PATCH))

# isis.bin is in assembler, creating the standard .obj output
isis.bin: isis.obj
	$(call abstool,$@,-i $^)


%.hex %.lst %.prn: %.plm
	$(PLM81) $^
	$(PLM82) $^
	@perl $(ITOOLS)/tools/pretty.pl $*.lst $*.prn
	@rm -fr $*.pol $*.sym

clean::
	rm -fr *.hex *.prn *.lst *.obj *.omf
