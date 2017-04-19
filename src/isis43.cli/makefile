ROOT = ../..
TARGETS = isis.cli
REF = ref
include $(ROOT)/tools/isis.mk

PLMFLAGS := DEBUG
ASMFLAGS := DEBUG
LOCATEFLAGS := SYMBOLS

OBJS :=	cli.obj cloop.obj setcmd.obj memck.obj cons.obj sblok1.obj\
	backci.obj load.obj openi.obj openo.obj readc.obj seek.obj whoci.obj\
	isiscl.obj renam.obj

all: $(TARGETS)

isis.cli: $(OBJS)
	$(call link,cli.rel,$^ $(V40)/plm80.lib)
	$(call locate,$@,cli.rel,code(3680h) name(CLI) stacksize(14) purge)
	@rm cli.rel

