ROOT := ../..
SHARED := ../common
PEXFILE=$(SHARED)/plm.pex

include $(ROOT)/tools/isis.mk

# force shared folder to be on isis drive :f3:
export ISIS_F3 := $(SHARED)/
PURGE := purge

TARGETS = plm80.ov2
NAME = PL8040
STACK = 240

LOCATEFLAGS:=SYMBOLS PUBLICS
PLMFLAGS:=
ASMFLAGS:=

OBJ =	main2.obj plm2a.obj plm2b.obj plm2c.obj plm2d.obj plm2e.obj plm2f.obj plm2g.obj plm2h.obj\
	$(SHARED)/movmem.obj $(SHARED)/fill.obj $(SHARED)/fatal.obj $(SHARED)/creatf.obj\
	$(SHARED)/fflush.obj $(SHARED)/fwrite.obj $(SHARED)/fread.obj $(SHARED)/chain.obj\
	$(SHARED)/gitype.obj $(SHARED)/gilnkv.obj $(SHARED)/silnkv.obj $(SHARED)/tiflag.obj\
	$(SHARED)/gieid.obj $(SHARED)/gidim.obj $(SHARED)/giparo.obj $(SHARED)/sidim.obj\
	$(SHARED)/gibsev.obj $(SHARED)/sibsev.obj $(SHARED)/giparv.obj $(SHARED)/gidtyp.obj\
	$(SHARED)/gipcnt.obj $(SHARED)/gipid.obj $(SHARED)/adninf.obj $(SHARED)/exit.obj\
	$(SHARED)/load.obj $(SHARED)/prints.obj $(SHARED)/fatlio.obj $(SHARED)/initf.obj\
	$(SHARED)/readf.obj $(SHARED)/writef.obj $(SHARED)/gilen.obj $(SHARED)/tstflg.obj\
	$(SHARED)/read.obj $(SHARED)/write.obj $(SHARED)/cpytil.obj $(SHARED)/itoa.obj

# the following require plm v3.1
plm2b.obj plm2g.obj: ISISTOOLS=$(V31)

all: $(TARGETS)

plm80.ov2: plm802.rel
	$(call locate,$@,$<,code(3EA0H) name($(NAME)) stacksize($(STACK)) $(PURGE))

.INTERMEDIATE: plm802.rel

plm802.rel: $(SHARED)/data.abs $(OBJ)
	$(call link,$@,publics($(SHARED)/data.abs) $(OBJ) $(V40)/plm80.lib)



# additional rules for shared objects
$(SHARED)/%.obj: LST=$(SHARED)

$(SHARED)/%.obj: $(SHARED)/%.plm
	$(call plm,$@,$<)

$(SHARED)/%.obj: $(SHARED)/%.asm
	$(call asm,$@,$<)


$(SHARED)/data.abs: $(SHARED)/data.obj
	$(call locate,$@,$<,DATA(3800H))

