# shared makefile configuration for isis tool builds
ITOOLS ?= $(abspath $(dir $(lastword $(MAKEFILE_LIST)))..)
include $(ITOOLS)/tools/common.mk

# define the CPM tools
CTOOLS =$(ITOOLS)/ctools
CPM =$(CTOOLS)/cpm
MAC =$(CPM) $(CTOOLS)/mac
RMAC = $(CPM) $(CTOOLS)/rmac
DRLINK = $(CPM) $(CTOOLS)/link


# CPM rules
# asm rules
%.hex: %.asm 
	$(MAC) $^

%.rel: %.asm
	$(RMAC) $^

# com from hex file
%.com: %.hex 
	$(call abstool,$@,$^)

%.spr: %.rel
	$(DRLINK) $@ = $^ [OS]

%.prl: %.rel
	$(DRLINK) $@ = $^ [OP]

# com from intel abs file 
%.com: %.abs
	$(call abstool,$@,$^)



