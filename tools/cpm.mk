# shared makefile configuration for isis tool builds
ITOOLS ?= $(abspath $(dir $(lastword $(MAKEFILE_LIST)))..)
include $(ITOOLS)/tools/common.mk

# define the CPM tools
CTOOLS =$(ITOOLS)/ctools
CPM =$(CTOOLS)/cpm
#MAC =$(CPM) $(CTOOLS)/mac
MAC = $(ITOOLS)/tools/mac
RMAC = $(CPM) $(CTOOLS)/rmac
DRLINK = $(CPM) $(CTOOLS)/link

# the Digital Research applications create tmp files that are not unique
# so force all build to be done serially
.NOTPARALLEL:

# CPM rules
# asm rules
%.hex: %.asm 
	$(MAC) $^

%.rel: %.asm
	$(RMAC) $^

# com from hex file
%.com: %.hex 
	$(call abstool,$@,$^)

%.spr: %0.hex %1.hex
	genmod $^ $@

%.prl: %1.hex %0.hex
	genmod $^ $@

# com from intel abs file 
%.com: %.abs
	$(call abstool,$@,$^)

# make the pair of files needed to generate a .spr/.prl file
%0.hex %1.hex: %.asm
	$(MAC) -p. -h$*1 -c+r $^
	$(MAC) -h$*0 $^ 


