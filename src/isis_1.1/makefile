# path to root of build tree
ROOT := ../..
TARGETS := format delete isis.cli attrib dir rename copy hexbin\
   		   isis.t0 isis.bin

.NOTPARALLEL:

include $(ROOT)/tools/isis.mk

REF = $(call ipath,isis,1.1)
PATCH = isis.t0_1.1.patch
PROTECT = $(PATCH)

all:: $(TARGETS)

%.omf: %.hex
	hexobj $^ $@

% : %.omf
	obj2bin -i $^ $@

isis.t0: isis.t0.omf $(PATCH)
	obj2bin $^ $@

isis.bin: isis.obj
	obj2bin -i $^ $@


%.hex %.lst %.prn: %.plm
	@echo >fort.1
	@cp $^ fort.2
	@echo "plm81 $^"
	@$(ROOT)/tools/plm81
	@mv fort.16 fort.4
	@mv fort.17 fort.7
	@mv fort.12 $*.lst
	@echo "plm82 $^"
	@$(ROOT)/tools/plm82
	@mv fort.17 $@
	@cat fort.12 >>$*.lst
	@perl $(ROOT)/pretty.pl $*.lst $*.prn
	@rm -fr fort.*

clean::
	rm -fr *.hex *.prn *.lst *.obj
