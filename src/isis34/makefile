ROOT:= ../..
TARGETS = isis.bin
REFDIR = ref
PEXFILE := isis34.pex
COMPARE := $(ROOT)/tools/isisc
include $(ROOT)/tools/isistools.mk

ISISTOOLS := $(V31)
LOCATEOPT := PUBLICS SYMBOLS
PLMOPT := DEBUG

OBJ =	loc3E.obj isis.obj load.obj open.obj path.obj\
	rename.obj rescan.obj rw.obj seek.obj trap.obj\
	aft.obj alloc.obj attrib.obj buffer.obj close.obj\
	consol.obj delete.obj direct.obj diskio.obj disk2.obj\
	error.obj exit.obj jmpto.obj reboot.obj memck.obj\
	upper.obj unpath.obj

all: $(TARGETS)

isis.bin: isis
	$(ROOT)/tools/packisis isis

isis: $(OBJ)
	$(call link,isis.rel,$^ $(call tools-v31,plm80.lib))
	$(call locate,$@,isis.rel,code(3eh) name(isis) stacksize(0) memory(3000h))
