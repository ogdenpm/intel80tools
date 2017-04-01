ROOT = ../..
TARGETS = isis.cli
REFDIR = ref
include $(ROOT)/tools/isistools.mk

PLMOPT := DEBUG
ASMOPT := DEBUG
LOCATEOPT := SYMBOLS

OBJS :=	cli.obj cloop.obj setcmd.obj memck.obj cons.obj sblok1.obj\
	backci.obj load.obj openi.obj openo.obj readc.obj seek.obj whoci.obj\
	isiscl.obj renam.obj

all: $(TARGETS)

isis.cli: $(OBJS)
	$(call link,cli.rel,$^ $(call tools-v40,plm80.lib))
	$(call locate,$@,cli.rel,code(3680h) name(CLI) stacksize(14) purge)
	@del cli.rel

