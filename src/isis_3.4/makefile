ITOOLS ?= ../..
TARGETS = isis.bin
PEXFILE := isis34.pex
include $(ITOOLS)/tools/isis.mk

REF = $(call ipath,isis,3.4)
PLM80 := 3.1
LOCATEFLAGS := PUBLICS SYMBOLS
PLMFLAGS := DEBUG

objs =	loc62.obj isis.obj load.obj open.obj path.obj\
	rename.obj rescan.obj rw.obj seek.obj trap.obj\
	aft.obj alloc.obj attrib.obj buffer.obj close.obj\
	consol.obj delete.obj direct.obj diskio.obj disk2.obj\
	error.obj exit.obj jmpto.obj reboot.obj memck.obj\
	upper.obj unpath.obj

all::
	$(MAKE) $(TARGETS)

isis.bin: isis.abs
	$(ABSBIN) -i $^ $@

isis.abs: isis.rel
	$(call locate,$@,$^,code(3eh) name(isis) stacksize(0) memory(3000h))

.INTERMEDIATE: isis.rel

isis.rel: $(objs)
	$(call link,$@,$^ $(plm80.lib))

