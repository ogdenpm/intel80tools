# common makefile info
ITOOLS ?=../..
TARGETS=format.com bio115.hex ldcpm.hex cpydsk.com
PLMFLAGS=code optimize
ASMFLAGS=
NOVERIFY = T
include $(ITOOLS)/tools/isis.mk

CTOOL=$(ITOOLS)/ctools/
mkLower = mv $(shell echo $1 | tr a-z A-Z) $1

PATH := $(PATH)

# cpm tools mess up the screen output so don't do parallel builds
.NOTPARALLEL:

all:: $(TARGETS)

bio115.hex: ORG := 0F200H
ldcpm.hex:  ORG := 0H

%.hex: %.obj
	$(call locate,$*.loc,$^,code($(ORG)) name($*))
	$(ABSBIN) -h $*.loc $@
	rm $*.loc

# the mkLower lines below compensate for cpm writing upper case names
%.com: %.asm
	$(CTOOL)cpm $(CTOOL)asm $*
	$(call mkLower,$*.hex)
	$(call mkLower,$*.prn)
	$(ABSBIN) $*.hex $@
	rm $*.hex

clean::
	rm -fr *.prn

verify:
	$(info no reference available to verify against)
