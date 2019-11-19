# path to root of build tree
ROOT := ..\..
TARGETS = lib.com
PEXFILE := lib.pex
REF=ref
PROTECT = patch.txt

include $(ROOT)/tools/isis.mk

PLMFLAGS := OPTIMIZE CODE DEBUG
PLM80:=3.1
ASMFLAGS := DEBUG
LOCATEFLAGS:= PUBLICS SYMBOLS STACKSIZE(76)


OBJS = lib.obj rel.obj find.obj parse.obj util.obj io.obj cpm.obj cpmvec.obj

.PHONY: all
all::
	$(MAKE) $(TARGETS)

%.com: %.abs patch.txt
	$(ROOT)/tools/obj2bin $@ $^

%.abs: %.rel
	$(info $@ $^)
	$(call locate,$@,$^,CODE(100h) purge)

lib.rel: $(OBJS)
	$(call link,$@,$^ $(plm80.lib))

$(OBJS): lib.pex

print: all lib.abs
	perl $(ROOT)/relst.pl lib.abs lib.map $(OBJS:.obj=.lst)
