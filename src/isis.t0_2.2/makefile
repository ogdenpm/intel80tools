# path to root of build tree
ROOT := ../..
TARGETS := isis.t0
PATCH = isis.t0_2.2.patch
PROTECT := $(PATCH)

include $(ROOT)/tools/isis.mk

REF = $(call ipath,isis,2.2)


all:: $(TARGETS)

%.omf: %.hex
	$(HEXOBJ) $^ $@
	
isis.t0: isis.t0.omf $(PATCH)
	$(OBJBIN) $^ $@


%.hex %.lst %.prn: %.plm
	$(PLM81) $^
	$(PLM82) $^
	@perl $(ROOT)/tools/pretty.pl $*.lst $*.prn
	@rm -fr $*.pol $*.sym $*.lst

clean::
	rm -fr *.hex *.lst *.prn *.omf
