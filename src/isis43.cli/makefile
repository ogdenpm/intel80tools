ROOT = ../..
TARGETS = isis.cli
REF = ref
include $(ROOT)/tools/isis.mk

PLMFLAGS := DEBUG
ASMFLAGS := DEBUG
LOCATEFLAGS := SYMBOLS

objs :=	cli.obj cloop.obj setcmd.obj memck.obj cons.obj sblok1.obj\
	backci.obj load.obj openi.obj openo.obj readc.obj seek.obj whoci.obj\
	isiscl.obj renam.obj

all:: $(TARGETS)

isis.cli: cli.rel
	$(call locate,$@,$^,code(3680h) name(CLI) stacksize(14) purge)

.INTERMEDIATE: cli.rel
cli.rel: $(objs)
	$(call link,$@,$^ $(plm80.lib))

