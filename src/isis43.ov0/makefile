# path to root of build tree
ROOT := ../..

PLMOPT := DEBUG OPTIMIZE
ASMOPT :=
LOCATEOPT:= PUBLICS SYMBOLS
TARGETS := isis.ov0
REFDIR := ref
include $(ROOT)/tools/isistools.mk

all:	isis.ov0

isis.ov0: getd.obj getd5.obj
	$(call link,isis.rel,$^ $(addprefix $(ISISTOOLS)/,system.lib plm80.lib))
	$(call locate,$@,isis.rel,CODE(0E800h) NAME(ISOV0) STACKSIZE(46) PURGE)
	@del isis.rel
