ROOT:= ../..
TARGETS = isis.bin
REF = ref
PEXFILE := isis34.pex
COMPARE := $(ROOT)/tools/isisc
include $(ROOT)/tools/isis.mk

ISISTOOLS := $(V31)
LOCATEFLAGS := PUBLICS SYMBOLS
PLMFLAGS := DEBUG

objs =	loc62.obj isis.obj load.obj open.obj path.obj\
	rename.obj rescan.obj rw.obj seek.obj trap.obj\
	aft.obj alloc.obj attrib.obj buffer.obj close.obj\
	consol.obj delete.obj direct.obj diskio.obj disk2.obj\
	error.obj exit.obj jmpto.obj reboot.obj memck.obj\
	upper.obj unpath.obj

all:: $(TARGETS)

isis.bin: isis
	$(ROOT)/tools/packisis isis

isis: isis.rel
	$(call locate,$@,$^,code(3eh) name(isis) stacksize(0) memory(3000h))

.INTERMEDIATE: isis.rel

isis.rel: $(objs)
	$(call link,$@,$^ $(V31)/plm80.lib)

