# common makefile info
ITOOLS ?=../..
TARGETS=format.com bio115.hex ldcpm.hex cpydsk.com
PLMFLAGS=code optimize
ASMFLAGS=
NOVERIFY = T
include $(ITOOLS)/tools/isis.mk

CTOOL=$(ITOOLS)/ctools/
# define the CPM tools
CTOOLS =$(ITOOLS)/ctools
CPM =$(CTOOLS)/cpm
MAC =$(CPM) $(CTOOLS)/mac
RMAC = $(CPM) $(CTOOLS)/rmac
DRLINK = $(CPM) $(CTOOLS)/link

PATH := $(PATH)

# cpm tools mess up the screen output so don't do parallel builds
.NOTPARALLEL:

all:: $(TARGETS)

bio115.hex: ORG := 0F200H
ldcpm.hex:  ORG := 0H

%.loc: %.obj
	$(call locate,$@,$^,code($(ORG)) name($*))

%.hex: %.loc
	$(call abstool,$@,-h $^)

%.com: %.asm
	$(MAC) $^
	$(call abstool,$@,$*.hex)
	rm $*.hex

clean::
	rm -fr *.prn

verify:
	$(info no reference available to verify against)
