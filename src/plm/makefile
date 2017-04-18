ROOT := ../..
SHARED := ../common
PEXFILE :=$(SHARED)/plm.pex

include $(ROOT)/tools/isis.mk

# force shared folder to be on isis drive :f3:
export ISIS_F3 := $(SHARED)/
PURGE := purge

TARGETS = plm80
NAME = PL8010
STACK = 136


LOCATEFLAGS:=SYMBOLS PUBLICS
PLMFLAGS:=
ASMFLAGS:=

OBJ =	main.obj plma.obj plmb.obj $(SHARED)/memchk.obj\
	$(SHARED)/movmem.obj $(SHARED)/fill.obj plmc.obj $(SHARED)/exit.obj\
	$(SHARED)/prints.obj $(SHARED)/fatal.obj $(SHARED)/fatlio.obj $(SHARED)/initf.obj\
	$(SHARED)/openf.obj $(SHARED)/readf.obj $(SHARED)/chain.obj $(SHARED)/siflag.obj\
	$(SHARED)/sidtyp.obj $(SHARED)/sipcnt.obj $(SHARED)/sibin.obj $(SHARED)/creati.obj\
	$(SHARED)/lookup.obj plmd.obj $(SHARED)/load.obj $(SHARED)/open.obj\
	$(SHARED)/read.obj $(SHARED)/write.obj $(SHARED)/strcmp.obj $(SHARED)/cpytil.obj\
	$(SHARED)/itoa.obj $(SHARED)/silen.obj $(SHARED)/sitype.obj $(SHARED)/sisym.obj\
	$(SHARED)/siscop.obj $(SHARED)/silnko.obj $(SHARED)/setflg.obj $(SHARED)/allocs.obj\
	$(SHARED)/alloc.obj plme.obj

all: $(TARGETS)

plm80: plm80.rel
	$(call locate,$@,$<,code(3EA0H) name($(NAME)) stacksize($(STACK)) $(PURGE))

.INTERMEDIATE: plm80.rel

plm80.rel: $(SHARED)/data.abs $(OBJ)
	$(call link,$@,$^ $(V40)/plm80.lib)

# additional rules for shared objects
$(SHARED)/%.obj: LST=$(SHARED)

$(SHARED)/%.obj: $(SHARED)/%.plm
	$(call plm,$@,$<)

$(SHARED)/%.obj: $(SHARED)/%.asm
	$(call asm,$@,$<)


$(SHARED)/data.abs: $(SHARED)/data.obj
	$(call locate,$@,$<,DATA(3800H))

