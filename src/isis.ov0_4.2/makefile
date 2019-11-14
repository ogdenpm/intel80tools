# path to root of build tree
ROOT := ../..
TARGETS := isis.ov0

include $(ROOT)/tools/isis.mk

REF=$(call ipath,isis,4.2)
PLMFLAGS := DEBUG OPTIMIZE CODE
ASMFLAGS :=
LOCATEFLAGS:= PUBLICS SYMBOLS
PLM80 := 3.1

all::
	$(MAKE) isis.ov0

isis.ov0: isisov.rel
	$(call locate,$@,$^,CODE(0E800h) NAME(ISOV0) STACKSIZE(46) PURGE)

.INTERMEDIATE: isisov.rel
isisov.rel: getd.obj getd5.obj
	$(call link,$@,$^ $(system.lib) $(plm80.lib))
