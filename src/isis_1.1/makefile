# path to root of build tree
ROOT := ../..
TARGETS := format delete isis.cli attrib dir rename copy hexbin\
   		   isis.t0 isis.bin

include $(ROOT)/tools/isis.mk

REF = $(call ipath,isis,1.1)
PATCH = isis.t0_1.1.patch
PROTECT = $(PATCH)

all:: $(TARGETS)

%.omf: %.hex
	$(HEXOBJ) $^ $@

% : %.omf
	$(OBJBIN) -i $^ $@

isis.t0: isis.t0.omf $(PATCH)
	$(OBJBIN) $^ $@

# isis.bin is in assembler, creating the standard .obj output
isis.bin: isis.obj
	$(OBJBIN) -i $^ $@


%.hex %.lst %.prn: %.plm
	$(PLM81) $^
	$(PLM82) $^
	@perl $(ROOT)/tools/pretty.pl $*.lst $*.prn
	@rm -fr $*.pol $*.sym

clean::
	rm -fr *.hex *.prn *.lst *.obj *.omf