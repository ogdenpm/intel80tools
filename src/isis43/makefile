ROOT:= ../..
TARGETS = isis.bin
REFDIR = ref
PEXFILE := isis43.pex
COMPARE := $(ROOT)/tools/isisc
KEEP=T

include $(ROOT)/tools/isistools.mk

LOCATEOPT := SYMBOLS PUBLICS
PLMOPT	:= DEBUG OPTIMIZE
ASMOPT	:= DEBUG
COMPARE := $(ROOT)/tools/isisc

OBJ =	loc62.obj aft.obj attrib.obj\
	close.obj consol.obj delete.obj diskio.obj\
	error.obj exit.obj getatt.obj load.obj open.obj\
	path.obj rename.obj rescan.obj rw.obj seek.obj\
	alloc.obj buffer.obj rw1.obj direct.obj disk2.obj\
	trap.obj fnddir.obj memck.obj

all: $(TARGETS)

isis.bin: isis
	$(ROOT)/tools/packisis isis

isis: $(OBJ)
	$(call link,isis.rel,$^ $(call tools-v40,plm80.lib))
	$(call locate,$@,isis.rel,code(0) name(isis) stacksize(0) memory(3000h))

