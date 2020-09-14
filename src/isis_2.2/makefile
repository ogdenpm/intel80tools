# path to root of build tree
ROOT := ../..
TARGETS := isis.bin
PROTECT := isis.bin.cfg

include $(ROOT)/tools/isis.mk

REF = $(call ipath,isis,2.2)


all::
	$(MAKE) $(TARGETS)

%.omf: %.hex
	$(HEXOBJ) $^ $@

%: %.omf
	$(OBJBIN) -i $^ $@

%.hex: %.plm
	$(PLM81) $^
	$(PLM82) $^
	@perl $(ROOT)/tools/pretty.pl $*.lst $*.prn
	rm -fr $*.pol $*.sym $*.lst

clean::
	rm -fr *.hex *.prn *.omf
