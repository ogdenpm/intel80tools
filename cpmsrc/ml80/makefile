# path to root of build tree
ROOT := ../..
TARGETS := m81.com l81.com l82.com l83.com l82fix.com l82a.com
PROTECT := loadAndFix.pl readme.txt m81.patch l81.patch l82.patch l83.patch

include $(ROOT)/tools/isis.mk

REF = ref
# due to the use of various fort.x files, prohibit parallel builds
.NOTPARALLEL:

all:: $(TARGETS)

%.com: %.hex
	perl loadAndFix.pl $^ $@

l82a.hex: l82.hex
	cp l82.hex l82a.hex

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

.INTERMEDIATE: l82.hex l82a.hex
