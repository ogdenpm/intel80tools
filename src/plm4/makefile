ROOT := ../..
SHARED := ../common
PEXFILE=$(SHARED)/plm.pex

include $(ROOT)/tools/isis.mk

# force shared folder to be on isis drive :f3:
export ISIS_F3 := $(SHARED)/
PURGE := purge

TARGETS = plm80.ov4
NAME = PL8045
STACK = 266

LOCATEFLAGS:=SYMBOLS PUBLICS
PLMFLAGS:=
ASMFLAGS:=

OBJ =	pdata4.obj main4.obj plm4a.obj plm4b.obj plm4c.obj\
	$(SHARED)/movmem.obj $(SHARED)/exit.obj $(SHARED)/itoa.obj $(SHARED)/fatal.obj\
	$(SHARED)/initf.obj $(SHARED)/openf.obj $(SHARED)/readf.obj $(SHARED)/tellf.obj\
	$(SHARED)/seekf.obj $(SHARED)/backup.obj $(SHARED)/closef.obj $(SHARED)/creatf.obj\
	$(SHARED)/fflush.obj $(SHARED)/fwrite.obj $(SHARED)/fread.obj $(SHARED)/chain.obj\
	$(SHARED)/deletf.obj $(SHARED)/gitype.obj $(SHARED)/gisym.obj $(SHARED)/gilnkv.obj\
	$(SHARED)/tiflag.obj $(SHARED)/gieid.obj wrec.obj $(SHARED)/lstinf.obj\
	$(SHARED)/endcom.obj $(SHARED)/putlst.obj lstsp4.obj $(SHARED)/lstlin.obj\
	$(SHARED)/fill.obj $(SHARED)/close.obj $(SHARED)/delete.obj $(SHARED)/load.obj\
	$(SHARED)/open.obj $(SHARED)/read.obj $(SHARED)/seek.obj $(SHARED)/cpytil.obj\
	$(SHARED)/prints.obj $(SHARED)/fatlio.obj $(SHARED)/writef.obj $(SHARED)/tstflg.obj\
	$(SHARED)/gibsev.obj $(SHARED)/wrclst.obj $(SHARED)/wr2lst.obj page.obj\
	$(SHARED)/write.obj $(SHARED)/wrslst.obj


all: $(TARGETS)

plm80.ov4: plm804.rel
	$(call locate,$@,$<,code(3EA0H) name($(NAME)) stacksize($(STACK)) $(PURGE))

.INTERMEDIATE: plm804.rel

plm804.rel: $(SHARED)/data.abs $(OBJ)
	$(call link,$@,publics($(SHARED)/data.abs) $(OBJ) $(V40)/plm80.lib)



# additional rules for shared objects
$(SHARED)/%.obj: LST=$(SHARED)

$(SHARED)/%.obj: $(SHARED)/%.plm
	$(call plm,$@,$<)

$(SHARED)/%.obj: $(SHARED)/%.asm
	$(call asm,$@,$<)


$(SHARED)/data.abs: $(SHARED)/data.obj
	$(call locate,$@,$<,DATA(3800H))

