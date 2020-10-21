# path to root of build tree
ITOOLS ?= ../..
TARGETS := isis.ov0

include $(ITOOLS)/tools/isis.mk

REF=$(call ipath,isis,4.3)
PLMFLAGS := DEBUG OPTIMIZE
ASMFLAGS :=
LOCATEFLAGS:= PUBLICS SYMBOLS

all::
	$(MAKE) isis.ov0

isis.ov0: isisov.rel
	$(call locate,$@,$^,CODE(0E800h) NAME(ISOV0) STACKSIZE(46) PURGE)

.INTERMEDIATE: isisov.rel
isisov.rel: getd.obj getd5.obj
	$(call link,$@,$^ $(system.lib) $(plm80.lib))
