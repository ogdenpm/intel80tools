# path to root of build tree
ROOT := ../..

PLMFLAGS := DEBUG OPTIMIZE
ASMFLAGS :=
LOCATEFLAGS:= PUBLICS SYMBOLS
TARGETS := isis.ov0
REF := ref
include $(ROOT)/tools/isis.mk

all:	isis.ov0

isis.ov0: getd.obj getd5.obj
	$(call link,isis.rel,$^ $(call v40dir,system.lib plm80.lib))
	$(call locate,$@,isis.rel,CODE(0E800h) NAME(ISOV0) STACKSIZE(46) PURGE)
	@rm isis.rel
