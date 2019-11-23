# path to root of build tree
ROOT := ..\..
TARGETS = link.com
PEXFILE := link.pex
REF=ref
include $(ROOT)/tools/isis.mk

PLMFLAGS := OPTIMIZE CODE DEBUG
PLM80:=3.1
ASMFLAGS := DEBUG
LOCATEFLAGS:= PUBLICS SYMBOLS STACKSIZE(138)


OBJS = link.obj link0.obj link1.obj link2.obj link3.obj relasm.obj\
	   link4.obj link5.obj link6.obj file.obj os.obj

link5.obj: PLM80 := 4.0
link6.obj: PLM80 := 3.0

.PHONY: all
all::
	$(MAKE) $(TARGETS)

%.com: %.abs
	$(ROOT)/tools/obj2bin $@ $^

%.abs: %.rel
	$(info $@ $^)
	$(call locate,$@,$^,CODE(100h) purge)

link.rel: $(OBJS)
	$(call link,$@,$^ $(plm80.lib))

$(OBJS): link.pex

print: all link.abs
	perl $(ROOT)/relst.pl link.abs link.map $(OBJS:.obj=.lst)
