# path to root of build tree
ITOOLS ?= ../..
TARGETS := algolm.com runagl.com
PROTECT := readme.txt m81.patch l81.patch l82.patch l83.patch

include $(ITOOLS)/tools/isis.mk

REF = ref

all:: $(TARGETS)

%.com: %.hex %.patch
	$(ABSBIN) $*.hex $*.patch $@

clean::
	rm -fr *.hex *.prn *.com

.PRECIOUS:	%.hex
