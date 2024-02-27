# path to root of build tree
ITOOLS ?= ../..
TARGETS := format delete isis.cli attrib dir rename copy hexbin\
   		   isis.t0 isis.bin edit as80

include $(ITOOLS)/tools/isis.mk

REF = $(call ipath,isis,1.1)

all::
	$(MAKE)	$(TARGETS)

% : %.hex
	$(ABSBIN) -i $^ $@

isis.t0: isis.t0.hex isis.t0.patch
	$(ABSBIN) $^ $@

# isis.bin is in assembler, creating the standard .obj output
# it requires the ported ASM80 to build as I have used long names
# If this is not available rename isis.asm to isis.asmx so that the asmx.pl wrapper is used
isis.bin: isis.obj
	$(ABSBIN) -i $^ $@


%.hex %.lst %.prn: %.plm
	$(PLM81) $^
	$(PLM82) $^
	@perl $(ITOOLS)/tools/pretty.pl $*.lst $*.prn
	@rm -fr $*.pol $*.sym

clean::
	rm -fr *.hex *.prn *.lst *.obj *.omf
