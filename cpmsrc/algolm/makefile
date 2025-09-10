# path to root of build tree
ITOOLS ?= ../..
TARGETS := algolm.com runagl.com

include $(ITOOLS)/tools/isis.mk

REF = ref

all:: $(TARGETS)

%.com: %.hex
	$(call abstool,$@,$^)

.PRECIOUS:	%.hex
