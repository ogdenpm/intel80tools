# path to root of build tree
ROOT := ../..
TARGETS := isis.t0
PATCH = isis.t0_2.2.patch
PROTECT := $(PATCH)

include $(ROOT)/tools/isis.mk

REF = $(call ipath,isis,2.2)


all:: $(TARGETS)

%.omf: %.hex
	hexobj $^ $@
	
isis.t0: isis.t0.omf $(PATCH)
	obj2bin $^ $@


%.hex %.lst %.prn: %.plm
	echo >fort.1
	cp $^ fort.2
	$(ROOT)/tools/plm81
	mv fort.16 fort.4
	mv fort.17 fort.7
	mv fort.12 $*.lst
	$(ROOT)/tools/plm82
	mv fort.17 $*.hex
	cat fort.12 >>$*.lst
	perl $(ROOT)/pretty.pl $*.lst $*.prn
	rm -fr fort.*

clean::
	rm -fr *.hex *.lst *.prn *.omf
