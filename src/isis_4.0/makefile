ROOT:= ../..
TARGETS = isis.bin
PEXFILE := isis40.pex
COMPARE := $(ROOT)/tools/isisc

include $(ROOT)/tools/isis.mk

REF = $(call ipath,isis,4.0)

LOCATEFLAGS := PUBLICS SYMBOLS

objs =	loc62.obj isis.obj aft.obj attrib.obj\
	close.obj consol.obj delete.obj diskio.obj\
	error.obj exit.obj load.obj open.obj\
	path.obj rename.obj rescan.obj rw.obj seek.obj reboot.obj\
	alloc.obj trap0b.obj buffer.obj rw1.obj direct.obj disk2.obj\
	jmpto.obj trap.obj fnddir.obj memck.obj

all:: $(TARGETS)

isis.bin: isis
	$(ROOT)/tools/packisis isis

isis: isis.rel
	$(call locate,$@,$^,code(3eh) name(isis) stacksize(0) memory(3000h) purge)

.INTERMEDIATE: isis.rel
isis.rel: $(objs)
	$(call link,$@,$^ $(plm80.lib))

