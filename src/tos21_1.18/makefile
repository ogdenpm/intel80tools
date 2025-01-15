# path to root of build tree
ITOOLS ?= ../..
TARGETS := system.omf

# build options
LOCATEFLAGS:=SYMBOLS LINES PUBLICS 
LINKFLAGS:=
PURGE:= purge

include $(ITOOLS)/tools/isis.mk

REF=ref

OBJS := main.obj time.obj edit.obj io.obj get.obj put.obj open.obj \
		clos.obj read.obj writ.obj intel.obj ibm.obj cart.obj bufr.obj \
		jtab.obj phys.obj assi.obj ctab.obj comi.obj

all::
	$(MAKE) $(TARGETS)

# link build rules

system.omf: $(OBJS)
	$(call link,system.rel,$(OBJS))
	$(call locate,$@,system.rel,STACKSIZE(0) COLUMNS(3) CODE(0D500H) NAME(UNKNOWN) $(PURGE))


