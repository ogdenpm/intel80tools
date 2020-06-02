# path to root of build tree
ROOT := ../..
TARGETS := m81.com l81.com l82.com l83.com
PROTECT := loadAndFix.pl comments.txt

include $(ROOT)/tools/isis.mk

REF = ref


all:: $(TARGETS)

%.com: %.hex
	perl loadAndFix.pl $^ $@

%.hex: %.plm
	@echo "$^:"
	@echo >fort.1
	@cp $^ fort.2
	$(ROOT)/tools/plm81
	@mv fort.16 fort.4
	@mv fort.17 fort.7
	@mv fort.12 $*.lst
	$(ROOT)/tools/plm82
	@mv fort.17 $@
	@cat fort.12 >>$*.lst
	perl $(ROOT)/pretty.pl $*.lst $*.prn
	@rm -fr fort.* $*.lst

clean::
	rm -fr *.hex *.prn
