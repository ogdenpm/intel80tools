# path to root of build tree
ITOOLS ?= ..\..
TARGETS = imgmds.hex
ASMFLAGS := DEBUG
LOCATEFLAG:= PUBLICS SYMBOLS
include $(ITOOLS)/tools/isis.mk

all:: $(TARGETS)

imgmds.hex: imgmds.abs
	$(ITOOLS)/tools/objhex imgmds.abs $@

imgmds.abs: imgmds.obj
	$(call locate,$@,$^,CODE(4000H))
	

