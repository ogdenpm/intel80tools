# path to root of build tree
ITOOLS ?= ../..
TARGETS := m81.com l81.com l82.com l83.com l82fix.com l82a.com
PROTECT := readme.txt m81.patch l81.patch l82.patch l83.patch

include $(ITOOLS)/tools/isis.mk

REF = ref

all:: $(TARGETS)

%.com: %.hex %.patch
	$(ABSBIN) $*.hex $*.patch $@

l82fix.patch:
	@echo "; not needed" >$@

l82a.hex: l82.hex
	cp l82.hex l82a.hex

clean::
	rm -fr *.hex *.prn *.omf *.com l82fix.patch

.INTERMEDIATE: l82.hex l82a.hex l82fix.patch
.PRECIOUS:	%.hex 
