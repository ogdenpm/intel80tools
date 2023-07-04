# path to root of build tree
ITOOLS ?= ../..
NOVERIFY=Y
include $(ITOOLS)/tools/isis.mk

# define a rule to make bin files from hex files

%.bin: %.hex
	hex2bin $*
