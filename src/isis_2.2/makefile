# path to root of build tree
ITOOLS ?= ../..
TARGETS := isis.bin
PROTECT := isis.bin.cfg

include $(ITOOLS)/tools/isis.mk

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
	@perl $(ITOOLS)/tools/pretty.pl $*.lst $*.prn
	rm -fr $*.pol $*.sym $*.lst

clean::
	rm -fr *.hex *.prn *.omf
