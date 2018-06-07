# path to root of build tree
ROOT := ../..
include $(ROOT)/tools/isis.mk

# define a rule to make bin files from hex files

%.bin: %.hex
	hex2bin $*
