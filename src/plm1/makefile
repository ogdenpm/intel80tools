ROOT := ../..
SHARED := ../common
PEXFILE=$(SHARED)/plm.pex

include $(ROOT)/tools/isis.mk

# force shared folder to be on isis drive :f3:
export ISIS_F3 := $(SHARED)/
PURGE := purge

TARGETS = plm80.ov1
NAME = PL8025
STACK = 274

LOCATEFLAGS:=SYMBOLS PUBLICS
PLMFLAGS:=
ASMFLAGS:=

OBJ =	main1.obj plm1a.obj plm1b.obj plm1c.obj plm1d.obj plm1e.obj plm1f.obj\
	$(SHARED)/movmem.obj $(SHARED)/openf.obj $(SHARED)/rewind.obj $(SHARED)/closef.obj\
	$(SHARED)/creatf.obj $(SHARED)/fflush.obj $(SHARED)/fwrite.obj $(SHARED)/fread.obj\
	$(SHARED)/chain.obj $(SHARED)/deletf.obj $(SHARED)/gitype.obj $(SHARED)/sisym.obj\
	$(SHARED)/giscop.obj $(SHARED)/gilnko.obj $(SHARED)/gilnkv.obj $(SHARED)/silnkv.obj\
	$(SHARED)/clrflg.obj $(SHARED)/siflag.obj $(SHARED)/tiflag.obj $(SHARED)/gieid.obj\
	$(SHARED)/sieid.obj $(SHARED)/gidim.obj $(SHARED)/sidim.obj $(SHARED)/gibseo.obj\
	$(SHARED)/giparo.obj $(SHARED)/siparo.obj $(SHARED)/gidtyp.obj $(SHARED)/gipcnt.obj\
	$(SHARED)/gibin.obj $(SHARED)/creati.obj $(SHARED)/findmi.obj $(SHARED)/adninf.obj\
	$(SHARED)/fi.obj $(SHARED)/alloc.obj $(SHARED)/fill.obj $(SHARED)/close.obj\
	$(SHARED)/delete.obj $(SHARED)/load.obj $(SHARED)/open.obj $(SHARED)/fatlio.obj\
	$(SHARED)/initf.obj $(SHARED)/readf.obj $(SHARED)/writef.obj $(SHARED)/seekf.obj\
	$(SHARED)/gilen.obj $(SHARED)/silen.obj $(SHARED)/sitype.obj $(SHARED)/siscop.obj\
	$(SHARED)/silnko.obj $(SHARED)/setflg.obj $(SHARED)/tstflg.obj $(SHARED)/fiscop.obj\
	$(SHARED)/exit.obj $(SHARED)/read.obj $(SHARED)/seek.obj $(SHARED)/write.obj\
	$(SHARED)/cpytil.obj $(SHARED)/itoa.obj $(SHARED)/prints.obj

# the following require plm v3.1
plm1b.obj: ISISTOOLS=$(V31)

all: $(TARGETS)

plm80.ov1: plm801.rel
	$(call locate,$@,$<,code(3EA0H) name($(NAME)) stacksize($(STACK)) $(PURGE))

.INTERMEDIATE: plm801.rel

plm801.rel: $(SHARED)/data.abs $(OBJ)
	$(call link,$@,publics($(SHARED)/data.abs) $(OBJ) $(V40)/plm80.lib)


# additional rules for shared objects
$(SHARED)/%.obj: LST=$(SHARED)

$(SHARED)/%.obj: $(SHARED)/%.plm
	$(call plm,$@,$<)

$(SHARED)/%.obj: $(SHARED)/%.asm
	$(call asm,$@,$<)


$(SHARED)/data.abs: $(SHARED)/data.obj
	$(call locate,$@,$<,DATA(3800H))

