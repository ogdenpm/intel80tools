# path to root of build tree
ROOT := ../..

PLMFLAGS := DEBUG OPTIMIZE
ASMFLAGS :=
LOCATEFLAGS:= PUBLICS SYMBOLS
TARGETS := isis.ov0
REF := ref
include $(ROOT)/tools/isis.mk

all::	isis.ov0

isis.ov0: isisov.rel
	$(call locate,$@,$^,CODE(0E800h) NAME(ISOV0) STACKSIZE(46) PURGE)

.INTERMEDIATE: isisov.rel
isisov.rel: getd.obj getd5.obj
	$(call link,$@,$^ $(call v40dir,system.lib plm80.lib))
