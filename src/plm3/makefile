ROOT := ../..
SHARED := ../common
PEXFILE=$(SHARED)/plm.pex

include $(ROOT)/tools/isis.mk

# force shared folder to be on isis drive :f3:
export ISIS_F3 := $(SHARED)/
PURGE := purge

TARGETS = plm80.ov3
NAME = PL8042
STACK = 194

LOCATEFLAGS:=SYMBOLS PUBLICS
PLMFLAGS:=
ASMFLAGS:=

OBJ =	main3.obj plm3a.obj $(SHARED)/exit.obj $(SHARED)/itoa.obj\
	$(SHARED)/openf.obj $(SHARED)/rewind.obj $(SHARED)/sekend.obj $(SHARED)/closef.obj\
	$(SHARED)/creatf.obj $(SHARED)/fflush.obj $(SHARED)/fwrite.obj $(SHARED)/fread.obj\
	$(SHARED)/chain.obj $(SHARED)/deletf.obj $(SHARED)/gitype.obj $(SHARED)/gisym.obj\
	$(SHARED)/giscop.obj $(SHARED)/gilnkv.obj $(SHARED)/silnkv.obj $(SHARED)/tiflag.obj\
	$(SHARED)/gieid.obj $(SHARED)/gidim.obj $(SHARED)/giparo.obj $(SHARED)/gidim2.obj\
	$(SHARED)/gibsev.obj $(SHARED)/giintn.obj $(SHARED)/adninf.obj plm3b.obj\
	$(SHARED)/endcom.obj $(SHARED)/movmem.obj $(SHARED)/fill.obj $(SHARED)/close.obj\
	$(SHARED)/delete.obj $(SHARED)/load.obj $(SHARED)/open.obj $(SHARED)/seek.obj\
	$(SHARED)/prints.obj $(SHARED)/fatlio.obj $(SHARED)/initf.obj $(SHARED)/readf.obj\
	$(SHARED)/writef.obj $(SHARED)/seekf.obj $(SHARED)/gilen.obj $(SHARED)/tstflg.obj\
	$(SHARED)/read.obj $(SHARED)/write.obj $(SHARED)/cpytil.obj

all: $(TARGETS)

plm80.ov3: plm803.rel
	$(call locate,$@,$<,code(3EA0H) name($(NAME)) stacksize($(STACK)) $(PURGE))

.INTERMEDIATE: plm803.rel

plm803.rel: $(SHARED)/data.abs $(OBJ)
	$(call link,$@,publics($(SHARED)/data.abs) $(OBJ) $(V40)/plm80.lib)



# additional rules for shared objects
$(SHARED)/%.obj: LST=$(SHARED)

$(SHARED)/%.obj: $(SHARED)/%.plm
	$(call plm,$@,$<)

$(SHARED)/%.obj: $(SHARED)/%.asm
	$(call asm,$@,$<)


$(SHARED)/data.abs: $(SHARED)/data.obj
	$(call locate,$@,$<,DATA(3800H))

