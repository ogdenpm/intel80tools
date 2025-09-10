# path to root of build tree
ITOOLS ?= ..\..
TARGETS = lib.com
PEXFILE := lib.pex
REF=ref

include $(ITOOLS)/tools/isis.mk

PLMFLAGS := OPTIMIZE CODE DEBUG
PLM80:=3.1
ASMFLAGS := DEBUG
LOCATEFLAGS:= PUBLICS SYMBOLS STACKSIZE(76)


OBJS = lib.obj rel.obj cmdlin.obj parsef.obj util.obj io.obj cpm.obj cpmvec.obj

.PHONY: all
all::
	$(MAKE) $(TARGETS)

%.com: %.abs
	$(call abstool,$@,$^)

%.abs: %.rel
	$(info $@ $^)
	$(call locate,$@,$^,CODE(100h) purge)

lib.rel: $(OBJS)
	$(call link,$@,$^ $(plm80.lib))

$(OBJS): lib.pex

print: all lib.abs
	perl $(ITOOLS)/tools/relst.pl lib.abs lib.map $(OBJS:.obj=.lst)
