ROOT = ../..
SHARED := ../common
PEXFILE :=$(SHARED)/plm.pex

include $(ROOT)/tools/isis.mk

# force shared folder to be on isis drive :f3:
export ISIS_F3 := $(SHARED)/
PURGE := purge

TARGETS = plm80.ov0
NAME = PL8020
STACK = 314


LOCATEFLAGS:=SYMBOLS PUBLICS
PLMFLAGS:=
ASMFLAGS:=

OBJ =	main0.obj plm0a.obj plm0b.obj plm0c.obj plm0d.obj plm0e.obj plm0f.obj\
	$(SHARED)/movmem.obj $(SHARED)/fill.obj $(SHARED)/strcmp.obj $(SHARED)/fatal.obj\
	$(SHARED)/initf.obj $(SHARED)/openf.obj $(SHARED)/readf.obj $(SHARED)/tellf.obj\
	$(SHARED)/seekf.obj $(SHARED)/rewind.obj $(SHARED)/backup.obj $(SHARED)/closef.obj\
	$(SHARED)/creatf.obj $(SHARED)/fflush.obj $(SHARED)/fwrite.obj $(SHARED)/chain.obj\
	$(SHARED)/gitype.obj $(SHARED)/sitype.obj $(SHARED)/gisym.obj $(SHARED)/giscop.obj\
	$(SHARED)/gilit.obj $(SHARED)/silit.obj $(SHARED)/setflg.obj $(SHARED)/tstflg.obj\
	$(SHARED)/zerflg.obj $(SHARED)/clrflg.obj $(SHARED)/cpyflg.obj $(SHARED)/siflag.obj\
	$(SHARED)/tiflag.obj $(SHARED)/sidim.obj $(SHARED)/sibseo.obj $(SHARED)/giparo.obj\
	$(SHARED)/siparo.obj $(SHARED)/gidtyp.obj $(SHARED)/sidtyp.obj $(SHARED)/siintn.obj\
	$(SHARED)/gipcnt.obj $(SHARED)/sipcnt.obj $(SHARED)/sipid.obj $(SHARED)/gicond.obj\
	$(SHARED)/sicond.obj $(SHARED)/creati.obj $(SHARED)/findmi.obj plm0g.obj\
	$(SHARED)/adninf.obj $(SHARED)/fiscop.obj $(SHARED)/fi.obj $(SHARED)/lookup.obj\
	$(SHARED)/alloc.obj plm0h.obj $(SHARED)/close.obj $(SHARED)/exit.obj\
	$(SHARED)/load.obj $(SHARED)/open.obj $(SHARED)/read.obj $(SHARED)/seek.obj\
	$(SHARED)/cpytil.obj $(SHARED)/prints.obj $(SHARED)/fatlio.obj $(SHARED)/writef.obj\
	$(SHARED)/gilen.obj $(SHARED)/silen.obj $(SHARED)/sisym.obj $(SHARED)/siscop.obj\
	$(SHARED)/gilnko.obj $(SHARED)/silnko.obj $(SHARED)/allocs.obj $(SHARED)/write.obj\
	$(SHARED)/itoa.obj

all: $(TARGETS)

plm80.ov0: plm800.rel
	$(call locate,$@,$<,code(3EA0H) name($(NAME)) stacksize($(STACK)) $(PURGE))

.INTERMEDIATE: plm800.rel

plm800.rel: $(SHARED)/data.abs $(OBJ)
	$(call link,$@,publics($(SHARED)/data.abs) $(OBJ) $(V40)/plm80.lib)


# additional rules for shared objects
$(SHARED)/%.obj: LST=$(SHARED)

$(SHARED)/%.obj: $(SHARED)/%.plm
	$(call plm,$@,$<)

$(SHARED)/%.obj: $(SHARED)/%.asm
	$(call asm,$@,$<)


$(SHARED)/data.abs: $(SHARED)/data.obj
	$(call locate,$@,$<,DATA(3800H))
