ITOOLS ?= ../..
TARGETS = isis.bin
PEXFILE := isis42w.pex

include $(ITOOLS)/tools/isis.mk

REF = $(call ipath,isis,4.2w)
LOCATEFLAGS := SYMBOLS PUBLICS
PLMFLAGS	:= DEBUG OPTIMIZE CODE
ASMFLAGS	:= DEBUG

objs =	loc62.obj aft.obj attrib.obj\
	close.obj consol.obj delete.obj diskio.obj\
	error.obj exit.obj getatt.obj load.obj open.obj\
	path.obj rename.obj rescan.obj rw.obj seek.obj\
	alloc.obj buffer.obj rw1.obj direct.obj disk2.obj\
	trap.obj fnddir.obj memck.obj

all::
	$(MAKE) $(TARGETS)

isis.bin: isis.abs
	$(call abstool,$@,-i $^,isis.patch)

isis.abs: isis.rel
	$(call locate,$@,$^,code(0) name(isis) stacksize(0) memory(3000h))

isis.rel: $(objs)
	$(call link,$@,$^ $(plm80.lib))

print: all
	perl $(ITOOLS)/tools/relst.pl isis.abs isis.map $(objs:.obj=.lst)
